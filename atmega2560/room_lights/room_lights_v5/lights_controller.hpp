#include "strips.hpp"
#define SECONDS_TO_BLACKOUT 10 //must be 1 or greater
#define BLACKOUT_EFFECT_MS 1000
#define TURNING_ON_EFFECT_MS 250
#define TURNING_OFF_EFFECT_MS 500
#define SLEEP_EFFECT_MS 1000

const uint8_t COLORS[][COLOR_CHANNELS_PER_PIXEL] = { // RGBW 0-31
  {31,22,19,31},//{18,8,0,22},//{16,7,0,19}{31,25,25,31}
  {20,0,0,0},
  {0,18,0,0},
  {0,0,20,0},
  {18,4,0,0},
  {5,0,15,0},
  {0,12,12,0},
};

const uint8_t PATTERNS[] = {21,12,6,2,1};//basically the brightness levels


#define MODE_STANDARD 0
#define MODE_GLISTEN 1
#define MODE_HELICOPTER 2
#define NUM_MODES 3

#define STATE_OFF 0 
#define STATE_ON 1 
#define STATE_SLEEPING 2
#define STATE_NIGHTLIGHT 3  
#define STATE_PARTY 4


#include "halos.hpp"
Halos halos;

class Lights_Controller{
  public:
  Lights_Controller(){}//delete?
  
  int16_t operate(){
    if (strips == NULL){
      strips = new Strips();
      turn_on();
    }//if
    int16_t status = strips->operate();
    halos.operate();
    
    uint8_t pixels_used = 0;
    for (int i=PIXELS_PER_STRIP; i>0; i-=PATTERNS[index_pattern]){
      pixels_used++;
    }
    
    if (effect_index_on > 0 and millis() - ms >= TURNING_ON_EFFECT_MS/pixels_used){
      ms = millis();
      if (effect_index_on >= 0){
        uint8_t color[4] = {COLORS[index_color][0], COLORS[index_color][1], COLORS[index_color][2], COLORS[index_color][3]};
        if (index_color == 0){//make sure it's white
          color[1] += green_blue_additive;
          color[2] += green_blue_additive;
        }
        strips->set_goal_ring(effect_index_on-1, color[0], color[1], color[2], color[3]);
        //Serial.println(String(color[0]) + ", " + color[1] + ", " + color[2] + ", " + color[3]);
      }
      
      effect_index_on -= PATTERNS[index_pattern];
      if (effect_index_on < 0){
        effect_index_on = 0;
      }
      
    }else if (effect_index_off > 0 and millis() - ms >= TURNING_OFF_EFFECT_MS/pixels_used){
      ms = millis();
      uint8_t color[4] = {COLORS[index_color][0], COLORS[index_color][1], COLORS[index_color][2], COLORS[index_color][3]};

      for (int i=0; i<4; i++){
        if (color[i] > 0){
          color[i] = 1;
        }
      }
      //strips->set_goal_ring(effect_index_off-1, color[0], color[1], color[2], color[3]);//colored offs
      strips->set_goal_ring(effect_index_off-1, 0,0,0,1);//just use white
      
      effect_index_off-=PATTERNS[index_pattern]; 
      if (effect_index_off <= 0){
        effect_index_off = 0;
        seconds_to_blackout = SECONDS_TO_BLACKOUT;
        //halos.set(1);
      }
      
    }else if (seconds_to_blackout > 0 and millis() - ms >= 1000){ //after every second passes
      ms = millis();
      seconds_to_blackout--;
      
      if (seconds_to_blackout == 0){
        effect_index_blackout = PIXELS_PER_STRIP;
      }
       
    }else if (effect_index_blackout > 0 and millis() - ms >= BLACKOUT_EFFECT_MS/pixels_used){
      ms = millis();
      strips->set_goal_ring(effect_index_blackout-1, 0,0,0,0);
      effect_index_blackout-=PATTERNS[index_pattern];
      
      // if (effect_index_blackout <= 0){
      //   halos.set(1);
      // }
      
    }else if(effect_index_sleep > 0 and millis() - ms >= SLEEP_EFFECT_MS/pixels_used){
      ms = millis();
      strips->set_goal_ring(effect_index_sleep-1, 0,1,5,1);//3
      effect_index_sleep-=PATTERNS[index_pattern];
      //halos.set(5);
      
    }else if (index_mode > 0){
      switch(index_mode){
        case MODE_GLISTEN: glisten(); break;
        case MODE_HELICOPTER: helicopter(); break;
      }
    }//else if tree
    this->status = status;
    return status;
  }//operate

  void toggle(){//just used by remote
    if (state == STATE_ON){
      turn_off();
    }else{
      turn_on();
    }
  }//toggle

  void turn_on(){
    //if (is_night_light){night_light();}
    //is_on = true;
    reset_indexes();
    effect_index_on = PIXELS_PER_STRIP;//will start turning on
    turn_off_unused();
    halos.set(PATTERNS[4-index_pattern]+10);
    state = STATE_ON;
  }//turn_on

  void turn_off(){
    reset_indexes();
    if (state == STATE_NIGHTLIGHT){
      strips->set_goal_all(0,0,0,0);
    }else{
      effect_index_off = PIXELS_PER_STRIP;//will start turning off
    }
    halos.set(1);
    state = STATE_OFF;
  }//turn_off
  
