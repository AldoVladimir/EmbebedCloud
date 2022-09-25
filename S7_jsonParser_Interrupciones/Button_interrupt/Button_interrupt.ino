//Doc:
//https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/gpio.html
#define PIN_BUTTON 33
int numberKeyPresses = 0;

void ARDUINO_ISR_ATTR isr(void* arg) {
  numberKeyPresses += 1;
  }


void setup() {
    Serial.begin(115200);
    pinMode(PIN_BUTTON, INPUT);
    //Quitar el resistor. Usar los resistores pullup y down integrados
    //pinMode(PIN_BUTTON, INPUT_PULLDOWN);
    attachInterruptArg(PIN_BUTTON, isr, &numberKeyPresses, RISING);
}

void loop() {
   
    Serial.printf("Button 1 has been pressed %u times\n", numberKeyPresses);

    static uint32_t lastMillis = 0;
    if (millis() - lastMillis > 10000) {
      lastMillis = millis();
      detachInterrupt(PIN_BUTTON);
    } 
      delay(1);
}
