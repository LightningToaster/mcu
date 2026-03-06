#define OLED_PIN_SDA 0
#define OLED_PIN_SCL 1

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pico/multicore.h"
//#include "logos.hpp"

class OledDisplay {
public:
    OledDisplay()
      : oled(128, 64, &Wire, -1)  // SSD1306 default: 128x64, reset pin = -1
    {
        text[0] = '\0';  // init empty string
        instance = this;
    }

    void begin(uint8_t i2c_addr = 0x3C) {
        Wire.setSDA(OLED_PIN_SDA);
        Wire.setSCL(OLED_PIN_SCL);
        if (!oled.begin(SSD1306_SWITCHCAPVCC, i2c_addr)) {
            for(;;); // loop forever if init fails
        }
        oled.clearDisplay();
        oled.display();
        multicore_launch_core1(core1_task);
    }

    void display_text(const char* msg) {
        strncpy(text, msg, sizeof(text) - 1);
        text[sizeof(text) - 1] = '\0';
    }

private:
    Adafruit_SSD1306 oled;
    char text[64];
    static OledDisplay* instance;

    static void core1_task() {
        if (!instance) return;
        instance->core1_loop();
    }

    void core1_loop() {
        while (true) {
            oled.clearDisplay();

          
          
            oled.setTextSize(2);
            oled.setTextColor(SSD1306_WHITE);
            int16_t x = 20;
            int16_t y = 0;
            oled.setCursor(x, y);

            for (const char *p = text; *p; p++) {
                if (*p == '\n') {
                    y += 22;
                    oled.setCursor(x, y);
                } else {
                    oled.write(*p);
                }
            }
            

            oled.display();
        }
    }
};

// --- static instance definition ---
OledDisplay* OledDisplay::instance = nullptr;
