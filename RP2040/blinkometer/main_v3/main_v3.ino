#define FIRMWARE_VERSION "v1.1" //Valkor 2026-3-10

#define PIN_BUTTON 12
#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9
#define PIN_SENSOR_1 26
//#define PIN_SENSOR_2 27 //not used yet
#define PIN_VOLTMETER 29
#define PIN_BUZZER 2
#define RESET_BUTTON_HOLD_MS 400
#define AUTOSAVE_INTERVAL_MS 5000 // testing, normally 10min

#include <atomic>
#include <EEPROM.h>
#include "button.hpp"
#include "oled.hpp"
#include "nitsmeter.hpp"
#include "buzzer.hpp"
#include "voltmeter.hpp"
#include <pico/multicore.h>

// ---------------- shared data ----------------
struct saved_data_t {
    uint8_t flicker_count;
    uint16_t time_5min_units; // 1 unit = 5 minutes
};

std::atomic<uint16_t> flicker_count(0);
std::atomic<uint32_t> stopwatch_seconds(0);
std::atomic<uint16_t> adc_value(0);
std::atomic<uint8_t> hours_atomic(0);
std::atomic<uint8_t> minutes_atomic(0);
std::atomic<uint8_t> seconds_atomic(0);
std::atomic<bool> usb_disconnected(false);
std::atomic<bool> update_flicker(false);
std::atomic<bool> core1_pause_request(false);
std::atomic<bool> core1_paused(false);
std::atomic<bool> save_requested(false);

saved_data_t saved_data;
uint32_t last_save_ms = 0;
uint32_t last_stopwatch_ms = 0;

// ---------------- core1 loop ----------------
Button button(PIN_BUTTON, RESET_BUTTON_HOLD_MS);
NitsMeter sensor1(PIN_SENSOR_1);
Buzzer buzzer(PIN_BUZZER);
Voltmeter voltmeter(PIN_VOLTMETER);

void core1_loop() {
    unsigned long last_stopwatch_ms_local = millis(); // local reference
    while(true) {
        // check for pause request
        if (core1_pause_request.load()) {
            Serial.println("p1");
            core1_paused.store(true);
            while(core1_pause_request.load()) delay(1);
            delay(9);
            core1_paused.store(false);
            Serial.println("p2");
        }

        // 1 kHz sensor read
        sensor1.operate();
        adc_value.store(sensor1.get_adc());

        // flicker detection
        if (sensor1.get_flicker() > 0) {
            flicker_count.fetch_add(1);
            update_flicker.store(true);
            buzzer.effect_flicker();
        }

        // button
        if (button.operate() == BUTTON_HOLD) {
            if (flicker_count.load() == 0) {
                stopwatch_seconds.store(0);
                last_stopwatch_ms_local = millis(); // reset timer reference
                buzzer.effect_reset2();
            } else {
                flicker_count.store(0);
                update_flicker.store(true);
                buzzer.effect_reset();
            }
            save_requested.store(true);// request save on core0
        }

        buzzer.operate(flicker_count.load() > 0);
        voltmeter.operate();

        // ---------- stopwatch update ----------
        unsigned long now = millis();
        if (now - last_stopwatch_ms_local >= 1000) {
            last_stopwatch_ms_local += 1000;
            stopwatch_seconds.fetch_add(1);
        }

    }
}

// ---------------- core0 OLED + EEPROM ----------------
Display oled(PIN_OLED_SDA, PIN_OLED_SCL);

void setup() {
    Serial.begin(9600);
    //EEPROM.begin(sizeof(saved_data_t));
    //EEPROM.get(0, saved_data);

    flicker_count.store(saved_data.flicker_count);
    stopwatch_seconds.store(saved_data.time_5min_units * 300); // 5 min units -> seconds
    

    oled.begin();
    buzzer.effect_turnon();
    if (flicker_count.load()) oled.set_flicker(flicker_count.load());

    multicore_launch_core1(core1_loop);
}

void safe_eeprom_commit() {
    Serial.print("s1");
    if (flicker_count.load() <= 127){
        saved_data.flicker_count = flicker_count.load();
    }else{
        saved_data.flicker_count = 127;
    }
    saved_data.time_5min_units = stopwatch_seconds.load() / 300; // 1 unit = 5 min

    core1_pause_request.store(true);
    while (!core1_paused.load()) delay(1);
    delay(9);
    //EEPROM.put(0, saved_data);
    //EEPROM.commit();
    delay(20);
    core1_pause_request.store(false);
    save_requested.store(false);

    last_save_ms = millis();
    Serial.println("s2");
}

// ---------------- loop ----------------
void loop() {
    uint32_t now = millis();
    oled.operate();

    // copy atomic values
    uint32_t t_seconds = stopwatch_seconds.load();
    uint16_t adc = adc_value.load();
    uint8_t sec = t_seconds % 60;
    uint8_t min = (t_seconds / 60) % 60;
    uint16_t hrs = t_seconds / 3600;

    hours_atomic.store(hrs);
    minutes_atomic.store(min);
    seconds_atomic.store(sec);

    // update OLED
    oled.set_adc_value(adc);
    oled.set_hours(hrs);
    oled.set_minutes(min);
    oled.set_seconds(sec);
    oled.set_USB(voltmeter.get_status() == BATTERY_DISCONNECTED);
    oled.set_battery(voltmeter.get_string(PERCENTAGE));
    if (update_flicker.load()) {
        oled.set_flicker(flicker_count.load());
        update_flicker.store(false);
    }

    // autosave
    if (now - last_save_ms > AUTOSAVE_INTERVAL_MS or save_requested.load()) {
        safe_eeprom_commit();
    }

}//loop0