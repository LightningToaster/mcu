#define PIN_ACCESS 10
#define PIN_STOCK 11
#define PIN_TRIGGER 12
#define PIN_TRIGGER_DEEP 13
#define PIN_REV 14
#define PIN_REV_DEEP 15

#define DEBOUNCE_DELAY 20  // ms

bool last_access = HIGH;
bool last_stock = HIGH;
bool last_trigger = HIGH;
bool last_trigger_deep = HIGH;
bool last_rev = HIGH;
bool last_rev_deep = HIGH;

unsigned long last_access_time = 0;
unsigned long last_stock_time = 0;
unsigned long last_trigger_time = 0;
unsigned long last_trigger_deep_time = 0;
unsigned long last_rev_time = 0;
unsigned long last_rev_deep_time = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_ACCESS, INPUT_PULLUP);
  pinMode(PIN_STOCK, INPUT_PULLUP);
  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_TRIGGER_DEEP, INPUT_PULLUP);
  pinMode(PIN_REV, INPUT_PULLUP);
  pinMode(PIN_REV_DEEP, INPUT_PULLUP);
}

void loop() {
  unsigned long now = millis();

  bool current_access = digitalRead(PIN_ACCESS);
  if (current_access != last_access && now - last_access_time > DEBOUNCE_DELAY) {
    last_access = current_access;
    last_access_time = now;
    Serial.print("access ");
    Serial.println(current_access == LOW ? "pressed" : "released");
  }

  bool current_stock = digitalRead(PIN_STOCK);
  if (current_stock != last_stock && now - last_stock_time > DEBOUNCE_DELAY) {
    last_stock = current_stock;
    last_stock_time = now;
    Serial.print("stock ");
    Serial.println(current_stock == LOW ? "pressed" : "released");
  }

  bool current_trigger = digitalRead(PIN_TRIGGER);
  if (current_trigger != last_trigger && now - last_trigger_time > DEBOUNCE_DELAY) {
    last_trigger = current_trigger;
    last_trigger_time = now;
    Serial.print("trigger ");
    Serial.println(current_trigger == LOW ? "pressed" : "released");
  }

  bool current_trigger_deep = digitalRead(PIN_TRIGGER_DEEP);
  if (current_trigger_deep != last_trigger_deep && now - last_trigger_deep_time > DEBOUNCE_DELAY) {
    last_trigger_deep = current_trigger_deep;
    last_trigger_deep_time = now;
    Serial.print("trigger_deep ");
    Serial.println(current_trigger_deep == LOW ? "pressed" : "released");
  }

  bool current_rev = digitalRead(PIN_REV);
  if (current_rev != last_rev && now - last_rev_time > DEBOUNCE_DELAY) {
    last_rev = current_rev;
    last_rev_time = now;
    Serial.print("rev ");
    Serial.println(current_rev == LOW ? "pressed" : "released");
  }

  bool current_rev_deep = digitalRead(PIN_REV_DEEP);
  if (current_rev_deep != last_rev_deep && now - last_rev_deep_time > DEBOUNCE_DELAY) {
    last_rev_deep = current_rev_deep;
    last_rev_deep_time = now;
    Serial.print("rev_deep ");
    Serial.println(current_rev_deep == LOW ? "pressed" : "released");
  }
}
