//XDL firmware 2026-7-16 -valkor
/*TODO
revisit buzzer using blinkometer v3 class 
compensate power with voltage
test loop timing make sure fast
add voltage to main darts fired screen for testing only
investigate low voltage, does it flip back and forth?  does it need a flag?
missing sounds on stuff
put voltge on menu
system lockout at 30min no input or 8h
change lipo
*/
#define BLASTER_NAME "BLASTOMATIC MK2"
#define BLASTER_OWNER "valkor"
#define BLASTER_SERIAL "0002"
#define BLASTER_BUILD_DATE "2026.6.12"
#define FIRMWARE_VERSION "v1.0"

#define PIN_SDA 0
#define PIN_SCL 1
#define PIN_MENU_SWITCH 5
#define PIN_TRIGGER_SHALLOW 3
#define PIN_TRIGGER_DEEP 2
#define PIN_TRIGGER_TOUCH 4
#define PIN_BUZZER 6  //passive, tone()
#define PIN_UV 14
#define PIN_IR_EMITTER 15
#define PIN_IR_RECEIVER 29
#define PIN_BATTERY 27
#define PIN_SOLENOID 7
#define PIN_ESC_0 8  //left esc, right motor
#define PIN_ESC_1 9  //right esc, left motor

#define MAX_POWER 50
#define RESET_SAVE false  //set true to override save with defaults
#define IGNORE_DART_DETECTION true
#define ARMED false // wheels and solenoid will only operate if this is true


bool is_locked = false;  //set to false to disable pinlock
static constexpr char pin_combo[] = "123";
#define PIN_LENGTH 3    //must be the length of pin_combo


#include <Arduino.h>

#include "battery.hpp"
Battery battery(PIN_BATTERY);

#include "core1.hpp"
Core1 core;

#include "buzzer.hpp"
Buzzer buzzer(PIN_BUZZER);

#include "indicator_led.hpp"
IndicatorLED indicator_led;

#include "uv_led.hpp"
LED uv_led(PIN_UV);

#include "switch.hpp"
Switch menu_switch(PIN_MENU_SWITCH);

#include "trigger.hpp"
Trigger trigger(PIN_TRIGGER_TOUCH, PIN_TRIGGER_SHALLOW, PIN_TRIGGER_DEEP);

#include "pinlock.hpp"
PinLock pin_lock;

#include "dart.hpp"
DartDetector dart(PIN_IR_EMITTER, PIN_IR_RECEIVER);

#include "solenoid.hpp"
Solenoid solenoid(PIN_SOLENOID);

#include "wheels.hpp"
Wheels wheels(PIN_ESC_0, PIN_ESC_1);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() <= 1000) { delay(50); }
  wheels.begin();
  core.begin(PIN_SDA, PIN_SCL);
  core.display_mode = display_logo;
  core.randomize_starting_pin();
  indicator_led.set(0, 0, 0, 0);
  delay(2000);  //TODO consider when adding wheels
  if (is_locked == false){
    core.display_mode = display_menu;
  }
}  //setup

TRIGGER_STATE last = TRIGGER_IDLE;

