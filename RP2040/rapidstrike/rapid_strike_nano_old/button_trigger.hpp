#define TRIGGER_IDLE 0
#define TRIGGER_CLICK 1
#define TRIGGER_HOLD 2
#define TRIGGER_HOLD_DEEP 3

class ButtonTrigger{ 
  public:
  uint8_t operate(){
    if (button_a.getSingleDebouncedRelease()){
      if (ms > 0){
        ms = 0;
      }
      if (skip_next_click == false){
        return TRIGGER_CLICK;
      }else{
        skip_next_click = false;
      }
      
    }

    if (button_b.isPressed()){
      if (ms == 0){
        ms = millis();
      }
      skip_next_click = true;
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
    Pushbutton button_a = Pushbutton(PIN_BUTTON_TRIGGER_A);
    Pushbutton button_b = Pushbutton(PIN_BUTTON_TRIGGER_B);
    bool skip_next_click = false;

};//TwoStageTrigger