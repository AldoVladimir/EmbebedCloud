const int digtialWritePin = 32;
const int digitalReadPin = 34;
//#define digitalReadPin 34

void setup() {
  pinMode(digtialWritePin, OUTPUT);
  pinMode(digitalReadPin, INPUT);
  Serial.begin(115200);
}

void loop() {
  digitalWrite(digtialWritePin,HIGH);
  Serial.println(digitalRead(digitalReadPin));
  //digitalWrite(digtialWritePin,1); Equivalente, menos legible
  delay(500);
  digitalWrite(digtialWritePin,LOW);
  Serial.println(digitalRead(digitalReadPin));
  delay(500);  
}
