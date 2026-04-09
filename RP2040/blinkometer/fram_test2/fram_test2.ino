//USE THIS ONE, IT ACTUALLY PASSES.  seemed to be a testing issue that
//caused it to not work on same i2c pins as oled, so put them back onto those.

#include <Wire.h>

#define PIN_SDA 4
#define PIN_SCL 5
#define FRAM_ADDR 0x50

struct DataBlock {
  uint32_t a;
  uint32_t b;
};

void framWrite(uint16_t addr, const uint8_t *data, uint8_t len) {
  Wire.beginTransmission(FRAM_ADDR);
  Wire.write((addr >> 8) & 0xFF);
  Wire.write(addr & 0xFF);

  for (uint8_t i = 0; i < len; i++) {
    Wire.write(data[i]);
  }

  Wire.endTransmission();
}

void framRead(uint16_t addr, uint8_t *data, uint8_t len) {
  Wire.beginTransmission(FRAM_ADDR);
  Wire.write((addr >> 8) & 0xFF);
  Wire.write(addr & 0xFF);
  Wire.endTransmission(false);

  Wire.requestFrom(FRAM_ADDR, len);

  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    data[i] = Wire.read();
  }
}

void setup() {
  Serial.begin(115200);
  delay(5000);

  Wire.setSDA(PIN_SDA);
  Wire.setSCL(PIN_SCL);
  Wire.begin();
  Wire.setClock(100000);

  DataBlock out;
  out.a = 0x12345678;
  out.b = 0xAABBCCDD;

  // write struct
  framWrite(0x0000, (uint8_t*)&out, sizeof(out));

  delay(5); // small safety margin

  // read back
  DataBlock in;
  framRead(0x0000, (uint8_t*)&in, sizeof(in));

  Serial.println("FRAM struct test:");

  Serial.print("A: 0x");
  Serial.println(in.a, HEX);

  Serial.print("B: 0x");
  Serial.println(in.b, HEX);

  if (in.a == out.a && in.b == out.b) {
    Serial.println("PASS");
  } else {
    Serial.println("FAIL");
  }
}

void loop() {}