void loop() {
  MENU_SWITCH_STATE menu_switch_state = menu_switch.operate();
  TRIGGER_STATE trigger_state = trigger.operate();

  buzzer.operate();
  core.battery_status = battery.operate();
  battery.get_string(core.voltage_cell, CELL);
  battery.get_string(core.voltage_pack, PACK);
  battery.get_string(core.voltage_percent, PERCENTAGE);
  indicator_led.operate();
  uv_led.operate();
  DART_STATE dart_status = dart.operate();
  core.IR = dart.get_reflection();
  bool wheels_revd = wheels.operate(10);
  bool dart_was_fired = solenoid.operate(wheels_revd);
  if (dart_was_fired) { core.dart_fired();}


  if (is_locked == true) {
    core.display_mode = display_lock;
    if (pin_lock.is_ready() == true) {
      core.pin_failed = false;
      if (trigger_state == TRIGGER_TOUCH_END) {
        buzzer.beep(120, 30);
        core.selected_char = (core.selected_char + 1) % PIN_LENGTH; //TODO core method()
      } else if (trigger_state == TRIGGER_SHALLOW_END) { 
        core.pin_entered[core.selected_char] =
          ((core.pin_entered[core.selected_char] - '0' + 1) % 10) + '0';
        buzzer.beep(300, 50);
      } else if (trigger_state == TRIGGER_DEEP_END) {
        if (pin_lock.verify(core.pin_entered) == true) {  //correct pin entered
          is_locked = false;
          if (menu_switch_state == SWITCH_CLOSED){
            core.display_mode = display_menu;
          }
          core.failed_logins_report = core.settings.failed_logins;
          core.settings.failed_logins = 0;
          buzzer.beep(500, 300);
          indicator_led.set(0, 100, 0, 0);
          indicator_led.set(0, 0, 0, 300);
        } else {  //wrong pin entered
          core.pin_failed = true;
          core.settings_dirty = true;
          core.settings.failed_logins++;
          core.randomize_starting_pin();
          buzzer.beep(80, 500);
          indicator_led.set(255, 0, 0, 0);
          indicator_led.set(0, 0, 0, 500);
        }//pin correct?
      }//deep click?  (pin attempt)
    }//pin_lock.is_ready()  (cooldown done?)
    return;//restart loop()
  }//is_locked?

  if (menu_switch_state == SWITCH_CLOSED_ACTION) {
    core.selection = 0;  //profile is selected by default
    core.display_mode = display_menu;
    core.logo_index = 0;
  }

  if (menu_switch_state == SWITCH_CLOSED or menu_switch_state == SWITCH_CLOSED_ACTION) {
    solenoid.stop();
    wheels.stop();
    if (core.display_mode == display_menu) {
      switch (trigger_state) {
        case TRIGGER_TOUCH_END:  //cycle through settings
          core.next_selection();
          buzzer.beep(120, 30);
        break;
        case TRIGGER_SHALLOW_END:
          switch (core.selection) {
            case 0://change profile
              core.next_profile();
              buzzer.beep(300, 50);
              break;
            case 1://change volley
              core.next_volley();
              buzzer.beep(240, 30);
              break;
            case 2:  //change power
              core.next_power();
              buzzer.beep(240, 30);
              break;
            case 3:  //change glow
              core.next_glow();
              buzzer.beep(240, 30);
              break;
          }  //core.selection
          break;
        case TRIGGER_DEEP_END:
          if (core.selection == 0){
            core.display_mode = display_metrics;
            buzzer.beep(180, 80);
          }

        case TRIGGER_DEEP_INCREMENT:
          if (core.selection == 2){ //power
            core.next_power(5);
            buzzer.beep(260, 30);
          }
        break;

      }//trigger_state

    }else{//not in menu
      if (trigger_state == TRIGGER_DEEP_END) {
        switch(core.display_mode){
          case display_metrics: core.display_mode = display_info; break;
          case display_info: 
          case display_logo: 
            core.display_mode = display_menu; 
          break;
        }
        buzzer.beep(180, 80);
      }else if (trigger_state == TRIGGER_SHALLOW_END){
        if (core.display_mode == display_info or core.display_mode == display_logo){
            core.logo_index = (core.logo_index + 1) % LOGO_COUNT;
          if (core.logo_index == 0){//show info instead, which has the first logo in it
            core.display_mode = display_info;
          }else{
            core.display_mode = display_logo;
          }
          buzzer.beep(350, 30);
        }
        
      }//shallow_end?
    }//not in menu

  }else{//FIRING MODE

    if (core.battery_status == BATTERY_GOOD or ARMED == false) {
      if (dart_status == DART_READY or IGNORE_DART_DETECTION) {
        core.display_mode = display_ammo;
        switch (trigger_state) {
          case TRIGGER_IDLE: wheels.idle();
            uv_led.fade(0);
          break;
          default:
            wheels.set_throttle_percentage(core.get_power());
            switch (core.get_glow()){
              case 1: uv_led.fade(10); break;
              case 2: uv_led.set(70); break;
              case 3: uv_led.set(255); break;
            }
            
          break;
        } //any trigger interaction?

        switch (trigger_state) {
        case TRIGGER_SHALLOW_START:solenoid.fire(1);break;

        case TRIGGER_DEEP_ONGOING:
        case TRIGGER_DEEP_INCREMENT:
        case TRIGGER_DEEP_START:
          if (core.get_volley() == 3){solenoid.fire();}//full auto
        break;
        
        case TRIGGER_IDLE:if (core.get_volley() == 3){solenoid.stop();}break;

        }  //switch
        if (trigger_state == TRIGGER_DEEP_START//fire a volley
        and core.get_volley() < 3){
          solenoid.fire(core.get_volley());
        }

      }else{// no dart
        if (dart_status == MAG_EMPTY or menu_switch_state == SWITCH_OPENED_ACTION){
          solenoid.stop();
          wheels.stop();
          core.display_mode = display_empty;
        }else if (dart_status == NEW_MAG){
          core.fired_from_mag = 0;
        }else if (dart_status == DART_NONE){
          //Serial.println("no dart");
          wheels.set_throttle_percentage(0);//from MK1, needed?
          solenoid.stop();
        }
        uv_led.fade(0);
      }//dart_status?
    }else{//voltage not ok
      solenoid.stop();
      wheels.stop();
      uv_led.fade(0);
      core.display_mode = display_battery;
    }
  }//is_menu?
}//loop