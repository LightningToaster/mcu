#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <pico/multicore.h>

#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

#define SAVE_INTERVAL_MS 1000

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

struct saved_data_t {
    uint32_t flicker_count;
    uint16_t hours;
    uint8_t minutes;
    uint32_t crc;
};

saved_data_t saved_data;

uint32_t last_save_ms = 0;

uint32_t crc32(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    while (len--) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
        }
    }
    return ~crc;
}

// core1 program
void core1_main() {

    Wire.setSDA(PIN_OLED_SDA);
    Wire.setSCL(PIN_OLED_SCL);
    Wire.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        while (true) {
            // oled init failed
        }
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    while (true) {

        uint32_t now = millis();

        // update oled
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("ms2:");
        display.setCursor(0,24);
        display.print(now);
        display.display();

        // save once per second
        if (now - last_save_ms >= SAVE_INTERVAL_MS) {

            saved_data.flicker_count++;
            saved_data.hours = now / 3600000;
            saved_data.minutes = (now / 60000) % 60;

            saved_data.crc = crc32(
                (uint8_t*)&saved_data,
                sizeof(saved_data_t) - sizeof(uint32_t)
            );

            EEPROM.put(0, saved_data);
            EEPROM.commit();

            last_save_ms = now;

            Serial.println("saved");
        }

        delay(50);
    }
}

void setup() {
    Serial.begin(115200);

    EEPROM.begin(sizeof(saved_data_t));

    multicore_launch_core1(core1_main);
}

void loop() {
    // core0 idle
    delay(1000);
}