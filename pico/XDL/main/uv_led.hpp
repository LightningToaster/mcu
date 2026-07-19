//valkor 2026-07-18

const uint8_t POWERS_64[64] = {  // gamma correction table 2026-07-18
    0,   1,   2,   3,   4,   5,   6,   7,
    8,   9,   10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  22,  24,  26,
    28,  30,  33,  36,  39,  42,  46,  50,
    55,  60,  65,  70,  75,  80,  85,  90,
    95, 100, 105, 110, 115, 120, 125, 130,
    135, 140, 145, 151, 158, 165, 173, 181,
    189, 197, 205, 215, 225, 235, 245, 255
};

class LED {
public:
  LED(uint8_t pin)
  : pin(pin) {
    digitalWrite(pin, LOW); // preload output latch
    pinMode(pin, OUTPUT);
  }

  void operate() {
    unsigned long ms = millis();
    if (actual < goal){
      if (ms - increase_ms >= INCREASE_DELAY_MS){
        actual++;
        increase_ms = ms;
      }
    }else if (actual > goal){
      if (ms - decrease_ms >= DECREASE_DELAY_MS){
        actual--;
        decrease_ms = ms;
      }
    }
    if (ARMED == true){
      analogWrite(pin, POWERS_64[actual]);
    }
  }

  void set(uint8_t value) {
    actual = find_power_index(value);
  }

  void fade(uint8_t value) {
    goal = find_power_index(value);
  }

private:
  uint8_t pin;
  const uint8_t INCREASE_DELAY_MS = 1;
  const uint8_t DECREASE_DELAY_MS = 12;
  unsigned long increase_ms = 0;
  unsigned long decrease_ms = 0;

  uint8_t actual = 0;//these use power indexing
  uint8_t goal = 0;

  uint8_t find_power_index(uint8_t value) {
    for (uint8_t i = 0; i < 64; i++) {
      if (POWERS_64[i] >= value)
        return i;
    }
    return 63;
  }
};  //LED