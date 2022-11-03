#define RELAY 15
#define PIN_BUTTON 33

bool relay_status = 0;

//debouncing
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 300; //delay para debouncing en ms

//Al presionar un botón, cambia el color del led
void IRAM_ATTR isr_button(){
  if ((millis() - lastDebounceTime) > debounceDelay){
     relay_status = !relay_status; 
     lastDebounceTime = millis();
  }
}

void setup() {  
  //Boton de interrupción
  pinMode(PIN_BUTTON, INPUT);
  attachInterrupt(PIN_BUTTON, isr_button, RISING);

  //Relay
  pinMode(RELAY,OUTPUT);

  Serial.begin(115200);
}

void loop() {    
  if(relay_status){digitalWrite(RELAY,HIGH);}
  else{digitalWrite(RELAY,LOW);}
}
