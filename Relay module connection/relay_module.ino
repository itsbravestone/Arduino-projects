int RelayPin = 6;

void setup() {
 pinMode(RelayPin, OUTPUT);
}

void loop() {
  digitalWrite(RelayPin, LOW);
  delay(3000);
  
  digitalWrite(RelayPin, HIGH);
  delay(3000);
}
