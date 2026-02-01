// Arc Tower by Valkor 2021-01-01 (~142 lines)
//in hindsight, maybe it wasnt working because the programmer was plugged in?

#define SLEEP_PIN PCINT2
#define RELAY_PIN 4

#define MODE_FIRE 8968
#define MODE_OFF 2

#include "Relay.hpp"
Relay relay;

#include "Sleeper.hpp"

#include <RCSwitch.h>
RCSwitch my_switch = RCSwitch();

void setup() {
  my_switch.enableReceive(0);
}//end setup()

void loop() {
  relay.operate();
  
  if (my_switch.available()) {
    int remote_result = my_switch.getReceivedValue();
    switch (remote_result){
      case MODE_FIRE: 
        relay.set_us(1000);
        break;
//      case MODE_OFF: 
//        relay.set_us(0);
//        relay.operate();
//        sleep(); 
//        break;
    }
    my_switch.resetAvailable();
  }//end remote check
}//end loop()
