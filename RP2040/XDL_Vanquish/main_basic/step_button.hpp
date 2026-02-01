#pragma once
#include <Pushbutton.h>

enum BUTTON_STATE : uint8_t {
  BUTTON_IDLE,       // 0
  BUTTON_HOLD,       // 1
  BUTTON_CLICK,      // 2
  BUTTON_DEEP_HOLD,  // 3
  BUTTON_DEEP_CLICK  // 4
};

class StepButton { 
private:
	Pushbutton button;
	Pushbutton button_d;
  unsigned long ms = 0;
	bool ignore_shallow = false;
  uint8_t button_d_pin;

public:
	// constructor for shallow+deep buttons
	StepButton(uint8_t pin, uint8_t pin_d) : button(pin), button_d(pin_d), button_d_pin(pin_d) {}

	// optional constructor for single shallow button
	StepButton(uint8_t pin) : button(pin), button_d(255), button_d_pin(255) {} // 255 = unused

	BUTTON_STATE operate() {
		unsigned long now = millis();

		// shallow button release
		if (button.getSingleDebouncedRelease()) {
			ms = 0;
			if (!ignore_shallow) {
				return BUTTON_CLICK;
			} else {
				ignore_shallow = false;
			}
		}

		// deep button release, only if valid pin
		if (button_d_pin != 255 && button_d.getSingleDebouncedRelease()) {
			ms = 0;
			return BUTTON_DEEP_CLICK;
		}

		// deep hold takes priority
		if (button_d_pin != 255 && button_d.isPressed()) {
			if (ms == 0) ms = now;
			ignore_shallow = true;
			return BUTTON_DEEP_HOLD;
		}

		// shallow hold
		if (button.isPressed()) {
			if (ms == 0) ms = now;
			return BUTTON_HOLD;
		}

		return BUTTON_IDLE;
	}
};//StepButton
