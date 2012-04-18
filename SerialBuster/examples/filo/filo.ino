#include "SerialBuster.h"
#include "Buffer.h"

Buffer b = Buffer();

void setup() {
  //ser.init(9600);
  Serial.begin(57600);
  Serial.println("init");
  b.init(16);
  
  delay(100);
  
  b.enqueueUInt8(1);
  b.enqueueUInt8(2);
  b.enqueueUInt8(3);
  
  while(b.getDataLength())
    Serial.println(b.pop());
  
  b.enqueueUInt8(4);
  Serial.println(b.pop());
}

void loop() {
  
}
