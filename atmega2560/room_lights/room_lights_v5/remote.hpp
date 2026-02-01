//Valkor 2022-04-08
//usage example; struct buttons_state input = buttons.operate();
//#define CALIBRATING //uncomment to show remote button ids
#define MINUMUM_MS_BETWEEN_CLICKS 300
 
#define REMOTE_POWER 1
#define REMOTE_DEMO 2
#define REMOTE_MODE_INCREASE 3
#define REMOTE_MODE_DECREASE 4
#define REMOTE_SPEED_INCREASE 5
#define REMOTE_SPEED_DECREASE 6
#define REMOTE_COLOR_INCREASE 7
#define REMOTE_COLOR_DECREASE 8
#define REMOTE_BRIGHTNESS_INCREASE 9
#define REMOTE_BRIGHTNESS_DECREASE 10

#define REMOTE_WHITE 11
#define REMOTE_RED 12
#define REMOTE_GREEN 13
#define REMOTE_BLUE 14
#define REMOTE_ORANGE 15
#define REMOTE_PURPLE 16
#define REMOTE_CYAN 17

#include <RCSwitch.h>
RCSwitch remote_switch = RCSwitch();

class Remote{
  public:
    Remote(){
      remote_switch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
    }//Remote
    
    uint8_t operate(){ 
      
      if (remote_switch.available()) {
        unsigned long value = remote_switch.getReceivedValue();
        remote_switch.resetAvailable();
        #ifdef CALIBRATING
          Serial.println(value);
        #endif
        switch(value){
          case 5120769: return REMOTE_POWER;
          case 5120776: return REMOTE_DEMO;
          case 5120773: return REMOTE_MODE_INCREASE;
          case 5120779: return REMOTE_MODE_DECREASE;
          case 5120777: return REMOTE_SPEED_INCREASE;
          case 5120775: return REMOTE_SPEED_DECREASE;
          case 5120778: return REMOTE_COLOR_INCREASE;
          case 5120781: return REMOTE_COLOR_DECREASE;
          case 5120780: return REMOTE_BRIGHTNESS_INCREASE;
          case 5120783: return REMOTE_BRIGHTNESS_DECREASE;
          case 5120782: return REMOTE_WHITE;
          case 5120784: return REMOTE_RED;
          case 5120785: return REMOTE_GREEN;
          case 5120786: return REMOTE_BLUE;
          case 5120787: return REMOTE_ORANGE;
          case 5120789: return REMOTE_PURPLE;
          case 5120788: return REMOTE_CYAN;
        }//switch
      }//if available
      return 0;
    }//operate
    
  private:
    uint8_t pin;
    unsigned long last_button_press = 0;

   
   
};//Remote class
