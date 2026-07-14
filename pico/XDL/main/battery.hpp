enum VOLTAGE_FORMAT : uint8_t {
  CELL,       // 0
  PACK,       // 1
  PERCENTAGE, // 2
};

enum BATTERY_STATUS : uint8_t {
  BATTERY_DISCONNECTED,       // 0
  BATTERY_LOW,       // 1
  GOOD, // 2
  BATTERY_OVERCHARGED, // 3
};

class Battery {
private:
    static constexpr float VOLTAGE_MIN_4S = 14.0;
    static constexpr float VOLTAGE_MAX_4S = 16.8;
    static constexpr float VOLTAGE_LIMIT = 17.2;//shuts off
    static constexpr float REFERENCE_VOLTAGE = 3.39;
    static constexpr float R1 = 47000.0;
    static constexpr float R2 = 10000.0;
    static constexpr uint8_t NUM_SAMPLES = 20;
    static constexpr uint8_t MS_BETWEEN_READS = 50;
    static constexpr uint8_t NUM_CELLS = 4;
    static constexpr float VOLTAGE_DIVIDER_RATIO = (R1 + R2) / R2;

    uint8_t pin;
    uint16_t analog_voltages[NUM_SAMPLES] = {};
    uint8_t buffer_index = 0;
    unsigned long last_read_time = 0;
    float cached_voltage = 0.0;

public:
    Battery(uint8_t pin) : pin(pin) {
        pinMode(pin, INPUT);
    }

    bool operate() {
        if (millis() - last_read_time < MS_BETWEEN_READS) return is_ok();
        last_read_time = millis();

        analog_voltages[buffer_index] = analogRead(pin);
        buffer_index = (buffer_index + 1) % NUM_SAMPLES;
        cached_voltage = compute_voltage();
        return is_ok();
    }

    void get_string(char* buffer, size_t length, uint8_t format = PACK) const {
        if (length == 0) return;

        if (!is_ok()) {
            snprintf(buffer, length, "0V");
            return;
        }

        switch (format) {

            case CELL:
                snprintf(buffer, length, "%.2fV", cached_voltage / NUM_CELLS);
                break;

            case PACK:
                snprintf(buffer, length, "%.1fV", cached_voltage);
                break;

            case PERCENTAGE: {
                int32_t v = (int32_t)(cached_voltage * 100.0f);

                const int32_t v_min = (int32_t)(VOLTAGE_MIN_4S * 100.0f);
                const int32_t v_max = (int32_t)(VOLTAGE_MAX_4S * 100.0f);

                if (v < v_min) v = v_min;
                if (v > v_max) v = v_max;

                int percent = ((v - v_min) * 100) / (v_max - v_min);

                snprintf(buffer, length, "%d%%", percent);
                break;
            }

            default:
                snprintf(buffer, length, "?");
                break;
        }
    }//get_string

    BATTERY_STATUS get_status() const {
        if (cached_voltage < 1.0) return BATTERY_DISCONNECTED;
        if (cached_voltage < VOLTAGE_MIN_4S) return BATTERY_LOW;
        if (cached_voltage < VOLTAGE_LIMIT) return GOOD;
        return BATTERY_OVERCHARGED;
    }

    bool is_ok() const {
        return cached_voltage >= VOLTAGE_MIN_4S && cached_voltage <= VOLTAGE_LIMIT;
    }

private:
    float compute_voltage() const {
        int total = 0;
        for (uint8_t i = 0; i < NUM_SAMPLES; i++) {
            if (analog_voltages[i] < 1) return 0.0;
            total += analog_voltages[i];
        }
        float average = static_cast<float>(total) / NUM_SAMPLES;
        float voltage = (average / 1023.0) * REFERENCE_VOLTAGE;
        return voltage * VOLTAGE_DIVIDER_RATIO;
    }

    static float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};//Battery
