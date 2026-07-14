#pragma once

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

const uint8_t COLORS[][3] = {
	{31,31,31}, {31,0,0}, {0,31,0}, {0,0,31},
	{31,31,0}, {31,0,31}, {0,31,31}, {31,18,0}, {18,0,31}
};

//TODO include a gamma correction table because brightness values like 254 and 255 are basically the same

//we'll need these somwhere
//#include <Adafruit_NeoPixel.h>
//Adafruit_NeoPixel strip(1, pin, NEO_GRB + NEO_KHZ800);

class LED {
public:
    LED(uint8_t pin = 25)//25 by default
        : pin(pin),
    {
        
    }

    void operate() {
        //called continously from loop()
    }

    void set(r,g,b, ms = 0){
        //when this is called, the led will be gradually set to the r,g,b value over the time
        //over the time indicated by ms.  0 ms is default and means instant
    }

    

private:
    uint8_t pin;
    //neopixel 
	uint16_t goals[3];
	unsigned long last_update_ms = 0;
	uint16_t num_pixels;

	uint8_t find_power_index(uint8_t value){
		for (uint8_t i=0; i<32; i++){
			if (POWERS[i] >= value) return i;
		}
		return 31; // max index
	}

    
};//LED