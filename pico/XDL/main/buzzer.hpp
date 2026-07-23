//valkor 2026-07-22

class Buzzer {
public:
	Buzzer(uint8_t buzzer_pin)
	: buzzer_pin(buzzer_pin){
		pinMode(buzzer_pin, OUTPUT);
		noTone(buzzer_pin);
	}

  void operate() {
		if (active) {
			run_effect();
			return;
		}
	}
  void beep(uint16_t freq, uint16_t duration_ms) {
    static step seq[] = {
      { true,  duration_ms, freq }
    };
    start_sequence(seq);
  }

	//void effect_reset()   { start_sequence(seq_reset); }
	//void effect_flicker() { start_sequence(seq_flicker); }
	void effect_battery_low() { start_sequence(seq_battery_low); }
  void effect_next_menu() { start_sequence(seq_next_menu); }
  void effect_next_logo() { start_sequence(seq_next_logo); }
	//void effect_turnon()  { start_sequence(seq_turnon); }

private:
	struct step {
		bool sound_enable;
		uint16_t duration_ms;
		uint16_t frequency_hz;
	};
	// static constexpr step seq_reset[] = {
	// 	{ true,  80, 600 },
	// 	{ true,  160, 800 },
	// };
	// static constexpr step seq_flicker[] = {
	// 	{ true,  80, 1000 },
	// 	{ false, 40,  0    },
	// 	{ true,  80, 1000 },
	// 	{ false, 40,  0    },
	// 	{ true,  80, 1000 },
	// 	{ false, 40,  0    },
	// 	{ true,  500, 300  },
	// };
	static constexpr step seq_battery_low[] = {
		{ true, 500,  300 },
		{ true, 750,  100 },
		{ true, 1000, 50  },
	};
  static constexpr step seq_next_menu[] = {
		{ true, 30,  300 },
		{ true, 60,  600 },
		{ true, 30, 300  },
	};
  static constexpr step seq_next_logo[] = {
		{ true, 40,  700 },
		{ true, 80,  400 },
		{ true, 40, 800  },
	};
	// static constexpr step seq_turnon[] = {
	// 	{ true, 100, 500 },
	// 	{ true, 150, 750 },
	// 	{ true, 200, 1000 },
	// };

	uint8_t buzzer_pin;
	const step* current = nullptr;
	size_t step_count = 0;
	size_t step_index = 0;
	bool active = false;
	unsigned long step_start_ms = 0;

	template <size_t N>
	void start_sequence(const step (&seq)[N]) {
		current = seq;
		step_count = N;
		step_index = 0;
		active = true;
		step_start_ms = millis();
		apply_step();
	}

	void run_effect() {
		unsigned long now = millis();
		if (now - step_start_ms < current[step_index].duration_ms) return;

		step_index++;
		if (step_index >= step_count) {
			noTone(buzzer_pin);
			active = false;
			return;
		}

		step_start_ms = now;
		apply_step();
	}

	void apply_step() {
		if (current[step_index].sound_enable) {
			tone(buzzer_pin, current[step_index].frequency_hz);
		} else {
			noTone(buzzer_pin);
		}
	}
};



/*class Buzzer {
public:
  Buzzer(uint8_t pin)
    : pin(pin),
      active(false),
      end_time(0) {
    pinMode(pin, OUTPUT);
    noTone(pin);
  }

  void operate() {
    if (!active) {
      return;
    }

    if (millis() >= end_time) {
      noTone(pin);
      active = false;
    }
  }

  void beep(uint16_t freq, uint16_t duration_ms) {
    tone(pin, freq);

    end_time = millis() + duration_ms;
    active = true;
  }

private:
  uint8_t pin;
  bool active;
  unsigned long end_time;
};  //Buzzer
*/