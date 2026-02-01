#define SETUP_MESSAGE "tie_v4@attiny85 (designed by Valkor 2021-12-04)"
//v4 is just a software update (total rewrite)

#define PIN_BUTTON 0
#define PIN_LEDS 3

#include "button.hpp" 
Button button(PIN_BUTTON);
#include "strip_driver.hpp" 
Strip_Driver driver(PIN_LEDS);

void setup() {

}//setup

void loop() {
  
  uint8_t input = button.operate();
  driver.operate();

  switch(input){
    case BUTTON_CLICK: driver.next_setting(); break;
    case BUTTON_HOLD: driver.next_mode(); break;
    case BUTTON_LONG_HOLD: driver.toggle_menu(); break;
  }
}//loop
