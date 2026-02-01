// Valkor Simpson 2020-11-5 

#define BUTTON_CLICK 1
#define BUTTON_HOLD 2
#define BUTTON_LONG_HOLD 3

#define MIN_CLICK_MS 20
#define MIN_HOLD_MS 250
#define MIN_LONG_HOLD_MS 1000
#define MIN_MS_BETWEEN_PRESSES 100

#include <Pushbutton.h>

class Button{ 
public:
  Pushbutton* button;
  Button(uint8_t pin){
    button = new Pushbutton(pin);
  }//end Button
  
  uint8_t operate(){
    uint8_t r = 0;
    if (button->isPressed() 
    and button_ms == 0
    and millis() - last_press_ms >= MIN_MS_BETWEEN_PRESSES){
      button_ms = millis();//record new press time
    }
    if (button->getSingleDebouncedRelease()){
      if (millis() - last_press_ms >= MIN_MS_BETWEEN_PRESSES){
        if (skip_next_debounce){
          skip_next_debounce = false;
        }else{
          if (millis() - button_ms >= MIN_HOLD_MS){//hold
            r = BUTTON_HOLD;
          }else if (millis() - button_ms >= MIN_CLICK_MS){//click
            r = BUTTON_CLICK;
          }
        }
        button_ms = 0;
        last_press_ms = millis();
      }
      
    }else if (millis() - button_ms >= MIN_LONG_HOLD_MS 
    and button_ms != 0
    and skip_next_debounce == false){
      r = BUTTON_LONG_HOLD;
      skip_next_debounce = true;
    }
    return r;
  }//end operate()

  void clear(){
    button_ms = 0;
    last_press_ms = millis();
  }
  
private:
  unsigned long button_ms = 0;
  unsigned long last_press_ms = 0;
  bool skip_next_debounce = false;
};//end class Button
