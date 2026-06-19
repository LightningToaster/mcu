//enter values to set speed gradually

#define THROTTLE_ARM 980 // 20 below for reliability
#define THROTTLE_MIN 1000
#define THROTTLE_MAX 2000
#define THROTTLE_IDLE 1100 // 1025

#include <Servo.h>
Servo esc_0;
Servo esc_1;

const int step_us = 2;
const int step_delay = 5;

int current_throttle = THROTTLE_MIN;
int target_throttle = THROTTLE_MIN;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 100) {};

  pinMode(PIN_LED, OUTPUT);

  esc_0.attach(0);
  esc_1.attach(1);

  esc_0.writeMicroseconds(THROTTLE_MIN);
  esc_1.writeMicroseconds(THROTTLE_MIN);
  delay(2000); // wait for arming

  Serial.println("enter throttle in µs (1040–1300):");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    int val = input.toInt();
    target_throttle = constrain(val, THROTTLE_MIN, THROTTLE_MAX);

    Serial.print("ramping to: ");
    Serial.print(target_throttle);
    Serial.println(" µs");
  }

  if (current_throttle != target_throttle) {
    if (current_throttle < target_throttle) {
      current_throttle += step_us;
      if (current_throttle > target_throttle) current_throttle = target_throttle;
    } else {
      current_throttle -= step_us;
      if (current_throttle < target_throttle) current_throttle = target_throttle;
    }

    esc_0.writeMicroseconds(current_throttle);
    esc_1.writeMicroseconds(current_throttle);
    delay(step_delay);
  }
}
