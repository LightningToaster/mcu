#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "pico/multicore.h"
#include "logos.hpp"

enum DISPLAY_MODE : uint8_t {
	DISPLAY_LOGO,			// 0
  DISPLAY_AMMO,     // 1
  DISPLAY_EMPTY,    // 2
  DISPLAY_BATTERY, 	// 3
  DISPLAY_MENU,  		// 4
};

class OledDisplay {
public:
	OledDisplay(uint8_t sda_pin, uint8_t scl_pin)
	  : oled(128, 64, &Wire, -1),
		sda_pin(sda_pin),
		scl_pin(scl_pin),
		current_logo_index(0),
		last_frame_ms(0),
		display_mode(0),
		ammo(0),
		darts_fired_session(0),
		voltage(""),
		battery_status(0)
	{}

	bool begin(uint8_t i2c_addr = 0x3C) {
		Wire.setSDA(sda_pin);
		Wire.setSCL(scl_pin);
		Wire.begin();

		// check if device responds
		Wire.beginTransmission(i2c_addr);
		if (Wire.endTransmission() != 0) {
			Serial.println("OLED not detected");
			return false;// device not found
		}

		oled.begin(i2c_addr, true);
		oled.clearDisplay();
		oled.display();

		instance = this; // safe to set once hardware is ready
		multicore_launch_core1(core1_task);

		return true;//oled is connected
	}//begin

	void display_logo(uint8_t index) {
		current_logo_index = index;
		display_mode = DISPLAY_LOGO;
	}

	void display_ammo(uint16_t ammo, uint16_t darts_fired_session = 0) {
		this->ammo = ammo;
		this->darts_fired_session = darts_fired_session;
		display_mode = DISPLAY_AMMO;
	}

	void display_empty(String voltage) {
		this->voltage = voltage;
		display_mode = DISPLAY_EMPTY;
	}

	void display_battery(uint8_t battery_status, String voltage) {
		this->battery_status = battery_status;
		this->voltage = voltage;
		display_mode = DISPLAY_BATTERY;
	}

	void display_menu() {
		display_logo(0);//TEMP
		//TODO
	}

private:
	Adafruit_SH1106G oled;
	uint8_t sda_pin;
	uint8_t scl_pin;
	uint8_t current_logo_index;

	uint8_t display_mode;
	uint16_t ammo;
	uint16_t darts_fired_session;
	String voltage;
	uint8_t battery_status;

	static OledDisplay* instance;

	unsigned long last_frame_ms;
	static constexpr unsigned long FRAME_INTERVAL_MS = 33;

	static void core1_task() {
		if (instance) {
			instance->core1_loop();
		}
	}//core1_task

	void core1_loop() {
		while (true) {
			unsigned long now = millis();
			if (now - last_frame_ms < FRAME_INTERVAL_MS) {
				delay(1);
				continue;
			}
			last_frame_ms = now;

			oled.clearDisplay();
			switch(display_mode){
				case DISPLAY_LOGO:draw_logo();break;
				case DISPLAY_AMMO:draw_ammo();break;
				case DISPLAY_EMPTY:draw_empty();break;
				case DISPLAY_BATTERY:draw_battery();break;
				//case DISPLAY_MENU:draw_menu();break;
			}
			oled.display();
		}
	}//core1_loop

	void draw_logo(){
		// const logo_t &logo = logos[current_logo_index];
		// 		oled.drawBitmap(logo.x, logo.y, logo.bitmap,
		// 		logo.width, logo.height,
		// 		SH110X_WHITE);


		oled.setTextColor(SH110X_WHITE);
		oled.setTextSize(1);
		oled.setCursor(27, 0); 
		oled.print("XDL Vanquish");
		oled.setCursor(12, 15);
		oled.print("#0001 v2025.11.2");
		oled.setCursor(8, 30);
		oled.print("Designed by Valkor");

		oled.display();
	}//draw_logo

	void draw_ammo(){
		if (ammo <= 9) {
			oled.setTextSize(9);
			oled.setCursor(46, 0);

		} else if (ammo <= 99){
			oled.setTextSize(8);
			oled.setCursor(22, 5);

		}else if (ammo <= 999){
			oled.setTextSize(7);
			oled.setCursor(2, 7);

		}else if (ammo <= 9999){
			oled.setTextSize(5);
			oled.setCursor(5, 15);
		}else{//shouldnt happen outside testing
			oled.setTextSize(2);
			oled.setCursor(0, 0);
		}
		oled.setTextColor(SH110X_WHITE);
		oled.print(ammo);

		
	}//display_ammo

	void draw_empty(){
		oled.setTextColor(SH110X_WHITE);
		oled.setTextSize(4);
		oled.setCursor(7, 15);
		oled.print("EMPTY");
		oled.setTextSize(1);
		oled.setCursor(50, 54);
		oled.print(voltage);

		//TEMP
		if (darts_fired_session > 0){
			oled.setCursor(0, 54);
			oled.setTextSize(1);
			oled.print(darts_fired_session);
		}
	}//display_empty

	void draw_battery(){
		oled.setTextColor(SH110X_WHITE);
		
		switch(battery_status){
			case BATTERY_DISCONNECTED:
				oled.setTextSize(1);
				oled.setCursor(45, 5);
				oled.print("BATTERY");
				oled.setCursor(30, 18);
				oled.print("DISCONNECTED");
			break;
			case BATTERY_LOW:
				
				oled.setTextSize(2);
				oled.setCursor(25, 5);
				oled.print("BATTERY");
				oled.setCursor(47, 24);
				oled.print("LOW");
				oled.setTextSize(1);
				oled.setCursor(50, 54);
				oled.print(voltage);
			break;
			case GOOD:
				oled.setTextSize(2);
				oled.setCursor(39, 25);
				oled.print(voltage);
			break;
			case BATTERY_OVERCHARGED:
				oled.setTextSize(3);
				oled.setCursor(2, 0);
				oled.print("DANGER!");
				oled.setTextSize(1);
				oled.setCursor(13, 28);
				oled.print("EXCESIVE VOLTAGE");
				oled.setCursor(50, 54);
				oled.print(voltage);
			break;
			
		}
	}//draw_battery

	//TODO draw_menu()
	
	

};

// static instance definition
OledDisplay* OledDisplay::instance = nullptr;