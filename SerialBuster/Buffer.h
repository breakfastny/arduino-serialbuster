
// FIFO Ring buffer of dynamic size
//
// Can also be used to read and write bytes 
// at specific indexes, altho it's stongly 
// discouraged to use both interfaces at the
// same time for one of these intances.
// all values are read and written as Little Endian.


#ifndef Buffer_h
#define Buffer_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

// For converting floats
union buffer_u {
  uint8_t b[4];
  float f;
};

class Buffer {
  public:
    Buffer();
    ~Buffer();
    void init(uint16_t length);

    // Queue functions
    uint16_t enqueueUInt8(uint8_t b);
    uint16_t enqueueUInt8(uint8_t * data, uint16_t len);
    uint16_t enqueueUInt16(uint16_t bb);
    uint8_t dequeue();
    uint8_t pop();

    // Index based functions
    void writeUInt8(uint8_t val, uint16_t offset);
    uint8_t readUInt8(uint16_t offset);

    void writeUInt16(uint16_t val, uint16_t offset);
    uint16_t readUInt16(uint16_t offset);

    void writeUInt32(uint32_t val, uint16_t offset);
    uint32_t readUInt32(uint16_t offset);

    void writeFloat(float val, uint16_t offset);
    float readFloat(uint16_t offset);

    // access position of cursor so we can read index based.
    uint16_t readCursorPos(); // returns the position of the read cursor.
    
    uint8_t peek();
    void clear();
    uint16_t getDataLength();
    uint16_t getSize();
    uint8_t& operator[] (uint16_t index);
    void release();
  protected:
    uint8_t * buf; // main byte store
    uint16_t size;  // size of the buffer
    uint16_t length; // length of data currently put in buffer
    uint16_t cursor_in; // cursor for writing
    uint16_t cursor_out; // cursor for reading
};

#endif // Buffer_h
