//XDL firmware 2026-7-16 -valkor
/*TODO verify works without OLED
add IR output to info screen
compensate power with voltage
compensate solenoid timings with voltage?
integrate UV into wheel class?  if becomes issue, then separate
revisit buzzer using blinkometer v3 class
test loop timing make sure fast
add voltage to main darts fired screen?
investigate low voltage, does it flip back and forth?  does it need a flag?
*/
#define BLASTER_NAME "BLASTOMATIC MK2"
#define BLASTER_OWNER "Valkor"
#define BLASTER_SERIAL "#0002"
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

#define MAX_POWER 100
#define RESET_SAVE false  //set true to override save with defaults
#define IGNORE_DART_DETECTION true

static constexpr char pin_combo[] = "123";
#define PIN_LENGTH 3    //must be the length of pin_combo
bool is_locked = true;  //set to false to disable pinlock

#define ARMED false // wheels and solenoid will only operate if this is true

#include <Arduino.h>

#include "core1.hpp"
Core1 core;

#include "buzzer.hpp"
Buzzer buzzer(PIN_BUZZER);

#include "battery.hpp"
Battery battery(PIN_BATTERY);

#include "led.hpp"
LED led;

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

bool oled_connected = false;  // TODO not used yet, move to core


void setup() {
  Serial.begin(115200);
  while (!Serial && millis() <= 1000) { delay(50); }
  analogWrite(PIN_UV, 0);  //TEMP so it stop blinking
  core.begin(PIN_SDA, PIN_SCL);
  core.display_mode = display_logo;  // TODO startup splash? logo?
  core.randomize_starting_pin();
  led.set(0, 0, 0, 0);
  if (is_locked == false){
    core.display_mode = display_menu;
  }
  delay(1000);  //TODO consider when adding wheels
}  //setup

TRIGGER_STATE last = TRIGGER_IDLE;

void loop() {
  MENU_SWITCH_STATE menu_switch_state = menu_switch.operate();
  TRIGGER_STATE trigger_state = trigger.operate();
  // if (trigger_state != last){
  //     Serial.print(millis());
  //     Serial.print("  ");
  //     Serial.println(trigger_state);
      
  //     last = trigger_state;
  // }

  // delay(10);
  // return;


  buzzer.operate();
  battery.operate();
  battery.get_string(core.voltage, sizeof(core.voltage), CELL);
  led.operate();
  DART_STATE dart_status = dart.operate();
  bool wheels_revd = wheels.operate();
  bool dart_was_fired = solenoid.operate(wheels_revd);
  if (dart_was_fired) { core.fired_from_mag++; }


  if (is_locked == true) {
    core.display_mode = display_lock;
    if (pin_lock.is_ready() == true) {
      core.pin_failed = false;
      if (trigger_state == TRIGGER_TOUCH_END) {
        buzzer.beep(120, 30);
        core.selected_char = (core.selected_char + 1) % PIN_LENGTH;
      } else if (trigger_state == TRIGGER_SHALLOW_END) {  //TODO ok that totally needs to be a core method()
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
          led.set(0, 100, 0, 0);
          led.set(0, 0, 0, 300);
        } else {  //wrong pin entered
          core.pin_failed = true;
          core.settings.failed_logins++;
          core.randomize_starting_pin();
          buzzer.beep(80, 500);
          led.set(255, 0, 0, 0);
          led.set(0, 0, 0, 500);
        }//pin correct?
      }//deep click?  (pin attempt)
    }//pin_lock.is_ready()  (cooldown done?)
    return;//restart loop()
  }//is_locked?

  if (menu_switch_state == SWITCH_CLOSED_ACTION) {
    core.selection = 0;  //profile is selected by default
    core.display_mode = display_menu;
    //TODO stop solenoid and motors!
  }

  if (menu_switch_state == SWITCH_CLOSED or menu_switch_state == SWITCH_CLOSED_ACTION) {
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
            core.display_mode = display_info;
          }

        case TRIGGER_SHALLOW_INCREMENT:
          if (core.selection == 2){ //power
            core.next_power(5);
            buzzer.beep(260, 30);
          }
        break;

      }//trigger_state

    } else if (core.display_mode == display_info) {
      if (trigger_state == TRIGGER_DEEP_END) {
        core.display_mode = display_menu;
      }
    }//if display_menu

  }else{//FIRING MODE

    //TODO very wip

    if (battery.is_ok() or ARMED == false) {
      core.display_mode = display_ammo;
      if (dart_status == DART_READY or IGNORE_DART_DETECTION) {
        switch (trigger_state) {
          case TRIGGER_IDLE: wheels.idle();break;
          default:wheels.set_throttle_percentage(core.get_power());break;
        } //any trigger interaction?

        switch (trigger_state) {
        case TRIGGER_SHALLOW_START:
            solenoid.fire(1);
        break;

        case TRIGGER_DEEP_ONGOING:
        case TRIGGER_DEEP_INCREMENT:
        case TRIGGER_DEEP_START:
          solenoid.fire(core.get_volley());//keeps shooting till default 
        break;
        
        default:
          if (core.get_volley() == 3){
            solenoid.stop();
          }
        break;

        }  //switch
      }else{// no dart
        
        if (dart_status == MAG_EMPTY){
          solenoid.stop();
          wheels.stop();
          //TODO
          //oled.display_empty(voltmeter.get_string(CELL));
        }else if (dart_status == NEW_MAG){
          core.fired_from_mag = 0;
        }else if (dart_status == DART_NONE){
          Serial.println("no dart");
          wheels.set_throttle_percentage(0);//from MK1, needed?
          solenoid.stop();
        }
        
      }//dart_status?


    }else{//voltage not ok
      Serial.println("badv");
      solenoid.stop();
      wheels.set_throttle_percentage(0);
      //TODO
      //oled.display_battery(voltmeter.get_status(),
      //    voltmeter.get_string(CELL) + "   " + voltmeter.get_string(PACK) + "   " + voltmeter.get_string(PERCENTAGE));
    }


  }//is_menu?
  
}//loop

/*random code dont know if needed
core.display_mode = display_battery;
                core.battery_state = battery_disconnected;
                snprintf(core.voltage, sizeof(core.voltage), "0.0V");
*/

