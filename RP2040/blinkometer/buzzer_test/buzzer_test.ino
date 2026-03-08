// piezo buzzer test for rp2350

const uint8_t buzzer_pin = 2;

void setup() {
    pinMode(buzzer_pin, OUTPUT);
}

void loop() {
    // simple frequency sweep to verify buzzer works
    for (uint16_t freq = 100; freq <= 1200; freq += 50) {
        tone(buzzer_pin, freq);
        delay(100);
    }

    // stop tone briefly
    noTone(buzzer_pin);
    delay(500);

    // fixed beeps
    for (uint8_t i = 0; i < 3; i++) {
        tone(buzzer_pin, 1000);
        delay(200);
        noTone(buzzer_pin);
        delay(200);
    }

    delay(1000);
}