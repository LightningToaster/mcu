// Valkor 2020-10-30 

/*  USAGE EXAMPLE
 uint16_t frequencies[] = {300, 0, 500, 0, 1000};
 //numbers represent centi-seconds (hundreths of sec
 //positive numbers are on, negative ones are off
 speaker.set_queue_frequencies(sizeof(frequencies)/2, frequencies);
 int8_t queue[] = {20, -10, 30, -10, 20};
 speaker.set_queue(5, queue);
 
 
*/

#define QUEUE_POSITIONS 5
#define DEFAULT_HZ 500

class Speaker{ 
public:
  Speaker(uint8_t pin){
    this->pin = pin;
    pinMode(pin, OUTPUT);
  }// end Speaker

  void operate(){
    if (not using_queue) {return;}
    
    if (queue[0] != 0){ 
      if (cs == 0){
        if (queue[0] > 0){
          turn_on(queue_frequency[0]);
          
        }else{
          turn_off();
        }
        cs = millis()/10;
      }
      if (millis()/10 - cs >= abs(queue[0])){
        for (int i = 0; i<QUEUE_POSITIONS-1; i++){
          queue[i] = queue[i+1];
           queue_frequency[i] = queue_frequency[i+1];
        }
        cs = 0;
        queue[QUEUE_POSITIONS-1] = 0;
        queue_frequency[QUEUE_POSITIONS-1] = 0;
      }
    }else{
      turn_off();
    }//if
  }//operate()

  void set_queue(uint8_t num, int8_t queue[]){
    using_queue = true;
    for (int i=0; i<num; i++){
      this->queue[i] = queue[i];
    }
  }//set_queue()

  void set_queue_frequencies(uint8_t num, uint16_t frequencies[]){
    for (int i=0; i<num; i++){
      queue_frequency[i] = frequencies[i];
    }
  }//set_queue_frequencies()

//  void enable(){
//    using_queue = false;
//    turn_on();
//  }//enable()
//
//  void disable(){
//    using_queue = false;
//    turn_off();
//  }//disable()

  void beep(uint16_t hz, uint16_t centiseconds){
    uint16_t frequencies[] = {hz, 0, 0, 0, 0};
    set_queue_frequencies(sizeof(frequencies)/sizeof(frequencies[0]), frequencies);
    int8_t queue[] = {centiseconds, 0, 0, 0, 0};
    set_queue(sizeof(queue)/sizeof(queue[0]), queue);
  }//beep()

  void play_power_up_sound(){
    uint16_t frequencies[] = {300, 0, 500, 0, 1000};
    set_queue_frequencies(sizeof(frequencies)/sizeof(frequencies[0]), frequencies);
    int8_t queue[] = {18, -9, 18, -9, 18};
    set_queue(sizeof(queue)/sizeof(queue[0]), queue);
  }//play_power_up_sound()
  
private:
  int8_t queue[QUEUE_POSITIONS]; //centi-seconds (.01s)
  uint16_t queue_frequency[QUEUE_POSITIONS]; //hz
  
  unsigned long cs = 0;
  uint8_t pin;
  bool using_queue = false;

  void turn_on(uint16_t hz = DEFAULT_HZ){
    tone(pin, hz);
  }//turn_on()

  void turn_off(){
    noTone(pin);
  }//turn_off()
};
