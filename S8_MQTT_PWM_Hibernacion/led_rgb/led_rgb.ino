#define RED 14
#define GREEN 27 
#define BLUE 12

void setup() {
  
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);

}

void loop() {
  delay(1000);
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,HIGH);
  
  delay(1000);
  digitalWrite(RED,LOW);
  delay(1000);
  digitalWrite(GREEN,LOW);
  delay(1000);
  digitalWrite(BLUE,LOW);
  
}
