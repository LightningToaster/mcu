#define PIN_UV 4
#include "led.hpp"
Led uv_led(PIN_UV);

void setup() {
	Serial.begin(9600);
	while (!Serial && millis() < 200);
	Serial.println(F("LED Brightness Control (0–31)"));
	Serial.print(F("> "));
}

void loop() {
	if (Serial.available()) {
		String input = Serial.readStringUntil('\n');
		input.trim();

		if (input.length() > 0) {
			int value = input.toInt();

			if (value >= 0 && value <= 31) {
				uv_led.set_goal(value);
				Serial.print(F("Setting brightness to "));
				Serial.println(value);
			} else {
				Serial.println(F("Invalid value. Enter a number between 0 and 31."));
			}
		}
		Serial.print(F("> "));
	}

	uv_led.operate();
}
