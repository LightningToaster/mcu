//Valkor 2021-04-29

#include "laser_sensors.hpp"
LaserSensors sensors;

void setup() {
  Serial.begin(9600);
  digitalWrite(LED_BUILTIN, LOW);
}// setup

unsigned long cycles = 0;
void loop() {
  uint8_t result = sensors.operate();
  if (result == TRIGGERED){
    Serial.println("detected");
    //digitalWrite(LED_BUILTIN, HIGH);

  }
  cycles++;
  if (millis()%1000 == 0 and cycles >= 500){
    Serial.println(cycles);
    cycles = 0;
  }
}// loop
