#define SERIAL_NUMBER "#0001"
#define SOFTWARE_VERSION "v2025.5.10" //~950 lines of code
#define DEVICE_NAME "rapidstrike@RP2040"
/*TODO
NOTE: project abadoned due to barrel issues, code works but see below TODO list..
temperature is completely dissabled, shared ADC was causing stability issues.
oled_OLD is from single core system

TODO authentication
TODO MENU deep pull trigger to lockdown
TODO power cycle saving data
TODO running out of darts, opening access, or stock, should reset solenoid queue.  
..blaster shot unexpectidly a few times

*/

#define ARMED_WHEELS true  //comment to disable wheels (ESCs will still arm but NOT spin)
#define ARMED_SOLENOID true //comment to disable solenoid (solenoid will NOT actuate)
#define PRINT_LOOP_SPEED false

struct data_t { //defaults shown here, saved over power cycles
  uint8_t volley = 1;//darts fired per click
  uint8_t power_a =  1;//normal rev
  uint8_t power_b = 9;//deep pull rev
  uint8_t power_q = 1;//quickshots (rev not pulled)
  bool dart_detection = false;
  bool uv = false;//turn on UV light while reving
  //stats are below
  unsigned long all_time_darts_launched = 0;
  uint8_t failed_login_attempts = 0;
  uint16_t seconds_on = 0;//at 4h, lockout, stop counting, reset if unlocked.  TODO why save this?  ..definitely would have cooked eeprom -valkor 2026
};
data_t data;
uint8_t selected_setting = 0;
unsigned long darts_launched = 0; //session, resets on power cycle
uint16_t darts_shot_from_mag = 0; //used for display

#define PIN_SOLENOID 29
#define PIN_BUILTIN_LED 25
#define PIN_VOLTAGE 28 //ADC
#define PIN_DART_SENSE 26 //ADC

#define PIN_ESC_0 0
#define PIN_ESC_1 1

#define PIN_BUTTON_TRIGGER 12
#define PIN_BUTTON_TRIGGER_D 13
#define PIN_BUTTON_REV 14
#define PIN_BUTTON_REV_D 15
#define PIN_BUTTON_STOCK 11
#define PIN_BUTTON_DOOR 10
#define PIN_UV 5

#define VOLLEYS 3
#define POWER_LIMIT 9//makes high pitch sound above 6 with 1960, reduce timing or have fewer steps?

bool is_voltage_ok = false;
uint8_t status_trigger_main = 0;
uint8_t status_trigger_rev = 0;
uint8_t status_door = 0;
uint8_t status_stock = 0;
uint8_t status_dart = 0;

#if PRINT_LOOP_SPEED
static unsigned long last_time = 0;
static unsigned long loop_count = 0;
static float avg_loop_time = 0;
#endif

//#include <EEPROM.h>

#include "oled.hpp"
Oled oled;

#include "step_button.hpp"
StepButton trigger_main(PIN_BUTTON_TRIGGER, PIN_BUTTON_TRIGGER_D);
StepButton trigger_rev(PIN_BUTTON_REV, PIN_BUTTON_REV_D);
#include "switch.hpp"
Switch door(PIN_BUTTON_DOOR);
Switch stock(PIN_BUTTON_STOCK);

#include "voltmeter.hpp"
Voltmeter voltmeter(PIN_VOLTAGE);

#include "dart_detector.hpp"
DartDetector dart_detector;

#include "wheels.hpp"
Wheels wheels; 

#include "solenoid.hpp"
Solenoid solenoid;

#include "led.hpp"
LED uv;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 200) {} // TODO try 100ms
  Serial.println(DEVICE_NAME);
  wheels.begin();// starts arming, non-blocking

  Oled::instance.begin();
  //Oled::instance.display_startup_info();//TODO will this be annoying?
}

