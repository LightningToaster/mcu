#define BUILTIN_LED 25
#define BLINK_DELAY 200
#define SETUP_OUTPUT "blink test has begun!"

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);

  Serial.begin(9600);
  while (!Serial && millis() < 200) {};
  Serial.println(SETUP_OUTPUT);

}

void loop() {
  //digitalWrite(BUILTIN_LED, (millis() & 0xFF) < 32 ? HIGH : LOW);
}