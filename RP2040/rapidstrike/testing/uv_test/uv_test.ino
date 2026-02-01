//uses about 200mA at max
#define PIN_UV 5

void setup() {
  pinMode(PIN_UV, OUTPUT);
}

int brightness = 0;
int direction = 1;

void loop() {
  analogWrite(PIN_UV, brightness);
  delay(2);
  brightness += direction;
  if (brightness == 255 || brightness == 0) {
    direction *= -1;
  }
}
