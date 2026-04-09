/*
  RP2350 R2D2 Sound Sketch
  Plays random R2D2-like beeps using tone() on a buzzer connected to pin 2
*/

const int buzzerPin = 2;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  randomSeed(analogRead(28));   // seed randomness
}

void loop() {
  r2d2Phrase();
  delay(random(300, 1500));     // pause between phrases
}

void r2d2Phrase() {
  int notes = random(5, 15);

  for (int i = 0; i < notes; i++) {
    int freq = random(400, 3200);   // R2D2 style pitch range
    int dur  = random(30, 180);     // short chirps

    tone(buzzerPin, freq, dur);

    delay(dur * 1.3);

    // occasional pitch slide
    if (random(0, 5) == 0) {
      chirp(freq, random(800, 3500), random(60, 200));
    }
  }
}

void chirp(int startFreq, int endFreq, int duration) {
  int steps = 20;
  int stepDelay = duration / steps;
  float step = (endFreq - startFreq) / (float)steps;

  for (int i = 0; i < steps; i++) {
    int f = startFreq + step * i;
    tone(buzzerPin, f);
    delay(stepDelay);
  }

  noTone(buzzerPin);
}