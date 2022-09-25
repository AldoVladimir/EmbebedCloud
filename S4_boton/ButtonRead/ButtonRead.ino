#define PIN_BUTTON 33
#define PIN_LED    32
// LED_BUILTIN


/*
 * Estados de señales digitales
 *   HIGH      LOW
 *     1        0
 *   3,3V       0V
 *   
 *   Estos se pueden leer con la función digitalRead()
 */

void setup() {
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  // pinMode(PIN_BUTTON, OUTPUT);   // Configuración para poner al pin en modo de salida, por ejemplo si queremos encender un led


}

void loop() {
  //Serial.println(digitalRead(PIN_BUTTON));
  //delay(1000);
  //Serial.print("Led Builtin: ");
  //Serial.println(LED_BUILTIN);


  // Polling -> Sondeo
  int btn_st = digitalRead(PIN_BUTTON);
  if(btn_st == HIGH){
    digitalWrite(PIN_LED, HIGH);
  }else{
    digitalWrite(PIN_LED, LOW);
  }

  // delay(5000);
}
