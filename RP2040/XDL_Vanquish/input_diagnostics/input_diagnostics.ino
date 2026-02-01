#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define PIN_TRIGGER			2
#define PIN_TRIGGER_B		1
#define PIN_TRIGGER_REV		3
#define PIN_SWITCH			14
#define PIN_VOLTMETER		29
#define PIN_DART_DETECTOR	28

#define PIN_OLED_SDA		8
#define PIN_OLED_SCL		9

Adafruit_SH1106G oled(128, 64, &Wire);// sh1106 128x64

void setup() {
  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_TRIGGER_B, INPUT_PULLUP);
  pinMode(PIN_TRIGGER_REV, INPUT_PULLUP);
  pinMode(PIN_SWITCH, INPUT_PULLUP);

	pinMode(PIN_DART_DETECTOR, INPUT);
	pinMode(PIN_VOLTMETER, INPUT);

	Wire.setSDA(PIN_OLED_SDA);
	Wire.setSCL(PIN_OLED_SCL);
	oled.begin(0x3C, true);
	oled.clearDisplay();
	oled.display();
}

void loop() {
	oled.clearDisplay();
	oled.setTextColor(SH110X_WHITE);
	oled.setTextSize(1);
	oled.setCursor(0, 0);

	oled.print("TA: "); oled.println(!digitalRead(PIN_TRIGGER));
	oled.print("TB: "); oled.println(!digitalRead(PIN_TRIGGER_B));
	oled.print("rev: "); oled.println(!digitalRead(PIN_TRIGGER_REV));
	oled.print("sw: "); oled.println(!digitalRead(PIN_SWITCH));
	oled.print("dart: "); oled.println(analogRead(PIN_DART_DETECTOR));
	oled.print("vraw: "); oled.println(analogRead(PIN_VOLTMETER));

	oled.display();
	delay(100);
}
