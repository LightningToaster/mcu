//valkor
#include <cstring>

class PinLock {
public:
  bool verify(const char *pin) {
    if (strcmp(pin, pin_combo) == 0) {
      Serial.println("CORRECT PIN!");
      return true;
    }
    ms = millis();
    Serial.println("WRONG PIN!");
    return false;
  }

  bool is_ready() {
    return (millis() - ms >= 3000);
  }

private:
  unsigned long ms = 0;
};