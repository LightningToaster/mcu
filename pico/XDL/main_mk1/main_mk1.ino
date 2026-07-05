#define SETUP_MESSAGE "XDL@RP2040 (Valkor 2025-11-28)" 
/*TODO

bring in the data
*/
//#define DEBUG //if defined:serial output
#define ARMED_WHEELS  //comment to disable wheels (ESCs will still arm but NOT spin)
#define ARMED_SOLENOID //comment to disable solenoid (solenoid will NOT actuate)

#define PIN_BUILTIN_LED 25
#define PIN_TRIGGER 2
#define PIN_TRIGGER_B 1
#define PIN_TRIGGER_REV 3
#define PIN_SWITCH 14
#define PIN_VOLTMETER 29
#define PIN_DART_DETECTOR 28
#define PIN_ESC_0 12 //controls left motor
#define PIN_ESC_1 13 //controls right motor
#define PIN_SOLENOID 11
#define PIN_OLED_SDA 8
#define PIN_OLED_SCL 9

#include "step_button.hpp"
StepButton trigger_main(PIN_TRIGGER, PIN_TRIGGER_B);
StepButton trigger_rev(PIN_TRIGGER_REV);

#include "switch.hpp"
Switch slide_switch(PIN_SWITCH);

#include "voltmeter.hpp"
Voltmeter voltmeter(PIN_VOLTMETER);

#include "dart_detector.hpp"
DartDetector dart_detector(PIN_DART_DETECTOR);

#include "wheels.hpp"
Wheels wheels(PIN_ESC_0, PIN_ESC_1);

#include "solenoid.hpp"
Solenoid solenoid(PIN_SOLENOID);

#include "oled.hpp"
OledDisplay oled(PIN_OLED_SDA, PIN_OLED_SCL);

bool oled_connected;

void setup() {
  Serial.begin(9600);
  oled_connected = oled.begin();
  if (oled_connected){
    oled.display_logo(0);
  }
  delay(1000);
}//setup

void loop() {
  bool voltage_ok = voltmeter.operate();
  bool wheels_revd = wheels.operate();
  bool dart_was_fired = solenoid.operate(wheels_revd);
  int8_t dart_status = dart_detector.operate();
  int8_t status_switch = slide_switch.operate();
  int8_t status_trigger_main = trigger_main.operate();
  int8_t status_trigger_rev = trigger_rev.operate();

  static uint16_t darts_fired_mag = 0;
  static uint32_t darts_fired_session = 0;
  

  if (dart_was_fired){
     darts_fired_mag++;
     darts_fired_session++;
     //TODO increment all time shot counter
  }
 
  if (status_switch == SWITCH_IS_OPENED and false){//TODO after menu added, make sure oled connected
    //TODO menu
    solenoid.stop();
    wheels.stop();
    oled.display_menu();

  }else{//fire mode
  
    if (voltage_ok){
      if (dart_status == DART_READY){
        if (status_trigger_rev == BUTTON_HOLD
        ||status_trigger_main == BUTTON_HOLD
        ||status_trigger_main == BUTTON_DEEP_HOLD
        ||digitalRead(6) == LOW)//TEMP for capacitive
        {
          if (oled_connected and false){//TODO after menu added
            //wheels.set_throttle_percentage(35);//TODO use data
          }else{//no OLED, use switch for 2 FPS settings
            if (status_switch == SWITCH_IS_CLOSED){
              wheels.set_throttle_percentage(64);//high FPS //63
            }else{
              wheels.set_throttle_percentage(30);//low FPS
            }
          }
          
        }else{
          wheels.idle();
        }

        if (status_trigger_main == BUTTON_CLICK
        || status_trigger_main == BUTTON_DEEP_HOLD)
        {
          solenoid.fire(1);//TODO data volley
        }
        oled.display_ammo(darts_fired_mag, darts_fired_session);

      }else if (dart_status == MAG_EMPTY){
        solenoid.stop();
        wheels.stop();
        oled.display_empty(voltmeter.get_string(CELL));

      }else if (dart_status == NEW_MAG){
        darts_fired_mag = 0;

      }else if (dart_status == DART_NONE){
        wheels.set_throttle_percentage(0);
        solenoid.stop();
      }//dart_status?
        
      
    }else{//voltage_ok?
      solenoid.stop();
      wheels.set_throttle_percentage(0);
      oled.display_battery(voltmeter.get_status(), 
          voltmeter.get_string(CELL) + "   " + voltmeter.get_string(PACK) + "   " + voltmeter.get_string(PERCENTAGE));
    }//voltage_ok?

  }

}//loop