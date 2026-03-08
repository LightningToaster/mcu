#define SETUP_MESSAGE "BLINKOMETER_V3@RP2350 (Valkor 2026-3-5)" 

//TODO why has it frozen?  
//TODO new led code
//TODO why batt percent only even numbers
//TODO version that pops up on startup
//make a test to use oled on core0 and eeprom, see if it freezes

#define PIN_BUTTON 12
#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9
#define PIN_SENSOR_1 27
#define PIN_VOLTMETER 29
#define PIN_BUZZER 2
#define RESET_BUTTON_HOLD_MS 400//how long button must be pressed to trigger reset
#define AUTOSAVE_INTERVAL_MS 5000 //600000 = 10min

#include "button.hpp"
Button button(PIN_BUTTON, RESET_BUTTON_HOLD_MS);

#include <atomic>
std::atomic<bool> core1_pause_request(false);
std::atomic<bool> core1_paused(false);
#include "oled.hpp"
Display oled(PIN_OLED_SDA, PIN_OLED_SCL);

#include "nitsmeter.hpp"
NitsMeter sensor1(PIN_SENSOR_1); //TODO use sensor2 as well

#include "buzzer.hpp"
Buzzer buzzer(PIN_BUZZER);

#include "voltmeter.hpp"
Voltmeter voltmeter(PIN_VOLTMETER);

static uint32_t flicker_count = 0;
static uint32_t last_stopwatch_ms = 0;
static uint32_t stopwatch_seconds = 0;
static uint32_t last_save_ms = 0;


#include <EEPROM.h>
struct saved_data_t {
    uint32_t flicker_count;
    uint16_t hours;
    uint8_t minutes;
    uint32_t crc;
};
static saved_data_t saved_data;
uint32_t crc32(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    while (len--) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++)
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
    }
    return ~crc;
}//crc32

void flicker(bool reset = false){
    if (reset)
        flicker_count = 0;
    else
        flicker_count++;

    oled.set_flicker(flicker_count);
    Serial.println("flick");
    save_data();
}//flicker

void save_data() {
    uint16_t hours = stopwatch_seconds / 3600;
    uint8_t minutes = (stopwatch_seconds / 60) % 60;

    saved_data.flicker_count = flicker_count;
    saved_data.hours = hours;
    saved_data.minutes = minutes;
    saved_data.crc = crc32(
        (uint8_t *)&saved_data,
        sizeof(saved_data_t) - sizeof(uint32_t)
    );

    
    // request core1 pause
    core1_pause_request.store(true);
    // wait until core1 confirms it is paused
    while (!core1_paused.load())
    {
        delay(1);
    }
    delay(9);
    // core1 is now guaranteed idle
    EEPROM.put(0, saved_data);
    delay(10);
    EEPROM.commit();
    // release core1
    core1_pause_request.store(false);

    last_save_ms = millis();
    Serial.println("save");
}//save_data

void setup() {
    Serial.begin(9600);
    EEPROM.begin(sizeof(saved_data_t));

    EEPROM.get(0, saved_data);
    uint32_t calc_crc = crc32(
        (uint8_t *)&saved_data,
        sizeof(saved_data_t) - sizeof(uint32_t)
    );

    if (calc_crc == saved_data.crc) {
        flicker_count = saved_data.flicker_count;
        stopwatch_seconds =
            (uint32_t)saved_data.hours * 3600 +
            (uint32_t)saved_data.minutes * 60;
    }else{
        //crc fail, loaded data is corrupt
    }

    oled.begin();
    buzzer.effect_turnon();

    if (flicker_count){//if loaded in value is >0, tell the oled
        oled.set_flicker(flicker_count);
    }
}//setup

void loop() {
    voltmeter.operate();
    if (button.operate() == BUTTON_HOLD){
        Serial.println("click");
        if (flicker_count == 0){
            stopwatch_seconds = 0;
            last_stopwatch_ms = millis(); // reset timer reference 
            buzzer.effect_reset2();
            save_data();
        }else{
            flicker(true);
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
        Serial.print("s");
        last_stopwatch_ms += 1000;  // advance exactly 1 second per tick
        stopwatch_seconds++;
    }
    uint8_t seconds = stopwatch_seconds % 60;
    uint8_t minutes = (stopwatch_seconds / 60) % 60;
    uint16_t hours = stopwatch_seconds / 3600;

    oled.set_adc_value(adc);
    const char* batt = voltmeter.get_string(PERCENTAGE);
    oled.set_battery(batt);
    oled.set_seconds(seconds);
    oled.set_minutes(minutes);
    oled.set_hours(hours);
    oled.set_USB(voltmeter.get_status() == BATTERY_DISCONNECTED);
    
    if (now - last_save_ms > AUTOSAVE_INTERVAL_MS) {
        save_data();
    }


}//loop