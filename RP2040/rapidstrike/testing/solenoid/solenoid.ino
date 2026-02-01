//send a number through serial and the solenoid will pulse that many times
#define SOLENOID_PIN 29

#define BUILTIN_LED 25

void setup() {
  Serial.begin(115200);
  Serial.println("ready:");

  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
    if (Serial.available()) {
      char c = Serial.read();
      if (c >= '0' && c <= '9') { // check if it's a valid digit
        int num = c - '0'; // convert char to int
        digitalWrite(BUILTIN_LED, HIGH);
        for (int i=0; i<num; i++){
          digitalWrite(SOLENOID_PIN, HIGH);
          delay(20);//18
          digitalWrite(SOLENOID_PIN, LOW);
          delay(80);
        }
        digitalWrite(BUILTIN_LED, LOW);
      }
    }
}
