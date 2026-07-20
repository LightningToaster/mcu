// valkor 2026-1-24
//only registers clicks and holds (holds do not require button release)
#pragma once
#include <Pushbutton.h>

enum BUTTON_STATE : uint8_t {
	BUTTON_IDLE,
	BUTTON_HOLD,
	BUTTON_CLICK
};

class Button {
private:
	Pushbutton button;
	unsigned long press_start_ms = 0;
	unsigned long hold_time_ms;
	bool hold_fired = false;

public:
	// hold_time_ms = threshold in milliseconds
	Button(uint8_t pin, unsigned long hold_time_ms)
		: button(pin), hold_time_ms(hold_time_ms) {}

BUTTON_STATE operate() {
	unsigned long now = millis();

	// button pressed
	if (button.isPressed()) {
		if (press_start_ms == 0) {
			press_start_ms = now;
			hold_fired = false;
		}

		if (!hold_fired && (now - press_start_ms >= hold_time_ms)) {
			hold_fired = true;
			return BUTTON_HOLD;
		}

		return BUTTON_IDLE;
	}

	// button not pressed -> reset press tracking
	if (press_start_ms != 0) {
		unsigned long held_ms = now - press_start_ms;
		press_start_ms = 0;

		if (!hold_fired && held_ms < hold_time_ms) {
			return BUTTON_CLICK;
		}
	}

	return BUTTON_IDLE;
}

};//Button
