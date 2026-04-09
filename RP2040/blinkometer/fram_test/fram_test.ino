#include <Wire.h>

#define PIN_SDA 4
#define PIN_SCL 5

#define FRAM_ADDR 0x50

void framWriteByte(uint16_t memAddr, uint8_t data) {
  Wire.beginTransmission(FRAM_ADDR);
  Wire.write((memAddr >> 8) & 0xFF);  // high byte address
  Wire.write(memAddr & 0xFF);         // low byte address
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t framReadByte(uint16_t memAddr) {
  Wire.beginTransmission(FRAM_ADDR);
  Wire.write((memAddr >> 8) & 0xFF);
  Wire.write(memAddr & 0xFF);
  Wire.endTransmission();

  Wire.requestFrom(FRAM_ADDR, 1);
  if (Wire.available()) return Wire.read();
  return 0xFF;
}

void setup() {
  Serial.begin(115200);
  delay(5000);

  Wire.setSDA(PIN_SDA);
Wire.setSCL(PIN_SCL);
Wire.begin();
Wire.setClock(100000);

  Serial.println("FRAM test start");

  // write test pattern
  for (uint16_t i = 0; i < 256; i++) {
    framWriteByte(i, (uint8_t)(i & 0xFF));
  }

  Serial.println("Write complete, verifying...");

  // verify
  int errors = 0;
  for (uint16_t i = 0; i < 256; i++) {
    uint8_t val = framReadByte(i);
    if (val != (uint8_t)(i & 0xFF)) {
      Serial.print("Mismatch at ");
      Serial.print(i);
      Serial.print(" got ");
      Serial.println(val);
      errors++;
    }
  }

  Serial.print("Done. Errors: ");
  Serial.println(errors);
}

void loop() {
  // optional continuous test: toggle first byte
  //static uint8_t v = 0;
  //framWriteByte(0, v++);
  Serial.println("loop");
  delay(1000);
  
}