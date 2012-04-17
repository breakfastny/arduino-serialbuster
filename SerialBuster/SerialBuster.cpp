
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

void SerialBuster::init(uint16_t baud_rate) {
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
  // Copy buffer and calc checksum
  // byte checksum = 0;
  // while(packet_buffer->getDataLength() > 0) {
  //   byte b = packet_buffer->get();
  //   _out_buf->put(b);
  //   checksum += b;
  // }
  return 0;
}

void SerialBuster::update(){
  
  if(Serial.available() != 0) {
    incomingByte(Serial.read());
  }

  if(_out_buf->getDataLength() > 0){
    // If serial port not currently busy
    if((UCSRA) & (1 << UDRE)) { 
      Serial.write(_out_buf->dequeue());
    }
  }
  
}

void SerialBuster::setPacketHandler(void (*cb)(Buffer*)){
  _cb = cb;
}

void SerialBuster::incomingByte(uint8_t incoming){

  if(_in_buf->getDataLength() == _in_buf->getSize()) {
    _in_buf->clear();
  }
  
  _in_buf->enqueue(incoming);

  // If we have a full packet ready
  if(true){

    // If checksums don't match, then do something ()
    
    // We have a successful packet

    if(_cb != NULL)
      _cb(_in_buf); // actually send the unescaped data here.

    // Clear variables
    _in_buf->clear();
  }
}

uint8_t SerialBuster::crc8(uint8_t *data, uint16_t len) {
  uint8_t crc=0;
  for (uint16_t i=0; i<len;i++) {
    uint8_t inbyte = data[i];
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