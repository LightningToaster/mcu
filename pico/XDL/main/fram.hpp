#pragma once

#include <Arduino.h>
#include <Wire.h>

class Fram {
public:
    static constexpr uint8_t base_addr = 0x50;

    void begin() {
        Wire.begin();
    }

    void read(uint16_t addr, void* data, uint16_t len) {
        uint8_t* buf = (uint8_t*)data;

        while (len) {
            uint8_t device_addr = base_addr | ((addr >> 8) & 0x07);
            uint8_t word_addr = addr & 0xFF;

            Wire.beginTransmission(device_addr);
            Wire.write(word_addr);
            Wire.endTransmission(false);

            Wire.requestFrom(device_addr, (uint8_t)1);
            if (!Wire.available()) return;

            *buf++ = Wire.read();
            addr++;
            len--;
        }
    }

    void write(uint16_t addr, const void* data, uint16_t len) {
        const uint8_t* buf = (const uint8_t*)data;

        while (len) {
            uint8_t device_addr = base_addr | ((addr >> 8) & 0x07);

            Wire.beginTransmission(device_addr);
            Wire.write(addr & 0xFF);

            uint8_t chunk = 0;

            while (len && chunk < 30) {
                Wire.write(*buf++);
                addr++;
                len--;
                chunk++;
            }

            Wire.endTransmission();
        }
    }
};