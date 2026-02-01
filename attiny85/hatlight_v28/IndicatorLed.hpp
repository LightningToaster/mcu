// Valkor Simpson 2020-10-30 

/*  USAGE EXAMPLE
 * set_queue(25, -10, 35)//on for 0.25s, off for 0.10s, on for 0.35s
 * //takes in int16_t
 * //numbers represent centi-seconds
 * //positive numbers are on, negative ones are off
 */

class IndicatorLed{ 
public:
  IndicatorLed(uint8_t pin){
    this->pin = pin;
    pinMode(pin, OUTPUT);
  }// end IndicatorLed

  void operate(){
    if (not using_queue) {return;}
    if (queue[0] != 0){ 
      if (cs == 0){
        if (queue[0] > 0){
          digitalWrite(pin, HIGH);
        }else{
          digitalWrite(pin, LOW);
        }
        cs = millis()/10;
      }
      if (millis()/10 - cs >= abs(queue[0])){
        for (int i = 0; i<2; i++){
          queue[i] = queue[i+1];
        }
        cs = 0;
        queue[2] = 0;
      }
    }else{
      digitalWrite(pin, LOW);
    }// end if
  }// end operate

  void set_queue(int16_t _0 = 0, int16_t _1 = 0, int16_t _2 = 0){
    using_queue = true;
    queue[0] = _0;
    queue[1] = _1;
    queue[2] = _2;
  }// end set_queue

  void enable(){
    using_queue = false;
    digitalWrite(pin, HIGH);
  }// end enable

  void disable(){
    using_queue = false;
    digitalWrite(pin, LOW);
  }// end disable

private:
  int16_t queue[3] = {0,0,0}; //centi-seconds (.01s)
  unsigned long cs = 0;
  uint8_t pin;
  bool using_queue = false;
};
