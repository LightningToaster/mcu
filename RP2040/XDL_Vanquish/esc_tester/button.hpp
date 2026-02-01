// Valkor 2024-08-03

#define BUTTON_NO_INPUT 0
#define BUTTON_PRESSED 1 //its just pressed, no minimum ms, no release
#define BUTTON_CLICK 2 //briefly clicked
#define BUTTON_LONG_CLICK 3 //long clicked
#define BUTTON_LONG_HOLD 4 //longer clicked, but debounce not required.  triggers at set ms
#define BUTTON_LONG_HOLD_CONTINUOUS 5 //triggers repeadidly after set ms

#define MIN_MS_CLICK 10
#define MIN_MS_LONG_CLICK 250
#define MIN_MS_LONG_HOLD 1000
#define MIN_MS_BETWEEN_PRESSES 50
#define MS_LONG_HOLD_REPEAT_START 1000
#define MS_LONG_HOLD_REPEAT_END 5
#define MS_LONG_HOLD_REPEAT_JOURNEY 5000
#include <Pushbutton.h>

class Button{ 
public:
  Pushbutton* button;
  Button(uint8_t pin){
    button = new Pushbutton(pin);
  }//constructor
  
  uint8_t operate(){
    uint8_t r = BUTTON_NO_INPUT;
    if (button->isPressed()
    and millis() >= last_press_ms + MIN_MS_BETWEEN_PRESSES){
      r = BUTTON_PRESSED;
      if (button_ms == 0){
        button_ms = millis();//record new press time
      }//
    }//isPressed
    if (button->getSingleDebouncedRelease()){
      if (millis() >= last_press_ms + MIN_MS_BETWEEN_PRESSES){
        if (skip_next_debounce){
          skip_next_debounce = false;
          long_hold_repeat_current_ms_setting = MS_LONG_HOLD_REPEAT_START;
        }else{
          if (millis() >= button_ms + MIN_MS_LONG_CLICK){//hold
            r = BUTTON_LONG_CLICK;
          }else if (millis() >= button_ms + MIN_MS_CLICK){//click
            r = BUTTON_CLICK;
          }//hold or click?
        }//skip_next_debounce?
        button_ms = 0;
        last_press_ms = millis();
      }//long enough to registor as any form of press?
      
    }else if (millis() >= button_ms + MIN_MS_LONG_HOLD 
    and button_ms != 0){

      if (skip_next_debounce == false){
        r = BUTTON_LONG_HOLD;
        skip_next_debounce = true;
      }else{//continuous
        if (long_hold_repeat_current_ms_setting > 0 
        and millis() >= button_ms + MIN_MS_LONG_HOLD + long_hold_repeat_current_ms_setting){
          button_ms = button_ms + long_hold_repeat_current_ms_setting;
          r = BUTTON_LONG_HOLD_CONTINUOUS;
          long_hold_repeat_current_ms_setting = move_towards_non_linear(long_hold_repeat_current_ms_setting, MS_LONG_HOLD_REPEAT_END, 0.12);
          //Serial.println(long_hold_repeat_current_ms_setting);
        }
      }
    }//ifs
    return r;
  }//end operate()

  void clear(){
    button_ms = 0;
    last_press_ms = millis();
  }//clear
  
private:
  unsigned long button_ms = 0;
  unsigned long last_press_ms = 0;
  int16_t long_hold_repeat_current_ms_setting = MS_LONG_HOLD_REPEAT_START;
  bool skip_next_debounce = false;

  int16_t move_towards_non_linear(int16_t current, int16_t target, float base_factor) {
    int32_t difference = target - current;
    uint32_t max_value = max(current, target);
    float scaled_difference = (float)(difference * difference);
    float adjustment_factor = base_factor * scaled_difference / (float)(max_value * max_value);
    int16_t adjustment = (int16_t)(adjustment_factor * difference);
    uint16_t new_value = current + adjustment;
    return new_value;
  }
};//class
