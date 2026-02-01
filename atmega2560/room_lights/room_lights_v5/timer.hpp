#define SECOND_TO_MS 1000

class Timer{ 
  public:
  
  uint8_t operate(){
    if (millis()-last_trigger_ms >= SECOND_TO_MS){
      last_trigger_ms = millis();
      if (seconds > 0){
        seconds--;
      }
    }
    return get();
  }//operate

  void set(unsigned long seconds){
    seconds = seconds;
  }//set

  unsigned long get(){
    return seconds;
  }
  
  private:
  uint8_t pin;
  unsigned long last_ms = 0;
  unsigned long seconds = 0;
  uint8_t status = 0;
  unsigned long stay_active_s = 0;  
};//Motion_Sensor
