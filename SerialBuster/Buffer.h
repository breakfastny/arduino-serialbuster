
#ifndef Buffer_h
#define Buffer_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


class Buffer {
  public:
    Buffer(uint16_t size);
    ~Buffer();
    void init(uint16_t length);
    uint8_t enqueue(uint8_t b);
    uint8_t dequeue();
    uint8_t peek();
    void clear();
    uint16_t getDataLength();
    uint16_t getSize();
    uint8_t& operator[] (uint16_t x);
  
  protected:
    uint8_t * buf; // main byte store
    uint16_t size;  // size of the buffer
    uint16_t length; // length of data currently put in buffer
    uint16_t cursor_in; // cursor for writing
    uint16_t cursor_out; // cursor for reading
};

#endif // Buffer_h