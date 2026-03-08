#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

class Display {
public:
    Display(uint8_t sda_pin, uint8_t scl_pin)
      : oled(128, 64, &Wire, -1),
        sda_pin(sda_pin),
        scl_pin(scl_pin),
        adc_value(0),
        adc_channel(1),
        flicker_count(0),
        seconds(0),
        minutes(0),
        hours(0),
        USB(false)
    {}

    bool begin(uint8_t i2c_addr = 0x3C) {
        Wire.setSDA(sda_pin);
        Wire.setSCL(scl_pin);
        Wire.begin();

        Wire.beginTransmission(i2c_addr);
        if (Wire.endTransmission() != 0) {
            Serial.println("oled not detected");
            return false;
        }

        oled.begin(SSD1306_SWITCHCAPVCC, i2c_addr);
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setTextColor(SSD1306_WHITE);
        oled.setCursor(0, 0);
        oled.display();

        last_frame_ms = millis();
        return true;
    }

    // ---------------- setters ----------------
    void set_adc_value(uint16_t value) { adc_value = value; }
    void set_adc_channel(uint16_t channel) { adc_channel = channel; }
    void set_flicker(uint16_t count) { flicker_count = count; }
    void set_seconds(uint8_t sec) { seconds = sec; }
    void set_minutes(uint8_t min) { minutes = min; }
    void set_hours(uint16_t h) { hours = h; }
    void set_USB(bool usb) { USB = usb; }
    void set_battery(const char* text) {
        strncpy(battery, text, sizeof(battery)-1);
        battery[sizeof(battery)-1] = '\0';
    }

    // call constantly from loop()
    void operate() {
        uint32_t now = millis();
        if (now - last_frame_ms < frame_interval_ms) return;
        last_frame_ms = now;

        oled.clearDisplay();

        // battery
        oled.setTextSize(1);
        oled.setCursor(102, 0);
        oled.print(battery);

        // ADC
        oled.setCursor(0, 0);
        oled.print("A");
        oled.print(adc_channel);
        oled.print("=");
        oled.print(adc_value);

        // stopwatch
        oled.setCursor(0, 56);
        if (hours > 0) {
            if (hours <= 9) oled.print("0");
            oled.print(hours);
            oled.print("h");
        }
        if (minutes > 0 || hours > 0) {
            if (minutes <= 9) oled.print("0");
            oled.print(minutes);
            oled.print("m");
        }
        if (seconds <= 9) oled.print("0");
        oled.print(seconds);
        oled.print("s");

        // USB indicator
        // if (USB) {
        oled.setCursor(104, 56);
        oled.print(FIRMWARE_VERSION);
        // }

        // flicker
        if (flicker_count > 0) {
            if (flicker_count <= 9) {
                oled.setTextSize(4); oled.setCursor(55, 15);
            } else if (flicker_count <= 99) {
                oled.setTextSize(4); oled.setCursor(42, 18);
            } else if (flicker_count <= 999) {
                oled.setTextSize(3); oled.setCursor(39, 20);
            } else {
                oled.setTextSize(3); oled.setCursor(31, 20);
            }
            oled.print(flicker_count);
        }

        oled.display();
    }

private:
    Adafruit_SSD1306 oled;
    uint8_t sda_pin, scl_pin;

    uint16_t adc_value;
    uint16_t adc_channel;
    uint16_t flicker_count;
    uint8_t seconds;
    uint8_t minutes;
    uint16_t hours;
    bool USB;
    char battery[8];

    uint32_t last_frame_ms;
    static constexpr uint32_t frame_interval_ms = 50; // 20 Hz
};