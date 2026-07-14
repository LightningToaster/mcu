#pragma once

#include <Arduino.h>
#include <cstring>

class PinLock {
public:
    bool verify(const char *pin) {
        if (strcmp(pin, pin_combo) == 0) {
            return true;
        }
        return false;
    }
};