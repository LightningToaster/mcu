//NOT USED, die temp isnt very useful and kept interfering with other adc
//retest with RP2350?
#include "pico/stdlib.h"
#include "hardware/adc.h"

class Thermometer {
public:
    Thermometer() : last_ms(0), temp_c(0.0f) {
        adc_init();
        adc_set_temp_sensor_enabled(true);
        //adc_select_input(4); // internal temperature sensor
    }
    void operate() {
        uint32_t now = millis();
        if (now - last_ms >= 1000) {
            last_ms = now;
            uint8_t old_channel = adc_get_selected_input();  // save current channel
            adc_select_input(4);      // temp sensor
            sleep_us(5);              // allow mux + cap to settle
            adc_read();               // throw away first sample
            uint16_t raw = adc_read(); // real reading
            adc_select_input(old_channel);                    // restore previous ADC
            constexpr float adc_27c = 0.706f / 3.0f * 4095.0f;
            constexpr float slope   = (0.001721f / 3.0f) * 4095.0f;
            temp_c = 27.0f - (raw - adc_27c) / slope;

        }
    }

    float get_temp() const { return temp_c; }

private:
    uint32_t last_ms;
    float temp_c;
};
