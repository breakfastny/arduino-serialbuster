#include "SerialBuster.h"
#include "Buffer.h"

SerialBuster sb = SerialBuster(512, 512);
//long mover = 0;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  sb.init(9600);
  sb.setCallback(incomingPacket);
  delay(100);
}

void loop() {
  sb.update();
}

void incomingPacket (uint8_t from, Buffer * buffer) {
  // Flash LED to debug that we have a valid package received.
  blink_led();
  blink_led();
  blink_led();
  blink_led();
}

void blink_led() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
}
