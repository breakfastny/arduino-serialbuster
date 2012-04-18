#include "SerialBuster.h"
#include "Buffer.h"

SerialBuster sb = SerialBuster(256, 256, 128);
long mover = 0;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  sb.init(57600);
  //sb.setCallback(incomingPacket);
  delay(50);
}

void loop() {
  sb.update();
  if(mover++ % 10000 == 0) {

    const uint8_t * payload = (const uint8_t *)"An end (\x03) char!";
    sb.sendPacket(255, payload, strlen((const char *)payload));
    
//    int a = availableMemory();
//    float percent = ((float)a) / 2048.0;
//    Serial.print("mem: ");
//    Serial.println(percent * 100);
  }
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
