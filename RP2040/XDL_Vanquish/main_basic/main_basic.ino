#define SETUP_MESSAGE "XDL@RP2040 (Valkor 2025-9-20)" 

//#define DEBUG //if defined:serial output
#define ARMED_WHEELS  //comment to disable wheels (ESCs will still arm but NOT spin)
#define ARMED_SOLENOID //comment to disable solenoid (solenoid will NOT actuate)

#define PIN_SOLENOID 11
#define PIN_ESC_0 12 //controls left motor
#define PIN_ESC_1 13 //controls right motor
#define PIN_SWITCH 14
#define PIN_TRIGGER 2
#define PIN_TRIGGER_B 1
#define PIN_TRIGGER_REV 3
#define PIN_BUILTIN_LED 25


#include "wheels.hpp"
Wheels wheels(PIN_ESC_0, PIN_ESC_1);//TODO pins input?

#include "solenoid.hpp"
Solenoid solenoid(PIN_SOLENOID);//TODO pins input?

#include "switch.hpp"
Switch slide_switch(PIN_SWITCH);

#include "step_button.hpp"
StepButton trigger_main(PIN_TRIGGER, PIN_TRIGGER_B);
StepButton trigger_rev(PIN_TRIGGER_REV);

void setup() {
  Serial.begin(9600);
  delay(500);Serial.println(SETUP_MESSAGE);
}//setup

int8_t status_switch;
int8_t status_trigger_main;
int8_t status_trigger_rev;

void loop() {
  wheels.operate();
  solenoid.operate(true);//pull and release of the trigger should allow enough time?

  status_switch = slide_switch.operate();
  status_trigger_main = trigger_main.operate();
  status_trigger_rev = trigger_rev.operate();

  if (status_switch == SWITCH_IS_CLOSED){
    if (status_trigger_rev == BUTTON_HOLD
    ||status_trigger_main == BUTTON_HOLD
    ||status_trigger_main == BUTTON_DEEP_HOLD)
    {
      wheels.set_speed(9);
    }else{
      wheels.idle();
    }

    if (status_trigger_main == BUTTON_CLICK
    || status_trigger_main == BUTTON_DEEP_HOLD)
    {
      solenoid.fire(1);
    }
  }else{
    solenoid.stop();
    wheels.stop();
  }

}//loop