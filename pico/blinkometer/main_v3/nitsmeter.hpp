class NitsMeter {
public:
	explicit NitsMeter(uint8_t pin)
		: pin(pin),
		last_flicker_time(0)
	{
		for (size_t i = 0; i < HISTORY_SIZE; i++) {
			adc_history[i] = 0;
		}
		last_read_time = 0;
		adc_sum = 0;
		flicker_start_ms = 0;
		flicker_end_ms = 0;
	}

    void operate(){
        unsigned long now = millis();
        if (now - last_read_time >= 1) { // read once per ms
            read_adc();
            check_flicker();
            last_read_time = now;
        }
    }

    uint16_t get_adc(){
        return adc_sum / HISTORY_SIZE;
    }

	uint16_t get_nits() {
		return map_lookup(get_adc());
	}

	unsigned long get_flicker() {
		if (flicker_start_ms > 0 && flicker_end_ms > 0) {
			unsigned long duration = flicker_end_ms - flicker_start_ms;
			flicker_start_ms = 0; // clear after reporting
            flicker_end_ms = 0; // clear after reporting
			return duration;
		}
		return 0;
	}

private:

  void read_adc() {
			adc_sum -= adc_history[history_index];
			adc_history[history_index] = analogRead(pin);
			adc_sum += adc_history[history_index];

			history_index = (history_index + 1) % HISTORY_SIZE;
	}

	void check_flicker() {
    uint16_t avg = adc_sum / HISTORY_SIZE;
    uint16_t current = adc_history[(history_index + HISTORY_SIZE - 1) % HISTORY_SIZE];

    unsigned long now = millis();

    // ignore detection if last flicker too recent
    if (now - last_flicker_time < 500) return;

    if (avg > current && (avg - current) > MAGNITUDE_THRESHOLD) {
        if (flicker_start_ms == 0) {
            flicker_start_ms = now;
        }
    } else {
        if (flicker_start_ms != 0 && flicker_end_ms == 0) {
            flicker_end_ms = now;
            last_flicker_time = flicker_end_ms; // mark end for rate limiting
        }
    }
	}


  uint16_t map_lookup(uint16_t in_val) const {
		if (in_val <= map_table[0].in_val) {
			return map_table[0].out_val;
		}
		if (in_val >= map_table[map_table_size - 1].in_val) {
			return map_table[map_table_size - 1].out_val;
		}
		for (size_t i = 0; i < map_table_size - 1; i++) {
			const auto &p1 = map_table[i];
			const auto &p2 = map_table[i + 1];
			if (in_val >= p1.in_val && in_val <= p2.in_val) {
				float ratio = float(in_val - p1.in_val) / float(p2.in_val - p1.in_val);
				return p1.out_val + ratio * (p2.out_val - p1.out_val);
			}
		}
		return 0;
	}

private:
	struct map_point_t {
		uint16_t in_val;
		uint16_t out_val;
	};

	static constexpr map_point_t map_table[] = {
		{20, 0},
		{136, 32},
		{152, 37},
		{174, 47},
		{191, 53},
		{211, 62},
		{262, 85},
		{289, 100},
		{319, 120},
		{354, 144},
		{401, 181},
		{433, 218},
		{453, 234},
		{493, 284},
		{562, 388},
		{623, 517},
		{682, 701},
		{1023, 1500}
	};

	static constexpr size_t map_table_size = sizeof(map_table) / sizeof(map_table[0]);
	static constexpr size_t HISTORY_SIZE = 50;
	static constexpr uint16_t MAGNITUDE_THRESHOLD = 20;

	unsigned long last_flicker_time;

	uint8_t pin;
	uint16_t adc_history[HISTORY_SIZE];
	size_t history_index = 0;
	uint32_t adc_sum;
	unsigned long last_read_time;

	unsigned long flicker_start_ms;
	unsigned long flicker_end_ms;
};
