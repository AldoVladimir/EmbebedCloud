#define PIN_LDR 34
#define PIN_LM35 35
#define PIN_LED 32
#define PIN_BUTTON 33

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED,OUTPUT);
}

void loop() {
  
  
  Serial.println(analogRead(PIN_LDR)); //Lectura LDR
  //Serial.println(analogRead(PIN_LM35)*0.1221); //Lectura LM35

  /*
  //Si esta oscuro, enciende LED
  float ligh_THLD = 300;
  if (analogRead(PIN_LDR)>300){
    digitalWrite(PIN_LED,LOW);    
  }
  else{
    digitalWrite(PIN_LED,HIGH);
  }
  */
  
}
