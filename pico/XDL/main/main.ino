//XDL firmware 2026-7-13 -valkor
/*TODO verify works without OLED
add IR output to info screen
failed login attempts (saved)
remove dart_detection option if that system always works
tick up op_hours (1min chunks)
compensate power with voltage
compensate solenoid timings with voltage?
red light when login fail, green when good
integrate UV into wheel class?  if becomes issue, then separate
*/
#define BLASTER_NAME "BLASTOMATIC MK2"
#define BLASTER_OWNER "Valkor"
#define BLASTER_SERIAL "#0002"
#define BLASTER_BUILD_DATE "2026.7.6"
#define FIRMWARE_VERSION "v1.0"
#define PIN_SDA 0
#define PIN_SCL 1
#define PIN_MENU 5
#define PIN_SHALLOW	3
#define PIN_DEEP 2
#define PIN_TOUCH 4
#define PIN_BUZZER 6 //passive, tone()
#define PIN_UV 14
#define PIN_IR_LED 15
#define PIN_IR_SENSE 29
#define PIN_BATTERY 27
#define MAX_POWER 100
#define RESET_SAVE false //set true to override save with defaults

static constexpr char pin_combo[] = "123";
#define PIN_LENGTH 3 //length of pinlock, set to 0 to disable


#include <Arduino.h>

#include "core1.hpp"
Core1 core;

#include "buzzer.hpp"
Buzzer buzzer(PIN_BUZZER);

#include "battery.hpp"
Battery battery(PIN_BATTERY);

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(1, 25, NEO_GRB + NEO_KHZ800);

#include "trigger.hpp"
Trigger trigger(PIN_MENU, PIN_TOUCH, PIN_SHALLOW, PIN_DEEP);

#include "pinlock.hpp"
PinLock pin_lock;

bool oled_connected = false; // not used yet
TRIGGER_STATE last_result = TRIGGER_IDLE;
uint8_t selection = 0;
bool is_menu = false;
bool is_locked = true;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 2000) {delay(50);}
    analogWrite(PIN_UV, 0);//TEMP so it stop blinking
    core.begin(PIN_SDA, PIN_SCL);
    //core.display_mode = display_splash;// TODO startup splash? logo?
    core.display_mode = display_lock;
    core.randomize_starting_pin();
    
    

    //delay(299000);//TODO consider when adding wheels
}//setup


void loop() {
    //uint32_t now = millis();
    TRIGGER_STATE trigger_state = trigger.operate(is_locked);
    buzzer.operate();
    battery.operate();
    battery.get_string(core.voltage, sizeof(core.voltage), CELL);

    if (is_locked == true){
        //TODO lock stuff
        //flash red led if wrong pin
        if (trigger_state == TRIGGER_TAP){
            buzzer.beep(120, 30);
            core.selected_char = (core.selected_char + 1) % PIN_LENGTH;
        }else if(trigger_state == TRIGGER_SHALLOW_CLICK){
            core.pin_entered[core.selected_char] =
                ((core.pin_entered[core.selected_char] - '0' + 1) % 10) + '0';
            buzzer.beep(300, 50);
        }else if(trigger_state == TRIGGER_DEEP_CLICK){
            if (pin_lock.verify(core.pin_entered) == true){
                Serial.println("success!");
                is_locked = false;
            }else{
                Serial.println("FAIL");
                core.randomize_starting_pin();
                buzzer.beep(80, 200);
                //TODO some type of message, wait 3s
                //TODO incrememnt fail counter
                //TODO buzzer gets mad
            }
        }
        return;//restart loop after this unless unlocked
    }

    if (trigger.is_menu() == true){
        if (is_menu == false){//just switched to menu
            selection = 0;//profile selection by default
            core.display_mode = display_menu;
        }
        is_menu = true;

        if (core.display_mode == display_menu){
            switch(trigger_state){
            case TRIGGER_TAP://cycle through settings
                buzzer.beep(120, 30);
                selection = (selection + 1) % 5;
            break;
            case TRIGGER_SHALLOW_CLICK:
                switch(selection){
                case 0: //change profile
                    core.settings.profile = (core.settings.profile % 3) + 1;
                    buzzer.beep(300, 50);
                break;
                case 1: //change volley
                    switch(core.settings.profile){
                    case 1:core.settings.volley1 = (core.settings.volley1 + 1) % 4;break;
                    case 2:core.settings.volley2 = (core.settings.volley2 + 1) % 4;break;
                    case 3:core.settings.volley3 = (core.settings.volley3 + 1) % 4;break;
                    }
                    buzzer.beep(240, 30);
                break;
                case 2: //change power
                    switch(core.settings.profile){
                    case 1:core.settings.power1 = (core.settings.power1 % 100) + 1;break;
                    case 2:core.settings.power2 = (core.settings.power2 % 100) + 1;break;
                    case 3:core.settings.power3 = (core.settings.power3 % 100) + 1;break;
                    }
                    buzzer.beep(240, 30);
                break;
                case 3: //change dart_detect
                    switch(core.settings.profile){
                    case 1:core.settings.dart_detection1 = !core.settings.dart_detection1;break;
                    case 2:core.settings.dart_detection2 = !core.settings.dart_detection2;break;
                    case 3:core.settings.dart_detection3 = !core.settings.dart_detection3;break;
                    }
                    buzzer.beep(240, 30);
                break;
                case 4: //change glow
                    switch(core.settings.profile){
                    case 1:core.settings.glow1 = (core.settings.glow1 + 1) % 4;break;
                    case 2:core.settings.glow2 = (core.settings.glow2 + 1) % 4;break;
                    case 3:core.settings.glow3 = (core.settings.glow3 + 1) % 4;break;
                    }
                    buzzer.beep(240, 30);
                break;
                }//selection
            break;
            case TRIGGER_DEEP_CLICK:
                switch(selection){
                    case 0: //profile
                        core.display_mode = display_info;
                       
                    break;
                    case 2: //power
                        auto next_power = [](uint8_t power) -> uint8_t {
                            power = ((power + 4) / 5) * 5;  // round up to nearest multiple of 5
                            if (power >= MAX_POWER) return 0;
                            return power + 5;
                        };
                        switch (core.settings.profile) {
                        case 1: core.settings.power1 = next_power(core.settings.power1); break;
                        case 2: core.settings.power2 = next_power(core.settings.power2); break;
                        case 3: core.settings.power3 = next_power(core.settings.power3); break;
                        }
                        buzzer.beep(280, 25);
                    break;
                }
            break;
            }//trigger_state

        }else if (core.display_mode = display_info){
            if (trigger_state == TRIGGER_DEEP_CLICK){
                core.display_mode = display_menu;
            }
        }//if display_menu
        
        core.selection = selection;

    }else{//FIRING MODE
        is_menu = false;
        core.display_mode = display_ammo;//TEMP

        //TODO firing stuff
        //TODO increment core.fired, core.darts_session, and core.settings.launched when firing
    }//is_menu?

}//loop
    

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
