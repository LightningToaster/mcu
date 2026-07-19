#pragma once
//valkor 2026-07-16
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "logos.hpp"

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

  void draw_lock(uint8_t selected_char, volatile char* pin_entered, bool failed) {
    oled->setTextColor(SH110X_WHITE);
    oled->setTextSize(1);

    oled->setCursor(0, 0);
    oled->printf("owner: %s", BLASTER_OWNER);

    if (failed) {
      oled->setCursor(29, 21);
      oled->setTextSize(3);
      oled->print("FAIL");
      oled->drawRect(25, 18, 78, 28, SH110X_WHITE);
      return;
    }

    oled->setCursor(34, 22);
    oled->print("Enter PIN:");


    const uint8_t char_width = 16;
    uint8_t start_x = 58;  //2 numbers
    if (PIN_LENGTH == 3) { start_x = 48; };
    if (PIN_LENGTH == 4) { start_x = 42; };
    if (PIN_LENGTH == 5) { start_x = 31; };
    if (PIN_LENGTH == 6) { start_x = 25; };

    for (uint8_t i = 0; i < PIN_LENGTH; i++) {
      uint8_t x = start_x + i * char_width;

      if (i == selected_char) {
        oled->setCursor(x - 1, 37);
        oled->print(pin_entered[i]);
        if ((millis() / 250) % 2 == 0) {
          oled->drawRect(x-1, 46, 5, 1, SH110X_WHITE);
        }
      } else {
        oled->fillCircle(x, 40, 2, SH110X_WHITE);
      }
    }
  }  //draw_lock

  void draw_splash() {
    oled->setTextColor(SH110X_WHITE);
    oled->setTextSize(1);

    oled->setCursor(27, 0);
    oled->print("XDL Vanquish");

    oled->setCursor(12, 15);
    oled->print("#0001 v2025.11.2");

    oled->setCursor(8, 30);
    oled->print("Designed by Valkor");
  }  //draw_splash

  void draw_logo(uint8_t logo_index = 0) {
    oled->setTextColor(SH110X_WHITE);
    oled->setTextSize(1);

    //oled->drawRect(0, 0, 128, 64, SH110X_WHITE);

    const logo_t& logo = logos[logo_index];
    oled->drawBitmap(logo.x, logo.y, logo.bitmap,
                     logo.width, logo.height,
                     SH110X_WHITE);
  }  //draw_logo

  void draw_menu(settings_t& settings, uint8_t selection, uint32_t darts_session, char* voltage, uint16_t failed_logins_report = 0) {
    oled->setTextColor(SH110X_WHITE);
    oled->setTextSize(1);

    if ((millis() / 200) % 2 == 0) {
      if (selection == 0) {
        oled->drawRect(0, 2, 39, 32, SH110X_WHITE);
      } else if (selection <= 4) {
        oled->setCursor(48, (selection - 1) * 14);
        oled->print(">");
      }
    } else {

      oled->setCursor(48, (selection - 1) * 14);
      oled->print("-");
      if (selection == 0) {
        oled->drawRect(1, 3, 37, 30, SH110X_WHITE);
      }
    }

    oled->setCursor(5, 10);
    oled->setTextSize(2);
    oled->print("P");
    oled->setCursor(20, 7);
    oled->setTextSize(3);
    oled->print(settings.profile);

    uint8_t volley, power, glow;
    switch (settings.profile) {
      case 1:
        volley = settings.volley1;
        power = settings.power1;
        glow = settings.glow1;
        break;
      case 2:
        volley = settings.volley2;
        power = settings.power2;
        glow = settings.glow2;
        break;
      case 3:
        volley = settings.volley3;
        power = settings.power3;
        glow = settings.glow3;
        break;
    }  //which profile

    oled->setTextSize(1);
    oled->setCursor(56, 0);
    if (volley == 0) {
      oled->printf("volley: OFF");
    } else if (volley < 3) {
      oled->printf("volley: +%u", volley);
    } else {
      oled->printf("volley: AUTO");
    }

    oled->setCursor(56, 14);
    oled->printf("power: %u%%", power);

    oled->setCursor(56, 28);
    switch (glow) {
      case 0: oled->printf("glow: OFF"); break;
      case 1: oled->printf("glow: LOW"); break;
      case 2: oled->printf("glow: MED"); break;
      case 3: oled->printf("glow: HIGH"); break;
    }

    oled->drawRect(0, 41, 127, 1, SH110X_WHITE);  //divider

    oled->setTextSize(2);
    //Serial.println(strlen(voltage));
    switch(strlen(voltage)){
      case 5: oled->setCursor(68, 48); break;
      case 4: oled->setCursor(80, 48); break;
      case 3: oled->setCursor(92, 48); break;
      case 2: oled->setCursor(104, 48); break;
    }
    //oled->printf("99%%");
    oled->printf("%s", voltage);

    oled->setTextSize(1);
    uint32_t seconds = millis() / 1000;
    uint16_t minutes = seconds / 60;
    uint16_t hours = minutes / 60;
    oled->setCursor(0, 46);
    oled->print("upT: ");
    if (hours) oled->printf("%uh ", hours);
    if (minutes) oled->printf("%um ", minutes % 60);
    if (!hours) oled->printf("%us", seconds % 60);

    oled->setCursor(0, 56);
    oled->printf("DLS: %u", darts_session);

    // if (failed_logins_report > 0) {
    //   oled->setCursor(112, 46);
    //   oled->printf("%u", failed_logins_report);
    // }

  }  //draw_menu

  void draw_info() {
    oled->setTextColor(SH110X_WHITE);
    oled->setTextSize(1);

    oled->setCursor(0, 0);
    oled->print(BLASTER_NAME);

    oled->setCursor(0, 9);
    oled->printf("DOM: %s", BLASTER_BUILD_DATE);
    
    oled->setCursor(0, 18);
    oled->printf("SN: %s", BLASTER_SERIAL);

    oled->setCursor(0, 27);
    oled->printf("FW: %s", FIRMWARE_VERSION);

    oled->setCursor(0, 36);
    oled->printf("owner: %s", BLASTER_OWNER);

    oled->setCursor(0, 56);
    oled->print("designed by valkor");

    const logo_t& logo = logos[0];
    oled->drawBitmap(logo.x, logo.y, logo.bitmap,
                     logo.width, logo.height,
                     SH110X_WHITE);
  }  //draw_info

  void draw_metrics(settings_t& settings, uint16_t darts_session, uint8_t IR, char* voltage_percent, char* voltage_cell, char* voltage_pack, uint16_t failed_logins_report) {
    oled->setTextColor(SH110X_WHITE);
    oled->setTextSize(1);

    uint32_t seconds = millis() / 1000;
    uint16_t minutes = seconds / 60;
    uint16_t hours = minutes / 60;
    oled->setCursor(0, 0);
    oled->print("up_time: ");
    if (hours) oled->printf("%uh ", hours);
    if (minutes) oled->printf("%um ", minutes % 60);
    oled->printf("%us", seconds % 60);

    oled->setCursor(0, 9);
    oled->print("op_time: ");
    if (settings.minutes >= 60) {
      oled->printf("%uh ", settings.minutes / 60);
    }
    oled->printf("%um", settings.minutes % 60);

    oled->setCursor(0, 18);
    oled->printf("DLS: %u", darts_session);
    oled->setCursor(0, 27);
    oled->printf("DLT: %u", settings.launched);

    oled->setCursor(0, 36);
    oled->printf("IR: %u", IR);

    oled->setCursor(0, 45);
    oled->printf("B: %s %s %s", voltage_percent, voltage_cell, voltage_pack);

    if (failed_logins_report > 0) {
      oled->setCursor(0, 54);
      oled->printf("failed logins: %u", failed_logins_report);
    }
  }  //draw_metrics

  void draw_ammo(uint16_t ammo, const char* voltage) {
    oled->setTextColor(SH110X_WHITE);

    if (AMMO_VOLTAGE_READOUT){
      oled->setTextSize(3);
      oled->setCursor(0, 40);//56
      oled->print(voltage);

      oled->setTextSize(4);
      oled->setCursor(0, 0);
    }else{
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
    }
    oled->print(ammo);
  }

  void draw_empty(const char* voltage, uint16_t fired) {
    oled->setTextColor(SH110X_WHITE);

    oled->setTextSize(4);
    oled->setCursor(7, 15);
    oled->print("EMPTY");

    oled->setTextSize(1);
    oled->setCursor(98, 56);//50
    oled->print(voltage);

    if (fired > 0) {
      oled->setCursor(0, 56);
      oled->printf("%u", fired);
    }
  }

  void draw_battery(uint8_t status, const char* voltage) {
    oled->setTextColor(SH110X_WHITE);

    switch (status) {
      case BATTERY_DISCONNECTED:
        oled->setTextSize(1);
        oled->setCursor(43, 8);
        oled->print("BATTERY");
        oled->setTextSize(1);
        oled->setCursor(28, 20);
        oled->print("DISCONNECTED");
        break;

      case BATTERY_LOW:
        oled->setTextSize(2);
        oled->setCursor(25, 5);
        oled->print("BATTERY");
        oled->setCursor(47, 24);
        oled->print("LOW");
        oled->setTextSize(1);
        oled->setCursor(50, 54);
        oled->print(voltage);
        break;

      case BATTERY_GOOD:
        oled->setTextSize(2);
        oled->setCursor(39, 25);
        oled->print(voltage);
        break;

      case BATTERY_OVERCHARGED:
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