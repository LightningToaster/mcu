//valkor
enum VOLTAGE_FORMAT : uint8_t {
  CELL,        // 0
  PACK,        // 1
  PERCENTAGE,  // 2
};

enum BATTERY_STATUS : uint8_t {
  BATTERY_DISCONNECTED,  // 0
  BATTERY_LOW,           // 1
  BATTERY_GOOD,          // 2
  BATTERY_OVERCHARGED,   // 3
};

class Battery {
private:
  static constexpr float VOLTAGE_MIN_4S = 14.0;
  static constexpr float VOLTAGE_MAX_4S = 16.8;
  static constexpr float VOLTAGE_LIMIT = 17.2;  //shuts off
  static constexpr float REFERENCE_VOLTAGE = 3.39;
  static constexpr float R1 = 47000.0;
  static constexpr float R2 = 10000.0;
  static constexpr uint8_t NUM_SAMPLES = 1;
  static constexpr uint8_t MS_BETWEEN_READS = 127;
  static constexpr uint8_t NUM_CELLS = 4;
  static constexpr float VOLTAGE_DIVIDER_RATIO = (R1 + R2) / R2;

  uint8_t pin;
  uint16_t analog_voltages[NUM_SAMPLES] = {};
  uint8_t buffer_index = 0;
  unsigned long last_read_time = 0;
  float cached_voltage = 0.0;
  uint8_t last_percent = 0;

public:
  Battery(uint8_t pin)
    : pin(pin) {
    pinMode(pin, INPUT);
  }

  BATTERY_STATUS operate() {
    if (millis() - last_read_time < MS_BETWEEN_READS) return get_status();
    last_read_time = millis();

    analog_voltages[buffer_index] = analogRead(pin);
    buffer_index = (buffer_index + 1) % NUM_SAMPLES;
    cached_voltage = compute_voltage();
    return get_status();
  }

  void get_string(char* buffer, uint8_t format = PACK) {
    size_t length = 8;
    if (length == 0) return;

    switch (format) {

      case CELL:{//55us
        uint16_t centivolts = (uint16_t)(cached_voltage * 25.0f + 0.5f); // 4 cells
        buffer[0] = '0' + (centivolts / 100);
        buffer[1] = '.';
        buffer[2] = '0' + (centivolts / 10) % 10;
        buffer[3] = '0' + centivolts % 10;
        buffer[4] = 'V';
        buffer[5] = '\0';
          //snprintf(buffer, length, "%.2fV", cached_voltage / NUM_CELLS);
      break;}

      case PACK:{ //44us
        uint16_t decivolts = (uint16_t)(cached_voltage * 10.0f + 0.5f);
        buffer[0] = '0' + (decivolts / 100);
        buffer[1] = '0' + (decivolts / 10) % 10;
        buffer[2] = '.';
        buffer[3] = '0' + decivolts % 10;
        buffer[4] = 'V';
        buffer[5] = '\0';
        //snprintf(buffer, length, "%.1fV", cached_voltage);
      break;}

      case PERCENTAGE: {
        int32_t v = (int32_t)(cached_voltage * 100.0f + 0.5f);
        constexpr int32_t v_min = (int32_t)(VOLTAGE_MIN_4S * 100.0f + 0.5f);
        constexpr int32_t v_max = (int32_t)(VOLTAGE_MAX_4S * 100.0f + 0.5f);
        if (v < v_min) v = v_min;
        if (v > v_max) v = v_max;
        uint8_t percent = ((v - v_min) * 100) / (v_max - v_min);
        if (percent < last_percent || percent > last_percent + 1) last_percent = percent;

        if (last_percent == 100) {
            buffer[0] = '1';
            buffer[1] = '0';
            buffer[2] = '0';
            buffer[3] = '%';
            buffer[4] = '\0';
        } else if (last_percent >= 10) {
            buffer[0] = '0' + last_percent / 10;
            buffer[1] = '0' + last_percent % 10;
            buffer[2] = '%';
            buffer[3] = '\0';
        } else {
            buffer[0] = '0' + last_percent;
            buffer[1] = '%';
            buffer[2] = '\0';
        }
      break;}

      default:
        snprintf(buffer, length, "?");
        break;
    }
  }  //get_string

  BATTERY_STATUS get_status() const {
    if (cached_voltage < 6.0) return BATTERY_DISCONNECTED;
    if (cached_voltage < VOLTAGE_MIN_4S) return BATTERY_LOW;
    if (cached_voltage < VOLTAGE_LIMIT) return BATTERY_GOOD;
    return BATTERY_OVERCHARGED;
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
};  //Battery