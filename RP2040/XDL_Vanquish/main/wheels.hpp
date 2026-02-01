#include <Servo.h>
//1115=20fps
//1300=70fps
//1415=100fps
class Wheels { 

  private:
  static constexpr uint16_t THROTTLE_ARM   = 1020; // 20 below for reliability
  static constexpr uint16_t THROTTLE_IDLE  = 1043; // seems quiet
  static constexpr uint16_t THROTTLE_MIN   = 1115; // determines lowest fps allowed
  static constexpr uint16_t THROTTLE_MAX   = 1960; // determines maximum fps allowed
  static constexpr uint16_t ESC_ARM_DELAY  = 3000; // ms to allow escs to arm

  uint8_t pin_left;
	uint8_t pin_right;
  Servo motor_left;
  Servo motor_right;
  bool begun = false;

  uint16_t speed_now   = THROTTLE_ARM;
  uint16_t speed_goal  = THROTTLE_ARM;
  uint32_t ms_stop = 0;
  uint32_t ms_started_spin = 0;
  uint32_t ms = 0;
  bool braking = false; // more abrupt stop, if out of darts

  public:

  Wheels(uint8_t pin_esc_0, uint8_t pin_esc_1) 
		: pin_left(pin_esc_0), pin_right(pin_esc_1) {}

  void begin() {
		motor_left.attach(pin_left, THROTTLE_ARM, THROTTLE_MAX);
		motor_right.attach(pin_right, THROTTLE_ARM, THROTTLE_MAX);
		motor_left.writeMicroseconds(THROTTLE_ARM);
		motor_right.writeMicroseconds(THROTTLE_ARM);
    begun = true;
	}

  bool operate() {
    if (not begun)(begin());
    uint32_t now = millis();
    if (now < ESC_ARM_DELAY) { return false; }

    #ifdef ARMED_WHEELS
      motor_left.writeMicroseconds(speed_now); 
      motor_right.writeMicroseconds(speed_now); 
    #endif
        
    int32_t difference = static_cast<int32_t>(speed_goal) - speed_now;

    if (difference > 0) { // need to speed up
      if (now - ms >= 1) {
        ms = now;
        speed_now += (2 + ceil(difference / 200.0));
        if (speed_now > speed_goal) {
          speed_now = speed_goal;
        }
        #ifdef DEBUG
          Serial.println(speed_now);
        #endif
      }
      
    } else if (difference < 0) { // need to slow down
      uint16_t step = (braking ? 3 : 1); // braking accelerates decel
      uint16_t interval = (braking ? 1 : 10);
      if (now - ms >= interval) {
        ms = now;
        if (speed_now > step) {
          speed_now -= step;
        } else {
          speed_now = 0;
        }
        if (speed_now <= speed_goal) {
          speed_now = speed_goal;
        }
        #ifdef DEBUG
          Serial.println(speed_now);
        #endif
      }
      
    } else {
      if (speed_now == 0) {
        ms_started_spin = 0;
      }
    }
    return (difference <= 5);
  }// operate

  void set_throttle_percentage(uint8_t percentage) {
    uint32_t now = millis();
    if (now < ESC_ARM_DELAY || percentage == 0) {
      speed_goal = THROTTLE_ARM;
      return;
    }
    if (percentage > 100) { percentage = 100; }
    int16_t new_percentage = map(percentage, 1, 100, THROTTLE_MIN, THROTTLE_MAX);
    speed_goal = static_cast<uint16_t>(new_percentage);

    if (ms_started_spin == 0) {
      ms_started_spin = now;
    }
    braking = false;
  }// set_speed

  void idle() {
    speed_goal = THROTTLE_IDLE;
  }

  void stop() {
    braking = true;
    speed_goal = THROTTLE_ARM;
  }
  

};// Wheels
