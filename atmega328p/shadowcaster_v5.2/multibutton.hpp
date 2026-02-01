//Valkor 2021-08-29

#define CALIBRATING //uncomment to show button analog values (when chosing resistors)
#define MINIMUM_MS_CLICK 5
#define MINIMUM_MS_HOLD 300
//#define MINIMUM_MS_LONG 1000 //comment to disable, 
//...when disabled, hold will always trigger if held for MINIMUM_MS_HOLD or longer
//#define MINIMUM_MS_BETWEEN_PRESSES 1000 // TODO add this?  with MINIMUM_MS_CLICK maybe pointless?
 
const uint16_t BUTTON_TARGETS[] = {//analog ~ 10kohm ~ G
  931, //1k ohms
  736, //3.9k ohms
  536, //9.1k ohms
  338, //20k ohms
  254, //30k ohms (separate  button)
};
#define TOLERANCE 8
#define SAMPLES 4 //deals with bad data effectively, keep at 3+
#define ZERO_THRESHOLD 10 // anything <= this counts as 0

#define NUM_BUTTONS sizeof(BUTTON_TARGETS)/2 //because 16/2 = 8

#define BUTTON_NONE 0
#define BUTTON_CLICK 1
#define BUTTON_HOLD 2
#define BUTTON_LONG 3

struct buttons_state {
  uint8_t action;
  uint8_t button;
};

class Buttons{
  public:
    Buttons(uint8_t pin){
      this->pin = pin;
    }//end constructor
    
    struct buttons_state operate(){ 
      for (uint8_t i = SAMPLES-1; i>0; i--){//shift values up one index
        value_array[i] = value_array[i-1];
      }
      value_array[0] = analogRead(this->pin);
      
      #ifdef CALIBRATING
        if (value_array[0] >= ZERO_THRESHOLD){//for calibrating
          Serial.println(value_array[0]);
        }
      #endif

      
      //see if any buttons got SAMPLES matches
      for (uint8_t i = 0; i<NUM_BUTTONS; i++){
        bool bad_value_found = false;
        for (uint8_t j = 0; j<SAMPLES; j++){
          if (value_array[j] < BUTTON_TARGETS[i] - TOLERANCE
          or value_array[j] > BUTTON_TARGETS[i] + TOLERANCE){
            bad_value_found = true;
            break; //this button is not pressed
          }
        }
        if (!bad_value_found){
          button_statuses[i] = true;
          if (last_button_press == 0){
            last_button_press = millis();
          }
        }
      }//end for

      //see if all buttons were released
      bool button_released = true;
      for (uint8_t i = 0; i<SAMPLES; i++){
        if (value_array[i] > ZERO_THRESHOLD){
          button_released = false;
          break;
        }
      }

      struct buttons_state r;
      r.action = BUTTON_NONE;
     
      unsigned long ms_held = millis() - last_button_press;
      for (uint8_t i = 0; i<NUM_BUTTONS; i++){
        //for all the buttons
        if (button_statuses[i] == true
        and ms_held >= MINIMUM_MS_CLICK){//humans aren't this fast
          if (button_released){
            button_statuses[i] = false;
            last_button_press = 0;
            if (skip_next_debounce == false){
              r.button = i;
              if (ms_held < MINIMUM_MS_HOLD){
                r.action = BUTTON_CLICK;
              }else{
                r.action = BUTTON_HOLD;
              }//if (which press type)
            }else{
              skip_next_debounce = false;
            }
            
          }else{
            #ifdef MINIMUM_MS_LONG
              if (ms_held >= MINIMUM_MS_LONG){
                button_statuses[i] = false;
                r.button = i;
                r.action = BUTTON_LONG;
                last_button_press = 0;
                skip_next_debounce = true;
              }//if
            #endif
          }//if else
        }//if
      }//for buttons
      return r;
    }//end operate
    
  private:
    uint8_t pin;
    unsigned long last_button_press = 0;//for timing, so we know what type of click
    unsigned long last_action = 0; // tracking time between actions
    uint16_t value_array[SAMPLES];
    bool button_statuses[NUM_BUTTONS]; // is pressed?
    bool skip_next_debounce = false;
};//end Buttons class
