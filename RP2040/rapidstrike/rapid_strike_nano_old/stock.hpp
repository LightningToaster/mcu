#define STOCK_IS_EXTENDED 0 // returns constantly
#define STOCK_IS_COLLAPSED 1 // ^
#define STOCK_EXTEND 2 //returns once when the action happens
#define STOCK_COLLAPSE 3 // ^

#define DEBOUNCE_MS 100

class Stock{ 
  public:

  Stock(){
    is_closed = not button.isPressed();
  }

  uint8_t operate(){
    if (button.isPressed()){
      if (is_closed == true and millis() - ms >= DEBOUNCE_MS){
        ms = millis();
        is_closed = false;
        return DOOR_OPEN;
      }
      return DOOR_IS_OPEN;

    }else{
      if (is_closed == false and millis() - ms >= DEBOUNCE_MS){
        ms = millis();
        is_closed = true;
        return DOOR_CLOSE;
      }
      return DOOR_IS_CLOSED;
    }
  }//operate

  
  private:
    bool is_closed;
    unsigned long ms = 0;
    Pushbutton button = Pushbutton(PIN_BUTTON_STOCK);

};//Stock


