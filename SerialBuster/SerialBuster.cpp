
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "SerialBuster.h"
#include "Buffer.h"


SerialBuster::SerialBuster(uint16_t in_size, uint16_t out_size, uint16_t max_packet_size) {
  _in_buf = (Buffer*)malloc(sizeof(Buffer));
  _out_buf = (Buffer*)malloc(sizeof(Buffer));
  _packet_buf = (Buffer*)malloc(sizeof(Buffer));
  _in_buf->init(in_size);
  _out_buf->init(out_size);
  _packet_buf->init(max_packet_size);
  _cb = NULL;
  _address = 0x00; // MASTER
}

void SerialBuster::init(long baud_rate) {
  Serial.begin(baud_rate);
}

void SerialBuster::setAddress(uint8_t address) {
  _address = address;
}

bool SerialBuster::isSending() {
  return _out_buf->getDataLength() > 0;
}

bool SerialBuster::isReceiving() {
  return _in_buf->getDataLength() > 0;
}

uint8_t SerialBuster::sendPacket(uint8_t recipient, const uint8_t * payload, uint16_t length) {

  _packet_buf->clear();
  
  // Write header
  _packet_buf->enqueueUInt8(SB_START);
  _packet_buf->enqueueUInt8(recipient);
  _packet_buf->enqueueUInt8(_address);
  _packet_buf->enqueueUInt16(length);
  
  // Append payload
  _packet_buf->enqueueUInt8((uint8_t *)payload, length);
  
  // Calculate checksum for outgoing packet
  uint8_t checksum = crc8((Buffer *)_packet_buf, SB_PACKET_HEADER_SIZE + length, 0);
  
  // Put the checksum in the packet
  _packet_buf->enqueueUInt8(checksum);
  
  // Put the start byte into the output buffer and
  // throw away start byte from packet buffer since we don't
  // want to escape it.
  _out_buf->enqueueUInt8(SB_START);
  _packet_buf->dequeue();
  
  // Now copy _packet_buf (skip SB_START) into output buffer
  // and escape all data while doing so.
  while(_packet_buf->getDataLength()) {
    uint8_t b = _packet_buf->dequeue();
    switch(b) {
      case SB_START:
        _out_buf->enqueueUInt8(SB_ESC);
        _out_buf->enqueueUInt8(SB_ESC_START);
      break;
      case SB_END:
        _out_buf->enqueueUInt8(SB_ESC);
        _out_buf->enqueueUInt8(SB_ESC_END);
      break;
      case SB_ESC:
        _out_buf->enqueueUInt8(SB_ESC);
        _out_buf->enqueueUInt8(SB_ESC_ESC);
      break;
      default:
        _out_buf->enqueueUInt8(b);
      break;
    }
  }
  
  // Finally add the END byte
  _out_buf->enqueueUInt8(SB_END);
  
  // while(_out_buf->getDataLength()) {
  //   Serial.print(_out_buf->dequeue(), HEX);
  //   Serial.print(' ');
  // }
  // Serial.print('\n');
}

void SerialBuster::update() {
  
  // read one byte at the time
  if(Serial.available() > 0) {
    appendIncoming(Serial.read());
    //Serial.write(Serial.read()); // echo
  }
  
  // send one byte at the time
  // TODO: set a flag for chunk size, it might 
  // be more efficient to send data in chunks
  // of 8 or 16 bytes at a time.
  if(_out_buf->getDataLength() > 0) {
    if((SB_UCSRA) & (1 << SB_UDRE)) {
      Serial.write(_out_buf->dequeue());
    }
  }
}

void SerialBuster::setCallback(void (*cb)(uint8_t recipient, Buffer * data)){
  _cb = cb;
}

void SerialBuster::appendIncoming(uint8_t inbyte){
  
  // TODO: Read head as soon as it's written and if recipient isn't for us, then
  // we can set a flag to ignore LENGTH many incoming bytes.
  
  uint16_t checksum_index = 0;
  uint8_t checksum;
  uint8_t recipient;
  uint8_t sender;
  uint16_t payload_length = 0;
  uint16_t esc_wait = 400; // max ticks to wait for next char
  
  //_in_buf->enqueueUInt8(inbyte);
  
  // if(inbyte == SB_END) {
  //   for(size_t i = 0; i < _in_buf->getDataLength(); ++i){
  //     Serial.write(_in_buf->readUInt8(i));
  //   }
  //   _in_buf->clear();
  // }
  
  
  switch(inbyte) {
    
    // We're starting a new packet
    case SB_START:
      _in_buf->clear();
      //Serial.print('S');
      // save startbyte since we need it for crc8 check
      _in_buf->enqueueUInt8(inbyte);
      break;
      
    // if it's an END character then we're done with the packet
    case SB_END:
      // last piece of the packet stored.
      _in_buf->enqueueUInt8(inbyte);
      //Serial.print('E');
      // for(size_t i = 0; i < _in_buf->getDataLength(); ++i){
      //   Serial.write(_in_buf->readUInt8(i));
      // }
      
      
      // Check the recipient
      recipient = _in_buf->readUInt8(1);
      
      // wrong recipient
      if(recipient != _address && recipient != SB_BROADCAST) {
        //Serial.print('R');
        return;
      }
      
      payload_length = _in_buf->readUInt16(3);
      
      // Now we'll see if the packet if valid by 
      // making a CRC8 check on the header + payload
      checksum_index = _in_buf->getDataLength() - 2;
      //checksum_index = SB_PACKET_HEADER_SIZE + payload_length + 1;
      checksum = crc8((Buffer *)_in_buf, checksum_index, 0);

      if(checksum == _in_buf->readUInt8(checksum_index) && _cb != NULL) {
        //Serial.print('O');
        
        // TODO: Make copy of the payload and send it to _cb
        sender = _in_buf->readUInt8(2);
        
        //get rid of the header
        for(uint8_t i = 0; i < SB_PACKET_HEADER_SIZE; ++i){
          _in_buf->dequeue();
        }
        
         // tail
        _in_buf->pop();
        _in_buf->pop();
        
        // dispatch payload
        _cb(sender, _in_buf);
        return;
      }else{
        //Serial.print('C');
      }
      
      break;

    // if it's the same code as an ESC character, we'll wait for the next char and see what to do
    case SB_ESC:

      do {
        inbyte = Serial.read();
      } while (inbyte == 255 && esc_wait-- != 0);
      
      // Serial.print("E");
      // Serial.write(inbyte);

      switch(inbyte) {
  
        //if "inbyte" is not one of these two, then we
        //have a protocol violation.  The best bet
        //seems to be to leave the byte alone and
        //just stuff it into the packet
        //
        case SB_ESC_END:
          inbyte = SB_END;
        break;
        case SB_ESC_START:
          inbyte = SB_START;
        break;
        case SB_ESC_ESC:
          inbyte = SB_ESC;
        break;
      }
    
    // here we fall into the default handler and let
    // it store the character for us
    default:
      //Serial.print('P');
      _in_buf->enqueueUInt8(inbyte);
  }
}

uint8_t SerialBuster::crc8(Buffer * data, uint16_t len, uint16_t offset) {
  uint8_t crc=0;
  for (uint16_t i=offset; i<len+offset;i++) {
    uint8_t inbyte = data->readUInt8(i);
    for (uint8_t j=0;j<8;j++){
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) 
        crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}


SerialBuster::~SerialBuster() {
  free(_in_buf);
  free(_out_buf);
  free(_packet_buf);
}