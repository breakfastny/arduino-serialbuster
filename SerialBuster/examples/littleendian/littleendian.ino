#include "SerialBuster.h"
#include "Buffer.h"

Buffer b = Buffer();

void setup() {
  //ser.init(9600);
  Serial.begin(9600);
  Serial.println("init");
  b.init(16);
  
  delay(100);
  
  b.writeUInt16(12345, 0);
  
  int i = b[1] << 8 | b[0];
  
  Serial.print("12345 == ");
  Serial.println(i);
  
  Serial.print("12345 == ");
  Serial.println(b.readUInt16(0));

}

void loop() {
  
}
