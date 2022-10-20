const int PWM_PIN = 12;
const int PWM_FREQ = 10000; //[Hz] //Frecuencia <1kHz poca eficiencia; >5kHz buena eficiencia
const int PWM_CHANNEL = 0;
const int PWM_RESOLUTION = 8; //Profundidad de bits


const int BUTTON = 4;


float t; //Tiempo
int  waveform = 0; //Menú de formas de onda para modular el ciclo de trabajo
float dutyCycle; //Que tan grande es el pulso - 100% Prendido todo el tiempo y 0% Apagado todo el tiempo

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON,INPUT_PULLDOWN);

  //Configuracion de PWM  
  ledcSetup(PWM_CHANNEL, PWM_FREQ , PWM_RESOLUTION); // Configuracion del canal PWM
  ledcAttachPin(PWM_PIN, PWM_CHANNEL); // Asignación del canal a un pin

}

void loop() {

  //Polling botón
  if(digitalRead(BUTTON)){
     waveform++;  
     if(waveform>7) {
        waveform=0;      
     }
     Serial.println(waveform);  
     delay(300);  
  }
  //Volverlo interrupción

  
  //Tiempo del ciclo
  t++;  
  if(t>=256) {
    t=0;
  } 


  //Menú de formas de onda disponibles
  switch (waveform){
    
    case 0:
     dutyCycle = 0;
    break;
    
    case 1:
     dutyCycle = 0.3;
    break;  
      
    case 2:
      dutyCycle = 0.5;
    break;
    
    case 3:
      dutyCycle = 0.8;
    break;

    case 4:
      dutyCycle = 1;
    break;

    //Graficar formas de onda en Desmos
    case 5:
      dutyCycle = (t+0.1)/256;
      delay(2.5);
    break;

    case 6:    
      dutyCycle = 0.5*sin(2*PI*t/256)+0.5;
      delay(0.5);
    break;

    case 7:    
      dutyCycle = pow(2.71828,((t/255)-1)*2);
      delay(4);
    break;
  }

  //Muestra forma de onda en serial plotter
  Serial.println(dutyCycle*100);
  
  //Envia señal PWM al pin seleccionado
  ledcWrite(PWM_CHANNEL, dutyCycle*255);

  
}
