#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "pico/multicore.h"
#include "pico/time.h"

class Display {
public:
	Display(uint8_t sda_pin, uint8_t scl_pin)
	  : oled(128, 64, &Wire, -1),
	    sda_pin(sda_pin),
	    scl_pin(scl_pin),
		adc_value(0),
	    adc_channel(1),
    	flicker_count(0),
		seconds(0),
		minutes(0),
		hours(0)
	{}

	bool begin(uint8_t i2c_addr = 0x3C) {
		Wire.setSDA(sda_pin);
		Wire.setSCL(scl_pin);
		Wire.begin();

		Wire.beginTransmission(i2c_addr);
		if (Wire.endTransmission() != 0) {
			Serial.println("oled not detected");
			return false;
		}

		oled.begin(SSD1306_SWITCHCAPVCC, i2c_addr);
		oled.clearDisplay();
		oled.setTextSize(1);
		oled.setTextColor(SSD1306_WHITE);
		oled.setCursor(0, 0);
		oled.display();

		instance = this;
		multicore_launch_core1(core1_task);

		return true;
	}

	// core0 setters
	void set_adc_value(uint16_t value) { adc_value = value; }
	void set_adc_channel(uint16_t channel) { adc_channel = channel; }
	void set_flicker(uint16_t count) { flicker_count = count; }
	void set_seconds(uint8_t seconds) { this->seconds = seconds; }
	void set_minutes(uint8_t minutes) { this->minutes = minutes; }
	void set_hours(uint16_t hours) { this->hours = hours; }
	void set_USB(bool USB) { this->USB = USB; }

	void set_battery(const char* text) {
		strncpy(battery, text, sizeof(battery)-1);
		battery[sizeof(battery)-1] = '\0';
	}

private:
	Adafruit_SSD1306 oled;
	uint8_t sda_pin;
	uint8_t scl_pin;

	volatile uint16_t adc_value;
	volatile uint16_t adc_channel;
	volatile uint16_t flicker_count;
	volatile uint8_t seconds;
	volatile uint8_t minutes;
	volatile uint16_t hours;

	volatile bool USB = false;
	char battery[8];

	static Display* instance;
	static constexpr uint32_t frame_interval_ms = 50;

	static void core1_task() {
		if (instance) instance->core1_loop();
	}

	void core1_loop() {
		uint32_t last_frame = 0;

		while (true) {
			// check for pause request
			if (core1_pause_request.load())
			{
					core1_paused.store(true);

					// wait until core0 clears request
					while (core1_pause_request.load())
					{
							delay(4);
					}
					delay(5);
					core1_paused.store(false);
			}


			uint32_t now = to_ms_since_boot(get_absolute_time());
			if (now - last_frame < frame_interval_ms) {
				sleep_ms(1);
				continue;
			}
			last_frame = now;

			oled.clearDisplay();

			oled.setTextSize(1);
			oled.setCursor(102,0);
			oled.print(battery);

			oled.setCursor(0,0);
			oled.print("A");
			oled.print(adc_channel);
			oled.print("=");
			oled.print(adc_value);

			oled.setCursor(0,56);

			if (hours > 0){
				if (hours <= 9) oled.print("0");
				oled.print(hours);
				oled.print("h");
			}

			if (minutes > 0 || hours > 0){
				if (minutes <= 9) oled.print("0");
				oled.print(minutes);
				oled.print("m");
			}

			if (seconds <= 9) oled.print("0");
			oled.print(seconds);
			oled.print("s");

			if (USB){
				oled.setCursor(110,56);
				oled.print("USB");
			}

			if (flicker_count > 0){
				if (flicker_count <= 9){
					oled.setTextSize(4);
					oled.setCursor(55,15);
				}else if (flicker_count <= 99){
					oled.setTextSize(4);
					oled.setCursor(42,18);
				}else if (flicker_count <= 999){
					oled.setTextSize(3);
					oled.setCursor(39,20);
				}else{
					oled.setTextSize(3);
					oled.setCursor(31,20);
				}

				oled.print(flicker_count);
			}

			oled.display();
		}
	}
};

Display* Display::instance = nullptr;