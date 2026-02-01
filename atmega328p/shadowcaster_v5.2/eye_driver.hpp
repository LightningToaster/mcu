// Valkor 2021-02-28

const uint8_t BRIGHTNESS_PERCENTAGES[] = {10,40,70,100}; //get multiplied by COLORS

const uint8_t COLORS[][3] = { // RGB 0-31
  {31,0,0},//RED
  {0,24,0},//GREEN
  {0,0,27},//BLUE
  {28,9,0},//ORANGE
  {14,0,24},//PURPLE
  {0,19,19},//CYAN
  {20,20,20},//WHITE
  //{0,0,0},//OFF
}; //each diode, for color mixing

#define RED 0
#define GREEN 1
#define BLUE 2
#define ORANGE 3 
#define PURPLE 4 
#define CYAN 5
#define WHITE 6
#define OFF 7

#define BUTTON_RED 0
#define BUTTON_GREEN 1
#define BUTTON_BLUE 2
#define BUTTON_TOGGLE 3
#define BUTTON_NEXT 4

//Strip needs these
#define NUM_PIXELS 1
#define MAX_BRIGHTNESS 255 //for each of the 3 pixels in a chip

#include "strip.hpp"

#include "speaker.hpp"
Speaker speaker(PIN_SPEAKER);

#include <EEPROM.h>
#define EEPROM_INDEX_COLOR 0
#define EEPROM_INDEX_BRIGHTNESS 1

class Eye_Driver{ 
public:
  Eye_Driver(uint8_t pin){
    strip = new Strip(pin, 2);
  }//Eye_Driver()
  
  void operate(){
    
    strip->operate();
    speaker.operate();
    if (not setup_complete){
      //run_led_test();
      uint8_t eeprom_index_color = EEPROM.read(EEPROM_INDEX_COLOR);
      if (eeprom_index_color < sizeof(COLORS)/3-1){
        index_color = eeprom_index_color;
      }
      uint8_t eeprom_index_brightness = EEPROM.read(EEPROM_INDEX_BRIGHTNESS);
      if (eeprom_index_brightness < sizeof(BRIGHTNESS_PERCENTAGES)){
        index_brightness = eeprom_index_brightness;
      }
      speaker.play_power_up_sound();
      setup_complete = true;
      enable();
    }
  }//operate()

  void select_color(uint8_t i){//0,1,2
    if (colors_selected[i] == false){
      colors_selected[i] = true;
      speaker.beep(250, 3);
    }else{
      display();
      speaker.beep(500, 7);
    }
  }//select_color()

  void display(){
    is_on = true;
    gset_index_color();
    enable();
  }//display()

  void enable(){
    strip->set_all(
      COLORS[index_color][RED] * (BRIGHTNESS_PERCENTAGES[index_brightness] / 100.0), 
      COLORS[index_color][GREEN] * (BRIGHTNESS_PERCENTAGES[index_brightness] / 100.0), 
      COLORS[index_color][BLUE] * (BRIGHTNESS_PERCENTAGES[index_brightness] / 100.0)
    );
    EEPROM.update(EEPROM_INDEX_COLOR, index_color);
  }//enable()

  void disable(){
    strip->set_all(0,0,0);
  }//disable()

  bool is_enabled(){
    return is_on;
  }//is_enabled()

  void toggle(){
    this->is_on = !this->is_on;
    if (this->is_on){
       enable();
       uint16_t frequencies[] = {300, 0, 800, 0, 0};
       speaker.set_queue_frequencies(sizeof(frequencies)/sizeof(frequencies[0]), frequencies);
    }else{
      disable();
      uint16_t frequencies[] = {800, 0, 300, 0, 0};
      speaker.set_queue_frequencies(sizeof(frequencies)/sizeof(frequencies[0]), frequencies);
    }
    int8_t queue[] = {6, -4, 9, 0, 0};
    speaker.set_queue(sizeof(queue)/sizeof(queue[0]), queue);
  }//toggle()

  void flash(uint8_t r = MAX_BRIGHTNESS, uint8_t g = MAX_BRIGHTNESS, uint8_t b = MAX_BRIGHTNESS){
    
    strip->set_all(r,g,b, true, true);//flash white
    disable();
    display();
    uint16_t frequencies[] = {900, 0, 900, 0, 900};
    speaker.set_queue_frequencies(sizeof(frequencies)/sizeof(frequencies[0]), frequencies);
    int8_t queue[] = {9, -4, 9, -4, 9};
    speaker.set_queue(sizeof(queue)/sizeof(queue[0]), queue);
  }//flash()
  
  void next_brightness(){
    index_brightness = (index_brightness+1)%sizeof(BRIGHTNESS_PERCENTAGES);
    
    uint16_t frequencies[] = {300, 0, 0, 0, 0};
    int8_t queue[] = {14, -4, 0, -4, 0};
    if (index_brightness >= 1){
      frequencies[2] = 700;
      queue[2] = 12;
    }
    if (index_brightness == 2){
      frequencies[4] = 1200;
      queue[4] = 12;
    }else if(index_brightness >= 3){
      frequencies[4] = 1400;
      queue[4] = 20;
    }

    speaker.set_queue_frequencies(sizeof(frequencies)/sizeof(frequencies[0]), frequencies);
    speaker.set_queue(sizeof(queue)/sizeof(queue[0]), queue);

    enable();
    EEPROM.update(EEPROM_INDEX_BRIGHTNESS, index_brightness);
  }//next_brightness()

  void clear_selection(){
    colors_selected[RED] = false;
    colors_selected[GREEN] = false;
    colors_selected[BLUE] = false;
    uint16_t frequencies[] = {900, 0, 400, 0, 400};
    speaker.set_queue_frequencies(sizeof(frequencies)/sizeof(frequencies[0]), frequencies);
    int8_t queue[] = {9, -4, 6, -4, 6};
    speaker.set_queue(sizeof(queue)/sizeof(queue[0]), queue);
  }//clear_selection()

  void next_color(){//used for single button 
    index_color = (index_color+1)%(sizeof(COLORS)/3);
    is_on = true; // needed?
    enable();
    speaker.beep(500, 7);
  }//next_color()
  
private:
  Strip* strip;
  uint8_t colors[3] = {0,0,0};
  bool colors_selected[3];
  uint8_t index_color = 0;
  bool is_on = false;
  uint8_t index_brightness = 0;
  bool setup_complete = false;

  void gset_index_color(){
    if (colors_selected[RED]){
      if (colors_selected[GREEN]){
        if (colors_selected[BLUE]){
          index_color = WHITE;
        }else{
          index_color = ORANGE;
        }
      }else if (colors_selected[BLUE]){
        index_color = PURPLE;
      }else{
        index_color = RED;
      }
    }else if(colors_selected[GREEN]){
      if (colors_selected[BLUE]){
          index_color = CYAN;
        }else{
          index_color = GREEN;
        }
    }else if (colors_selected[BLUE]){
      index_color = BLUE;
    }else{
      index_color = OFF;
    }
    colors_selected[RED] = false;
    colors_selected[GREEN] = false;
    colors_selected[BLUE] = false;
  }//gset_index_color


//  void run_led_test(){
//    uint16_t delay_ms = 1000;
//    strip->set_all(3,0,0); strip->operate(true); delay(delay_ms);
//    strip->set_all(0,3,0); strip->operate(true); delay(delay_ms);
//    strip->set_all(0,0,3); strip->operate(true); delay(delay_ms);
//    strip->set_all(0,0,0); strip->operate(true);
//  }//run_led_test
};//class
