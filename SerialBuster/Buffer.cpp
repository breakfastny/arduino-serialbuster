#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Buffer.h"

Buffer::Buffer() {}

void Buffer::init(uint16_t _size) { 
	buf = (uint8_t *) calloc(_size, sizeof(uint8_t));
	size = _size;
  length = 0;
	cursor_in = 0;
  cursor_out = 0;
}

uint8_t Buffer::enqueue(uint8_t b) {
  if (length < size) {
    length++;
    cursor_in %= size;
    buf[cursor_in] = b;
    cursor_in++;
    return 1;
  }
  return 0;
}

uint8_t Buffer::peek() {
  return buf[cursor_out % size];
}

void Buffer::clear() {
  cursor_in = 0;
  cursor_out = 0;
  length = 0;
}

uint8_t Buffer::dequeue() {
  length--;
  cursor_out %= size;
  return buf[cursor_out++];
}

uint16_t Buffer::writeUInt8(uint8_t val, uint16_t offset){
  buf[offset] = val;
}

uint8_t Buffer::readUInt8(uint16_t offset){
  return buf[offset];
}

uint16_t Buffer::writeUInt16LE(uint16_t val, uint16_t offset) {
  buf[offset] = (val >> 8) & 0xFF;
  buf[offset+1] = val & 0xFF;
	return val;
}

uint16_t Buffer::readUInt16LE(uint16_t offset) {
  return (uint16_t) (buf[offset] << 8 | buf[offset+1]);
}

uint16_t Buffer::getDataLength() {
  return length;
}

uint16_t Buffer::getSize() {
  return size;
}

uint8_t & Buffer::operator[] (uint16_t index) {
  return buf[index];
}

Buffer::~Buffer() {
  free(buf);
}