#include "oled.hpp"
OledDisplay oled(8,9);//SDA,SCL

unsigned long last_print = 0;

void setup() {
    Serial.begin(9600);
    oled.begin();
    oled.display_logo(2);
    delay(2000);
}



void loop() {
    
    static uint8_t logo_index = 0;
    static unsigned long last_loop = 0;
    unsigned long now = micros();
    float loop_time = (now - last_loop) / 1000.0; // convert to ms with decimals
    last_loop = now;

    // print every second
    static unsigned long last_print = 0;

    if (millis() - last_print >= 500) {
        //Serial.print("loop delay (ms): ");
        //Serial.println(loop_time, 4);

        uint16_t adc0 = analogRead(26); // gpio26
        uint16_t adc1 = analogRead(27); // gpio27
        uint16_t adc2 = analogRead(28); // gpio28
        uint16_t adc3 = analogRead(29); // gpio29

        char buf[64];
        snprintf(buf, sizeof(buf), "%u\n%u\n%u\n%u", adc0, adc1, adc2, adc3);
        oled.display_text(buf);

        last_print = millis();

        //oled.display_logo(3);
        //oled.display_logo(logo_index = (logo_index++) & 3);
    }  
}