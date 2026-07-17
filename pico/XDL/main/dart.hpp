//valkor
enum DART_STATE : uint8_t {
  DART_NONE = 0,  // no dart detected
  DART_READY,     // dart detected and stable
  MAG_EMPTY,      // one-shot pulse when mag goes empty
  NEW_MAG         // one-shot pulse when new mag inserted
};

class DartDetector {
public:
  explicit DartDetector(uint8_t emitter_pin, uint8_t receiver_pin)
    : emitter_pin(emitter_pin), receiver_pin(receiver_pin) {
    pinMode(emitter_pin, OUTPUT);
    digitalWrite(emitter_pin, LOW);
  }

  DART_STATE operate() {

    // perform one step of the non-blocking sampler
    sample();

    uint32_t now = millis();

    if (reflection > threshold) {

      no_dart_timer = now;

      if (mag_empty) {
        mag_empty = false;
        return NEW_MAG;
      }

      return DART_READY;
    }

    // no dart detected
    if (!mag_empty && (now - no_dart_timer >= mag_empty_delay_ms)) {
      mag_empty = true;
      return MAG_EMPTY;
    }

    return DART_NONE;
  }

  int get_reflection() const {
    return reflection;
  }

private:

  void sample() {

    switch (sample_state) {

      case SAMPLE_AMBIENT_START:
        digitalWrite(emitter_pin, LOW);
        sample_timer = micros();
        sample_state = SAMPLE_AMBIENT_READ;
        break;

      case SAMPLE_AMBIENT_READ:
        if (micros() - sample_timer >= settle_time_us) {

          ambient_sum += analogRead(receiver_pin);

          if (++ambient_samples >= sample_count) {

            ambient = ambient_sum / sample_count;
            ambient_sum = 0;
            ambient_samples = 0;

            digitalWrite(emitter_pin, HIGH);
            sample_timer = micros();
            sample_state = SAMPLE_ACTIVE_READ;

          } else {
            sample_state = SAMPLE_AMBIENT_START;
          }
        }
        break;

      case SAMPLE_ACTIVE_READ:
        if (micros() - sample_timer >= settle_time_us) {

          active_sum += analogRead(receiver_pin);

          if (++active_samples >= sample_count) {

            digitalWrite(emitter_pin, LOW);

            active = active_sum / sample_count;
            active_sum = 0;
            active_samples = 0;

            reflection = active - ambient;

            sample_state = SAMPLE_AMBIENT_START;

          } else {
            digitalWrite(emitter_pin, LOW);
            sample_state = SAMPLE_AMBIENT_START;
          }
        }
        break;
    }
  }

  enum SAMPLE_STATE : uint8_t {
    SAMPLE_AMBIENT_START,
    SAMPLE_AMBIENT_READ,
    SAMPLE_ACTIVE_READ
  };

  uint8_t emitter_pin;
  uint8_t receiver_pin;

  SAMPLE_STATE sample_state = SAMPLE_AMBIENT_START;

  uint32_t sample_timer = 0;
  uint32_t no_dart_timer = 0;

  uint32_t ambient_sum = 0;
  uint32_t active_sum = 0;

  uint8_t ambient_samples = 0;
  uint8_t active_samples = 0;

  int ambient = 0;
  int active = 0;
  int reflection = 0;

  bool mag_empty = false;

  static constexpr uint16_t settle_time_us = 200;
  static constexpr uint8_t sample_count = 8;
  static constexpr int threshold = 10;
  static constexpr uint16_t mag_empty_delay_ms = 250;
};