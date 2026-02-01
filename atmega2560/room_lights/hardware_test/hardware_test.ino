#include <Adafruit_NeoPixel.h>
#define NUM_PIXELS 20

Adafruit_NeoPixel strips[8] = {                         
  Adafruit_NeoPixel(NUM_PIXELS, 29, NEO_RGBW + NEO_KHZ800),// 0 = door
  Adafruit_NeoPixel(NUM_PIXELS, 27, NEO_RGBW + NEO_KHZ800),// 1 = computer
  Adafruit_NeoPixel(NUM_PIXELS, 25, NEO_RGBW + NEO_KHZ800),// 2 = bathroom
  Adafruit_NeoPixel(NUM_PIXELS, 24, NEO_RGBW + NEO_KHZ800),// 3 = bed
  Adafruit_NeoPixel(NUM_PIXELS, 26, NEO_RGBW + NEO_KHZ800),// 4 = pillow
  Adafruit_NeoPixel(NUM_PIXELS, 28, NEO_RGBW + NEO_KHZ800),// 5 = window
  Adafruit_NeoPixel(NUM_PIXELS, 30, NEO_RGBW + NEO_KHZ800),// 6 = drawers
  Adafruit_NeoPixel(NUM_PIXELS, 31, NEO_RGBW + NEO_KHZ800) // 7 = closet
};

#define PIN_MOTION_CENTER 4
#define PIN_MOTION_PLATE 14
#define PIN_HALO_UPPER 12
#define PIN_HALO_LOWER 10
#define PIN_BUTTONS A14
#define PIN_RF 0//physical pin 2

#include <RCSwitch.h>
RCSwitch remote = RCSwitch();

void setup() {

  for (uint8_t strip = 0; strip<8; strip++){
    strips[strip].begin();
    strips[strip].setBrightness(255);
    for (uint8_t led = 0; led<NUM_PIXELS; led++){
      //strips[strip].setPixelColor(led,255,60,0,255);
      //strips[strip].setPixelColor(led,255,200,200,255);
      strips[strip].setPixelColor(led,30,7,0,50);
      //strips[strip].setPixelColor(led,255,255,255,255);

    }//inner for
    strips[strip].show();
  }//outer for

  Serial.begin(9600);

  remote.enableReceive(PIN_RF);
}//setup


//uint8_t count = 0;
void loop() {
  //test_halos();
  test_buttons();
  test_motion_center();
  //test_motion_plate();

   if (remote.available()) {
    int value = remote.getReceivedValue();
    Serial.println(value);
    remote.resetAvailable();
  }
//  if (count < 10 ){
//      for (uint8_t i = 0; i<255; i++){
//      for (uint8_t led = 0; led<NUM_PIXELS; led++){
//        strips[0].setPixelColor(led,0,0,i,0);
//      }//inner for
//      strips[0].show();
//    }//outer for
//    count++;
//  }
  
  
}//loop

void test_halos(){
  for (uint8_t i = 0; i<255; i++){
    analogWrite(PIN_HALO_UPPER, i);
    analogWrite(PIN_HALO_LOWER, 255-i);
    delay(4);
  }
  for (uint8_t i = 0; i<255; i++){
    analogWrite(PIN_HALO_UPPER, 255-i);
    analogWrite(PIN_HALO_LOWER, i);
    delay(4);
  }
}//test_halos

void test_buttons(){
  uint16_t input = analogRead(PIN_BUTTONS);
  if (input > 100){
    Serial.println(input);
  }
}//test_buttons

bool last_state_center = LOW;
void test_motion_center(){
  bool state = digitalRead(PIN_MOTION_CENTER);
  if (state != last_state_center){
    Serial.println(state);
    last_state_center = state;
  }
}//test_motion_center

bool last_state_plate = LOW;
void test_motion_plate(){//it flips between 1 and 0 when activated
  bool state = digitalRead(PIN_MOTION_PLATE);
  if (state != last_state_plate){
    Serial.println(state);
    last_state_plate = state;
  }
}//test_motion_plate
