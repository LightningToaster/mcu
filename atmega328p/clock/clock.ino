#define SETUP_MESSAGE "clock@atmega328p (designed by Valkor 2024-11-29)" //519 LoC
/*
  Instructions:
  Short click to switch between Clock & Temperature display
  While in Clock, click and hold the mode button to advance time.
  While in Temperature, long click the mode button to switch between °C & °F
  While in Temperature, continuously hold the mode button to toggle the decimal place
  (only for Celsius)

  known issues:
    slight dim flicker of disabled pixels
    can be seen in very low ambient lighting
    when a digit updates.

    time-set incrementing math is a convoluted mess

*/
#define DISABLE_TEMPERATURE//comment to re-enable temperature (its inacurate though)

#define PIN_LEDS 13 //NeoPixels data input 
#define PIN_BUTTON 2 
#define TEMPERATURE_ADJUSTMENT -1

#include "button.hpp" 
Button button(PIN_BUTTON);
#include "display.hpp" 
Display display(PIN_LEDS);
#include "time.hpp" 
Time time;

#define MODE_CLOCK 1
#define MODE_TEMPERATURE 2

bool show_temperature_decimal = false;
bool convert_to_fake_units = false;// fahrenheit.. disgusting.
uint8_t mode = MODE_CLOCK;
unsigned long ms = 0;

void setup() {
  Serial.begin(9600);
}//setup

void loop() {
  display.operate();
  time.operate_adjustment();
  if (mode == MODE_TEMPERATURE){
    time.operate_temperature();
  }//if mode temp
  
  
  uint8_t button_status = button.operate();
  
  if (button_status == BUTTON_CLICK){
    if (mode == MODE_CLOCK){
      #ifndef DISABLE_TEMPERATURE
        mode = MODE_TEMPERATURE;
        display.select_color(ICE_BLUE);
        show_temperature();
      #else
        time.add_minutes(1);
      #endif
    }else if (mode == MODE_TEMPERATURE){
      mode = MODE_CLOCK;
      display.select_color(GREEN);
      show_time();
    }
  }else if (button_status == BUTTON_LONG_CLICK){
    if (mode == MODE_TEMPERATURE){
      convert_to_fake_units = !convert_to_fake_units;
      if (convert_to_fake_units){
        display.select_color(MEH_BLUE);
      }else{
        display.select_color(ICE_BLUE);
      } 
      show_temperature();
    }else{
      time.add_minutes(-1);
    }
  }else if (button_status == BUTTON_LONG_HOLD_CONTINUOUS){
    if (mode == MODE_CLOCK){
      time.add_minutes(1);
      show_time();

    }else if (mode == MODE_TEMPERATURE){
      show_temperature_decimal = !show_temperature_decimal;
      show_temperature();
    }
  }//if button_status

  if (millis() >= ms+1000){
    if (mode == MODE_CLOCK){
      show_time();
    }else if (mode == MODE_TEMPERATURE){
      show_temperature();
    }
    ms = millis();
  }
}//loop

void show_time(){
  display.set_digit(LEFT, time.get_hour());
  display.set_digit(RIGHT, time.get_minute());
}//show_time

void show_temperature(){ 
  float temp = time.get_temperature();
  if (convert_to_fake_units){
    temp = temp * 9.0 / 5.0 + 32.0 + 0.5; // +0.5 for rounding
  }
  display.set_digit(LEFT, uint8_t(temp));
  if (show_temperature_decimal & !convert_to_fake_units){
    display.set_digit(THIRD, uint8_t(temp*10)%10 );
  }else{
    display.set_digit(THIRD, OFF);
  }
  
  if (convert_to_fake_units){
    display.set_digit(LAST, FAHRENHEIT);// stands for "F"
  }else{
    display.set_digit(LAST, CELSIUS);// stands for "C"
  }
  
}//show_temperature