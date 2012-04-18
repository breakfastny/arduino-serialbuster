#include "SerialBuster.h"
#include "Buffer.h"

SerialBuster sb = SerialBuster(512, 512);
long mover = 0;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  sb.init(115200);
  //sb.setCallback(incomingPacket);
  delay(50);
}

void loop() {
  sb.update();
  if(mover++ % 4000 == 0) {

    const uint8_t * payload = (const uint8_t *)"Hello from Arduino.";
    sb.sendPacket(254, payload, 19);

    int a = availableMemory();
    float percent = ((float)a) / 2048.0;
    Serial.print("mem: ");
    Serial.println(percent * 100);
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
