#define SETUP_MESSAGE "ShadowCaster_v5.2@atmega328p (designed by Valkor 2021-08-15)"
//5.2 is 5.1 converted from attiny85 to atmega328p (pro mini)
//some other features and refactors were also added
//TODO
//hold blue could do blink affect?

#define PIN_BUTTONS A7
#define PIN_LEDS 2
#define PIN_SPEAKER 10

#include "multibutton.hpp" 
Buttons buttons(PIN_BUTTONS);
#include "eye_driver.hpp" 
Eye_Driver eyes(PIN_LEDS);


void setup() {
  Serial.begin(9600); while (! Serial); Serial.print(SETUP_MESSAGE);
}//setup

void loop() {
  
  struct buttons_state input = buttons.operate();
  eyes.operate();

  switch(input.action){
    case BUTTON_CLICK: 
        Serial.println(String(input.button) + " clicked");
      switch(input.button){
        case BUTTON_RED: eyes.select_color(RED); break;
        case BUTTON_GREEN: eyes.select_color(GREEN); break;
        case BUTTON_BLUE: eyes.select_color(BLUE); break;
        case BUTTON_TOGGLE: // on/off button
          eyes.toggle();
        break;
        case BUTTON_NEXT: eyes.next_color(); break;
      }

      break;
    case BUTTON_HOLD:
        Serial.println(String(input.button) + " held");
      switch(input.button){
        case BUTTON_RED: eyes.next_brightness(); break;
        case BUTTON_GREEN: eyes.clear_selection(); break;
        case BUTTON_TOGGLE: 
          eyes.flash(); 
        break;
        case BUTTON_NEXT: eyes.next_brightness(); break;
      }
    break;
  }
}//loop
