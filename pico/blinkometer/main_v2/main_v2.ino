#define SETUP_MESSAGE "BLINKOMETER_V2@RP2040 (Valkor 2026-1-28)" 

//TODO why batt percent only even numbers

#define PIN_USB_DETECT 1

#define PIN_BUTTON 12
#include "button.hpp"
Button button(PIN_BUTTON, 333);//ms for flicker counter reset

#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9
#include "oled.hpp"
Display oled(PIN_OLED_SDA, PIN_OLED_SCL);

#include "nitsmeter.hpp"
NitsMeter sensor1(26); //TODO use other sensor?

#include "buzzer.hpp"
#define PIN_BUZZER 0
Buzzer buzzer(PIN_BUZZER);

#include "voltmeter.hpp"
Voltmeter voltmeter(29);

static uint32_t flicker_count = 0;
static uint32_t last_stopwatch_ms = 0;
static uint32_t stopwatch_seconds = 0;
//static uint32_t last_save_ms = 0;
//static constexpr uint32_t autosave_interval_ms = 600000; // 10 min


//#include <EEPROM.h>

// struct saved_data_t {
//     uint32_t flicker_count;
//     uint16_t hours;
//     uint8_t minutes;
//     uint32_t crc;
// };
// static saved_data_t saved_data;

// uint32_t crc32(const uint8_t *data, size_t len) {
//     uint32_t crc = 0xFFFFFFFF;
//     while (len--) {
//         crc ^= *data++;
//         for (uint8_t i = 0; i < 8; i++)
//             crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
//     }
//     return ~crc;
// }

void setup() {
    Serial.begin(9600);
    //EEPROM.begin(sizeof(saved_data_t));

    //EEPROM.get(0, saved_data);
    // uint32_t calc_crc = crc32(
    //     (uint8_t *)&saved_data,
    //     sizeof(saved_data_t) - sizeof(uint32_t)
    // );

    // if (calc_crc == saved_data.crc) {
    //     flicker_count = saved_data.flicker_count;
    //     stopwatch_seconds =
    //         (uint32_t)saved_data.hours * 3600 +
    //         (uint32_t)saved_data.minutes * 60;
    // }else{
    //     //crc fail, loaded data is corrupt
    // }

    oled.begin();
    buzzer.effect_turnon();
    pinMode(PIN_USB_DETECT, INPUT_PULLDOWN);

    if (flicker_count){//if loaded in value is >0, tell the oled
        oled.set_flicker(flicker_count);
    }
}


void flicker(uint16_t a = 1){
	if (a == 1){ //if no input, just increment
		flicker_count++;
	}else if (a == 0){//if input is 0, reset
		flicker_count = 0;
	}
	oled.set_flicker(flicker_count);
    //save_data();
}//flicker

// void save_data() {
//     uint16_t hours = stopwatch_seconds / 3600;
//     uint8_t minutes = (stopwatch_seconds / 60) % 60;

//     saved_data.flicker_count = flicker_count;
//     saved_data.hours = hours;
//     saved_data.minutes = minutes;
//     saved_data.crc = crc32(
//         (uint8_t *)&saved_data,
//         sizeof(saved_data_t) - sizeof(uint32_t)
//     );

//     EEPROM.put(0, saved_data);
//     EEPROM.commit();
//     last_save_ms = millis();
//     Serial.println("save");
// }


void loop() {
    bool USB_connected = digitalRead(PIN_USB_DETECT); 
    // if (voltmeter.get_status() == BATTERY_LOW and not USB_connected) {
    //    shutdown();
    // }
    voltmeter.operate();
    if (button.operate() == BUTTON_HOLD){
        if (flicker_count == 0){
            stopwatch_seconds = 0;
            last_stopwatch_ms = millis(); // reset timer reference 
            buzzer.effect_reset2();
            //save_data();
        }else{
            flicker(0);
            buzzer.effect_reset();
        }
        
        
    }//button held
    buzzer.operate(flicker_count > 0);
    sensor1.operate();
    uint16_t adc = sensor1.get_adc();
    oled.set_adc_channel(1);        
    if (sensor1.get_flicker() > 0){
        flicker();
        buzzer.effect_flicker();
    }//flicker detected

    uint32_t now = millis();
    if (now - last_stopwatch_ms >= 1000) {
        last_stopwatch_ms += 1000;  // advance exactly 1 second per tick
        stopwatch_seconds++;
    }
    uint8_t seconds = stopwatch_seconds % 60;
    uint8_t minutes = (stopwatch_seconds / 60) % 60;
    uint16_t hours = stopwatch_seconds / 3600;

    oled.set_adc_value(adc);
    oled.set_battery(voltmeter.get_string(PERCENTAGE));
    oled.set_seconds(seconds);
    oled.set_minutes(minutes);
    oled.set_hours(hours);
    oled.set_USB(USB_connected);
    
    oled.update();

    // if (now - last_save_ms > autosave_interval_ms) {
    //     save_data();
    // }


}//loop

// void shutdown(){
//     oled.battery_died();
//     digitalWrite(LED_BUILTIN, LOW);
//     noTone(PIN_BUZZER);

//     save_data();

//     while (true) {
//         __wfi();
//     }
// }


