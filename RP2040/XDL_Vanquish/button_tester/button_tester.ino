#define SETUP_MESSAGE "XDL_button_tester@RP2040 (Valkor 2025-9-12)"

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(1, 16, NEO_GRB + NEO_KHZ800);

#include <Pushbutton.h>
Pushbutton deep_trigger(1);
Pushbutton trigger(2);
Pushbutton rev_trigger(3);

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 200);
  Serial.println(SETUP_MESSAGE);
  strip.begin();

  //pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, HIGH);
}

uint8_t r,g,b = 0;
void loop() {
  r = 0; g = 0; b = 0;
  if (deep_trigger.isPressed()){r = 12;}
  if (trigger.isPressed())     {b = 40;}
  if (rev_trigger.isPressed()) {g = 12;}
  strip.setPixelColor(0, r,g,b);
  strip.show();
}
