#include "SerialBuster.h"
#include "Buffer.h"

Buffer b = Buffer();

void setup() {
  //ser.init(9600);
  Serial.begin(9600);
  Serial.println("init");
  b.init(16);
  
  delay(100);
  
  b.enqueueUInt8(1);
  b.enqueueUInt8(2);
  b.enqueueUInt8(3);
  
  while(b.getDataLength())
    Serial.println(b.dequeue());
  
  b.enqueueUInt8(-4);
  b.enqueueUInt8(-51);

  while(b.getDataLength())
    Serial.println((int8_t)b.dequeue());
  
}

void loop() {
  
}
