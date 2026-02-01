#define TRIGGER_IDLE 0
#define TRIGGER_CLICK 1
#define TRIGGER_HOLD 2
#define TRIGGER_HOLD_DEEP 3

class ButtonRev{ 
  public:
  uint8_t operate(){
    if (button_a.getSingleDebouncedRelease()){
      if (ms > 0){
        ms = 0;
      }else{
        //Serial.println("rev");
        
      }
      return TRIGGER_CLICK;
    }

    if (button_b.isPressed()){//deep hold takes priority
      if (ms == 0){
        ms = millis();
      }
      return TRIGGER_HOLD_DEEP;
    }

    if (button_a.isPressed()){
      if (ms == 0){
        ms = millis();
      }
      return TRIGGER_HOLD;
    }

    
    return TRIGGER_IDLE;
  }//operate

  
  private:
    unsigned long ms = 0;
    Pushbutton button_a = Pushbutton(PIN_BUTTON_REV_A);
    Pushbutton button_b = Pushbutton(PIN_BUTTON_REV_B);
    

};//TwoStageTrigger