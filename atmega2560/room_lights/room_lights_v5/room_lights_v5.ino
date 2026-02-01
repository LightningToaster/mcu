#define SETUP_MESSAGE "room_lights_v5.5.0@atmega2560 (designed by Valkor 2023-04-12)" //963? lines
/* Serial Input instructions
 * 0 upper click (if off/night/sleep, turn on.  if on, go to sleep)
 * 1 upper hold (if on, turn off.  else, turn on min brightness)
 * 2 upper long (turn on nightlight)
 * 3 lower click (if on, next brightness.  else turn on at minimum brightness)
 * 4 lower hold (next zone)
 * 5 lower long (next mode (animations))
 * 6 both click (unreliable & unused)
 * 7 both hold
 * 8 both long (toggle insomnia 3 hours)
 */
/* TODO list
 * bring back glimmer mode?
 * RWB glimmer mode?
 * remote only lock mode?
 * music reactive mode?
 * another remote?
 */
#define AUTO_SHUTOFF_SECONDS 600 // 300 is 5min
#define NIGHTLIGHT_SHUTOFF_SECONDS 60

#define PIN_MOTION_CEILING 4
#define PIN_MOTION_WALL 14
#define PIN_INDICATOR_LED 6
#define PIN_HALO_UPPER 12
#define PIN_HALO_LOWER 10
#define PIN_BUTTONS A14

#include "buttons.hpp"
Buttons buttons(PIN_BUTTONS);

#include "remote.hpp"
Remote remote;

#include "lights_controller.hpp"
Lights_Controller lights_controller;

#include "indicator_led.hpp"
Indicator_Led indicator_led;

#include "motion_sensor.hpp"
Motion_Sensor motion_sensor_ceiling(PIN_MOTION_CEILING);
Motion_Sensor motion_sensor_wall(PIN_MOTION_WALL);

void setup() {
  Serial.begin(9600);
  while (!Serial){}
  Serial.println(SETUP_MESSAGE);
}//setup

void loop(){
  
  int16_t status = lights_controller.operate() + indicator_led.operate();
  
  if (status == 0){
    handle_remote_input();
  }
  handle_button_input();


  long wall = motion_sensor_wall.operate();
  long ceiling = motion_sensor_ceiling.operate();

//  Serial.print(wall);
//  Serial.print(", ");
//  Serial.println(ceiling);
  

  switch(lights_controller.state){
    case STATE_ON:
      //should we sleep?
      if (wall >= AUTO_SHUTOFF_SECONDS or ceiling >= AUTO_SHUTOFF_SECONDS){
        lights_controller.sleep();
        Serial.println("sleep");
      }
    break;

    case STATE_OFF:
      //should we go into night light?
      if (wall == 0 and lights_controller.is_blacked_out()){//check only wall, might sure blackout is done
        lights_controller.night_light();
      }
    break;

    case STATE_SLEEPING:
      //should we wake up? 
      if (wall == 0 or ceiling == 0){
        lights_controller.turn_on();
        motion_sensor_wall.reset();
        motion_sensor_ceiling.reset();
        Serial.println("wake up, timers reset");
      }
    break;

    case STATE_NIGHTLIGHT:
      //should we reset nightlight timer?
      if (motion_sensor_wall.operate() == 0){
        motion_sensor_wall.reset();
        Serial.println("reset nightlight timer");
        
      //should we turn off nightlight?
      }else if (motion_sensor_wall.get_s() >= NIGHTLIGHT_SHUTOFF_SECONDS){
        lights_controller.turn_off();
        Serial.println("turning off nightlight");
      }
    break;
  }
  
}//loop

void check_for_serial_input(struct buttons_state &input){
  if (Serial.available() > 0) {// use serial input from computer if available
    char rx_byte = Serial.read();  
    int num = rx_byte - '0';
    switch(num){
      case 0: input.button = BUTTON_UPPER; input.action = BUTTON_CLICK; break;
      case 1: input.button = BUTTON_UPPER; input.action = BUTTON_HOLD; break;
      case 2: input.button = BUTTON_UPPER; input.action = BUTTON_LONG; break;
      case 3: input.button = BUTTON_LOWER; input.action = BUTTON_CLICK; break;
      case 4: input.button = BUTTON_LOWER; input.action = BUTTON_HOLD; break;
      case 5: input.button = BUTTON_LOWER; input.action = BUTTON_LONG; break;
      case 6: input.button = BUTTON_BOTH; input.action = BUTTON_CLICK; break;
      case 7: input.button = BUTTON_BOTH; input.action = BUTTON_HOLD; break;
      case 8: input.button = BUTTON_BOTH; input.action = BUTTON_LONG; break;
    }//switch
  }//if
}//check_for_serial_input

