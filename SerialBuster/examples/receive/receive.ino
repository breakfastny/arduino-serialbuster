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
  send(2);
}

void loop() {
  sb.update();
}

void incomingPacket (uint8_t from, Buffer * buffer, uint16_t len) {
  
  uint8_t payload[len];
  int counter = 0;
  while(buffer->getDataLength()) {
    payload[counter++] = buffer->dequeue();
  }

  switch(testCase) {
    
    case 1:
      payload[0] == 0x01 ? ok() : fail();
    break;
      
      
    case 2:
      payload[0] == 0xCA ? ok() : fail();
    break;
    
    
    case 3:
      (int)(payload[1] << 8 | payload[0]) == 12345 ? ok() : fail();
    break;
    
    
    case 4:
      (long)(payload[3] << 24 | payload[2] << 16 | payload[1] << 8 | payload[0]) == 100 * 1000 * 1000 ? ok() : fail();
    break;
    

    case 5:
      (long)(payload[3] << 24 | payload[2] << 16 | payload[1] << 8 | payload[0]) == 100 * 1000 * 1000 * -1 ? ok() : fail();
    break;


    case 6:
      (int)(payload[1] << 8 | payload[0]) == -10293 ? ok() : fail();
    break;
    

    case 7:
      union u_tag u1;
      for (int i = 0; i < 4; i++)
        u1.b[i] = payload[i];
      
      abs(u1.f - 5.1) < FLOAT_TOLERANCE ? ok() : fail();
    break;
    

    case 8:
      union u_tag u2;
      for (int i = 0; i < 4; i++)
        u2.b[i] = payload[i];
      
      abs(u2.f - (-39172.3971)) < FLOAT_TOLERANCE ? ok() : fail();
    break;

  };
    
    
  // get ready for the next test
  testCase++;
  
  // tell master we're ready
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
