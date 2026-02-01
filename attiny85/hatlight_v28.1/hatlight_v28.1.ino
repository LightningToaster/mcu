// Hatlight v28.1 by Valkor Simpson 2020-12-10 (~? lines)

#define MAIN_LIGHT_PIN 1
#define BUTTON_PIN_LEFT 0 //change sleeper if you change this
#define BUTTON_PIN_RIGHT 2

#include "Flashlight.hpp"
Flashlight light(MAIN_LIGHT_PIN);

#include "Button.hpp"
Button button_left(BUTTON_PIN_LEFT);
Button button_right(BUTTON_PIN_RIGHT);

#include "Sleeper.hpp"

#include "VccReader.hpp"

void setup() {
  light.show_battery_level(read_vcc());
}// end setup

void loop() {
  light.operate();
  uint8_t button_value_left = button_left.operate();
  uint8_t button_value_right = button_right.operate();

  switch(button_value_left){
    case BUTTON_CLICK:// SLEEP
      light.sleep_animation();
      light.show_battery_level(read_vcc());
      sleep();//yields (low power) until BUTTON_PIN_LEFT is pressed
      light.wake_animation();
      button_left.clear();
    break;
    case BUTTON_HOLD:
      light.show_battery_level(read_vcc());
    break;
    case BUTTON_LONG_HOLD:// DEEP SLEEP
      light.deep_sleep_animation();
      do{//enter sleep cycle
        sleep();
        light.set_brightness(3);//on while waiting for hold
        unsigned long temp_awake_ms = millis();
        while(millis() - temp_awake_ms < 3000){//try constantly for 3 seconds
          button_value_left = button_left.operate();
          if (button_value_left == BUTTON_LONG_HOLD){
            button_value_left = 0;
            light.deep_wake_animation();
            delay(400);
            break;
          }//if
        }//while
        light.set_brightness(0);//no hold, turn it back off
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