  void sleep(){
    if (index_mode != MODE_STANDARD){return;}
    reset_indexes();
    effect_index_sleep = PIXELS_PER_STRIP;
    state = STATE_SLEEPING;
    halos.set(PATTERNS[index_pattern]);//may not be needed
  }//sleep

  void next_color(int8_t i = -1){
    if (i == -1){//no arg, increment
      index_color = (index_color+1)%(sizeof(COLORS)/COLOR_CHANNELS_PER_PIXEL);
    }else{//arg provide, set it
      index_color = i;
    }

    if (index_mode == 0){
      turn_on();
    }
  }//next_color

  void previous_color(){
    index_color--;
    if (index_color < 0){
      index_color = (sizeof(COLORS)/COLOR_CHANNELS_PER_PIXEL)-1;
    }
    turn_on();
  }//previous_color
  
  void next_pattern(int8_t i = -1){
    if (i == -1){//no arg, increment
      index_pattern = (index_pattern+1)%sizeof(PATTERNS);
    }else{//arg provide, set it
      index_pattern = i;
    }
    turn_on();
  }//set_pattern

  void previous_pattern(){
    //TODO make one liner?
    index_pattern--;
    if (index_pattern < 0){
      index_pattern = sizeof(PATTERNS)-1;
    }
    turn_on();
  }//previous_pattern

  void night_light(){
    strips->set_goal_all(0,0,0,0);
    reset_indexes();
    strips->set_goal_ring(0, 0,1,0,0);
    strips->set_goal_ring(PIXELS_PER_STRIP-1, 0,0,0,1);
    state = STATE_NIGHTLIGHT;
  }//night_light

  void next_mode(int8_t i = -1){
    if (i == -1){//no arg, increment
      index_mode = (index_mode+1)%NUM_MODES;
    }else{//arg provide, set it
      index_mode = i;
    }
    strips->set_goal_all(0,0,0,0);
    if (index_mode == 0){
      turn_on();
    }
  }//next_mode

  void previous_mode(){
    //TODO make one liner?
    index_mode--;
    if (index_mode < 0){
      index_mode = NUM_MODES-1;
    }
    strips->set_goal_all(0,0,0,0);
    if (index_mode == 0){
      turn_on();
    }
  }//previous_mode

  void increase_warmth(){
    if (green_blue_additive < 9){
      green_blue_additive+=3;
      turn_on();
    }
  }//increase_warmth

  void decrease_warmth(){
    if (green_blue_additive > -9){
      green_blue_additive-=3;
      turn_on();
    }
  }//decrease_warmth

  bool is_blacked_out(){
    return (seconds_to_blackout <= 0);
  }

  void next_zone(){
    strips->next_zone();
    turn_on();
  }
  
  uint8_t state = STATE_ON;
  
  private:
  Strips* strips;

  int16_t status = 0;
  int8_t effect_index_on = 0;
  int8_t effect_index_off = 0;
  int8_t effect_index_blackout = 0;
  int8_t effect_index_sleep = 0;
  int8_t effect_index_wake = 0;
  unsigned long ms = 0;
  unsigned long seconds_to_blackout = 0;
  int8_t green_blue_additive = 0;
  
  int8_t index_color = 0;
  int8_t index_pattern = 0;
  
  int8_t index_mode = 0;

  void reset_indexes(){
    effect_index_on = 0;
    effect_index_blackout = 0;
    seconds_to_blackout = 0;
    effect_index_sleep = 0;
    effect_index_wake = 0;
    index_mode = 0;
  }//reset_indexes

  void turn_off_unused(){

    for (int i=PIXELS_PER_STRIP-1, offs_to_go = 0; i>=0; i--){
      if (offs_to_go == 0){
        offs_to_go = PATTERNS[index_pattern]-1;
      }else{
        offs_to_go--;
        strips->set_goal_ring(i,0,0,0,0);
      }
    }//for
  }//turn_off_unused

  void glisten(){
    if (millis() - ms < 150){return;}ms = millis();
      
    uint8_t led = random(0,PIXELS_PER_STRIP);
    uint8_t strip = random(0,NUM_STRIPS);
    strips->set_goal(strip, led, 
      COLORS[index_color][0],
      COLORS[index_color][1],
      COLORS[index_color][2],
      COLORS[index_color][3]
    );

    for (int i = 0; i<7; i++){//attempt to turn some off
      uint8_t led = random(0,PIXELS_PER_STRIP);
      uint8_t strip = random(0,NUM_STRIPS);
      strips->set_goal(strip, led, 0,0,0,0);
    }
  }//glisten

  int8_t helicoper_i = 0;
  void helicopter(){
    if (millis() - ms < 200 or status > 0){return;}ms = millis();
    
    strips->set_goal_all(0,0,0,0);
    
    helicoper_i = (helicoper_i+1)%NUM_STRIPS;
    int8_t opposite_strip = (helicoper_i+4)%NUM_STRIPS;
    
    for (int led = 0; led<PIXELS_PER_STRIP; led++){
      int8_t d = 2;
      strips->set_goal(helicoper_i, led, 
        ceil(COLORS[index_color][0]/d),
        ceil(COLORS[index_color][1]/d),
        ceil(COLORS[index_color][2]/d),
        ceil(COLORS[index_color][3]/d)
      );
      strips->set_goal(opposite_strip, led, 
        ceil(COLORS[index_color][0]/d),
        ceil(COLORS[index_color][1]/d),
        ceil(COLORS[index_color][2]/d),
        ceil(COLORS[index_color][3]/d)
      );
    }

  }//helicopter
  
};//class Lights_Controller
