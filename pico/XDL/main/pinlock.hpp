//valkor
#include <cstring>

static constexpr char pin_combo[] = "123";
#define PIN_LENGTH 3    //must be the length of pin_combo

class PinLock {
public:
  bool verify(const char *pin) {
    if (strcmp(pin, pin_combo) == 0) {
      Serial.println("CORRECT PIN!");
      locked = false;
      return true;
    }
    ms = millis();
    Serial.println("WRONG PIN!");
    return false;
  }

  bool is_ready() {
    return (millis() - ms >= 3000);
  }

  bool is_locked(){
    return locked;
  }

  void lock(){
    locked = true;
  }

private:
  unsigned long ms = 0;
  bool locked = REQUIRE_PIN;
};