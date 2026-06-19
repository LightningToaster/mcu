//Valkor 2026-6-14

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(1, 25, NEO_GRB + NEO_KHZ800);

#define PIN_IR_LED      15
#define PIN_IR_SENSE    29
#define PIN_UV          14
#define IR_SAMPLES      10

const int threshold = 10;

void setup() {
    Serial.begin(115200);
    pinMode(PIN_IR_LED, OUTPUT);

    strip.begin();
    strip.clear();
    strip.show();

}//setup

int read_reflection() {
    int ambient = 0;
    int illuminated = 0;

    // read ambient light
    digitalWrite(PIN_IR_LED, LOW);
    delayMicroseconds(200);

    for (int i = 0; i < IR_SAMPLES; i++) {
        ambient += analogRead(PIN_IR_SENSE);
    }

    // read reflected IR
    digitalWrite(PIN_IR_LED, HIGH);
    delayMicroseconds(200);

    for (int i = 0; i < IR_SAMPLES; i++) {
        illuminated += analogRead(PIN_IR_SENSE);
    }

    digitalWrite(PIN_IR_LED, LOW);

    ambient /= IR_SAMPLES;
    illuminated /= IR_SAMPLES;

    return illuminated - ambient;
}

void loop() {
    int reflection = read_reflection();

    if (reflection > threshold/2) {
      Serial.println(reflection);
    }
    if (reflection > threshold) {
        analogWrite(PIN_UV, 25);
        strip.setPixelColor(0, 5, 0, 20);
    } else {
        analogWrite(PIN_UV, 0);
        strip.setPixelColor(0, 1, 0, 0);
    }
    strip.show();

    delay(10);
}//loop