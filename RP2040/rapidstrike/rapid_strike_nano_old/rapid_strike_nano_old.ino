#define SETUP_MESSAGE "rapidstrike_v0.1.0@atmega328p (by Valkor 2023-07-17)"
/*TODO
  
  
  dart detection

  optimise solenoid based on voltage??
 

  AFTER getting pi pico zero
  oled class
  reorder the pins to make sense
  all classes have variables in scopes that make sense?
  motor idle speed?
*/



#define ARMED // comment to disable flywheels & solenoid


#define PIN_SOLENOID 2
#define PIN_VOLTAGE A7

#define PIN_ESC_A 3
#define PIN_ESC_B 4

#define PIN_BUTTON_TRIGGER_A 8
#define PIN_BUTTON_TRIGGER_B 10
#define PIN_BUTTON_REV_A 11
#define PIN_BUTTON_REV_B 9
#define PIN_BUTTON_DOOR 12
#define PIN_BUTTON_STOCK 7

#define PIN_IR_EMITTER 5
#define PIN_IR_RECEIVER A6

#define OPTIONS 3
#define VOLLEYS 3
#define POWERS 7//11

#define OPTION_VOLLEY 0
#define OPTION_POWER_A 1
#define OPTION_POWER_B 2

//#include <EEPROM.h>

//#include "oled.hpp"
//Oled oled;

#include "memory_reader.hpp"

#include "voltmeter.hpp"
Voltmeter voltmeter;

#include "dart_detector.hpp"
DartDetector dart_detector;

#include "wheels.hpp"
Wheels wheels; 

#include "solenoid.hpp"
Solenoid solenoid;

#include <Pushbutton.h>
#include "button_trigger.hpp"
ButtonTrigger button_trigger;
#include "button_rev.hpp"
ButtonRev button_rev;

#include "door.hpp"
Door door;

#include "stock.hpp"
Stock stock;

float voltage = 0;
bool is_voltage_ok = false;
uint8_t option = 0;
uint8_t volley = 1; //TODO use eeprom
uint8_t power_a = 1; 
uint8_t power_b = 2;
unsigned long darts_launched = 0;
unsigned long darts_launched_total = 0; //TODO eeprom

uint8_t button_status_trigger;
uint8_t button_status_rev;
uint8_t door_status;

void setup() {
  Serial.begin(9600);while (!Serial){}
  Serial.println(SETUP_MESSAGE);

  //oled.begin();

  Serial.println(freeMemory());

}//setup

void loop() {
  voltmeter.operate();
  voltage = voltmeter.get();
  is_voltage_ok = voltmeter.is_ok();
  dart_detector.operate();
  wheels.operate(is_voltage_ok);
  solenoid.operate(is_voltage_ok, dart_detector.is_dart_ready);
  
  
  button_status_trigger = button_trigger.operate();
  button_status_rev = button_rev.operate();

  door_status = door.operate();
  if (door_status >= DOOR_IS_OPEN){//CONFIG MODE
    if (door_status == DOOR_OPEN) {//config mode just opened
      go_cold();
      option = 0;//reset back to top option
      //oled.draw_static_menu();
      //oled.update_arrow();
      //oled.draw_menu(volley, power_a, power_b, darts_launched, darts_launched_total);
    }

    if (button_status_trigger == TRIGGER_CLICK){
      if (option == OPTION_VOLLEY){
        volley = (volley % VOLLEYS) + 1;
        //oled.draw_volley(volley);

      }else if (option == OPTION_POWER_A){
        power_a = (power_a + 1) % POWERS;
        //oled.draw_power_a(power_a);

      }else if (option == OPTION_POWER_B){
        power_b = (power_b + 1) % POWERS;
        //oled.draw_power_b(power_b);

      }
      //oled.update();
      print_config();
    }// trigger clicked

    if (button_status_rev == TRIGGER_CLICK){
      option = (option+1)%OPTIONS;
      //oled.update_arrow();
      print_config();
    }

    
    
  }else if (stock.operate() == STOCK_IS_COLLAPSED){//DISPLAY MODE
    go_cold();
    Serial.println(F("display mode"));
    
    //well display no worky bc ram go bye-bye so no need this rn

  }else{//OPERATING MODE

    if (dart_detector.get_ms_since_dart() <= 100 or true){//disabled for testing!
      if (button_status_rev == TRIGGER_HOLD_DEEP){
        wheels.set_speed(map(power_b, 0, 10,0,100));
      }else if (button_status_rev == TRIGGER_HOLD
      or button_status_trigger == TRIGGER_HOLD 
      or button_status_trigger == TRIGGER_HOLD_DEEP
      or solenoid.pulses_queued > 0){
        wheels.set_speed(map(power_a, 0, 10,0,100));
      }else{
        wheels.set_speed(0);
      }
      
      if (button_status_trigger == TRIGGER_CLICK){//shoot volley
        solenoid.pulses_queued = volley;
      }else if (button_status_trigger == TRIGGER_HOLD_DEEP){//full auto
        solenoid.pulses_queued = 1;
      }
    }else{//no dart
      //Serial.println(dart_detector.get_ms_since_dart());
      wheels.set_speed(0);
      
      //TODO tell oled to display "EMPTY"
     }
      
    solenoid.wheels_ready = wheels.spunup();
  }//if


//NEVER USE THIS CODE
  // if (millis() > 10000 and millis() <11000){ //AUTO TEST MOTORS
  //   wheels.set_speed(1200);
  // }else{
  //   wheels.stop();
  // }

}//loop

void go_cold(){
  wheels.set_speed(0);
  solenoid.pulses_queued = 0;
  solenoid.retract();
}

void print_config(){
  Serial.print("opt=");
  Serial.print(option);
  Serial.print(", #=");
  Serial.print(volley);
  Serial.print(", A=");
  Serial.print(power_a);
  Serial.print(", B=");
  Serial.print(power_b);
  Serial.print(", DL=");
  Serial.print(darts_launched);
  Serial.print(", DLT=");
  Serial.print(darts_launched_total);
  Serial.print(",   ");
  Serial.println(voltmeter.get_string());
}




