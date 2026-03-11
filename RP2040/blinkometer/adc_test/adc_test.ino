void setup() {
  Serial.println(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t a = analogRead(26);
  uint16_t b = analogRead(27);
  delay(50);
  Serial.print(a);
  Serial.print("   ");
  Serial.println(b);
  
}
