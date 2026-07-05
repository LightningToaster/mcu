#pragma once

/*
INFO:
voltage in a corner
darts launched session 
darts launched all time
hours
serial number
firmware 
made by valkor

log time spend using blaster, saved as 1min chunks, read in hours, uint32_t, 70mil max hours
*/
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>


class Oled {
public:
    Adafruit_SH1106G* oled = nullptr;

    void begin(Adafruit_SH1106G* driver) {
        oled = driver;
    }

    void clear() {
        oled->clearDisplay();
    }

    void display() {
        oled->display();
    }

    void draw_logo() {
        oled->setTextColor(SH110X_WHITE);
        oled->setTextSize(1);

        oled->setCursor(27, 0);
        oled->print("XDL Vanquish");

        oled->setCursor(12, 15);
        oled->print("#0001 v2025.11.2");

        oled->setCursor(8, 30);
        oled->print("Designed by Valkor");
    }

    void draw_ammo(uint16_t ammo) {
        oled->setTextColor(SH110X_WHITE);

        if (ammo <= 9) {
            oled->setTextSize(9);
            oled->setCursor(46, 0);
        } else if (ammo <= 99) {
            oled->setTextSize(8);
            oled->setCursor(22, 5);
        } else if (ammo <= 999) {
            oled->setTextSize(7);
            oled->setCursor(2, 7);
        } else {
            oled->setTextSize(5);
            oled->setCursor(5, 15);
        }

        oled->print(ammo);
    }

    void draw_empty(const char* voltage, uint16_t fired) {
        oled->setTextColor(SH110X_WHITE);

        oled->setTextSize(4);
        oled->setCursor(7, 15);
        oled->print("EMPTY");

        oled->setTextSize(1);
        oled->setCursor(50, 54);
        oled->print(voltage);

        if (fired > 0) {
            oled->setCursor(0, 54);
            oled->print(fired);
        }
    }

    void draw_battery(uint8_t state, const char* voltage) {
        oled->setTextColor(SH110X_WHITE);

        switch (state) {
            case battery_disconnected:
                oled->setTextSize(1);
                oled->setCursor(45, 5);
                oled->print("BATTERY");
                oled->setCursor(30, 18);
                oled->print("DISCONNECTED");
                break;

            case battery_low:
                oled->setTextSize(2);
                oled->setCursor(25, 5);
                oled->print("BATTERY");
                oled->setCursor(47, 24);
                oled->print("LOW");
                oled->setTextSize(1);
                oled->setCursor(50, 54);
                oled->print(voltage);
                break;

            case battery_good:
                oled->setTextSize(2);
                oled->setCursor(39, 25);
                oled->print(voltage);
                break;

            case battery_overcharged:
                oled->setTextSize(3);
                oled->setCursor(2, 0);
                oled->print("DANGER!");
                oled->setTextSize(1);
                oled->setCursor(13, 28);
                oled->print("EXCESS VOLTAGE");
                oled->setCursor(50, 54);
                oled->print(voltage);
                break;
        }
    }
};