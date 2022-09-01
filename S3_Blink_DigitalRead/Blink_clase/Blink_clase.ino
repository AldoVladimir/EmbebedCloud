const int LED_PIN = 34;

void setup() {
  pinMode(LED_PIN,OUTPUT);
  Serial.begin(115200);
}

void loop() {
  digitalWrite(LED_PIN,HIGH);
  delay(500);  //Pausa en ms
  digitalWrite(LED_PIN,LOW);
  delay(500);
}
