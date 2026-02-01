#define BUTTON_IDLE 0
#define BUTTON_HOLD 1
#define BUTTON_CLICK 2
#define BUTTON_DEEP_HOLD 3
#define BUTTON_DEEP_CLICK 4

#include <Pushbutton.h>

class StepButton{ 
  private:
  unsigned long ms = 0;
  Pushbutton button;
  Pushbutton button_d;
  bool skip_next_shallow = false;

  public:
  StepButton(uint8_t pin, uint8_t pin_d) :button(pin), button_d(pin_d) {}

  uint8_t operate(){
    if (button.getSingleDebouncedRelease()){
      if (ms > 0){
        ms = 0;
      }
      if (skip_next_shallow == false){
        return BUTTON_CLICK;
      }else{
        skip_next_shallow = false;
      }
    }

    if (button_d.getSingleDebouncedRelease()){
      if (ms > 0){
        ms = 0;//remove?
      }
      return BUTTON_DEEP_CLICK;
    }

    if (button_d.isPressed()){//deep hold takes priority
      if (ms == 0){
        ms = millis();
      }
      skip_next_shallow = true;
      return BUTTON_DEEP_HOLD;
    }

    if (button.isPressed()){
      if (ms == 0){
        ms = millis();
      }
      return BUTTON_HOLD;
    }

    
    return BUTTON_IDLE;
  }//operate
};//StepButton