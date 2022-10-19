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
#include <Adafruit_BMP280.h>
#include <ArduinoJson.h>

#include <Credentials.h>

//Credenciales de red Wifi
const char* ssid = SSID;
const char* password = PASSWORD; 

//Servidor MQTT
const char* mqtt_server = AWS_MQTT_SERVER;
const int mqtt_port = 8883;
String clientId = "Axolote_";
const char* PUBLISH_TOPIC = "embebed_cloud/Axolote/test_topic_pub";
const char* SUBSCRIBE_TOPIC = "embebed_cloud/Axolote/test_topic_sub";

String Read_rootca;
String Read_cert;
String Read_privatekey;
//********************************
#define BUFFER_LEN  256
long lastMsg = 0;
char payload[BUFFER_LEN]; //Datos a enviar por MQTT
byte mac[6];
char mac_Id[18];

#define JSON_BUFFER_INCOMING_LEN 200
#define JSON_BUFFER_OUTGOING_LEN 200
StaticJsonDocument<JSON_BUFFER_INCOMING_LEN> payload_in;
StaticJsonDocument<JSON_BUFFER_OUTGOING_LEN> payload_out;
//********************************

//Configuración de cliente MQTT
WiFiClientSecure espClient;
PubSubClient client(espClient);

//Configuración de BME y LED
#define PIN_LED 32
#define PIN_LDR 34
Adafruit_BMP280 bmp;

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
  deserializeJson(payload_in, payload);
  const char* message = payload_in["message"];
  Serial.println(message);
  payload_in.clear();
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
      client.subscribe(SUBSCRIBE_TOPIC);
      
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
  bmp.begin(0x76); //Inicializar comunicación I2C con el sensor barométrico 
  Serial.setDebugOutput(true);
  
  // Inicializa con el PIN led2.
  pinMode(PIN_LED, OUTPUT);
  setup_wifi();
  delay(1000);
  clientId += AXOLOTE_ID;
  
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
  File file4 = SPIFFS.open("/692c-certificate.pem.crt", FILE_READ);
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
  File file6 = SPIFFS.open("/692c-private.pem.key", FILE_READ);
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

  //******************************************
  WiFi.macAddress(mac);
  snprintf(mac_Id, sizeof(mac_Id), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  //Serial.println(mac_Id);
  //****************************************
  delay(2000);
 
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    //=============================================================================================    
    //Json Serializer
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();
    float light = analogRead(PIN_LDR);

    
    payload_out["mac_Id"] = mac_Id;
    payload_out["device_Id"] = clientId;
    payload_out["temp_C"] = serialized(String(temperature,2));
    payload_out["press_hPa"] = serialized(String(pressure/100,2));
    payload_out["ligh_adim"] = serialized(String(light,2));


    serializeJson(payload_out, payload);
        
    Serial.print("Publicando mensaje: ");
    Serial.println(payload);

    client.publish(PUBLISH_TOPIC, payload);
    payload_out.clear();
    //================================================================================================
  }
                      
}
