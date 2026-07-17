#pragma once
//valkor 2026-7-5
//XDL custom 3 stage trigger with menu switch 
//this class really needs a refactor
/*
  ok so what do we need in menu mode?  and in fire mode?
  and can we stop differentiating menu mode or not?
  the main loop can just look for what we need, this class shouldnt really have to change its behavior based on menu or not
  there will still need to be priorities, will that mess up this strategy?
  maybe just have a lot more states?  ones for continuous and ones that return once?

  TRIGGER_IDLE = 0,
  TRIGGER_TOUCH_START, //const for min ms
  TRIGGER_TOUCH_ONGOING, 
  TRIGGER_TOUCH_END,
  TRIGGER_SHALLOW_START, //const for min ms
  TRIGGER_SHALLOW_ONGOING,
  TRIGGER_SHALLOW_END,
  TRIGGER_DEEP_START, //const for min ms
  TRIGGER_DEEP_ONGOING,
  TRIGGER_DEEP_END,
  TRIGGER_DEEP_INCREMENT, 

*/

//const uint8_t TOUCH_START_MS = 2 //has to be at least 2
//const uint8_t SHALLOW_START_MS = 4
//const uint8_t DEEP_START_MS = 9
//const uint16_t DEEP_INCREMENT_MS = 200 //for example, wait 200ms then return, and repeat 

//touch END should be ignored via flag if SHALLOW_START happens
//touch END and shallow END should be ignored via flag if DEEP_START happens

//get rid of micros, ms comparisons break touch and the extra math is not worth that precision.  1ms is fine.
//ok actually might have to do 2ms because 1ms is 0-1ms depending on when exactly the touch started..

//getter for menu switch

//IF double presses are problem, add debounces

enum TRIGGER_STATE : uint8_t {
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
#define DEEP_HOLD_MILLISECONDS 200

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

  TRIGGER_STATE operate(bool locked = false) {
    unsigned long us = micros();
    unsigned long ms = millis();
    if (ms < 1000) return TRIGGER_IDLE;//trigger state invalid on power on
    menu_mode = !digitalRead(pin_menu) or locked;//force menu controls if locked

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
              debounce_shallow = false;
              return TRIGGER_DEEP_CLICK;
            }
            
          }else if (ms - ms_deep >= DEEP_HOLD_MILLISECONDS){
            ms_deep = ms;
            debounce_shallow = false;
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
        if (debounce_shallow == false && ms - ms_shallow >= 5){          
          debounce_shallow = true;
          //Serial.print("click");
          return TRIGGER_SHALLOW_CLICK;
        }
      }else{//menu
        ignore_next_tap = true;
        if (debounce_shallow == false && ignore_shallow == false && ms - ms_shallow >= SHALLOW_CLICK_MIN_MILLISECONDS){          
          debounce_shallow = true;
          //return TRIGGER_SHALLOW_CLICK;
        }else{
          return TRIGGER_IDLE;
        }
      }
      
    }else{
      ignore_shallow = false;
      ms_shallow = 0;
      if (debounce_shallow == true and ms - ms_shallow >= 50){
        //Serial.println("debounce");
        debounce_shallow = false;
        if (menu_mode == true){
          return TRIGGER_SHALLOW_CLICK;
        }
      }
      
      
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
    ignore_next_tap = false; //make sure this doesnt break everything!
    return TRIGGER_IDLE;    
  }//operate

  bool is_menu(){
    return menu_mode;
  }
};//Trigger