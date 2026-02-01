class Solenoid{ 
  private:
  constexpr uint8_t SOLENOID_MS_ON = 22;
  constexpr uint8_t SOLENOID_MS_OFF = 78;
  bool extending = false;
  bool retracting = false;
  unsigned long ms = 0;
  uint8_t pulses_queued = 0;

  public:
  Solenoid(){
    pinMode(PIN_SOLENOID, OUTPUT);
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
        if (ready and is_voltage_ok){
          extend();
        }
        
      }
    }
    return false;
  }

  void fire(uint8_t volley){
    pulses_queued = volley;
  }//fire

  private:

  void extend(){
    extending = true;
    ms = millis();
    digitalWrite(PIN_BUILTIN_LED, HIGH);
    #if ARMED_SOLENOID
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