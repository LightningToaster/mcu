#pragma once

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

    uint8_t dart_detection1;
    uint8_t dart_detection2;
    uint8_t dart_detection3;

    uint8_t glow1;
    uint8_t glow2;
    uint8_t glow3;

    uint8_t failed_logins;
    uint32_t minutes;
    uint32_t launched;
};

enum display_mode : uint8_t {//might not need all these
    display_splash = 0,
    display_lock,
    display_logo,
    display_info,
    display_menu,
    display_ammo,
    display_empty,
    display_battery,
};

enum battery_state : uint8_t {
    battery_disconnected = 0,
    battery_low,
    battery_good,
    battery_overcharged,
};



#include "fram.hpp"
#include "oled.hpp"

class Core1 {
public:
    Fram fram;
    Oled oled;

    Adafruit_SH1106G display_driver{128, 64, &Wire, -1};

    volatile uint8_t selected_char = 0;
    char pin_entered[PIN_LENGTH + 1];//extra for null char

    volatile uint8_t display_mode = 0;
    volatile uint8_t selection = 2;
    volatile uint16_t ammo = 0;
    volatile uint16_t fired = 0;
    volatile uint16_t darts_session = 0;
    volatile uint8_t battery_state = 0;

    char voltage[8] = {0};

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
    }

private:
    unsigned long last_frame = 0;
    static constexpr uint32_t frame_ms = 33;

    static void core1_entry() {
        if (instance) instance->loop();
    }

    void loop() {
        while (true) {
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
                case display_lock: oled.draw_lock(selected_char, pin_entered); break;
                case display_logo: oled.draw_logo(); break;
                case display_info: oled.draw_info(settings.minutes, settings.launched, voltage); break;
                case display_menu: oled.draw_menu(settings, selection, darts_session); break;
                case display_ammo: oled.draw_ammo(ammo); break;
                case display_empty: oled.draw_empty(voltage, fired); break;
                case display_battery: oled.draw_battery(battery_state, voltage); break;
            }

            oled.display();
        }
    }

    // --------------------
    // FRAM persistence
    // --------------------

    static constexpr uint32_t settings_magic = 0x58444C31; // "XDL1"
    static constexpr uint16_t settings_version = 1;

    struct fram_header_t {
        uint32_t magic;
        uint16_t version;
    };

    void load_defaults() {
        settings.profile = 1;

        settings.volley1 = 0;
        settings.volley2 = 2;
        settings.volley3 = 3;

        settings.power1 = 10;
        settings.power2 = 20;
        settings.power3 = 30;

        settings.dart_detection1 = 1;
        settings.dart_detection2 = 1;
        settings.dart_detection3 = 1;

        settings.glow1 = 1;//low
        settings.glow2 = 2;//med
        settings.glow3 = 3;//high

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