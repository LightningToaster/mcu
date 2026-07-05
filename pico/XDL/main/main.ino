//XDL firmware 2026-6-20 -valkor
//TODO verify works without OLED

#define PIN_MENU 5
#define PIN_SHALLOW	3
#define PIN_DEEP 2
#define PIN_TOUCH 4
#define PIN_BUZZER 6 //passive, tone()
#define PIN_UV 14
#define PIN_IR_LED 15
#define PIN_IR_SENSE 29
#define PIN_BATTERY 27

#define PIN_SDA 0
#define PIN_SCL 1

#include <Arduino.h>
#include "core1_i2c.hpp"
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(1, 25, NEO_GRB + NEO_KHZ800);

#include "trigger.hpp"
Trigger trigger(PIN_MENU, PIN_TOUCH, PIN_SHALLOW, PIN_DEEP);

bool oled_connected = false; // not used yet



Core1I2C core; //TODO rename core1?

uint32_t t0 = 0;//testing, remove later
uint8_t state = 0;//testing, remove later

void setup() {
    Serial.begin(115200);
    delay(1000);

    core.begin(PIN_SDA, PIN_SCL);

    // start on logo
    core.display_mode = display_logo;
}

BUTTON_STATE last_result = TRIGGER_TOUCH;

void loop() {
    //uint32_t now = millis();
    BUTTON_STATE result = trigger.operate();
    if (result != last_result){
        Serial.print("T=");
        Serial.println(result);
        last_result = result;
    }

//   if (result != BUTTON_IDLE) {

//     Serial.print("button_state = ");

//     switch (result) {

//       case BUTTON_TOUCH:
//         Serial.println("BUTTON_TOUCH");
//         break;

//       case BUTTON_TAP:
//         Serial.println("BUTTON_TAP");
//         break;

//       case BUTTON_SHALLOW_CLICK:
//         Serial.println("BUTTON_SHALLOW_CLICK");
//         break;

//       case BUTTON_SHALLOW_HOLD:
//         Serial.println("BUTTON_SHALLOW_HOLD");
//         break;

//       case BUTTON_DEEP_CLICK:
//         Serial.println("BUTTON_DEEP_CLICK");
//         break;

//       case BUTTON_DEEP_HOLD:
//         Serial.println("BUTTON_DEEP_HOLD");
//         break;

//       default:
//         Serial.println("UNKNOWN");
//         break;
//     }
  

  //delay(20);
}
    

/*
    // cycle screens every 2 seconds
    if (now - t0 > 2000) {
        t0 = now;

        state++;

        switch (state) {
            case 0:
                core.display_mode = display_logo;
                break;

            case 1:
                core.display_mode = display_ammo;
                core.ammo = 3;
                break;

            case 2:
                core.ammo = 25;
                break;

            case 3:
                core.ammo = 9000;
                break;

            case 4:
                core.display_mode = display_empty;
                snprintf(core.voltage, sizeof(core.voltage), "15.2V");
                core.fired = 17;
                break;

            case 5:
                core.display_mode = display_battery;
                core.battery_state = battery_disconnected;
                snprintf(core.voltage, sizeof(core.voltage), "0.0V");
                break;

            case 6:
                core.battery_state = battery_low;
                snprintf(core.voltage, sizeof(core.voltage), "14.1V");
                break;

            case 7:
                core.battery_state = battery_good;
                snprintf(core.voltage, sizeof(core.voltage), "16.4V");
                break;

            case 8:
                core.battery_state = battery_overcharged;
                snprintf(core.voltage, sizeof(core.voltage), "17.8V");
                break;

            case 9:
                state = 0;
                core.settings.launched++;
                core.request_save();
                break;
        }
    }

    // simulate ammo change continuously
    if (core.display_mode == display_ammo) {
        core.ammo--;
        if (core.ammo > 1000) core.ammo = 1000;
    }*/

    //delay(50);
