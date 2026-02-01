#include <Arduino.h>
#include "oled.hpp"

#define PIN_SDA 8
#define PIN_SCL 9

OledDisplay display(PIN_SDA, PIN_SCL);
bool oled_connected = false;

void setup() {
	Serial.begin(115200);
	delay(500);
	Serial.println("starting oled test...");

	oled_connected = display.begin(0x3C);
	delay(1000);
}

void loop() {
	if (oled_connected){
		display.display_logo(0);
		Serial.println("oled is connected");
	}else{
		Serial.println("oled not detected");
	}
	delay(2000);

	// display.display_ammo(8);
	// delay(1000);
  // display.display_ammo(88);
	// delay(1000);
  // display.display_ammo(888);
	// delay(1000);

	// display.display_empty("15.8v");
	// delay(4000);

	// display.display_battery(1, "15.9v");
	// delay(3000);

	// display.display_battery(2, "14.0v");
	// delay(4000);

	// display.display_battery(3, "17.1v");
	// delay(3000);
}
