#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9

#include <Wire.h>
#include "oled.hpp"

Display oled(PIN_OLED_SDA, PIN_OLED_SCL);

// ===== FRAM CONFIG =====
#define FRAM_ADDR 0x50   // adjust if needed
#define FRAM_MEM_ADDR 0x0000  // where we store millis()

// timing
static uint32_t last_stopwatch_ms = 0;
static uint32_t stopwatch_seconds = 0;
static uint32_t last_fram_write = 0;
uint32_t fram_stored_value = 0;

// ===== FRAM HELPERS =====
void framWrite32(uint16_t addr, uint32_t value) {
    Wire.beginTransmission(FRAM_ADDR);
    Wire.write((addr >> 8) & 0xFF);   // high byte
    Wire.write(addr & 0xFF);          // low byte

    Wire.write((value >> 24) & 0xFF);
    Wire.write((value >> 16) & 0xFF);
    Wire.write((value >> 8) & 0xFF);
    Wire.write(value & 0xFF);

    Wire.endTransmission();
}

uint32_t framRead32(uint16_t addr) {
    Wire.beginTransmission(FRAM_ADDR);
    Wire.write((addr >> 8) & 0xFF);
    Wire.write(addr & 0xFF);
    Wire.endTransmission();

    Wire.requestFrom(FRAM_ADDR, 4);

    uint32_t value = 0;
    for (int i = 0; i < 4 && Wire.available(); i++) {
        value = (value << 8) | Wire.read();
    }
    return value;
}

// ===== SETUP =====
void setup() {
    Serial.begin(9600);

    oled.begin();

   fram_stored_value = framRead32(FRAM_MEM_ADDR);
    Serial.print("FRAM stored millis: ");
    Serial.println(fram_stored_value);
}

// ===== LOOP =====
void loop() {
    uint32_t now = millis();

    // Stopwatch logic
    if (now - last_stopwatch_ms >= 1000) {
        last_stopwatch_ms += 1000;
        stopwatch_seconds++;
    }

    // Write millis to FRAM every 100ms
    if (now - last_fram_write >= 100) {
        last_fram_write += 100;
        framWrite32(FRAM_MEM_ADDR, now);
        fram_stored_value = now;
    }

    uint8_t seconds = stopwatch_seconds % 60;
    uint8_t minutes = (stopwatch_seconds / 60) % 60;
    uint16_t hours = stopwatch_seconds / 3600;

    oled.set_adc_value(fram_stored_value);
    oled.set_battery("BATT");
    oled.set_seconds(seconds);
    oled.set_minutes(minutes);
    oled.set_hours(hours);
    oled.set_USB("USB_con");

    oled.update();
}