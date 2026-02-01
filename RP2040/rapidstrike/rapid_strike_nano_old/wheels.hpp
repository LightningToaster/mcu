//#define DO_FANCY_BRAKING
#define THROTTLE_MIN 990//990
#define THROTTLE_BASE 1500//determines lowest FPS allowed
#define THROTTLE_MAX 1950//1950

#define SPINUP_MIN_MS 400 //approximate

#include <Servo.h>
Servo esc_a;
Servo esc_b;

class Wheels{ 
  public:

  void begin(){
    esc_a.attach(PIN_ESC_A,THROTTLE_MIN,THROTTLE_MAX); // (pin, min pulse width, max pulse width in microseconds) 
    esc_b.attach(PIN_ESC_B,THROTTLE_MIN,THROTTLE_MAX); // (pin, min pulse width, max pulse width in microseconds) 
    begun = true;
  }

  void operate(bool is_voltage_ok){
    if (not begun){begin();}

    #ifdef ARMED
      if (is_voltage_ok){
        esc_a.writeMicroseconds(speed_now); 
        esc_b.writeMicroseconds(speed_now); 
      }else{
        esc_a.writeMicroseconds(THROTTLE_MIN); 
        esc_b.writeMicroseconds(THROTTLE_MIN); 
      }
    #endif
        
    int16_t difference = speed_goal - speed_now;

    if (difference > 0){//need to speed up
      if (millis() - ms >= 5){
        ms = millis();
        speed_now += ceil(500.0/difference);//10
        if (speed_now > speed_goal){
          speed_now = speed_goal;[]
        }
      }
      
    }else if (difference < 0){//need to slow down
      #ifdef DO_FANCY_BRAKING
        if (millis() - ms >= 80){
          ms = millis();
          speed_now -= ceil(difference/-5.0);
          if (speed_now <= speed_goal){
            speed_now = speed_goal;
            //TODO at speed bool
          }
        }
      #else
        if (millis() - ms >= 5){
          ms = millis();
          speed_now -= ceil(difference/-100.0);
          if (speed_now <= speed_goal){
            speed_now = speed_goal;
          }
        }
      #endif
      
    }else{
      if (speed_now == 0){
        ms_started_spin = 0;
      }
    }
    

  }//operate

  void set_speed(uint8_t new_speed_percent){
    
    if (millis() < 5000 or new_speed_percent == 0){
      speed_goal = THROTTLE_MIN;
      return;
    }
    int new_speed = map(new_speed_percent, 0, 100,THROTTLE_BASE,THROTTLE_MAX);
    speed_goal = new_speed;

    if (ms_started_spin == 0){
      ms_started_spin = millis();
    }

  }


  bool spunup(){
    if (ms_started_spin == 0){
      return false;
    }else{
      return (millis() - ms_started_spin >= SPINUP_MIN_MS);
    }
    
  }


  private:
  bool begun = false;
  int16_t speed_now = THROTTLE_MIN;
  int16_t speed_goal = THROTTLE_MIN;

  unsigned long ms_stop = 0;
  unsigned long ms_started_spin = 0;
  unsigned long ms = 0;

};//Wheels


