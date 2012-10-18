#include "SerialBuster.h"
#include "Buffer.h"

Buffer b = Buffer();

void setup() {
  //ser.init(9600);
  Serial.begin(9600);
  Serial.println("init");
  b.init(256);
  
  //delay(100);
  
  // store using [] operator
  b[0] = 1;
  b[1] = 2;
  
  Serial.println(b[0]);
  Serial.println(b[1]);
  
  // store 16bit vars
  b.writeUInt16(65535, 2);
  Serial.println(b.readUInt16(2));
  
  b.writeUInt16(-25000, 2);
  Serial.println((int)b.readUInt16(2));


  //b.clear();
  
  b.writeUInt8(0xCA, 0);
  Serial.println(b.readUInt8(0), HEX);
  
  uint32_t l = 2000l * 1000l * 1000l; // two billion
  b.writeUInt32(l, 0);
  Serial.println(b.readUInt32(0));


  int32_t l2 = 2000l * 1000l * 1000 *-1l; // negative two billion
  b.writeUInt32(l2, 0);
  Serial.println((int32_t)b.readUInt32(0));
  
  
}

void loop() {
  
}
