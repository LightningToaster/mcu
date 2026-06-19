//valkor 2025-11-23
#include <Adafruit_NeoPXL8.h>

#define PIN_LED_STRIP 15
#define NUM_PIXELS 576
#define FADE_FACTOR 0.99f
#define RANDOM_INTERVAL_MS 20

#define PIN_BUTTON 0
#include "step_button.hpp"
StepButton button(PIN_BUTTON);

int8_t pins[8] = { PIN_LED_STRIP, -1, -1, -1, -1, -1, -1, -1 };
Adafruit_NeoPXL8 strip(NUM_PIXELS, pins, NEO_GRB + NEO_KHZ800);

// keep your own copy of pixel data
uint8_t r_buf[NUM_PIXELS];
uint8_t g_buf[NUM_PIXELS];
uint8_t b_buf[NUM_PIXELS];

unsigned long last_random_ms = 0;

// effect colors (rgb)
const uint8_t effect_colors[][3] = {
    {20, 40, 40},  // white
    {20, 40, 40},  // white
    {20, 40, 40},  // white
    {20, 40, 40},  // white
    {20, 40, 40},  // white
    {0,  50, 255},  // blue
    {0,  255, 50}    // green
};
const uint8_t NUM_EFFECT_COLORS = sizeof(effect_colors) / 3;

bool is_enabled = false;

void setup() {
    strip.begin();
    strip.show();
    memset(r_buf, 0, NUM_PIXELS);
    memset(g_buf, 0, NUM_PIXELS);
    memset(b_buf, 0, NUM_PIXELS);
    randomSeed(analogRead(0));
}

void loop() {
    unsigned long now = millis();

	int8_t status_button = button.operate();
	if (status_button == BUTTON_CLICK){
		is_enabled = !is_enabled;

	}else if(status_button == BUTTON_HOLD){
        
    }
    // occasionally inject a sparkle
if (is_enabled and now - last_random_ms >= RANDOM_INTERVAL_MS) {
    last_random_ms = now;

    uint16_t i = random(NUM_PIXELS);

    // pick a random color from table
    uint8_t ci = random(NUM_EFFECT_COLORS);
    r_buf[i] = effect_colors[ci][0];
    g_buf[i] = effect_colors[ci][1];
    b_buf[i] = effect_colors[ci][2];
}


    // fade and push to neopixels
    for (uint16_t i = 0; i < NUM_PIXELS; i++) {
        // fade with proper float multiply
				float multi = FADE_FACTOR;
				if (is_enabled == false){
					multi = FADE_FACTOR * 0.8;
				}
        r_buf[i] = (uint8_t)(r_buf[i] * multi);
        g_buf[i] = (uint8_t)(g_buf[i] * multi);
        b_buf[i] = (uint8_t)(b_buf[i] * multi);

        // write to strip
        strip.setPixelColor(i, r_buf[i], g_buf[i], b_buf[i]);
    }

    strip.show();
}
