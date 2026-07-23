//XDL firmware 2026-7-20 -valkor
/*TODO

*/
#define BLASTER_NAME "VALKONATOR MK2"
#define BLASTER_OWNER "valkor"
#define BLASTER_SERIAL "0002"
#define BLASTER_BUILD_DATE "2026.6.12"
#define FIRMWARE_VERSION "v1.0"

#define PIN_SDA 0
#define PIN_SCL 1
#define PIN_ESC_0 8  //left esc, right motor
#define PIN_ESC_1 9  //right esc, left motor
#define PIN_SOLENOID 7
#define PIN_MENU_SWITCH 5
#define PIN_TRIGGER_TOUCH 4
#define PIN_TRIGGER_SHALLOW 3
#define PIN_TRIGGER_DEEP 2
#define PIN_BATTERY 27
#define PIN_IR_EMITTER 15
#define PIN_IR_RECEIVER 29
#define PIN_BUZZER 6  //passive, tone()
#define PIN_UV 14

#define REQUIRE_PIN false  //set to false to disable pinlock
#define AUTOLOCK_INACTIVITY_MINUTES 30  //if require pin is enabled, will lock after this many minutes of inactivity

#define MAX_POWER 100 //just for limiting during testing
#define ARMED true // wheels and solenoid will only operate if this is true
#define RESET_SAVE false  //set true to override save with defaults
#define IGNORE_DART_DETECTION true
#define AMMO_VOLTAGE_READOUT true

#include <Arduino.h>

#include "pinlock.hpp"
PinLock pinlock;

#include "battery.hpp"
Battery battery(PIN_BATTERY);

#include "core1.hpp"
Core1 core;

#include "uv_led.hpp"
LED uv_led(PIN_UV);

#include "indicator_led.hpp"
IndicatorLED indicator_led;

#include "buzzer.hpp"
Buzzer buzzer(PIN_BUZZER);

#include "switch.hpp"
Switch menu_switch(PIN_MENU_SWITCH);

#include "trigger.hpp"
Trigger trigger(PIN_TRIGGER_TOUCH, PIN_TRIGGER_SHALLOW, PIN_TRIGGER_DEEP);

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
  delay(2000);
  core.display_mode = display_menu;//overwritten by lockscreen if enabled
}  //setup

void loop() {//print_loop_speed();
  MENU_SWITCH_STATE menu_switch_state = menu_switch.operate();
  TRIGGER_STATE trigger_state = trigger.operate();
  if (REQUIRE_PIN and menu_switch.was_inactive_for_minutes(AUTOLOCK_INACTIVITY_MINUTES) and trigger.was_inactive_for_minutes(AUTOLOCK_INACTIVITY_MINUTES)){
    core.randomize_starting_pin();//display only, not security related
    pinlock.lock();
    menu_switch.reset_inactivity();
    trigger.reset_inactivity();
  }
  buzzer.operate();
  core.battery_status = battery.operate();
  battery.get_string(core.voltage_cell, CELL);
  battery.get_string(core.voltage_pack, PACK);
  battery.get_string(core.voltage_percent, PERCENTAGE);
  indicator_led.operate();
  uv_led.operate();
  DART_STATE dart_status = dart.operate();
  core.IR = dart.get_reflection();
  bool wheels_revd = wheels.operate();
  bool dart_was_fired = solenoid.operate(wheels_revd);
  if (dart_was_fired) { core.dart_fired();}

  if (pinlock.is_locked()) {
    core.display_mode = display_lock;
    if (pinlock.is_ready() == true) {
      core.pin_failed = false;
      if (trigger_state == TRIGGER_TOUCH_END) {
        buzzer.beep(120, 30);
        core.next_char_position();
      } else if (trigger_state == TRIGGER_SHALLOW_END) { 
        core.increment_number_entry();
        buzzer.beep(300, 50);
      } else if (trigger_state == TRIGGER_DEEP_END) {
        if (pinlock.verify(core.pin_entered) == true) {  //correct pin entered
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
  }

  if (menu_switch_state == SWITCH_CLOSED or menu_switch_state == SWITCH_CLOSED_ACTION) {
    solenoid.stop();
    wheels.stop();
    uv_led.fade(0);
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
            //buzzer.beep(180, 80);
            buzzer.effect_next_menu();
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
        core.logo_index = 0;
        switch(core.display_mode){
          case display_metrics: core.display_mode = display_info; break;
          case display_info: 
          case display_logo: 
            core.display_mode = display_menu; 
          break;
        }
        buzzer.effect_next_menu();
      }else if (trigger_state == TRIGGER_SHALLOW_END){
        if (core.display_mode == display_info or core.display_mode == display_logo){
            core.logo_index = (core.logo_index + 1) % LOGO_COUNT;
          if (core.logo_index == 0){//show info instead, which has the first logo in it
            core.display_mode = display_info;
          }else{
            core.display_mode = display_logo;
          }
          //buzzer.beep(350, 30);
          buzzer.effect_next_logo();
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
            wheels.set_throttle_percentage(core.get_power(), battery.get_percent());
            //wheels.set_throttle_percentage(core.get_power(), 50);
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
      //buzzer.effect_battery_low();//TODO a way to only fire once
    }
  }//is_menu?
}//loop

void print_loop_speed(){
  static unsigned long last_report_ms = 0;
  static unsigned long loop_start_us = micros();
  static uint64_t total_loop_us = 0;
  static uint32_t loop_count = 0;

  unsigned long now_us = micros();
  total_loop_us += (unsigned long)(now_us - loop_start_us);
  loop_count++;
  loop_start_us = now_us;

  if (millis() - last_report_ms >= 1000) {
      last_report_ms += 1000;

      Serial.print("avg loop: ");
      Serial.print(total_loop_us / loop_count);
      Serial.print(" us, loops/s: ");
      Serial.println(loop_count);

      total_loop_us = 0;
      loop_count = 0;
  }
}//print_loop_speed