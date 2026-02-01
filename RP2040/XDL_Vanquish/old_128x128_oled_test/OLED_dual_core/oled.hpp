#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "pico/multicore.h"
#include "logos.hpp"

class OledDisplay {
public:
	OledDisplay(uint8_t sda_pin, uint8_t scl_pin)
	  : oled(128, 128, &Wire, -1, 400000, 100000),
		sda_pin(sda_pin),
		scl_pin(scl_pin),
		current_logo_index(0),
		logo_mode(false),
		new_text_ready(false),
		last_frame_ms(0)
	{
		text_write_buf[0] = '\0';
		text_render_buf[0] = '\0';
	}

	void begin(uint8_t i2c_addr = 0x3C) {
		Wire.setSDA(sda_pin);
		Wire.setSCL(scl_pin);
		oled.begin(i2c_addr, true);
		oled.clearDisplay();
		oled.display();

		instance = this; // safe to set once hardware is ready
		multicore_launch_core1(core1_task);
	}

	void display_logo(uint8_t index) {
		current_logo_index = index;
		logo_mode = true;
	}

	void display_text(const char* msg) {
		strncpy(text_write_buf, msg, sizeof(text_write_buf) - 1);
		text_write_buf[sizeof(text_write_buf) - 1] = '\0';
		new_text_ready = true;
		logo_mode = false;
	}

private:
	Adafruit_SH1107 oled;
	uint8_t sda_pin;
	uint8_t scl_pin;
	uint8_t current_logo_index;

	char text_write_buf[64];
	char text_render_buf[64];
	volatile bool new_text_ready;
	volatile bool logo_mode;

	static OledDisplay* instance;

	unsigned long last_frame_ms;
	static constexpr unsigned long FRAME_INTERVAL_MS = 33;

	static void core1_task() {
		if (instance) {
			instance->core1_loop();
		}
	}

	void core1_loop() {
		while (true) {
			unsigned long now = millis();
			if (now - last_frame_ms < FRAME_INTERVAL_MS) {
				delay(1);
				continue;
			}
			last_frame_ms = now;

			if (new_text_ready) {
				strncpy(text_render_buf, text_write_buf, sizeof(text_render_buf));
				new_text_ready = false;
			}

			oled.clearDisplay();

			if (logo_mode) {
				const logo_t &logo = logos[current_logo_index];
				oled.drawBitmap(logo.x, logo.y, logo.bitmap,
								logo.width, logo.height,
								SH110X_WHITE);

			} else {
				oled.setTextSize(1);
				oled.setTextColor(SH110X_WHITE);
				int16_t x = 2, y = 2;
				oled.setCursor(x, y);

				for (const char *p = text_render_buf; *p; p++) {
					if (*p == '\n') {
						y += 10;
						oled.setCursor(x, y);
					} else {
						oled.write(*p);
					}
				}
			}

			oled.display();
		}
	}
};

// static instance definition
OledDisplay* OledDisplay::instance = nullptr;