#pragma once
#include <Pushbutton.h>

// strongly typed enum for switch states
enum SWITCH_STATE : uint8_t {
	SWITCH_IS_OPENED, // 0
	SWITCH_IS_CLOSED, // 1
	SWITCH_OPEN,      // 2
	SWITCH_CLOSE      // 3
};

class Switch { 
private:
	Pushbutton button;
	unsigned long ms = 0;
	bool is_closed = false;
	static constexpr int8_t DEBOUNCE_MS = 100;

public:
	Switch(uint8_t pin) : button(pin) {}

	SWITCH_STATE operate() {
		unsigned long now = millis();

		if (button.isPressed()) {
			if (!is_closed && now - ms >= DEBOUNCE_MS) {
				ms = now;
				is_closed = true;
				return SWITCH_CLOSE;
			}
			return SWITCH_IS_CLOSED;
		} else {
			if (is_closed && now - ms >= DEBOUNCE_MS) {
				ms = now;
				is_closed = false;
				return SWITCH_OPEN;
			}
			return SWITCH_IS_OPENED;
		}
	}
};//Switch