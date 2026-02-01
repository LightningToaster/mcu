#define PIN_DART_SENSE 28

#include "dart_detector.hpp"
DartDetector dart_detector(PIN_DART_SENSE);

void setup() {
  Serial.begin(9600);
}

void loop() {
  static uint8_t last_status = 0;  // stores previous state

  uint8_t status_dart = dart_detector.operate();
  
  if (status_dart != last_status) {
    switch(status_dart){
      case DART_NONE:
        //Serial.println(0);
      break;
      case DART_READY:
        Serial.println("ready");
      break;
      case MAG_EMPTY:
        Serial.println("MAG_EMPTY");
      break;
      case NEW_MAG:
        Serial.println("NEW_MAG");
      break;
    }
    last_status = status_dart;
  }
}