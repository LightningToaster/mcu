#define DART_DETECTOR_DELAY 10

class DartDetector{ 
  public:  
  void begin(){
    pinMode(PIN_IR_EMITTER, OUTPUT);
    pinMode(PIN_IR_RECEIVER, INPUT);
    begun = true;
  }

  uint8_t operate(){
    if (not begun){begin();}
    if (millis() - ms >= DART_DETECTOR_DELAY){
      int16_t value = analogRead(PIN_IR_RECEIVER);
      digitalWrite(PIN_IR_EMITTER, LOW);
      ms = millis();
      if (value < 800){
        if (not is_dart_ready){
          is_dart_ready = true;
        }
        ms_since_dart = millis();

      }else{
        if (is_dart_ready){
          is_dart_ready = false;
        }

      }

    }else if (millis() - ms >= DART_DETECTOR_DELAY-5){//turn on IR a few ms ahead of reading
      digitalWrite(PIN_IR_EMITTER, HIGH);
    }
    
  }//operate

  unsigned long get_ms_since_dart(){
    return millis() - ms_since_dart;
  }

  // bool is_dart(){
  //   return (get_ms_since_dart() <= 70);
  // }
  // bool is_dart_ready(){
  //   return dart;
  // }
  bool is_dart_ready = false;

  private:
  bool begun = false;
  unsigned long ms = 0;
  unsigned long ms_since_dart = 0;

};//DartDetector


