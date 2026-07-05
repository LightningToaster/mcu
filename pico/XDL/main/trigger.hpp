#pragma once
//valkor 2026-7-5
//XDL custom 3 stage trigger with menu switch 
enum BUTTON_STATE : uint8_t {
  TRIGGER_IDLE = 0,
  TRIGGER_TOUCH,
  TRIGGER_TAP,
  TRIGGER_SHALLOW_CLICK,
  TRIGGER_DEEP_CLICK,
};

#define TOUCH_MIN_MICROSECONDS 1000 //filters out false positives
#define TOUCH_TAP_MIN_MILLISECONDS 25
#define TOUCH_TAP_MAX_MILLISECONDS 250
#define SHALLOW_CLICK_MIN_MILLISECONDS 25
#define DEEP_CLICK_MIN_MILLISECONDS 25
#define DEEP_CLICK_MAX_MILLISECONDS 100
#define DEEP_HOLD_MILLISECONDS 100

class Trigger {
private:

  uint8_t pin_menu;
  uint8_t pin_touch;
  uint8_t pin_shallow;
  uint8_t pin_deep;

  bool menu_mode = false;

  unsigned long us_touch = 0;
  unsigned long ms_shallow = 0;
  unsigned long ms_deep = 0;

  bool debounce_shallow = false;
  bool debounce_deep = false;
  bool ignore_next_tap = false;
  bool ignore_shallow = false;


public:

  Trigger(
    uint8_t pin_menu,
    uint8_t pin_touch,
    uint8_t pin_shallow,
    uint8_t pin_deep
  ):
    pin_menu(pin_menu),
    pin_touch(pin_touch),
    pin_shallow(pin_shallow),
    pin_deep(pin_deep)
  {
    
    pinMode(pin_menu, INPUT_PULLUP);
    pinMode(pin_touch, INPUT_PULLUP);
    pinMode(pin_shallow, INPUT_PULLUP);
    pinMode(pin_deep, INPUT_PULLUP);

  }

  BUTTON_STATE operate() {
    unsigned long us = micros();
    unsigned long ms = millis();
    if (ms < 1000) return TRIGGER_IDLE;//trigger state invalid on power on
    menu_mode = !digitalRead(pin_menu);

    uint8_t deep = !digitalRead(pin_deep);
    if (deep == true){
      if (ms_deep == 0){//if not start recorded
        ms_deep = ms;//record start
      }else if (menu_mode == false){
        if (ms - ms_deep >= 1){    
           return TRIGGER_DEEP_CLICK;
        }
      }else{//menu
        ignore_next_tap = true;
        ignore_shallow = true;
        if (ms - ms_deep >= DEEP_CLICK_MIN_MILLISECONDS){
          if (ms - ms_deep <= DEEP_CLICK_MAX_MILLISECONDS){
            if (debounce_deep == false){
              debounce_deep = true;
              ms_deep = 0;
              return TRIGGER_DEEP_CLICK;
            }
            
          }else if (ms - ms_deep >= DEEP_HOLD_MILLISECONDS){
            ms_deep = ms;
            return TRIGGER_DEEP_CLICK;
          }
          
        }else{
          return TRIGGER_IDLE;
        }
      }
      
    }else{
      debounce_deep = false;
      ms_deep = 0;
    }


    uint8_t shallow = !digitalRead(pin_shallow);
    if (shallow == true){
      if (ms_shallow == 0){//if not start recorded
        ms_shallow = ms;//record start
      }else if (menu_mode == false){
        if (debounce_shallow == false && ms - ms_shallow >= 1){          
          debounce_shallow = true;
          return TRIGGER_SHALLOW_CLICK;
        }
      }else{//menu
        ignore_next_tap = true;
        if (ignore_shallow == false && ms - ms_shallow >= SHALLOW_CLICK_MIN_MILLISECONDS){          
          debounce_shallow = true;
          return TRIGGER_SHALLOW_CLICK;
        }else{
          return TRIGGER_IDLE;
        }
      }
      
    }else{
      debounce_shallow = false;
      ignore_shallow = false;
      ms_shallow = 0;
    }

    
    uint8_t touch = digitalRead(pin_touch);
    if (touch == true){//is touching
      if (us_touch == 0){//if not touch start recorded
        us_touch = us;//record touch start
      }else if (menu_mode == false && us - us_touch >= TOUCH_MIN_MICROSECONDS){// >=1ms of touch = report
        return TRIGGER_TOUCH; 
      }
    }else{//if no touch, reset timer
      unsigned long ms_touch_dif = ms - us_touch/1000;
      if(menu_mode == true && ms_touch_dif >= TOUCH_TAP_MIN_MILLISECONDS && ms_touch_dif <= TOUCH_TAP_MAX_MILLISECONDS){
        us_touch = 0;
        if (ignore_next_tap == false){
          return TRIGGER_TAP; 
        }else{
          ignore_next_tap = false; 
        }
        
      }
      us_touch = 0;

    }
    return TRIGGER_IDLE;    
  }//operate

  bool is_menu(){
    return menu_mode;
  }
};//Trigger