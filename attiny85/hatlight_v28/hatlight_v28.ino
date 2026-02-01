// Hatlight v28 by Valkor Simpson 2020-11-04 (~478 lines)

#define RED_INDICATOR_PIN 0
#define GREEN_INDICATOR_PIN 4
#define MAIN_LIGHT_PIN 1
#define BUTTON_PIN_LEFT 2
#define BUTTON_PIN_RIGHT 3


#include "IndicatorLed.hpp"
IndicatorLed red_indicator(RED_INDICATOR_PIN);
IndicatorLed green_indicator(GREEN_INDICATOR_PIN);

#include "Flashlight.hpp"
Flashlight light(MAIN_LIGHT_PIN);

#include "Button.hpp"
Button button_left(BUTTON_PIN_LEFT);
Button button_right(BUTTON_PIN_RIGHT);

#include "Sleeper.hpp"

#include "VccReader.hpp"
bool voltage_enabled = true;

void setup() {
  show_voltage();
}// end setup

void loop() {
  light.operate();
  red_indicator.operate();
  green_indicator.operate(); 
  uint8_t button_value_left = button_left.operate();
  uint8_t button_value_right = button_right.operate();

  switch(button_value_left){
    case BUTTON_CLICK:// SLEEP
      light.sleep_animation();
      show_voltage();
      sleep();//yields (low power) until BUTTON_PIN_LEFT is pressed
      light.wake_animation();
      show_voltage();
    break;
    case BUTTON_HOLD://toggle voltage indicator
      voltage_enabled = !voltage_enabled;
      show_voltage();
    break;
    case BUTTON_LONG_HOLD:// DEEP SLEEP
      light.deep_sleep_animation();
      do{//enter sleep cycle
        sleep();
        red_indicator.disable();
        green_indicator.disable();
        light.set_brightness(3);//on while waiting for hold
        unsigned long temp_awake_ms = millis();
        while(millis() - temp_awake_ms < 3000){//try constantly for 3 seconds
          button_value_left = button_left.operate();
          if (button_value_left == BUTTON_LONG_HOLD){
            button_value_left = 0;
            light.deep_wake_animation();
            delay(400);
            show_voltage();
            break;
          }//if
        }//while
        light.set_brightness(0);//no hold, turn it back off
        show_voltage();
      }while(light.get_activity_mode() == ACTIVITY_DEEP_SLEEP);
    break;
  }

  switch(button_value_right){
    case BUTTON_CLICK:
      light.change_brightness();
    break;
    case BUTTON_HOLD:
      light.change_mode();
    break;
    case BUTTON_LONG_HOLD:
      //UNUSED (maybe spooky mode for eyes attachment?
      light.set_brightness(1);//super dim glow
    break;
  }

}//end loop


void show_voltage(){
  if (voltage_enabled == false) {
    red_indicator.disable();
    green_indicator.disable();
    return;
  }
  long v = read_vcc();

  if (v >= 4400){//on programmer
    red_indicator.enable();
    green_indicator.enable();
  }else{//on battery
    if (v >= 3900){
      red_indicator.disable();
      green_indicator.enable();
    }else if (v >= 3600){// medium (3.6v - 3.99_v)
      red_indicator.enable();
      green_indicator.enable();
    }else{
      red_indicator.enable();
      green_indicator.disable();
    }
  }
}// end show_voltage
