static const uint8_t POWERS[] = {0,1,2,3,4 // (0-4) (v4, 2020-12-10)
    ,5,6,7,8,10 // (5-9)
    ,12,14,16,18,20 // (10-14) 
    ,25,30,35,40,50 // (15-19(
    ,60,70,80,90,110 // (20-24)
    ,130,150,170,190,210 // (25-29)
    ,230,254 // (30-31)
  };//powers

class LED{ 

  private:  
  bool begun = false;
  uint16_t brightness_now = 0;
  uint16_t brightness_goal = 0;
  unsigned long ms = 0;

  public:

  void begin(){
    pinMode(PIN_UV, OUTPUT);
    begun = true;
  }

  void operate(){
    if (not begun){begin();}
    int16_t difference = brightness_goal - brightness_now;

    if (difference > 0){
      if (millis() - ms >= 1){
        ms = millis();
        brightness_now += 1;
        if (is_voltage_ok){
          if (brightness_now > 31){
            brightness_now = 31;
          }
        }else{
          if (brightness_now > 9){
            brightness_now = 9;
          }
        }//voltage
      }//ms
      
    }else if (difference < 0){
      if (millis() - ms >= 5){
        ms = millis();
        brightness_now--;
      }//ms
      
    }//difference?

    analogWrite(PIN_UV, POWERS[brightness_now]);
  }//operate

  void set_goal(uint8_t new_brightness){
    if (data.uv){
      brightness_goal = new_brightness;
    }else{
      brightness_goal = 0;
    }
  }//set_goal
};//LEDs