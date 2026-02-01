#define SPINUP_MS 1000
#define SUSTAINED_DUTY_CYCLE 100 // 0-255
#define PWM_PIN 1 // pin connected to OC1A (PB1)

void setup() {
  // configure Timer1 for fast PWM mode
  pinMode(PWM_PIN, OUTPUT);
  //TCCR1 = _BV(CS10) | _BV(PWM1A) | _BV(COM1A1); // no prescaler, fast PWM, non-inverting
  //GTCCR = _BV(PWM1B) | _BV(COM1B1); // optional for dual outputs
  //OCR1C = 255; // set TOP value for 8-bit resolution
  
  //OCR1A = 220; // max PWM duty cycle initially
  analogWrite(PWM_PIN, 100);
  delay(SPINUP_MS);
  analogWrite(PWM_PIN, 50);
  //OCR1A = SUSTAINED_DUTY_CYCLE;
}

void loop() {
  // nothing needed
}
