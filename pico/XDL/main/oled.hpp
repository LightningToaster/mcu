#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "logos.hpp"


class Oled {
public:
    Adafruit_SH1106G* oled = nullptr;

    void begin(Adafruit_SH1106G* driver) {
        oled = driver;
    }

    void clear() {//TODO remove this...
        oled->clearDisplay();
    }

    void display() {
        oled->display();
    }

    void draw_lock(uint8_t selected_char, volatile char *pin_entered) {
        oled->setTextColor(SH110X_WHITE);
        oled->setTextSize(1);

        oled->setCursor(0, 0);
        oled->printf("owner: %s", BLASTER_OWNER);

        oled->setCursor(35, 22);
        oled->print("Enter PIN:");


        const uint8_t char_width = 16;
        uint8_t start_x = 58;//2 numbers
        if (PIN_LENGTH == 3){start_x = 48;};
        if (PIN_LENGTH == 4){start_x = 42;};
        if (PIN_LENGTH == 5){start_x = 31;};
        if (PIN_LENGTH == 6){start_x = 25;};

        for (uint8_t i = 0; i < PIN_LENGTH; i++) {
            uint8_t x = start_x + i * char_width;

            if (i == selected_char) {
                oled->setCursor(x-1, 37);
                oled->print(pin_entered[i]);
            } else {
                oled->fillCircle(x, 40, 2, SH110X_WHITE);
            }
        }

        //oled->setCursor(0, 56);
        //oled->print("Authentication Failed.");
    }//draw_lock

    void draw_splash() {
        oled->setTextColor(SH110X_WHITE);
        oled->setTextSize(1);

        oled->setCursor(27, 0);
        oled->print("XDL Vanquish");

        oled->setCursor(12, 15);
        oled->print("#0001 v2025.11.2");

        oled->setCursor(8, 30);
        oled->print("Designed by Valkor");
    }//draw_splash

    void draw_logo() {
        oled->setTextColor(SH110X_WHITE);
        oled->setTextSize(1);

        const logo_t &logo = logos[0];
            oled->drawBitmap(logo.x, logo.y, logo.bitmap,
            logo.width, logo.height,
            SH110X_WHITE);
    }//draw_logo

    void draw_menu(settings_t& settings, uint8_t selection, uint32_t darts_session) {
        oled->setTextColor(SH110X_WHITE);
        oled->setTextSize(1);

        if ((millis()/200)%2 == 0){
            if (selection == 0){
                oled->drawRect(0, 3, 35, 30, SH110X_WHITE);
            }else if (selection <= 4){
                oled->setCursor(46, (selection-1) * 9);
                oled->print(">");
            }
        }else{
            
            oled->setCursor(46, (selection-1) * 9);
            oled->print("-");
            if (selection == 0){
                oled->drawRect(1, 4, 33, 28, SH110X_WHITE);
            }
            
        }

        oled->setCursor(3, 10);//102
        oled->setTextSize(2);
        oled->print("P");
        oled->setCursor(17, 7);
        oled->setTextSize(3);
        oled->print(settings.profile);
        
        uint8_t volley, power, dart_detection, glow;
        switch(settings.profile){
            case 1:
                volley = settings.volley1;
                power = settings.power1;
                dart_detection = settings.dart_detection1;
                glow = settings.glow1;
            break;
            case 2:
                volley = settings.volley2;
                power = settings.power2;
                dart_detection = settings.dart_detection2;
                glow = settings.glow2;
            break;
            case 3:
                volley = settings.volley3;
                power = settings.power3;
                dart_detection = settings.dart_detection3;
                glow = settings.glow3;
            break;            
        }//which profile

        oled->setTextSize(1);
        oled->setCursor(54, 0);
        if (volley == 0){
            oled->printf("volley: auto");//TODO infinity symbol?
        }else{
            oled->printf("volley: %u", volley);
        }

        oled->setCursor(54, 9);
        oled->printf("power: %u%", power);

        oled->setCursor(54, 18);
        oled->printf("dartdt: %s", dart_detection ? "ON" : "OFF");
        
        oled->setCursor(54, 27);
        switch(glow){
            case 0: oled->printf("glow: OFF"); break;
            case 1: oled->printf("glow: LOW"); break;
            case 2: oled->printf("glow: MED"); break;
            case 3: oled->printf("glow: HIGH"); break;
        }

        oled->drawRect(0, 40, 127, 1, SH110X_WHITE);//divider
        
        uint32_t seconds = millis() / 1000;
        uint16_t minutes = seconds / 60;
        uint16_t hours = minutes / 60;
        oled->setCursor(0, 46);
        oled->print("uptime: ");
        if (hours) oled->printf("%uh ", hours);
        if (minutes) oled->printf("%um ", minutes % 60);
        if (!hours) oled->printf("%us", seconds % 60);

        oled->setCursor(0, 56);
        oled->printf("darts launched: %u", darts_session);

    }//draw_menu

    void draw_info(uint32_t minutes, uint32_t launched, char* voltage) {
        oled->setTextColor(SH110X_WHITE);
        oled->setTextSize(1);

        oled->setCursor(0, 0);
        oled->print(BLASTER_NAME);

        oled->setCursor(0, 9);
        oled->printf("OWNER: %s", BLASTER_OWNER);

        oled->setCursor(0, 18);
        oled->printf("%s %s %s", BLASTER_SERIAL, BLASTER_BUILD_DATE, FIRMWARE_VERSION);

        oled->setCursor(0, 27);
        oled->print("Designed by Valkor");

        oled->drawRect(0, 36, 127, 1, SH110X_WHITE);//divider


        oled->setCursor(0, 39);
        oled->print("op_hours: ");
        if (minutes >= 60){
            oled->printf("%uh ", minutes/60);
        }
        oled->printf("%um", minutes%60);

        oled->setCursor(0, 48);
        oled->printf("ATDL: %u", launched);

        oled->setCursor(0, 56);
        oled->print(voltage);

        //TODO add IR output
        
    }//draw_info

    


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