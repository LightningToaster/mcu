#define DART_READY 1
#define MAG_CHANGED 2

class DartDetector{ 

  private:
  static constexpr uint8_t THRESHOLD = 70;
  static const uint8_t DART_DETECTOR_DELAY = 20;
  unsigned long ms = 0;
  unsigned long ms_dart = 0;
  
  public: 

  DartDetector(){
    pinMode(PIN_DART_SENSE, INPUT);
  } 

  uint8_t operate(){
    static uint8_t r = 0;
    if (millis()-ms < DART_DETECTOR_DELAY){return r;}ms = millis();

    int16_t value = analogRead(PIN_DART_SENSE);
    
    if (value <= THRESHOLD){
      if (millis() - ms_dart < 500){
        r = DART_READY;
      }else{
        r = MAG_CHANGED;
      }
      ms_dart = millis();

  
    }else{
      r = 0;
    }

    return r;
  }//operate

};//DartDetector