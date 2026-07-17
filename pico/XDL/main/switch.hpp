#pragma once
//valkor 2026-7-16

enum MENU_SWITCH_STATE : uint8_t {
  SWITCH_OPENED = 0,
  SWITCH_CLOSED,
  SWITCH_OPENED_ACTION, 
  SWITCH_CLOSED_ACTION,
};

class Switch {
private:
  uint8_t pin_menu;
  bool is_switch_closed = false;

public:
  Switch(uint8_t pin_menu): pin_menu(pin_menu){
    pinMode(pin_menu, INPUT_PULLUP);
  }//Switch

  MENU_SWITCH_STATE operate(){
    bool is_switch_closed_new = digitalRead(pin_menu);
    if (is_switch_closed_new == false){
      if (is_switch_closed == true){
        is_switch_closed = false;
        return SWITCH_CLOSED_ACTION;
      }else{
        return SWITCH_CLOSED;
      }
    }else{//switch is closed now
      if (is_switch_closed == false){
        is_switch_closed = true;
        return SWITCH_OPENED_ACTION;
      }else{
        return SWITCH_OPENED;
      }
    }
  }//operate

};//Switch