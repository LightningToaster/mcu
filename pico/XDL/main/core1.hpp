#pragma once
//valkor and AI 2026-07-16
#include <Wire.h>
#include "pico/multicore.h"

struct settings_t {
  uint8_t profile;

  uint8_t volley1;
  uint8_t volley2;
  uint8_t volley3;

  uint8_t power1;
  uint8_t power2;
  uint8_t power3;

  uint8_t glow1;
  uint8_t glow2;
  uint8_t glow3;

  uint8_t failed_logins;
  uint32_t minutes;
  uint32_t launched;
};

enum display_mode : uint8_t {  //might not need all these
  display_splash = 0,
  display_lock,
  display_logo,
  display_info,
  display_menu,
  display_metrics,
  display_ammo,
  display_empty,
  display_battery,
};


#include "fram.hpp"
#include "oled.hpp"

class Core1 {
public:
  Fram fram;
  Oled oled;

  Adafruit_SH1106G display_driver{ 128, 64, &Wire, -1 };

  volatile uint8_t selected_char = 0;
  char pin_entered[PIN_LENGTH + 1];  //extra for null char

  volatile uint8_t display_mode = display_logo;
  volatile uint8_t logo_index = START_LOGO_INDEX;
  volatile uint8_t selection = 0;
  volatile uint16_t fired_from_mag = 0;
  volatile uint16_t darts_session = 0;
  volatile BATTERY_STATUS battery_status = BATTERY_DISCONNECTED;
  volatile bool pin_failed = false;
  uint16_t failed_logins_report = 0;
  uint32_t last_minute_ms = 0; //use for op_time
  uint8_t IR = 0;

  char voltage_cell[8] = { 0 };
  char voltage_pack[8] = { 0 };
  char voltage_percent[8] = { 0 };

  settings_t settings;

  bool settings_dirty = false;

  static Core1* instance;

  bool begin(uint8_t sda, uint8_t scl, uint8_t addr = 0x3C) {
    Wire.setSDA(sda);
    Wire.setSCL(scl);
    Wire.begin();

    fram.begin();

    display_driver.begin(addr, true);
    display_driver.clearDisplay();
    display_driver.display();

    oled.begin(&display_driver);

    load_settings();

    instance = this;
    multicore_launch_core1(core1_entry);

    randomSeed(rp2040.hwrand32());
    randomize_starting_pin();

    return true;
  }

  void request_save() {
    settings_dirty = true;
  }

  void randomize_starting_pin() {
    for (uint8_t i = 0; i < PIN_LENGTH; i++) {
      pin_entered[i] = '0' + random(10);
    }

    pin_entered[PIN_LENGTH] = '\0';
    selected_char = 0;
  }

  void next_profile(){
    settings.profile = (settings.profile % 3) + 1;
    settings_dirty = true;
  }

  void next_selection(){
    selection = (selection + 1) % 4;
  }

  void next_volley(){
    switch (settings.profile) { 
      case 1: settings.volley1 = (settings.volley1 + 1) % 4; break;
      case 2: settings.volley2 = (settings.volley2 + 1) % 4; break;
      case 3: settings.volley3 = (settings.volley3 + 1) % 4; break;
    }
    settings_dirty = true;
  }
  void next_power(uint8_t increment = 1) {
    auto next_power = [increment](uint8_t power) -> uint8_t {
      power = ((power + increment) / increment) * increment;
      if (power > MAX_POWER) return 1;
      return power;
    };

    switch (settings.profile) {
      case 1: settings.power1 = next_power(settings.power1); break;
      case 2: settings.power2 = next_power(settings.power2); break;
      case 3: settings.power3 = next_power(settings.power3); break;
    }
    settings_dirty = true;
  }

  void next_glow(){
    switch (settings.profile) {
      case 1: settings.glow1 = (settings.glow1 + 1) % 4; break;
      case 2: settings.glow2 = (settings.glow2 + 1) % 4; break;
      case 3: settings.glow3 = (settings.glow3 + 1) % 4; break;
    }
    settings_dirty = true;
  }

  uint8_t get_volley(){
    switch (settings.profile) {
    case 1: return settings.volley1; break;
    case 2: return settings.volley2; break;
    case 3: return settings.volley3; break;
    }
    return 0;
  }

  uint8_t get_power(){
    switch (settings.profile) { 
      case 1: return settings.power1; break;
      case 2: return settings.power2; break;
      case 3: return settings.power3; break;
    }
    return 0;
  }

   uint8_t get_glow(){
    switch (settings.profile) { 
      case 1: return settings.glow1; break;
      case 2: return settings.glow2; break;
      case 3: return settings.glow3; break;
    }
    return 0;
  }

  void dart_fired(){
    fired_from_mag++; 
    darts_session++;
    settings.launched++; 
    settings_dirty = true;
  }
private:
  unsigned long last_frame = 0;
  static constexpr uint32_t frame_ms = 33;

  static void core1_entry() {
    if (instance) instance->loop();
  }

  void loop() {
    while (true) {

      uint32_t now = millis();
      while (now - last_minute_ms >= 60000) {
        last_minute_ms += 60000;
        settings.minutes++;
        settings_dirty = true;
      }  //op_time incrementer

      if (millis() - last_frame < frame_ms) {
        delay(1);
        continue;
      }
      last_frame = millis();

      if (settings_dirty) {
        save_settings();
        settings_dirty = false;
      }

      oled.clear();

      switch (display_mode) {
        case display_splash: oled.draw_splash(); break;
        case display_lock: oled.draw_lock(selected_char, pin_entered, pin_failed); break;
        case display_logo: oled.draw_logo(logo_index); break;
        case display_info: oled.draw_info(); break;
        case display_metrics: oled.draw_metrics(settings, darts_session, IR, voltage_percent, voltage_cell, voltage_pack, failed_logins_report); break;
        case display_menu: oled.draw_menu(settings, selection, darts_session, voltage_percent, failed_logins_report); break;
        case display_ammo: oled.draw_ammo(fired_from_mag, voltage_cell); break;
        case display_empty: oled.draw_empty(voltage_cell, fired_from_mag); break;
        case display_battery: oled.draw_battery(battery_status, voltage_cell); break;
      }

      oled.display();
    }
  }

  // --------------------
  // FRAM persistence
  // --------------------

  static constexpr uint32_t settings_magic = 0x58444C31;  // "XDL1"
  static constexpr uint16_t settings_version = 1;

  struct fram_header_t {
    uint32_t magic;
    uint16_t version;
  };

  void load_defaults() {
    settings.profile = 1;

    settings.volley1 = 0;
    settings.volley2 = 1;
    settings.volley3 = 2;

    settings.power1 = 10;
    settings.power2 = 20;
    settings.power3 = 30;

    settings.glow1 = 1;  //low
    settings.glow2 = 2;  //med
    settings.glow3 = 3;  //high

    settings.failed_logins = 0;
    settings.minutes = 0;
    settings.launched = 0;
  }

  void load_settings() {
    fram_header_t hdr;
    fram.read(0, &hdr, sizeof(hdr));

    if (hdr.magic != settings_magic || hdr.version != settings_version || RESET_SAVE == true) {
      load_defaults();
      save_settings();
      return;
    }

    fram.read(sizeof(fram_header_t), &settings, sizeof(settings));
  }

  void save_settings() {
    fram_header_t hdr;
    hdr.magic = settings_magic;
    hdr.version = settings_version;

    fram.write(0, &hdr, sizeof(hdr));
    fram.write(sizeof(fram_header_t), &settings, sizeof(settings));
  }
};

Core1* Core1::instance = nullptr;