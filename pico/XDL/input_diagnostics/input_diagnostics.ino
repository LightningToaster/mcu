//valkor 2026-6-17 (MK2)
//uv turns on briefly at startup if batt connected
//beeps and changes LED based on trigger inputs
//shows all inputs on oled if connected
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define PIN_MENU			5
#define PIN_SHALLOW			3
#define PIN_DEEP		2
#define PIN_TOUCH		4
#define PIN_BUZZER 6 //passive, tone()
#define PIN_UV 14
#define PIN_VOLTMETER		27
#define PIN_DART_DETECTOR	29

#define PIN_OLED_SDA		0
#define PIN_OLED_SCL		1

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(1, 25, NEO_GRB + NEO_KHZ800);

Adafruit_SH1106G oled(128, 64, &Wire);// sh1106 128x64

bool oled_connected = false;

void setup() {
	Serial.begin(115200);
	delay(1000);
	Serial.println("starting...");
  pinMode(PIN_SHALLOW, INPUT_PULLUP);
  pinMode(PIN_DEEP, INPUT_PULLUP);
  pinMode(PIN_TOUCH, INPUT_PULLUP);
  pinMode(PIN_MENU, INPUT_PULLUP);
	pinMode(PIN_DART_DETECTOR, INPUT);
	pinMode(PIN_VOLTMETER, INPUT);
	analogWrite(PIN_UV, 0);//make sure uv is off

	strip.begin();
  strip.clear();
  strip.show();

	Wire.setSDA(PIN_OLED_SDA);
	Wire.setSCL(PIN_OLED_SCL);
	oled_connected = oled.begin(0x3C, true);
	if (oled_connected){
		Serial.println("oled is connected");
		oled.clearDisplay();
		oled.display();
	}else{
		Serial.println("oled not detected");
	}

}//setup

void loop() {
	uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;

if (digitalRead(PIN_MENU) == LOW) {
    r = 12;
  }

  if (digitalRead(PIN_DEEP) == LOW) {
    g = 9;
    b = 9;
    tone(PIN_BUZZER, 900);
  }
  else if (digitalRead(PIN_SHALLOW) == LOW) {
    b = 9;
    tone(PIN_BUZZER, 500);
  }
  else if (digitalRead(PIN_TOUCH) == HIGH) {
    g = 9;
    tone(PIN_BUZZER, 100);
  }
  else {
    noTone(PIN_BUZZER);
  }

  strip.setPixelColor(0, r, g, b);
  strip.show();

	if (oled_connected){
		do_oled();
	}
	
	delay(10);
}

void do_oled(){
	oled.clearDisplay();
	oled.setTextColor(SH110X_WHITE);
	oled.setTextSize(1);


	oled.setCursor(0, 0); 
	oled.print("BLASTOMATIC MK2");
	oled.setCursor(0, 10);
	oled.print("#0002 v2026.6.17");
	oled.setCursor(0, 20);
	oled.print("-valkor");

	oled.setCursor(0, 30);
	oled.print("trigger: "); oled.print(digitalRead(PIN_TOUCH));
	oled.print(!digitalRead(PIN_SHALLOW));
	oled.println(!digitalRead(PIN_DEEP));
	oled.print("menu: "); oled.println(!digitalRead(PIN_MENU));
	oled.print("dart: "); oled.println(analogRead(PIN_DART_DETECTOR));
	oled.print("vraw: "); oled.println(analogRead(PIN_VOLTMETER));

	oled.display();
}//do_oled


