#include "SerialBuster.h"
#include "Buffer.h"

Buffer b = Buffer();

void setup() {
  //ser.init(9600);
  Serial.begin(9600);
  Serial.println("init");
  b.init(32);
  
  delay(100);
  
  // store using [] operator
  b[0] = 1;
  b[1] = 2;
  
  Serial.println(b[0]);
  Serial.println(b[1]);
  
  // store 16bit vars
  b.writeUInt16LE(65535, 2);
  Serial.println(b.readUInt16LE(2));
  
}

void loop() {
  
}
