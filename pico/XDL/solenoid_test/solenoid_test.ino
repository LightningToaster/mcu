//send a number through serial and the solenoid will pulse that many times
//send "on 70" to set on pulse to 70ms
//send "off 20" to set off to 70ms
#define SOLENOID_PIN 7
#define PIN_UV 14

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(1, 25, NEO_GRB + NEO_KHZ800);

uint8_t on_ms = 19;
uint8_t off_ms = 70;

void setup() {
  Serial.begin(9600);
  strip.begin();
  analogWrite(PIN_UV, 0);//TEMP so it stop blinking

  while (!Serial && millis() < 200) {}

  Serial.println("ready:");

  pinMode(SOLENOID_PIN, OUTPUT);
}

void pulse_solenoid() {
  strip.setPixelColor(0, 50, 0, 0);
  strip.show();

  digitalWrite(SOLENOID_PIN, HIGH);
  delay(on_ms);

  strip.setPixelColor(0, 0, 0, 0);
  strip.show();

  digitalWrite(SOLENOID_PIN, LOW);
  delay(off_ms);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    // handle "on 20"
    if (cmd.startsWith("on ")) {
      int value = cmd.substring(3).toInt();

      if (value > 0 && value <= 255) {
        on_ms = value;
        Serial.print("on_ms=");
        Serial.println(on_ms);
      }
      return;
    }

    // handle "off 80"
    if (cmd.startsWith("off ")) {
      int value = cmd.substring(4).toInt();

      if (value > 0 && value <= 255) {
        off_ms = value;
        Serial.print("off_ms=");
        Serial.println(off_ms);
      }
      return;
    }

    // if input is just a number, pulse that many times
    bool is_number = true;

    for (uint16_t i = 0; i < cmd.length(); i++) {
      if (!isDigit(cmd[i])) {
        is_number = false;
        break;
      }
    }

    if (is_number && cmd.length() > 0) {
      int num = cmd.toInt();

      for (int i = 0; i < num; i++) {
        pulse_solenoid();
      }
    }
  }
}