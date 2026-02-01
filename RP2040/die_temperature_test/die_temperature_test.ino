#include <Arduino.h>
#include "hardware/adc.h"

static uint32_t last_print_ms = 0;

void setup() {
    Serial.begin(9600);
    while (!Serial) {;}

    adc_init();
    adc_set_temp_sensor_enabled(true);

    Serial.println("rp2040 temp test");
}

float read_rp2040_temp_c() {
    adc_select_input(4);   // internal temperature sensor

    sleep_us(5);           // allow adc mux + s/h to settle
    adc_read();            // discard first sample
    uint16_t raw = adc_read();

    // use conservative vref estimate (closer to reality than 3.3v)
    constexpr float v_ref = 3.3f;
    constexpr float adc_max = 4095.0f;

    constexpr float adc_27c = (0.706f / v_ref) * adc_max;
    constexpr float slope   = (0.001721f / v_ref) * adc_max;

    float temp_c = 27.0f - (raw - adc_27c) / slope;
    return temp_c;
}

void loop() {
    uint32_t now = millis();
    if (now - last_print_ms < 500) return; // 2 Hz
    last_print_ms = now;

    float temp_c = read_rp2040_temp_c();
    Serial.print("temp_c=");
    Serial.println(temp_c, 2);
}
