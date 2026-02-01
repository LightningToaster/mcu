// Valkor 2024-08-03

#define GREEN 0
#define DIM_GREEN 1
#define ICE_BLUE 2
#define MEH_BLUE 3
const uint8_t COLORS[][3] = {
  {3,31,10},//0  green
  {0,5,0},//1  dim green
  {12,31,31},//2  ice blue
  {17,31,24},//3  meh blue
};

#define OFF 10
#define CELSIUS 11
#define FAHRENHEIT 12
const bool NUMBERS[][7] = {
  {1,1,1,0,1,1,1},//0
  {0,0,1,0,0,0,1},//1
  {0,1,1,1,1,1,0},//2
  {0,1,1,1,0,1,1},//3
  {1,0,1,1,0,0,1},//4
  {1,1,0,1,0,1,1},//5
  {1,1,0,1,1,1,1},//6
  {0,1,1,0,0,0,1},//7
  {1,1,1,1,1,1,1},//8
  {1,1,1,1,0,1,1},//9
  {0,0,0,0,0,0,0},//OFF
  {1,1,0,0,1,1,0},//CELSIUS
  {1,1,0,1,1,0,0},//FAHRENHEIT
};

#define LEFT 0
#define RIGHT 1
#define THIRD 2
#define LAST 3

#define NUM_PIXELS 28
#include "strip.hpp"

class Display{ 
public:
  Display(uint8_t pin){
    strip = new Strip(pin, NUM_PIXELS);
  }//constructor
  
  void operate(){
    if (not setup_complete){
      test_color(31,0,0);
      test_color(0,31,0);
      test_color(0,0,31);
      
      setup_complete = true;
    }
    strip->operate();
  }//operate

  void clear(){
    strip->set_all(0,0,0);
  }//clear

  void set_digit(uint8_t position, uint8_t number){
    if (position == LEFT){
      set(0,(uint8_t)number/10);
      set(1,(uint8_t)number%10);
    }else if (position == RIGHT){
      set(2,(uint8_t)number/10);
      set(3,(uint8_t)number%10);
    }else if (position == THIRD){
      set(2,number);
    }else if (position == LAST){
      set(3,number);
    }
  }//set_digit

  void select_color(uint8_t color){
    selected_color = color;
  }//select_color
  

private:
  Strip* strip;
  bool setup_complete = false;
  uint8_t selected_color = GREEN;
  
  void set(uint8_t position, uint8_t number){
    for (int i = 0; i < 7; i++) {
      if (NUMBERS[number][i] == true){
        if (position == 0 and number == 0){
          strip->set(i+(7*position),0,12,0);//first 0 looks dimmer
        }else{
          strip->set(i+(7*position),
            COLORS[selected_color][0],
            COLORS[selected_color][1],
            COLORS[selected_color][2]);
        }
        
      }else{
        strip->set(i+(7*position),0,0,0);
      }
    }//for each segment of digit
  }//set

  void test_color(uint8_t r, uint8_t g, uint8_t b){
    for (int i = 0; i<NUM_PIXELS; i++){
      strip->set(i,r,g,b);
      strip->operate();delay(21);
      strip->operate();delay(21);
    }
    while (strip->operate() != 0) {delay(30);}
    delay(100);
  }//test_color

};//class
