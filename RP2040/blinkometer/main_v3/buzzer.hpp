#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
static Adafruit_NeoPixel status_led(1, 25, NEO_GRB + NEO_KHZ800);

class Buzzer {
public:
	explicit Buzzer(uint8_t buzzer_pin)
	: buzzer_pin(buzzer_pin){
		pinMode(buzzer_pin, OUTPUT);
		//pinMode(LED_BUILTIN, OUTPUT);

		status_led.begin();
		//status_led.setBrightness(255);
		status_led.clear();
		status_led.show();

		buzzer_off();
	}

	// effect triggers
	void effect_reset()   { start_sequence(seq_reset); }
	void effect_reset2()   { start_sequence(seq_reset2); }
	void effect_flicker() { start_sequence(seq_flicker); }
	void effect_turnoff() { start_sequence(seq_turnoff); }
	void effect_turnon()  { start_sequence(seq_turnon); }
	void effect_beep()  { start_sequence(seq_beep); }

	void operate(bool idle_blink = false) {
		if (active) {
			run_effect();
			return;
		}

		if (idle_blink) {
			run_idle_blink();
		} else {
			//digitalWrite(LED_BUILTIN, LOW);
		}
	}

private:
	struct step {
		bool led_enable;
		bool sound_enable;
		uint16_t duration_ms;
		uint16_t frequency_hz;
	};

	// effect data (top-level)
	static constexpr step seq_reset[] = {
		{ false, true,  80, 600 },
		{ false, true,  160, 800 },
	};
	static constexpr step seq_reset2[] = {
		{ false, true,  80, 640 },
		{ false, true,  160, 800 },
	};

	static constexpr step seq_flicker[] = {
		{ true,  true,  80, 1000 },
		{ false, false, 40,  0    },
		{ true,  true,  80, 1000 },
		{ false, false, 40,  0    },
		{ true,  true,  80, 1000 },
		{ false, false, 40,  0    },
		{ true,  true,  500, 300  },
	};

	static constexpr step seq_turnoff[] = {
		{ true, true, 700,  350 },
		{ true, true, 800,  150 },
		{ true, true, 1000, 50  },
	};

	static constexpr step seq_turnon[] = {
		{ false, true, 100, 500 },
		{ false, true, 150, 750 },
		{ false, true, 200, 1000 },
	};

	static constexpr step seq_beep[] = {
		{ false, true, 100, 850 },
	};


	uint8_t buzzer_pin;
	const step* current = nullptr;
	size_t step_count = 0;
	size_t step_index = 0;
	bool active = false;
	unsigned long step_start_ms = 0;

	bool idle_led_state = false;
	unsigned long idle_led_ms = 0;
	static constexpr uint16_t idle_blink_period_ms = 300;


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
			buzzer_off();
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

		//digitalWrite(
		//	LED_BUILTIN,
		//	current[step_index].led_enable ? HIGH : LOW
		//);
		if (current[step_index].led_enable) {
			status_led.setPixelColor(0, status_led.Color(80, 0, 170));
		} else {
			status_led.setPixelColor(0, 0);
		}
		status_led.show();
	}

	void run_idle_blink() {
		unsigned long now = millis();
		if (now - idle_led_ms >= idle_blink_period_ms) {
			idle_led_ms = now;
			idle_led_state = !idle_led_state;
			//digitalWrite(LED_BUILTIN, idle_led_state ? HIGH : LOW);
			if (idle_led_state) {
				status_led.setPixelColor(0, status_led.Color(240, 15, 0));
			} else {
				status_led.setPixelColor(0, 0);
			}
			status_led.show();
		}
	}

	void buzzer_off() {
		noTone(buzzer_pin);
		//digitalWrite(LED_BUILTIN, LOW);
		status_led.setPixelColor(0, 0);
		status_led.show();
	}
};
