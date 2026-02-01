//(button combo version) Valkor 2022-04-05
//usage example; struct buttons_state input = buttons.operate();
//#define CALIBRATING //uncomment to show button analog values (when choosing resistors)
#define MINIMUM_MS_CLICK 40
#define MINIMUM_MS_HOLD 333
#define MINIMUM_MS_LONG 1000 //comment to disable (hold will always trigger if held for MINIMUM_MS_HOLD or longer)
 
const uint16_t BUTTON_TARGETS[] = {//analog ~ 3kohm ~ G
  512, //1kohms (top button)
  768, //3kohms (bottom button)
  820, //1k+3k=4kohms (both buttons)
};
#define TOLERANCE 25
#define SAMPLES 3 //deals with bad data effectively, keep at 3+
#define ZERO_THRESHOLD 200 // anything <= this counts as 0

#define NUM_BUTTONS sizeof(BUTTON_TARGETS)/2 //because 16/2 = 8

#define BUTTON_UPPER 0
#define BUTTON_LOWER 1
#define BUTTON_BOTH 2

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
    }//constructor
    
    struct buttons_state operate(){ 
      insert_new_sample();
      show_calibration_data();
      check_for_downed_buttons();

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
    }//operate
    
  private:
    uint8_t pin;
    unsigned long last_button_press = 0;//for timing, so we know what type of click
    uint16_t value_array[SAMPLES];
    bool button_statuses[NUM_BUTTONS]; // is pressed?
    bool skip_next_debounce = false;

    void insert_new_sample(){
      for (uint8_t i = SAMPLES-1; i>0; i--){//shift values up one index
        value_array[i] = value_array[i-1];
      }
      value_array[0] = analogRead(this->pin);
    }//insert_new_sample
    
    
    void show_calibration_data(){
      #ifdef CALIBRATING
        if (value_array[0] >= ZERO_THRESHOLD){//for calibrating
        Serial.println(value_array[0]);
      }
      #endif
    }//show_calibration_data

    void check_for_downed_buttons(){
      //see if any buttons got SAMPLES matches
      for (uint8_t i_button = 0; i_button<NUM_BUTTONS; i_button++){//for every button
        
        bool bad_value_found = false;
        
        for (uint8_t i_sample = 0; i_sample<SAMPLES; i_sample++){//compare to every sample
          uint16_t sample = value_array[i_sample];
          uint16_t target = BUTTON_TARGETS[i_button];
          
          if (sample < target - TOLERANCE or sample > target + TOLERANCE){//is within range?
            bad_value_found = true;
            break; //this button is not pressed, continue outer loop to next button
          }
        }
        
        if (!bad_value_found){ //if all values indicate button is down
          button_statuses[i_button] = true;//mark as down
          if (last_button_press == 0){//if not already down
            last_button_press = millis();//set ms that it went down
          }
        }
      }//for
    }//check_for_downed_buttons
   
};//Buttons class
