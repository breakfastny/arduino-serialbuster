#include "SerialBuster.h"
#include "Buffer.h"

#define FLOAT_TOLERANCE 0.0001

union u_tag {
  uint8_t b[4];
  float f;
};

SerialBuster sb = SerialBuster(256, 256, 128);
int testCase = 1;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  sb.init(9600);
  sb.setCallback(incomingPacket);
  delay(5000);
  send(2);
}

void loop() {
  sb.update();
}

void incomingPacket (uint8_t from, Buffer * buffer, uint16_t len) {
  
  int readcursor = buffer->readCursorPos();
  
  switch(testCase) {
    
    case 1:
      buffer->readUInt8(readcursor) == 0x01 ? ok() : fail();
    break;
    
      
    case 2:
      buffer->readUInt8(readcursor) == 0xCA ? ok() : fail();
    break;
    
    
    case 3:
      buffer->readUInt16(readcursor) == 12345 ? ok() : fail();
    break;
    
    
    case 4:
      (long)buffer->readUInt32(readcursor) == 100l * 1000l * 1000l ? ok() : fail();
    break;
    

    case 5:
      (long)buffer->readUInt32(readcursor) == 100l * 1000l * 1000l * -1 ? ok() : fail();
    break;


    case 6:
      (int)buffer->readUInt16(readcursor) == -10293 ? ok() : fail();
    break;
    

    case 7:
        abs(buffer->readFloat(readcursor) - 5.1) < FLOAT_TOLERANCE ? ok() : fail();
    break;
    

    case 8:
        abs(buffer->readFloat(readcursor) - (-39172.3971)) < FLOAT_TOLERANCE ? ok() : fail();
    break;
    

    case 9:
        buffer->readUInt8(readcursor) == 'a' ? ok() : fail();
    break;
    

    case 10:
        char a = buffer->readUInt8(readcursor);
        char b = buffer->readUInt8(readcursor + 1);
        char bang = buffer->readUInt8(readcursor + 2);
        (a == 'a' && b == 'b' && bang == '!') ? ok() : fail();
    break;

  };
    
    
  // get ready for the next test
  testCase++;
}

void ok() {
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(50);
  send(1);
}

void fail() {
  for(int i = 0; i < 10; i++) {
    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13, LOW);
    delay(50);
  }
  send(0);
}

void send(int n) {
  sb.sendPacket(SB_MASTER, n);
}

// only for debugging
int availableMemory() {
  int size = 2048; // Use 2048 with ATmega328
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL)
    ;

  free(buf);
  return size;
}
