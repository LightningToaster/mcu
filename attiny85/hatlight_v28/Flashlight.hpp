
const uint8_t POWERS[] = {0,1,2,3,4 // (0-4) (v4)
  ,5,6,7,8,10 // (5-9)
  ,12,14,16,18,20 // (10-14) 
  ,25,30,35,40,50 // (15-19(
  ,60,70,80,90,110 // (20-24)
  ,130,150,170,190,210 // (25-29)
  ,230,254 // (30-31)
}; // powers

#define DEFAULT_BRIGHTNESS_INDEX 2

class Flashlight{ 
  
  public:
    Flashlight(uint8_t pin){
      this->pin = pin;
    }//end Flashlight
  #include "LightModes.hpp"

  void change_brightness(int8_t index = -1){
    if (index == -1){
      switch (current_brightness_index){
        case DEFAULT_BRIGHTNESS_INDEX: current_brightness_index = 14; break;
        case 14: current_brightness_index = 28; break;
        default: current_brightness_index = DEFAULT_BRIGHTNESS_INDEX; break;
      }
    }else{
      current_brightness_index = index;
    }
  }// end change_brightness

  void change_mode(int8_t new_mode = -1){
    if (new_mode == -1){
      mode++;
    }
   if (mode > MAX_MODES){
      mode = MODE_STEADY;
    }
  }// end change_mode
  
  void operate(bool instant = false){ 
    if (activity_mode == ACTIVITY_ON){
      if (mode == MODE_STEADY){
        steady();
      }else if (mode == MODE_FLASH){
        flash();
      }else if (mode == MODE_PULSE){
        pulse();
      }else if (mode == MODE_STUTTER){
        stutter();
      }
    }else{
      led_goal = 0;
    }
    transition(instant);
  }//end operate()

  void transition(bool instant = false){
    bool ready = false;
    if(led_now < led_goal){//turn on faster than turning off
      ready = (millis() - transition_ms >= transition_delay/2);
    }else{
      ready = (millis() - transition_ms >= transition_delay);
    }
    if (ready or instant){
      if (led_now < led_goal){
        analogWrite(pin, POWERS[++led_now]);
      }else if(led_now > led_goal){
        analogWrite(pin, POWERS[--led_now]);
      }
      transition_ms = millis();
    }
  }// end transition

  uint8_t find_power_index(uint8_t value){
    for (uint8_t i = 0; i < sizeof(POWERS)/sizeof(POWERS[0]); i++){
      if (POWERS[i] == value){
        return i;
      }
    }
    return find_power_index(value-1);
  }//end find_power_index

  void set_brightness(uint8_t v){
    led_goal = find_power_index(v);
    analogWrite(pin, v);
    mode = MODE_STEADY;
  }

  int8_t get_activity_mode(){
    return activity_mode;
  }

  private:
    uint8_t pin;
    uint8_t mode = MODE_STEADY;//steady, flash, pulse, etc..
    uint8_t activity_mode = ACTIVITY_ON;//sleep stuff
    uint8_t led_now = 0;
    uint8_t led_goal = 0;
    uint8_t current_brightness_index = DEFAULT_BRIGHTNESS_INDEX;
    unsigned long transition_ms = 0;
    uint8_t transition_delay = 5; //delay between increments towards led_goal
    unsigned long effect_ms = 0;
    
    bool effects_need_reset = true;
};// end Flashlight