void loop() {
  status_trigger_main = trigger_main.operate();
  status_trigger_rev = trigger_rev.operate();
  status_door = door.operate();
  status_stock = stock.operate();
  status_dart = dart_detector.operate();
  is_voltage_ok = voltmeter.operate();

  wheels.operate();
  uv.operate();

  bool dart_fired = solenoid.operate(status_dart == DART_READY or data.dart_detection == false);
  if (dart_fired){
    darts_shot_from_mag++;
    darts_launched++;
    data.all_time_darts_launched++;
  }
  if (status_dart == MAG_CHANGED){
    darts_shot_from_mag = 0;
  }
  
  //TODO check authenticator class, first if expression 

  if (status_door == SWITCH_OPEN){//access door opened, open menu
    selected_setting = 0;//start at volley everytime we enter settings

  }else if (status_door == SWITCH_CLOSE){//access door closed, close menu
    
  }else if (status_door == SWITCH_IS_OPEN){//is access door open?
    wheels.stop();//best to call continuously for safety purposes..
    uv.set_goal(1);
    if (data.dart_detection == false){
      darts_shot_from_mag = 0;//access door resets dart counter if dart detection disabled
    }
    if (status_trigger_main == BUTTON_CLICK){
      switch (selected_setting) {
        case 0: data.volley = (data.volley % VOLLEYS) + 1; break;
        case 1: data.power_a = (data.power_a % POWER_LIMIT) + 1; break;
        case 2: data.power_b = (data.power_b % POWER_LIMIT) + 1; break;
        case 3: data.power_q = (data.power_q % POWER_LIMIT) + 1; break;
        case 4: data.dart_detection = !data.dart_detection; break;
        case 5: data.uv = !data.uv; break;
      }
      //TODO save EEPROM HERE
    }

    if (status_trigger_rev == BUTTON_CLICK){
      selected_setting = (selected_setting + 1) % 6;
    }else if (status_trigger_rev == BUTTON_DEEP_CLICK){
      selected_setting = (selected_setting < 6) ? 6 : 0;
    }

    if (selected_setting < 6){
      Oled::instance.display_menu(data, selected_setting);
    }else{
      Oled::instance.display_stats(data, voltmeter.get());
    }

  }else if (status_door == SWITCH_IS_CLOSED){//is access door closed?
    if (status_stock == SWITCH_IS_OPEN){//stock collapsed?
      Oled::instance.display_logo();
      wheels.stop();
      if (data.dart_detection == false){
        darts_shot_from_mag = 0;//stock resets dart counter if dart detection disabled
      }

    }else{//door is shut and stock is extended
      if (is_voltage_ok){
        if (status_dart == DART_READY or data.dart_detection == false){
          Oled::instance.display_mag(darts_shot_from_mag);
          
          if (status_trigger_rev == BUTTON_HOLD){
            wheels.set_speed(data.power_a);
            uv.set_goal(31);

          }else if (status_trigger_rev == BUTTON_DEEP_HOLD){
            wheels.set_speed(data.power_b);
            uv.set_goal(31);

          }else{
            
            if (status_trigger_main == BUTTON_HOLD or status_trigger_main == BUTTON_DEEP_HOLD){
              wheels.set_speed(data.power_q);
              uv.set_goal(10);

            }else{
              wheels.idle();
              uv.set_goal(1);
            }
            
          }

          switch (status_trigger_main) {
            case BUTTON_CLICK: solenoid.fire(data.volley); break;
            case BUTTON_DEEP_HOLD: solenoid.fire(1); break; // full auto
          }
                    


        }else{
          Oled::instance.display_empty();
          wheels.stop();
          uv.set_goal(0);
          darts_shot_from_mag = 0;

        }
      }else{//voltage wasnt ok
        Oled::instance.display_low_voltage(voltmeter.get_string());
        wheels.stop();
      }
    }

  }//if tree

  #if PRINT_LOOP_SPEED
    loop_count++;
    if (millis() - last_time >= 1000) { // every 1 second
      avg_loop_time = 1000.0 / loop_count;
      Serial.print("loop ms: ");
      Serial.println(avg_loop_time, 2); // 2 decimal places
      loop_count = 0;
      last_time = millis();
    }
  #endif
}//loop
