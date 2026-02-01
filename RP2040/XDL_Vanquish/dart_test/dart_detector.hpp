enum DART_STATE : uint8_t {
	DART_NONE = 0,   // no dart detected
	DART_READY,      // dart detected and stable
	MAG_EMPTY,       // one-shot pulse when mag goes empty
	NEW_MAG          // one-shot pulse when new mag inserted
};

class DartDetector {
public:
	explicit DartDetector(uint8_t pin)
		: pin(pin), ms_none(0), ms_dart(0),
		  last_state(DART_NONE), pulse_pending(DART_NONE) {
		pinMode(pin, INPUT);
	}

	DART_STATE operate() {
		unsigned long now = millis();
		int16_t value = analogRead(pin);

		// handle one-shot pulse first if queued
		if (pulse_pending != DART_NONE) {
			DART_STATE p = pulse_pending;
			pulse_pending = DART_NONE;
			return p;
		}

		// dart present
		if (value >= THRESHOLD) {
			if (last_state != DART_READY) {
				// transition: no dart -> dart
				if (mag_empty_sent) {
					// we had an empty mag, now we see new dart
					pulse_pending = NEW_MAG;
					mag_empty_sent = false;
					last_state = DART_READY;
					return DART_READY; // still return ready in same cycle
				}
			}
			// require dart stable for DART_LOAD_MS
			if (now - ms_none >= DART_LOAD_MS) {
				last_state = DART_READY;
			}
			ms_dart = now;
		} 
		// no dart
		else {
			if (last_state == DART_READY) {
				// record first time we saw none
				ms_none = now;
			}

			if (now - ms_none >= MIN_MAG_CHANGE_MS && !mag_empty_sent) {
				// been empty long enough → one-shot MAG_EMPTY
				pulse_pending = MAG_EMPTY;
				mag_empty_sent = true;
				last_state = DART_NONE;
				return MAG_EMPTY;
			}

			last_state = DART_NONE;
		}

		return last_state;
	}

private:
	static constexpr int16_t THRESHOLD = 300; // analog threshold
	static constexpr int16_t DART_LOAD_MS = 20;
	static constexpr int16_t MIN_MAG_CHANGE_MS = 250;

	uint8_t pin;
	unsigned long ms_none;
	unsigned long ms_dart;
	DART_STATE last_state;
	DART_STATE pulse_pending;
	bool mag_empty_sent = false;
};
