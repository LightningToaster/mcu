#include <Servo.h>
Servo esc_0;
Servo esc_1;

class Wheels{ 

  private:
  static constexpr uint16_t THROTTLE_ARM = 1020; // 20 below for reliability
  static constexpr uint16_t THROTTLE_IDLE = 1038;
  static constexpr uint16_t THROTTLE_MIN = 1200;//determines lowest FPS allowed
  static constexpr uint16_t THROTTLE_MAX = 1600;//determines maximum FPS allowed

  bool begun = false;
  int16_t speed_now = THROTTLE_ARM;
  int16_t speed_goal = THROTTLE_ARM;
  unsigned long ms_stop = 0;
  unsigned long ms_started_spin = 0;
  unsigned long ms = 0;
  bool braking = false;

  public:

  void begin(){
    esc_0.attach(PIN_ESC_0, THROTTLE_ARM, THROTTLE_MAX); // pulse width in microseconds
    esc_1.attach(PIN_ESC_1, THROTTLE_ARM, THROTTLE_MAX); // ^
    esc_0.writeMicroseconds(THROTTLE_ARM);
    esc_1.writeMicroseconds(THROTTLE_ARM);
    begun = true;
  }

  void operate(){
    if (not begun){begin();}
    if (millis()<3000){return;}//before 3s, escs might not yet be armed

    #if ARMED_WHEELS
      if (is_voltage_ok){
        esc_0.writeMicroseconds(speed_now); 
        esc_1.writeMicroseconds(speed_now); 
      }else{
        esc_0.writeMicroseconds(THROTTLE_ARM); 
        esc_1.writeMicroseconds(THROTTLE_ARM); 
      }
    #endif
        
    int16_t difference = speed_goal - speed_now;

    if (difference > 0){//need to speed up
      if (millis() - ms >= 1){
        ms = millis();
        speed_now += (2+ceil(difference/200.0));
        Serial.println(speed_now);
        if (speed_now > speed_goal){
          speed_now = speed_goal;
        }
      }
      
    }else if (difference < 0){//need to slow down
      if (millis() - ms >= 10 or (braking and millis() - ms >= 1)){
        speed_now -= 1;
        Serial.println(speed_now);
        if (speed_now <= speed_goal){
          speed_now = speed_goal;
        }
        ms = millis();
      }
      
    }else{
      if (speed_now == 0){
        ms_started_spin = 0;
      }
    }
  }//operate

  void set_speed(uint8_t power_setting){
    if (millis() < 3000 or power_setting == 0){
      speed_goal = THROTTLE_ARM;
      return;
    }
    int16_t new_speed = map(power_setting, 1, 9,THROTTLE_MIN,THROTTLE_MAX);
    speed_goal = new_speed;

    if (ms_started_spin == 0){
      ms_started_spin = millis();
    }
    braking = false;
  }//set_speed

  void idle(){
    speed_goal = THROTTLE_IDLE;
  }

  void stop(){
    braking = true;
    speed_goal = THROTTLE_ARM;
  }

};//Wheels