void handle_button_input(){
  struct buttons_state input = buttons.operate();
  check_for_serial_input(input);

  if (input.action != 0){
    motion_sensor_wall.reset();
    motion_sensor_ceiling.reset();//reset motion detectors if button(s) are pressed
    
    switch(input.button){//upper or lower button?
      case BUTTON_UPPER:
        switch(input.action){
          case BUTTON_CLICK: upper_click(); break;
          case BUTTON_HOLD: upper_hold(); break;
          case BUTTON_LONG: upper_long(); break;
          
        }//action
      break;//BUTTON_UPPER
    
      case BUTTON_LOWER:
        switch(input.action){
          case BUTTON_CLICK: lower_click(); break;
          case BUTTON_HOLD: lower_hold(); break;
          case BUTTON_LONG: lower_long(); break;
        }//action
      break;//BUTTON_LOWER

      case BUTTON_BOTH:
        switch(input.action){
          case BUTTON_CLICK: Serial.println("both click (finicky, don't use this)"); break;
          case BUTTON_HOLD: Serial.println("both hold (does nothing)"); break;
          case BUTTON_LONG: toggle_insomnia(); Serial.print("both long (toggle insomnia)"); break;
        }//action
      break;//BUTTON_BOTH
      
    }//which button?
  }//if input
}//handle_button_input

void upper_click(){Serial.print("upper click");
  switch(lights_controller.state){
    case STATE_NIGHTLIGHT: 
    case STATE_SLEEPING: 
    case STATE_OFF: 
      lights_controller.turn_on(); 
      Serial.println(" (wake up)");
    break;

    case STATE_ON: 
      lights_controller.sleep(); 
      Serial.println(" (go to sleep)");
    break;

    case STATE_PARTY:
      Serial.println(" (TODO in party)");
    break;
  }
}//upper click

void upper_hold(){            
  switch(lights_controller.state){
    case STATE_ON: 
      lights_controller.turn_off();
      Serial.println(" (turn off)");
    break;

    default: 
      lights_controller.next_pattern(0);
      Serial.println(" (next pattern)");
    break;
  }
}

void upper_long(){Serial.println("upper long (night light)");
  lights_controller.night_light(); 
}

void lower_click(){
  switch(lights_controller.state){
    case STATE_ON: 
      lights_controller.next_pattern();
      Serial.println("lower click (next pattern)");
    break;

    default: 
      lights_controller.next_pattern(0); 
      Serial.println("lower click (minimum brightness pattern)");
    break;
  }
}

void lower_hold(){
  switch(lights_controller.state){
    case STATE_ON: 
      lights_controller.next_zone();
      Serial.println("lower hold (next zone)");
    break;

  }
}

void lower_long(){
  Serial.println("lower long hold (next mode)");
  lights_controller.next_mode();
}

void toggle_insomnia(){
  if (motion_sensor_ceiling.get_s() < 0 or motion_sensor_wall.get_s() < 0){
    motion_sensor_wall.set_dormant_s(0);
    motion_sensor_ceiling.set_dormant_s(0);
    indicator_led.set_color(0,0,0);
    Serial.println("(disabled exam mode)");
  }else{
    motion_sensor_wall.set_dormant_s(10800);
    motion_sensor_ceiling.set_dormant_s(10800);
    indicator_led.set_color(9,0,0);
    Serial.println("(exam mode, auto-off disabled for 3 hours)");
  }
}//toggle_exam_mode

void handle_remote_input(){
  uint8_t remote_input = remote.operate();
  switch(remote_input){
    case REMOTE_POWER: lights_controller.toggle(); break;
    //case REMOTE_MODE_INCREASE: lights_controller.next_mode();break;//DOESNT WORK
    //case REMOTE_MODE_DECREASE: lights_controller.previous_mode();break;//DOESNT WORK
    case REMOTE_SPEED_INCREASE: break;
    case REMOTE_SPEED_DECREASE: break;
    case REMOTE_COLOR_INCREASE: lights_controller.increase_warmth(); break; 
    case REMOTE_COLOR_DECREASE: lights_controller.decrease_warmth(); break;
    case REMOTE_BRIGHTNESS_INCREASE: lights_controller.next_pattern(); break;
    case REMOTE_BRIGHTNESS_DECREASE: lights_controller.previous_pattern(); break;
    case REMOTE_WHITE: lights_controller.next_color(0); break;
    case REMOTE_RED: lights_controller.next_color(1); break;
    case REMOTE_GREEN: lights_controller.next_color(2); break;
    case REMOTE_BLUE: lights_controller.next_color(3); break;
    case REMOTE_ORANGE: lights_controller.next_color(4); break;
    case REMOTE_PURPLE: lights_controller.next_color(5); break;
    case REMOTE_CYAN: lights_controller.next_color(6); break;
    case REMOTE_DEMO: toggle_insomnia(); break; 
    
  }
  if (remote_input != 0){
    motion_sensor_wall.reset();
    motion_sensor_ceiling.reset();
  }
  
}//handle_remote_input
