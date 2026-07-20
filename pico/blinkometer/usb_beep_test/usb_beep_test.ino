// piezo buzzer test
// buzzer on pin 2
// beep when pin 0 is high

const uint8_t buzzer_pin = 2;
const uint8_t sense_pin  = 0;

void setup() {
    pinMode(buzzer_pin, OUTPUT);
    pinMode(sense_pin, INPUT);

    Serial.begin(9600);
    
}

void loop() {
  Serial.println(digitalRead(sense_pin));
    if (digitalRead(sense_pin) == HIGH) {
        tone(buzzer_pin, 1000); // 2 khz tone
    } else {
        tone(buzzer_pin, 400); // 2 khz tone
    }
    delay(100);
    noTone(buzzer_pin);
    delay(400);
}