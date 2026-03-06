#include <Pushbutton.h>
#define BUTTON_PIN 12

Pushbutton button(BUTTON_PIN);

void setup(){
  Serial.begin(9600);
}

void loop()
{
  if (button.getSingleDebouncedRelease()){
    Serial.println("click");
  }
}
