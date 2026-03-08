#pragma once
#include <Adafruit_NeoPXL8.h>

#define BRIGHTNESS_MULTIPLIER 1
#define COLOR_CHANNELS_PER_PIXEL 3
#define TRANSITION_DELAY 10

const uint8_t POWERS[32] = {
	0,1,2,3,4,5,6,7,8,10,
	12,14,16,18,20,
	25,30,35,40,50,
	60,70,80,90,110,
	130,150,170,190,210,
	230,255
};

const uint8_t COLORS[][COLOR_CHANNELS_PER_PIXEL] = {
	{31,31,31}, {31,0,0}, {0,31,0}, {0,0,31},
	{31,31,0}, {31,0,31}, {0,31,31}, {31,18,0}, {18,0,31}
};



class Strip {
public:
	Strip(uint8_t pin, uint16_t num_pixels)
	: num_pixels(num_pixels)
	{
		// initialize goals to 0
		for (uint16_t i = 0; i < num_pixels; i++){
			for (uint8_t j = 0; j < COLOR_CHANNELS_PER_PIXEL; j++){
				goals[i][j] = 0;
			}
		}

		int8_t pins[1] = { static_cast<int8_t>(pin) };
		strip = new Adafruit_NeoPXL8(num_pixels, pins, NEO_GRB + NEO_KHZ800);
	}

	void begin() {
		strip->begin();
		strip->show();
		setup_complete = true;
	}

	uint16_t operate(bool wait_for_completion = false){
		if (!setup_complete) begin();

		uint16_t status = 0;

		if (millis() - last_update_ms > TRANSITION_DELAY){
			for (uint16_t i=0; i<num_pixels; i++){
				uint32_t c = strip->getPixelColor(i);
				uint8_t r = (c >> 16) & 0xFF;
				uint8_t g = (c >> 8) & 0xFF;
				uint8_t b = c & 0xFF;

				r = find_power_index(r);
				g = find_power_index(g);
				b = find_power_index(b);

				if (r < goals[i][0]) r++; else if (r > goals[i][0]) r--;
				if (g < goals[i][1]) g++; else if (g > goals[i][1]) g--;
				if (b < goals[i][2]) b++; else if (b > goals[i][2]) b--;

				if (r != goals[i][0] || g != goals[i][1] || b != goals[i][2])
					status++;

				strip->setPixelColor(i, POWERS[r], POWERS[g], POWERS[b]);
			}
			last_update_ms = millis();
			strip->show();
		}

		if (status > 0 && wait_for_completion)
			return operate(true);

		return status;
	}

	void set_all(uint8_t r, uint8_t g, uint8_t b){
		for (uint16_t i=0; i<num_pixels; i++){
			set(i, r, g, b);
		}
	}

	void set(uint16_t led, uint8_t r, uint8_t g, uint8_t b){
		if (led>=num_pixels) led %= num_pixels;
		goals[led][0] = r * BRIGHTNESS_MULTIPLIER;
		goals[led][1] = g * BRIGHTNESS_MULTIPLIER;
		goals[led][2] = b * BRIGHTNESS_MULTIPLIER;
	}

private:
	Adafruit_NeoPXL8* strip;
	uint16_t goals[576][COLOR_CHANNELS_PER_PIXEL]; // max 576 LEDs
	bool setup_complete = false;
	unsigned long last_update_ms = 0;
	uint16_t num_pixels;

	uint8_t find_power_index(uint8_t value){
		for (uint8_t i=0; i<32; i++){
			if (POWERS[i] >= value) return i;
		}
		return 31; // max index
	}
};
