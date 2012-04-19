#include "SerialBuster.h"
#include "Buffer.h"

SerialBuster sb = SerialBuster(256, 256, 128);
boolean led_toggle = true;
long mover = 0;


void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  sb.init(115200);
  sb.setCallback(incomingPacket);
  sb.setAddress(1);
  delay(50);
}

void loop() {
  sb.update();
    
//  if(mover++ % 100000 == 0) {
//    Serial.print("mem: ");
//    Serial.println((((float)availableMemory()) / 2048.0) * 100);
//    Serial.flush();
//  }
}

void incomingPacket (uint8_t from, Buffer * buffer) {
  blink_led();
  uint16_t length = buffer->getDataLength();
  uint8_t payload[length];
  int i = 0;
  while(buffer->getDataLength()) {
    payload[i++] = buffer->dequeue();
  }
  // echo payload back to sender
  sb.sendPacket(from, payload, length);
}

void blink_led() {
  digitalWrite(13, led_toggle ? HIGH : LOW);
  led_toggle = !led_toggle;
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
