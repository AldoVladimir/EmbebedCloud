#include <math.h>

int digitalWritePin = 32;
int digitalReadPin = 4;

int analogWritePin=13;
int analogReadPin = 15;

int t=0;

void setup() {

  pinMode(digitalWritePin, OUTPUT);
  Serial.begin(115200);

}

void loop() {
  //Prender y apagar LED
  //Revisar el valor de la lectura digital
  /*
    digitalWrite(digitalWritePin,HIGH);
    Serial.println(digitalRead(digitalReadPin));
    delay(500);
    digitalWrite(digitalWritePin,LOW);
    Serial.println(digitalRead(digitalReadPin));
    delay(500);
  */

  //Leer Boton con digitalRead
  //Ver rebote del boton con pulsos cortos
  //Usar condensador para mitigarlo
  //Serial.println(digitalRead(digitalReadPin));
  //delay(1);

  //Leer Boton con analogRead
  //Ver rebote del boton con pulsos cortos
  //Usar condensador para mitigarlo
  //Serial.println(analogRead(analogReadPin);
  //delay(1);


  //Leer el boton en paralelo en digital y en analogico
  //Serial.print(digitalRead(digitalReadPin));
  //Serial.print('\t');
  //Serial.println(analogRead(analogReadPin)/1024);
  //delay(1);

  //Leer el fotoresistor
  //Serial.println(analogRead(analogReadPin));
  //delay(100);

  //Leer sensor de temperatura
  //Convertir la temperatura a °C
  //3300/(10*4095)=0.0805
  //Serial.println(analogRead(analogReadPin) * 0.0805);

  //Escribir con el DAC
  //Dibujar un sinusoide
  //t++;
  //float signal=127*sin(2*PI*t/256)+128;
  //Serial.println(signal);
  //dacWrite(analogWritePin,signal);

  delay(100);
}
