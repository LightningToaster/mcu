//2 20A BLHeli_s ESCs are powered with 4s.  
//signal goes through level shifters then to escs

#define PIN_TOUCH		4
#define PIN_SHALLOW	3
#define PIN_DEEP    2
#define PIN_UV 14

#define THROTTLE_ARM 1030 // below for reliability
#define THROTTLE_MIN 1020 //copied from ESC settings
#define THROTTLE_MAX 1960 //^
#define THROTTLE_IDLE 1060 // whatever seems quiet

#include <Servo.h>
Servo esc_0;
Servo esc_1;

constexpr int step_up_us = 10;
constexpr int step_down_us = 1;
constexpr int step_delay = 5;

int current_throttle = THROTTLE_ARM;
int target_throttle = 1200;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 500) {};

  
  //delay(500);
  esc_0.attach(8,THROTTLE_ARM, THROTTLE_MAX);
  esc_1.attach(9, THROTTLE_ARM, THROTTLE_MAX);

  esc_0.writeMicroseconds(THROTTLE_ARM);
  esc_1.writeMicroseconds(THROTTLE_ARM);
  delay(3000); // wait for arming

  analogWrite(PIN_UV, 0);//make sure uv is off
  pinMode(PIN_DEEP, INPUT_PULLUP);
  pinMode(PIN_SHALLOW, INPUT_PULLUP);

  Serial.println("enter throttle in µs (1020–1960):");
}//setup

//void loop(){}

void loop() {
  
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    int val = input.toInt();
    target_throttle = constrain(val, THROTTLE_MIN, THROTTLE_MAX);

    Serial.print("throttle value: ");
    Serial.print(target_throttle);
    Serial.println(" µs, press trigger to test");
  }

  if (digitalRead(PIN_DEEP) == LOW){
    current_throttle += step_up_us;
    if (current_throttle > target_throttle) current_throttle = target_throttle;
  }else if (digitalRead(PIN_SHALLOW) == LOW){
    current_throttle -= step_down_us;
    if (current_throttle < THROTTLE_IDLE) current_throttle = THROTTLE_IDLE;
  }else{
    current_throttle -= step_down_us;
    if (current_throttle < THROTTLE_MIN) current_throttle = THROTTLE_MIN;
  }
    
  esc_0.writeMicroseconds(current_throttle);
  esc_1.writeMicroseconds(current_throttle);
  delay(step_delay);

}//loop