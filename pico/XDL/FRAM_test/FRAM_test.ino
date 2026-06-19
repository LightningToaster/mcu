#include <Wire.h>

#define FM24_ADDR 0x50  // A0/A1/A2 all tied low = 0x50

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }

    Serial.println("\nFM24CL16B test");

    // pico2 default I2C0 pins:
    // SDA = GP0
    // SCL = GP1
    Wire.setSDA(0);
    Wire.setSCL(1);
    Wire.begin();

    // scan for device
    Serial.println("scanning i2c...");

    bool found = false;

    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);

        if (Wire.endTransmission() == 0) {
            Serial.print("found device at 0x");
            if (addr < 16) {
                Serial.print("0");
            }
            Serial.println(addr, HEX);

            if ((addr & 0xF8) == FM24_ADDR) {
                found = true;
            }
        }
    }

    if (!found) {
        Serial.println("fm24cl16b not found");
        return;
    }

    Serial.println("testing read/write...");

    uint16_t mem_addr = 0x0123;
    uint8_t write_value = 0x5A;

    // FM24CL16B uses:
    // upper memory bits in slave address
    // lower 8 bits as register address

    uint8_t device_addr = FM24_ADDR | ((mem_addr >> 8) & 0x07);
    uint8_t word_addr = mem_addr & 0xFF;

    // write byte
    Wire.beginTransmission(device_addr);
    Wire.write(word_addr);
    Wire.write(write_value);

    if (Wire.endTransmission() != 0) {
        Serial.println("write failed");
        return;
    }

    // read back
    Wire.beginTransmission(device_addr);
    Wire.write(word_addr);

    if (Wire.endTransmission(false) != 0) {
        Serial.println("address set failed");
        return;
    }

    Wire.requestFrom(device_addr, (uint8_t)1);

    if (Wire.available()) {
        uint8_t read_value = Wire.read();

        Serial.print("wrote: 0x");
        Serial.println(write_value, HEX);

        Serial.print("read:   0x");
        Serial.println(read_value, HEX);

        if (read_value == write_value) {
            Serial.println("PASS");
        } else {
            Serial.println("FAIL");
        }
    } else {
        Serial.println("read failed");
    }
}

void loop() {
}