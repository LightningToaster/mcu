#define MINIMUM_MS_BETWEEN_TRIGGERS 3200
#define SECOND_TO_MS 1000

class Motion_Sensor{ 
  public:

  Motion_Sensor(uint8_t pin){
    this->pin = pin;
  }
  long operate(){
    if (millis()-last_ms >= SECOND_TO_MS){
      last_ms = millis();
      seconds++;
    }
    
    if (millis()-last_trigger_ms >= MINIMUM_MS_BETWEEN_TRIGGERS){
      if (digitalRead(pin) == HIGH){
        if (seconds >=0){
          //TRIGGER
          reset();
          return 0;
        }
      }
    }
    
    return seconds;
  }//operate

  
  long get_s(){
    return seconds;
  }

  void set_dormant_s(long s){
    seconds = -s;
  }

  void reset(){
    
    if (seconds >= 0 ){
      seconds = 1;
    }
    
    last_trigger_ms = millis();
    last_ms = millis();
  }
  
  private:
  uint8_t pin;
  long last_ms = 0;
  long last_trigger_ms = 0;
  long seconds = 0;
};//Motion_Sensor
