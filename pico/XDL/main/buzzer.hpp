#pragma once


class Buzzer {
public:
    Buzzer(uint8_t pin)
        : pin(pin),
          active(false),
          end_time(0)
    {
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

    bool is_busy() const {
        return active;
    }

private:
    uint8_t pin;
    bool active;
    unsigned long end_time;
};//Buzzer