#include <Adafruit_NeoPXL8.h>

#define PIN_LED_STRIP 15
#define NUM_PIXELS 578
#define FADE_FACTOR 0.97f
#define RANDOM_INTERVAL_MS 100

int8_t pins[8] = { PIN_LED_STRIP, -1, -1, -1, -1, -1, -1, -1 };
Adafruit_NeoPXL8 strip(NUM_PIXELS, pins, NEO_GRB + NEO_KHZ800);

unsigned long last_random_ms = 0;

void setup() {
	strip.begin();
	strip.show();
	randomSeed(analogRead(0));
}

void loop() {
	unsigned long now = millis();

	// pick random led occasionally
	if (now - last_random_ms >= RANDOM_INTERVAL_MS) {
		last_random_ms = now;

		uint16_t i = random(NUM_PIXELS);
		strip.setPixelColor(i, 0, 255, 255);	// bright white sparkle
	}

	// fade all leds
	for (uint16_t i = 0; i < NUM_PIXELS; i++) {
		uint32_t c = strip.getPixelColor(i);

		uint8_t r = (uint8_t)((c >> 16) - FADE_FACTOR);
		uint8_t g = (uint8_t)((c >> 8)  * FADE_FACTOR);
		uint8_t b = (uint8_t)(c * FADE_FACTOR);

		strip.setPixelColor(i, r, g, b);
	}

	strip.show();
}
