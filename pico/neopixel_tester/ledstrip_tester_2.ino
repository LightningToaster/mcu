#define PIN_LED_STRIP 15
#define NUM_PIXELS 64
#define PIN_BUTTON 0



#include "button.hpp"
Button button(PIN_BUTTON);

#include "strip.hpp"
Strip strip(PIN_LED_STRIP, NUM_PIXELS);

void setup() {
	Serial.begin(9600);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);

	delay(5000);

	strip.begin();
}

uint8_t mode = 0;
void loop() {
	strip.operate();
	uint8_t button_status = button.operate();
	
	if (button_status == BUTTON_CLICK){
		mode = (mode % 9) + 1;
		strip.set_all(
			COLORS[mode-1][0]/5,
			COLORS[mode-1][1]/5,
			COLORS[mode-1][2]/5
		);
	} 
}
