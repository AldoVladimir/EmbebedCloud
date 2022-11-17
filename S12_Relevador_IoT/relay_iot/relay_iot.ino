/*
 Conectar ESP32 a AWS
 https://www.todomaker.com
 Colaboración: Néstor Ccencho
 Todos los derechos reservados.
*/
//Añadir bibliotecas
#include "SPIFFS.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Credentials.h>

//Credenciales de red Wifi
const char* ssid = SSID;
const char* password = PASSWORD; 

//Servidor MQTT
const char* mqtt_server = AWS_MQTT_SERVER;
const int mqtt_port = 8883;
String clientId = "Axolote_";
String PUB_TOPIC = "$aws/things/";
String SUB_TOPIC = "$aws/things/";
const char* BUTTON_TOPIC = "embebed_cloud/Axolote/relay";

String Read_rootca;
String Read_cert;
String Read_privatekey;
//********************************
#define BUFFER_LEN  256
int current_message_version = 0;

#define JSON_BUFFER 400
DynamicJsonDocument in_status(JSON_BUFFER);
DynamicJsonDocument report_status(JSON_BUFFER);
DynamicJsonDocument desired_status(JSON_BUFFER);
//********************************

//debouncing
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 250; //delay para debouncing en ms

//Configuración de cliente MQTT
WiFiClientSecure espClient;
PubSubClient client(espClient);

//Configuración de LED y boton
#define RELAY 15
#define PIN_BUTTON 33

//Color LED
bool relay_status = 0;
bool button_pressed = 0;

//Al presionar un botón, cambia el color del led
void IRAM_ATTR isr_button(){
  if ((millis() - lastDebounceTime) > debounceDelay){
     relay_status = !relay_status; 
     lastDebounceTime = millis();
  }
}

//Conectar a red Wifi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando.. ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
}

//Callback
void callback(char* topic, byte* payload, unsigned int length) {

  //Json parser deserializer
  deserializeJson(in_status, payload);
  serializeJson(in_status, Serial);
  
  int incoming_message_version = in_status["version"];
  Serial.print("current version = ");
  Serial.println(current_message_version);
  Serial.print("incoming version = ");
  Serial.println(incoming_message_version);


  //Si la versión del cambio es más reciente,
  //Cambiar el led RGB al color que se indica
  if(incoming_message_version > current_message_version){
    current_message_version = incoming_message_version;
    bool relay_status = in_status["state"]["status"];
    
    if(relay_status){digitalWrite(RELAY,HIGH);}
    else{digitalWrite(RELAY,LOW);}
  
    //Publicar el estado actual  
    report_status["state"]["reported"]["status"] = relay_status;
  
    char report[BUFFER_LEN];
    serializeJson(report_status, report);   
    Serial.println("Current status");
    serializeJson(report_status, Serial);  
    client.publish(PUB_TOPIC.c_str(), report);      
  }  
}


//Conectar a broker MQTT
void reconnect() {
  
  // Loop para reconección
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");  

    // Intentando conectarse
    if (client.connect(clientId.c_str())) {
      Serial.println("conectada");      
   
    // ... y suscribiendo
      client.subscribe(SUB_TOPIC.c_str());
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" Esperando 5 segundos");
      
      // Tiempo muerto de 5 segundos
      delay(5000);
    }
  }
}

void setup() {
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  //Boton de interrupción
  pinMode(PIN_BUTTON, INPUT);
  attachInterrupt(PIN_BUTTON, isr_button, RISING);
  
  //Relay
  pinMode(RELAY,OUTPUT);
  
  setup_wifi();
  delay(1000);
  clientId += AXOLOTE_ID;
  PUB_TOPIC += clientId+"/shadow/update";
  SUB_TOPIC += clientId+"/shadow/update/delta";

  
  //****************
  if (!SPIFFS.begin(true)) {
    Serial.println("Se ha producido un error al montar SPIFFS");
    return;
  }
  //**********************
  //Root CA leer archivo.
  File file2 = SPIFFS.open("/AmazonRootCA1.pem", FILE_READ);
  if (!file2) {
    Serial.println("No se pudo abrir el archivo para leerlo");
    return;
  }
  Serial.println("Leido Root CA File");
  while (file2.available()) {
    Read_rootca = file2.readString();
    }
    
  //*****************************
  // Cert leer archivo
  File file4 = SPIFFS.open("/615f-certificate.pem.crt", FILE_READ);
  if (!file4) {
    Serial.println("No se pudo abrir el archivo para leerlo");
    return;
  }
  Serial.println("Leido Cert File");
  while (file4.available()) {
    Read_cert = file4.readString();
    }
    
  //***************************************
  //Privatekey leer archivo
  File file6 = SPIFFS.open("/615f-private.pem.key", FILE_READ);
  if (!file6) {
    Serial.println("No se pudo abrir el archivo para leerlo");
    return;
  }
  Serial.println("Leido privateKey");
  while (file6.available()) {
    Read_privatekey = file6.readString();
    }
  
  //=====================================================

  char* pRead_rootca;
  pRead_rootca = (char *)malloc(sizeof(char) * (Read_rootca.length() + 1));
  strcpy(pRead_rootca, Read_rootca.c_str());

  char* pRead_cert;
  pRead_cert = (char *)malloc(sizeof(char) * (Read_cert.length() + 1));
  strcpy(pRead_cert, Read_cert.c_str());

  char* pRead_privatekey;
  pRead_privatekey = (char *)malloc(sizeof(char) * (Read_privatekey.length() + 1));
  strcpy(pRead_privatekey, Read_privatekey.c_str());
  
  
  espClient.setCACert(pRead_rootca);
  espClient.setCertificate(pRead_cert);
  espClient.setPrivateKey(pRead_privatekey);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  delay(2000);

  //Configuración inicial: Todo apagado.
  //Publica el primer estado por mqtt

  if (!client.connected()) {
    reconnect();
  }  
  //Json Serializer  
  digitalWrite(RELAY,LOW);
  
  report_status["state"]["reported"]["status"] = 0;
  char payload[BUFFER_LEN]; 
  serializeJson(report_status, payload);
  serializeJson(report_status, Serial);
  delay(1000);
  client.publish(PUB_TOPIC.c_str(), payload);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

  if(button_pressed){
    desired_status["state"]["desired"]["status"] = relay_status;
    char payload[BUFFER_LEN]; 
    serializeJson(desired_status, payload);
    client.publish(BUTTON_TOPIC, payload);
    button_pressed = 0;    
  }
                       
}
