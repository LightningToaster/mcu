#define SETUP_MESSAGE "esc_tester@RP2040 (Valkor 2025-9-20)" 

#include <Pushbutton.h>
Pushbutton rev_trigger(12);//3

#define THROTTLE_ARM 1020 // 20 below for reliability
#define THROTTLE_MIN 1020 //copied from ESC settings
#define THROTTLE_MAX 1960 //^
#define THROTTLE_IDLE 1043 // 1043 seems quiet

#include <Servo.h>
Servo esc_0;
Servo esc_1;

constexpr int step_up_us = 10;
constexpr int step_down_us = 1;
constexpr int step_delay = 5;

int current_throttle = THROTTLE_ARM;
int target_throttle = 1300;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 200) {};
  Serial.println(SETUP_MESSAGE);

  esc_0.attach(5);//12
  esc_1.attach(6);//13

  esc_0.writeMicroseconds(THROTTLE_ARM);
  esc_1.writeMicroseconds(THROTTLE_ARM);
  delay(2000); // wait for arming

  Serial.println("enter throttle in µs (1043–1960):");
}//setup


void loop() {
  
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    int val = input.toInt();
    target_throttle = constrain(val, THROTTLE_MIN, THROTTLE_MAX);

    Serial.print("throttle value: ");
    Serial.print(target_throttle);
    Serial.println(" µs, press rev trigger to test");
  }

  if (rev_trigger.isPressed()){
    current_throttle += step_up_us;
    if (current_throttle > target_throttle) current_throttle = target_throttle;
  }else{
    current_throttle -= step_down_us;
    if (current_throttle < THROTTLE_IDLE) current_throttle = THROTTLE_IDLE;
  }
    
  esc_0.writeMicroseconds(current_throttle);
  esc_1.writeMicroseconds(current_throttle);
  delay(step_delay);

}//loop