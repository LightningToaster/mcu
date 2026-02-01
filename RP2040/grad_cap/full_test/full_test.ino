#include <Adafruit_NeoPXL8.h>

#define PIN_LED_STRIP 15
#define NUM_PIXELS 578
#define BATCH_SIZE 128   // update in batches to avoid blocking USB
#define DELAY_MS 10      // time between LED updates

int8_t pins[8] = {PIN_LED_STRIP, -1, -1, -1, -1, -1, -1, -1};
Adafruit_NeoPXL8 strip(NUM_PIXELS, pins, NEO_GRB + NEO_KHZ800);

void setup() {
	strip.begin();
	strip.show(); // clear first
}

// simple palette to cycle through
uint8_t COLORS[][3] = {
	{9,0,0}, {0,9,0}, {0,0,9}, {9,9,0},
	{9,0,9}, {0,9,9}, {9,9,9}
};

void loop() {
	for (uint16_t i = 0; i < NUM_PIXELS; i++) {
		// pick a color based on LED index for variety
		uint8_t color_index = i % 7;
		strip.setPixelColor(i, COLORS[color_index][0], COLORS[color_index][1], COLORS[color_index][2]);

		// update in batches
		if ((i % BATCH_SIZE == BATCH_SIZE - 1) || i == NUM_PIXELS - 1) {
			strip.show();       // write the batch to LEDs
			delay(DELAY_MS);    // give USB time to respond
		}
	}
}
