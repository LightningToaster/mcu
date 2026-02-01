// Hatlight v29 by Valkor Simpson 2020-12-10 (400 lines)

#define MAIN_LIGHT_PIN 1
#define SLEEP_PIN PCINT3
#define BUTTON_PIN_LEFT 3 //change sleeper line 7 & 16 if you change this
#define BUTTON_PIN_RIGHT 0


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
    case BUTTON_LONG_HOLD:
      //unused
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
      light.set_brightness(1);//super dim glow
    break;
  }

}//end loop
