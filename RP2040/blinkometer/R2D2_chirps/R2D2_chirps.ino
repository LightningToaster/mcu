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
  delay(5000);
   r2d2Phrase();
  // delay(random(300, 1500));     // pause between phrases
  //r2d2Shot();
  //r2d2SlideScream();
  //r2d2Scream();
  
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

void r2d2Shot() {

  // quick startled chirp
  tone(buzzerPin, 1400, 40);
  delay(50);

  // fast upward panic sweep
  for (int f = 1200; f < 3400; f += 80) {
    tone(buzzerPin, f);
    delay(4);
  }

  // shaky falling "damage" sound
  for (int f = 3400; f > 700; f -= 60) {
    tone(buzzerPin, f + random(-120,120)); // jitter
    delay(6);
  }

  // dying wobble
  for (int i=0;i<6;i++) {
    tone(buzzerPin, 600 + random(-150,150), 40);
    delay(50);
  }

  // final power-down chirp
  tone(buzzerPin, 280, 200);
  delay(220);

  noTone(buzzerPin);
}

void r2d2Scream() {

  // repeat sweep several times to make the scream
  for (int cycle = 0; cycle < 12; cycle++) {

    // fast upward sweep
    for (int f = 900; f < 3600; f += 120) {
      tone(buzzerPin, f);
      delay(3);
    }

    // fast downward sweep
    for (int f = 3600; f > 900; f -= 120) {
      tone(buzzerPin, f);
      delay(3);
    }
  }

  // abrupt stop like the movie sound
  noTone(buzzerPin);
}

void r2d2SlideScream() {

  int base = 3200;

  for (int i = 0; i < 120; i++) {

    // irregular warble around a slowly dropping base pitch
    int f = base + random(-700, 700);

    if (f < 300) f = 300;

    tone(buzzerPin, f);
    delay(random(4,10));

    // gradual downward trend
    base -= 18;
  }

  // final panicked chirps
  for (int i = 0; i < 6; i++) {
    tone(buzzerPin, random(900,2200), 40);
    delay(60);
  }

  noTone(buzzerPin);
}