#define SETUP_MESSAGE "NIT_METER@RP2040 (Valkor 2025-9-28)" 

//#define DEBUG

#include "oled.hpp"
OledDisplay oled;

#include "nitsmeter.hpp"
NitsMeter meter(28);

unsigned long last_print = 0;

void setup() {
    Serial.begin(9600);
    Serial.println(SETUP_MESSAGE);
    oled.begin();
}

void loop() {
    
    static unsigned long last_loop = 0;
    unsigned long now = micros();
    float loop_time = (now - last_loop) / 1000.0; // ms with decimals
    last_loop = now;
    meter.operate();
    const uint16_t adc = meter.get_adc();
    const uint16_t nits = meter.get_nits();
    static uint32_t flicker_count = 0;

    static unsigned long last_print = 0;
    if (millis() - last_print >= 50) {

        char buf[64];
        snprintf(buf, sizeof(buf), "a: %u\nn: %u\nf: %u", adc, nits, flicker_count);
        oled.display_text(buf);
        last_print = millis();
        
        
        unsigned long flicker_ms = meter.get_flicker();
        if (flicker_ms > 0){

            Serial.println(++flicker_count);
        }
        
        #ifdef DEBUG
        Serial.print("loop_ms: ");
        Serial.print(loop_time, 4);
        Serial.print("   adc: ");
        Serial.print(adc);
        Serial.print("  nits: ");
        Serial.println(nits);
        #endif
    }
}



