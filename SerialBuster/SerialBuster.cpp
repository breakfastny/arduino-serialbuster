
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

uint8_t SerialBuster::sendPacket(uint8_t recipient, const uint8_t * payload, uint16_t length) {

  uint16_t packet_start_index = _out_buf->enqueueUInt8(SB_START);
  _out_buf->enqueueUInt8(recipient);
  _out_buf->enqueueUInt8(_address);
  _out_buf->enqueueUInt16(length);
  _out_buf->enqueueUInt8((uint8_t *)payload, length);
  
  uint8_t checksum = crc8((Buffer *)_out_buf, SB_PACKET_HEADER_SIZE + length, packet_start_index);  
  
  
  
  //crc8_buf->release();
  //free(crc8_buf);
  
    // var crc8_buffer = new Buffer(this.payload.length + PACKET_HEADER_SIZE);
    // crc8_buffer[0] = CONSTANTS.START;
    // crc8_buffer[1] = this.recipient;
    // crc8_buffer[2] = this.sender;
    // crc8_buffer.writeInt16LE(this.payload.length, 3);
    // this.payload.copy(crc8_buffer, PACKET_HEADER_SIZE);
    // 
    // var crc8 = this.crc8(crc8_buffer);
    // 
    // var escape_buffer = new Buffer(crc8_buffer.length + 1);
    // crc8_buffer.copy(escape_buffer);
    // escape_buffer[escape_buffer.length-1] = crc8;
    // 
    // // How many chars to do we have to escape?
    // var escaped_chars = _u.filter(escape_buffer, function (item) {
    //   return (item == CONSTANTS.ESC || item == CONSTANTS.END || item == CONSTANTS.START);
    // }).length - 1; // minus 1 since we don't want to count the START byte
    // 
    // // payload data
    // // packet header
    // // another byte for each escape
    // // leave 2 bytes for crc8 and END
    // var outgoing_buffer = new Buffer(this.payload.length + PACKET_HEADER_SIZE + escaped_chars + 2);
    // 
    // outgoing_buffer[0] = CONSTANTS.START;
    // var outgoing_buffer_pos = 1;
    // 
    // // Escape the buffer
    // for (var i=1; i < escape_buffer.length; i++) {
    //   var b = escape_buffer[i];
    //   switch(b) {
    //     case CONSTANTS.START:
    //       outgoing_buffer[outgoing_buffer_pos++] = CONSTANTS.ESC;
    //       outgoing_buffer[outgoing_buffer_pos++] = CONSTANTS.ESC_START;
    //     break;
    //     case CONSTANTS.END:
    //       outgoing_buffer[outgoing_buffer_pos++] = CONSTANTS.ESC;
    //       outgoing_buffer[outgoing_buffer_pos++] = CONSTANTS.ESC_END;
    //     break;
    //     case CONSTANTS.ESC:
    //       outgoing_buffer[outgoing_buffer_pos++] = CONSTANTS.ESC;
    //       outgoing_buffer[outgoing_buffer_pos++] = CONSTANTS.ESC_ESC;
    //     break;
    //     default:
    //       outgoing_buffer[outgoing_buffer_pos++] = b;
    //     break;
    //   }
    // }
    // 
    // // add the final end byte
    // outgoing_buffer[outgoing_buffer_pos++] = CONSTANTS.END;
    // 
    // return outgoing_buffer;
  
  
  
  
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
      //Serial.write(_out_buf->dequeue());
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
      _in_buf->enqueueUInt8(inbyte);
      break;
      
    // if it's an END character then we're done with the packet
    case SB_END:
      // last piece of the packet stored.
      _in_buf->enqueueUInt8(inbyte);
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
      checksum = crc8((Buffer *)_in_buf, checksum_index, 0);

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
      _in_buf->enqueueUInt8(inbyte);
      break;
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
}