#define SWITCH_IS_CLOSED 0 // returns constantly
#define SWITCH_IS_OPEN 1 // ^
#define SWITCH_CLOSE 2 //returns once when the action happens
#define SWITCH_OPEN 3 // ^

#define DEBOUNCE_MS 100

class Switch{ 
  private:
  bool is_closed;
  unsigned long ms = 0;
  Pushbutton button;

  public:

  Switch(uint8_t pin):button(pin){}

  uint8_t operate(){
    if (button.isPressed()){
      if (is_closed == true and millis() - ms >= DEBOUNCE_MS){
        ms = millis();
        is_closed = false;
        return SWITCH_OPEN;
      }
      return SWITCH_IS_OPEN;

    }else{
      if (is_closed == false and millis() - ms >= DEBOUNCE_MS){
        ms = millis();
        is_closed = true;
        return SWITCH_CLOSE;
      }
      return SWITCH_IS_CLOSED;
    }
  }//operate

  
};//Switch