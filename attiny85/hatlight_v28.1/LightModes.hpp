#define MODE_STEADY 1
#define MODE_FLASH 2
#define MODE_PULSE 3
#define MODE_STUTTER 4
#define MAX_MODES 4

#define ACTIVITY_DEEP_SLEEP 1
#define ACTIVITY_SLEEP 2
#define ACTIVITY_ON 3

void steady(){
  led_goal = current_brightness_index;
}//end staticWhite


void flash(){
  if (effects_need_reset){
    led_goal = 0;
    effects_need_reset = false;
  }
  if ((millis() - effect_ms) >= 100){
    effect_ms = millis();
    if (led_goal == current_brightness_index){
      led_goal = 0;
      led_now = 1;
    }else{
      led_goal = current_brightness_index;
      led_now = current_brightness_index-1;
    }
    transition(true);
  }
}//end flash


void pulse(){
  if (effects_need_reset){
    led_goal = 0;
    effects_need_reset = false;
  }
  if ((millis() - effect_ms) >= 100){
    effect_ms = millis();
    if (led_goal == current_brightness_index){//current_brightness_index
      led_goal = 1;
    }else{
      led_goal = current_brightness_index;//current_brightness_index
    }
    transition(true);
  }
}//end pulse

int8_t stutter_count = 0;
void stutter(){
  if (effects_need_reset){
    led_goal = 0;
    effects_need_reset = false;
  }

  if ((millis() - effect_ms) >= 40){
    effect_ms = millis();
    if (stutter_count < 10){
      if (stutter_count%2 == 0){
        led_goal = current_brightness_index;
        led_now = current_brightness_index-1;
      }else{
        led_goal = 0;
        led_now = 1;
      }
      transition(true);
    }else{
      if(stutter_count >= 15){
        stutter_count = 0;
      }
    }
    stutter_count++;
  }
}//end stutter


//THESE ARE SPECIAL MODES WITH DELAYS
void blink(uint8_t count, uint8_t delay_ms){//only used by deep sleep
  for (uint8_t i = 0; i<(count*2-1); i++){
    if (i%2 == 0){
      analogWrite(pin, POWERS[DEFAULT_BRIGHTNESS_INDEX]);
    }else{
      digitalWrite(pin, 0);
    }
    delay(delay_ms);
  }
  digitalWrite(pin, 0);
}//end blink
  
void sleep_animation(){
  activity_mode = ACTIVITY_SLEEP;
  while (led_now > 0){
    operate();
  }
}//end sleep_animation

void wake_animation(){
  activity_mode = ACTIVITY_ON;
  led_goal = current_brightness_index;
  while (led_now != led_goal){
    operate();
  }
}//end wake_animation

void deep_sleep_animation(){
  activity_mode = ACTIVITY_DEEP_SLEEP;
  while (led_now > 0){
    operate();
  }
  blink(3,90);//num, delay
  analogWrite(pin, POWERS[DEFAULT_BRIGHTNESS_INDEX]);
  delay(1000);
  analogWrite(pin, 0);
}//end deepSleepAnimation
  
void deep_wake_animation(){
  activity_mode = ACTIVITY_ON;
  blink(3,60);//count, delay
}//end deep_wake_animation
