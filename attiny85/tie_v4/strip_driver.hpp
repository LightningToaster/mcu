// Valkor 2021-12-04

const uint8_t BRIGHTNESS_PERCENTAGES[] = {10,25,50,75,100}; //get multiplied by COLORS

const uint8_t COLORS[][3] = { // RGB 0-31
  {31,0,0},//RED
  {0,22,0},//GREEN
  {0,0,26},//BLUE
  {24,13,0},//ORANGE
  {14,0,24},//PURPLE
  {0,19,19},//CYAN
  {15,15,15},//WHITE
}; //each diode, for color mixing

//modes
#define NUM_MODES 3
#define MODE_STATIC 0
#define MODE_GLISTEN 1
#define MODE_ASCENDING 2
//TODO sparkle mode?

//Strip needs these
#define NUM_PIXELS 25

#include "strip.hpp"
#include "vcc_reader.hpp"

#include <EEPROM.h>
#define EEPROM_INDEX_COLOR 0
#define EEPROM_INDEX_MODE 1
#define EEPROM_INDEX_BRIGHTNESS 2

class Strip_Driver{ 
public:
  Strip_Driver(uint8_t pin){
    strip = new Strip(pin, NUM_PIXELS);
  }//Strip_Driver
  
  void operate(){
    strip->operate();
    if (not setup_complete){
      //run_led_test();
      uint8_t eeprom_index_color = EEPROM.read(EEPROM_INDEX_COLOR);
      if (eeprom_index_color < sizeof(COLORS)/3-1){
        index_color = eeprom_index_color;
      }

      uint8_t eeprom_index_mode = EEPROM.read(EEPROM_INDEX_MODE);
      if (eeprom_index_mode < NUM_MODES){
        index_mode = eeprom_index_mode;
      }
      
      uint8_t eeprom_index_brightness = EEPROM.read(EEPROM_INDEX_BRIGHTNESS);
      if (eeprom_index_brightness < sizeof(BRIGHTNESS_PERCENTAGES)-1){
        index_brightness = eeprom_index_brightness;
      }
      
      setup_complete = true;
    }
    
    if (is_in_menu){
      //run_menu();
    }else{
      switch(index_mode){
        case MODE_STATIC: mode_static(); break;
        case MODE_GLISTEN: mode_glisten(); break;
        case MODE_ASCENDING: mode_ascending(); break;
      }
    }
  }//operate()

  void next_setting(){
    mode_needs_update = true;
    if (is_in_menu){
      index_brightness = (index_brightness+1)%sizeof(BRIGHTNESS_PERCENTAGES);
      EEPROM.update(EEPROM_INDEX_BRIGHTNESS, index_brightness);
      show_brightness();
    }else{
      index_color = (index_color+1)%(sizeof(COLORS)/3);
      EEPROM.update(EEPROM_INDEX_COLOR, index_color);
    }
  }//next_setting

  void next_mode(){
    index_mode = (index_mode+1)%(NUM_MODES);
    EEPROM.update(EEPROM_INDEX_MODE, index_mode);
    mode_needs_update = true;
  }//next_mode

  void toggle_menu(){
    is_in_menu = not is_in_menu;
    if (is_in_menu){
      strip->set_all(0,0,0);
      show_brightness();
      show_voltage();
      
    }
  }//toggle_menu
  
private:
  Strip* strip;
  uint8_t index_color = 0;
  uint8_t index_mode = 0;
  uint8_t index_brightness = 0;
  bool setup_complete = false;
  bool is_in_menu = false;
  bool mode_needs_update = true;
  unsigned long ms = 0;

  void mode_static(){
    if (mode_needs_update){
      uint8_t r = COLORS[index_color][0] * BRIGHTNESS_PERCENTAGES[index_brightness]/100;
      uint8_t g = COLORS[index_color][1] * BRIGHTNESS_PERCENTAGES[index_brightness]/100;
      uint8_t b = COLORS[index_color][2] * BRIGHTNESS_PERCENTAGES[index_brightness]/100;
      strip->set_all_goal(r,g,b);
      mode_needs_update = false;
    }//if
  }//mode_static

  void mode_glisten(){
    if (millis()-ms < 200){ return;}
    ms = millis();
    if (mode_needs_update){
      strip->set_all(0,0,0);
      mode_needs_update = false;
    }
    uint8_t led = random(0,NUM_PIXELS);
    uint8_t r = COLORS[index_color][0] * BRIGHTNESS_PERCENTAGES[index_brightness]/100;
    uint8_t g = COLORS[index_color][1] * BRIGHTNESS_PERCENTAGES[index_brightness]/100;
    uint8_t b = COLORS[index_color][2] * BRIGHTNESS_PERCENTAGES[index_brightness]/100;
    strip->set_goal(led, r,g,b);

    for (int i = 0; i<3; i++){//attempt to turn some off
      uint8_t led = random(0,NUM_PIXELS);
      strip->set_goal(led, 0,0,0);
    }
  }//mode_glisten()

 int8_t effect_progress = NUM_PIXELS-1;
  void mode_ascending(){
    if (millis()-ms < 40){ return;}
    ms = millis();
    
    uint8_t r = COLORS[index_color][0] * BRIGHTNESS_PERCENTAGES[index_brightness]/100;
    uint8_t g = COLORS[index_color][1] * BRIGHTNESS_PERCENTAGES[index_brightness]/100;
    uint8_t b = COLORS[index_color][2] * BRIGHTNESS_PERCENTAGES[index_brightness]/100;
    strip->set(effect_progress, r,g,b);
    strip->set_goal(effect_progress+1, 0,0,0);

    if (effect_progress <= 0){
      effect_progress = NUM_PIXELS;
    }
    effect_progress--;
  }//mode_ascending
  
  void show_brightness(){
    for (int i=NUM_PIXELS-1; i>=NUM_PIXELS-sizeof(BRIGHTNESS_PERCENTAGES); i--){
      if (i>=NUM_PIXELS-index_brightness-1){
        strip->set_goal(i,3,4,3,true);
      }else{
         strip->set_goal(i,1,0,0,true);
      }
    }
  }//show_brightness

  void show_voltage(){
    int16_t percentage = (read_vcc()-3500)/7;
    if (percentage < 0){ percentage = 0;}
    if (percentage > 100){ percentage = 100;}
    uint8_t r = 0;
    uint8_t g = percentage;
    if (percentage <= 75){
      r = 100-percentage;
    } 
    strip->set_goal(0,r/10,g/10,0);
  }//show_voltage

 

  
//  void run_led_test(){
//    uint16_t delay_ms = 1000;
//    strip->set_all(3,0,0); strip->operate(true); delay(delay_ms);
//    strip->set_all(0,3,0); strip->operate(true); delay(delay_ms);
//    strip->set_all(0,0,3); strip->operate(true); delay(delay_ms);
//    strip->set_all(0,0,0); strip->operate(true);
//  }//run_led_test
};//class
