//Valkor 2020-12-29
#define BUTTON_PIN_ON 2
#define BUTTON_PIN_OFF 3
#define BUTTON_PIN_FIRE 4

#include <Pushbutton.h>
Pushbutton button_on = Pushbutton(BUTTON_PIN_ON);
Pushbutton button_off = Pushbutton(BUTTON_PIN_OFF);
Pushbutton button_fire = Pushbutton(BUTTON_PIN_FIRE);

#include <RCSwitch.h>
RCSwitch my_switch = RCSwitch();

void setup(){
  my_switch.enableTransmit(10);//pin
  my_switch.setPulseLength(300);
  my_switch.setRepeatTransmit(2);
  Serial.begin(9600);
  Serial.println("remote control started");
}//end setup

void loop(){
  if (button_on.isPressed()){
    my_switch.send(1, 7);
    Serial.println("sending 1");
  }else if(button_off.isPressed()){
    my_switch.send(2, 7);
    Serial.println("sending 2");
  }else if(button_fire.isPressed()){
    my_switch.send(3, 7);
    Serial.println("sending 3");
  }
}// end loop
