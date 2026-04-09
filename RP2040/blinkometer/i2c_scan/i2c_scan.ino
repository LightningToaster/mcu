#include <Wire.h>

#define PIN_SDA 4
#define PIN_SCL 5

void setup() {
  delay(5000);
  Serial.begin(115200);

  Wire.setSDA(PIN_SDA);
  Wire.setSCL(PIN_SCL);
  Wire.begin();

  Serial.println("Scanning...");

  for (int addr = 1; addr < 127; addr++) {
  int hits = 0;

    for (int i = 0; i < 3; i++) {
      Wire.beginTransmission(addr);
      if (Wire.endTransmission() == 0) hits++;
    }

    if (hits == 3) {
      Serial.print("Stable device at 0x");
      Serial.println(addr, HEX);
    }
  }
}

void loop() {}

