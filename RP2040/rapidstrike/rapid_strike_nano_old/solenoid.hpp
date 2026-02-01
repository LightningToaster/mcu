#define SOLENOID_MS_ON 22 //TODO dynamic with voltage
#define SOLENOID_MS_OFF 80

class Solenoid{ 
  public:

  Solenoid(){
    pinMode(PIN_SOLENOID, OUTPUT);
  }

  uint8_t operate(bool is_voltage_ok, bool is_dart_ready){
    this->is_voltage_ok = is_voltage_ok;

    if (extending){
      if (millis() - ms >= SOLENOID_MS_ON){//finished extending
        retract();
      }
    }else if (retracting){
      if (millis() - ms >= SOLENOID_MS_OFF){//finished retracting
        finish();
      }
    }else{//doing nothing
      if (pulses_queued > 0 and wheels_ready){
        if (is_dart_ready){
          extend();
        }else{
          pulses_queued = 0;
        }
        
      }
    }
  }

  void extend(){
    extending = true;
    ms = millis();
    #ifdef ARMED
      if (is_voltage_ok){
        digitalWrite(PIN_SOLENOID, HIGH);
      }
    #endif 
  }

  void retract(){
    extending = false;
    retracting = true;
    ms = millis();
    digitalWrite(PIN_SOLENOID, LOW);
    pulses_queued = (pulses_queued > 0) ? pulses_queued - 1 : 0;
  }

  void finish(){
    retracting = false;
  }

  uint8_t pulses_queued = 0;
  bool wheels_ready = false;

  private:
    bool extending = false;
    bool retracting = false;
    
    unsigned long ms = 0;
    bool is_voltage_ok = 0;

};//Solenoid


