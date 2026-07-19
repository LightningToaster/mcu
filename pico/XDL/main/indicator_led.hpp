//valkor 2026-07-16

//call operate() continously
//set(r,g,b,ms,sudden)
//ms is how long the fade to the target takes.  if sudden == true then it'll wait ms and change suddenly

const uint8_t POWERS[32] = {  //gamma correction table
  0, 1, 2, 3, 4, 5, 6, 7, 8, 10,
  12, 14, 16, 18, 20,
  25, 30, 35, 40, 50,
  60, 70, 80, 90, 110,
  130, 150, 170, 190, 210,
  230, 255
};

const uint8_t COLORS[][3] = {
  { 31, 31, 31 }, { 31, 0, 0 }, { 0, 31, 0 }, { 0, 0, 31 }, { 31, 31, 0 }, { 31, 0, 31 }, { 0, 31, 31 }, { 31, 18, 0 }, { 18, 0, 31 }
};


#include <Adafruit_NeoPixel.h>

class IndicatorLED {
public:
  IndicatorLED(uint8_t pin = 25)  //25 by default
  : pin(pin) {
    strip = new Adafruit_NeoPixel(1, pin, NEO_GRB + NEO_KHZ800);
    strip->begin();
    strip->clear();
    strip->show();
  }//IndicatorLED

  void set_color(uint8_t color, uint16_t ms = 0) {
    set(
      COLORS[color][0],
      COLORS[color][1],
      COLORS[color][2],
      ms);
  }

  void operate() {

    unsigned long now = millis();

    //handle delayed instant change
    if (delayed_change) {

      if (now >= change_time_ms) {

        delayed_change = false;

        current_index[0] = goal_index[0];
        current_index[1] = goal_index[1];
        current_index[2] = goal_index[2];

        strip->setPixelColor(
          0,
          POWERS[current_index[0]],
          POWERS[current_index[1]],
          POWERS[current_index[2]]);
        strip->show();
      }

      return;
    }

    if (!fading)
      return;

    if (now - last_update_ms < update_interval_ms)
      return;

    last_update_ms = now;

    bool changed = false;
    bool done = true;

    for (uint8_t i = 0; i < 3; i++) {

      if (current_index[i] < goal_index[i]) {
        current_index[i]++;
        changed = true;
      } else if (current_index[i] > goal_index[i]) {
        current_index[i]--;
        changed = true;
      }

      if (current_index[i] != goal_index[i])
        done = false;
    }

    if (changed) {
      strip->setPixelColor(
        0,
        POWERS[current_index[0]],
        POWERS[current_index[1]],
        POWERS[current_index[2]]);
      strip->show();
    }

    fading = !done;
  }

  void set(uint8_t r, uint8_t g, uint8_t b, uint16_t ms = 0, bool sudden = false) {

    goal_index[0] = find_power_index(r);
    goal_index[1] = find_power_index(g);
    goal_index[2] = find_power_index(b);

    fading = false;
    delayed_change = false;

    if (ms == 0) {

      current_index[0] = goal_index[0];
      current_index[1] = goal_index[1];
      current_index[2] = goal_index[2];

      strip->setPixelColor(
        0,
        POWERS[current_index[0]],
        POWERS[current_index[1]],
        POWERS[current_index[2]]);
      strip->show();

      return;
    }

    if (sudden) {

      delayed_change = true;
      change_time_ms = millis() + ms;
      return;
    }

    uint8_t max_steps = 0;

    for (uint8_t i = 0; i < 3; i++) {
      uint8_t steps = abs((int)goal_index[i] - (int)current_index[i]);
      if (steps > max_steps)
        max_steps = steps;
    }

    if (max_steps == 0)
      return;

    update_interval_ms = ms / max_steps;

    if (update_interval_ms == 0)
      update_interval_ms = 1;

    last_update_ms = millis();
    fading = true;
  }

private:
  uint8_t pin;

  Adafruit_NeoPixel *strip = nullptr;

  uint8_t current_index[3] = { 0, 0, 0 };
  uint8_t goal_index[3] = { 0, 0, 0 };

  bool fading = false;
  bool delayed_change = false;
  unsigned long change_time_ms = 0;

  unsigned long last_update_ms = 0;
  uint16_t update_interval_ms = 1;

  uint8_t find_power_index(uint8_t value) {
    for (uint8_t i = 0; i < 32; i++) {
      if (POWERS[i] >= value)
        return i;
    }
    return 31;
  }


};//IndicatorLED