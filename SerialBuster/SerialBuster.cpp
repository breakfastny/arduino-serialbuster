
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "SerialBuster.h"
#include "Buffer.h"


SerialBuster::SerialBuster(uint16_t in_size, uint16_t out_size){
  _in_buf = (Buffer*)malloc(sizeof(Buffer));
  _out_buf = (Buffer*)malloc(sizeof(Buffer));
  _in_buf->init(in_size);
  _out_buf->init(out_size);
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

uint8_t SerialBuster::sendPacket(uint8_t recipient, Buffer* payload) {
  return 0;
}

void SerialBuster::update() {
  // read one byte at the time
  if(Serial.available() > 0) {
    appendIncoming(Serial.read());
    //Serial.write(Serial.read());
  }
  // send one byte at the time
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
  
  //Serial.write(inbyte);
  
  uint16_t checksum_index = 0;
  uint8_t checksum;
  uint8_t recipient;
  
  switch(inbyte) {
    
    // We're starting a new packet
    case SB_START:
      _in_buf->clear();
      //Serial.print('S');
      // save startbyte since we need it for crc8 check
      _in_buf->enqueue(inbyte);
      break;
      
    // if it's an END character then we're done with the packet
    case SB_END:
      // last piece of the packet stored.
      _in_buf->enqueue(inbyte);
      //Serial.print('E');
      // Check the recipient
      recipient = _in_buf->readUInt8(1);
      
      // bad recipient
      if(recipient != _address && recipient != SB_BROADCAST) {
        //Serial.print('R');
        return;
      }
      
      // Now we'll see if the packet if valid by 
      // making a CRC8 check on the header + payload
      checksum_index = _in_buf->getDataLength() - 2;
      checksum = crc8((Buffer *)_in_buf, checksum_index);

      if(checksum == _in_buf->readUInt8(checksum_index) && _cb != NULL) {
        // Make copy of the payload and send it to _cb
        //Serial.print('O');
        _cb(recipient, _in_buf);
        return;
      }else{
        //Serial.print('C');
      }
      
      break;

    // if it's the same code as an ESC character, we'll wait for the next char and see what to do
    case SB_ESC:

      // read next byte
      // TODO: should we hang/wait for serial here?
      inbyte = Serial.read();

      switch(inbyte) {
  
        /* if "inbyte" is not one of these two, then we
         * have a protocol violation.  The best bet
         * seems to be to leave the byte alone and
         * just stuff it into the packet
         */
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

    /* here we fall into the default handler and let
     * it store the character for us
     */
    default:
      //Serial.print('P');
      _in_buf->enqueue(inbyte);
      break;
    }
}

uint8_t SerialBuster::crc8(Buffer * data, uint16_t len) {
  uint8_t crc=0;
  for (uint16_t i=0; i<len;i++) {
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
}