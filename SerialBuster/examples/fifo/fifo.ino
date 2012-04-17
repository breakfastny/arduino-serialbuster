#include "SerialBuster.h"
#include "Buffer.h"

Buffer b = Buffer();

void setup() {
  //ser.init(9600);
  Serial.begin(9600);
  Serial.println("init");
  b.init(16);
  
  delay(100);
  
  b.enqueue(1);
  b.enqueue(2);
  b.enqueue(3);
  
  while(b.getDataLength())
    Serial.println(b.dequeue());
  
  b.enqueue(-4);
  b.enqueue(-51);

  while(b.getDataLength())
    Serial.println((int8_t)b.dequeue());
  
}

void loop() {
  
}
