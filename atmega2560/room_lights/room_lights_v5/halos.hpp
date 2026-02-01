
class Halos{ public:
  #define TRANSITION_DELAY 10
  Halos(){}

  void operate(){
    if (millis() - transition_ms < TRANSITION_DELAY){return;}
    if (led_upper_now < led_upper_goal){ led_upper_now++; 
    }else if (led_upper_now > led_upper_goal){ led_upper_now--; }
    if (led_lower_now < led_lower_goal){ led_lower_now++; 
    }else if (led_lower_now > led_lower_goal){ led_lower_now--; }
    analogWrite(PIN_HALO_UPPER, POWERS[led_upper_now]);
    analogWrite(PIN_HALO_LOWER, POWERS[led_lower_now]);
    transition_ms = millis();
  }//end operate()

  void set_upper(uint8_t value){
    led_upper_goal = value;
  }//set_upper

  void set_lower(uint8_t value){
    led_lower_goal = value;
  }//set_upper

  void set(uint8_t value){
    set_upper(value);
    set_lower(value);
  }//set

  uint8_t find_power_index(uint8_t value){
    for (uint8_t i = 0; i < sizeof(POWERS)/sizeof(POWERS[0]); i++){
      if (POWERS[i] == value){
        return i;
      }
    }
    return find_power_index(value-1);
  }//find_power_index

  private:
    uint8_t led_upper_now = 0;
    uint8_t led_upper_goal = 0;
    uint8_t led_lower_now = 0;
    uint8_t led_lower_goal = 0;
    
    unsigned long transition_ms = 0;
};//end class HaloLed
