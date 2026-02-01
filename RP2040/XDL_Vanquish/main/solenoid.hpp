
class Solenoid{ 
  private:
  static constexpr uint8_t SOLENOID_MS_ON = 24;
  static constexpr uint8_t SOLENOID_MS_OFF = 76;
  const uint8_t pin; // store solenoid control pin
  bool extending = false;
  bool retracting = false;
  unsigned long ms = 0;
  uint8_t pulses_queued = 0;

  public:
  Solenoid(uint8_t pin_solenoid) : pin(pin_solenoid) {
		pinMode(pin, OUTPUT);
		pinMode(PIN_BUILTIN_LED, OUTPUT);
	}


  bool operate(bool ready){

    if (extending){
      if (millis() - ms >= SOLENOID_MS_ON){//finished extending
        retract();
        return true; //fired a dart
      }
    }else if (retracting){
      if (millis() - ms >= SOLENOID_MS_OFF){//finished retracting
        retracting = false;
      }
    }else{//doing nothing
      if (pulses_queued > 0){
        if (ready){
          extend();
        }
        
      }
    }
    return false;
  }

  void fire(uint8_t volley){
    pulses_queued = volley;
  }//fire
  void stop(){
    fire(0);//empty the queue
  }//fire

  private:

  void extend(){
    extending = true;
    ms = millis();
    digitalWrite(PIN_BUILTIN_LED, HIGH);
    #ifdef ARMED_SOLENOID
      digitalWrite(PIN_SOLENOID, HIGH);
    #endif 
  }//extend

  void retract(){
    extending = false;
    retracting = true;
    ms = millis();
    digitalWrite(PIN_BUILTIN_LED, LOW);
    digitalWrite(PIN_SOLENOID, LOW);
    pulses_queued = (pulses_queued > 0) ? pulses_queued - 1 : 0;
  }
};//Solenoid