#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9
#include "oled.hpp"
Display oled(PIN_OLED_SDA, PIN_OLED_SCL);

static uint32_t last_stopwatch_ms = 0;
static uint32_t stopwatch_seconds = 0;

void setup() {
    Serial.begin(9600);
    //delay(3000);
    oled.begin();
}

void loop() {
    uint32_t now = millis();
    if (now - last_stopwatch_ms >= 1000) {
        last_stopwatch_ms += 1000;  // advance exactly 1 second per tick
        stopwatch_seconds++;
    }
    uint8_t seconds = stopwatch_seconds % 60;
    uint8_t minutes = (stopwatch_seconds / 60) % 60;
    uint16_t hours = stopwatch_seconds / 3600;

    oled.set_adc_value(now);
    oled.set_battery("BATT");
    oled.set_seconds(seconds);
    oled.set_minutes(minutes);
    oled.set_hours(hours);
    oled.set_USB("USB_con");
    
    oled.update();

    //Serial.println("loop");

}//loop