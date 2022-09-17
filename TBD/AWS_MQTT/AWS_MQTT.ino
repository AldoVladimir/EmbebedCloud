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

#include <Credentials.h>

//Credenciales de red Wifi
const char* ssid = SSID;
const char* password = PASSWORD; 

//Servidor MQTT
const char* mqtt_server = AWS_MQTT_SERVER;
const int mqtt_port = 8883;

String Read_rootca;
String Read_cert;
String Read_privatekey;
//********************************
#define BUFFER_LEN  256
long lastMsg = 0;
char payload[BUFFER_LEN]; //Datos a escribir en SD y serial
int value = 0;
byte mac[6];
char mac_Id[18];
int count = 1;
//********************************

//Configuración de cliente MQTT
WiFiClientSecure espClient;
PubSubClient client(espClient);

//Configuración de BME y LED
#define PIN_LED 32
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
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//Conectar a broker MQTT
void reconnect() {
  
  // Loop para reconección
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Creando un ID como ramdon
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    
    // Intentando conectarse
    if (client.connect(clientId.c_str())) {
      Serial.println("conectada");
      
    // Conectado, publicando un payload...
      client.publish("ei_out", "hello world");
    
    // ... y suscribiendo
      client.subscribe("ei_in");
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
  
  //****************
  if (!SPIFFS.begin(true)) {
    Serial.println("Se ha producido un error al montar SPIFFS");
    return;
  }
  //**********************
  //Root CA leer archivo.
  File file2 = SPIFFS.open("/AmazonRootCA1.pem", "r");
  if (!file2) {
    Serial.println("No se pudo abrir el archivo para leerlo");
    return;
  }
  Serial.println("Root CA File Content:");
  while (file2.available()) {
    Read_rootca = file2.readString();
    Serial.println(Read_rootca);
  }
  //*****************************
  // Cert leer archivo
  File file4 = SPIFFS.open("/615f-certificate.pem.crt", "r");
  if (!file4) {
    Serial.println("No se pudo abrir el archivo para leerlo");
    return;
  }
  Serial.println("Cert File Content:");
  while (file4.available()) {
    Read_cert = file4.readString();
    Serial.println(Read_cert);
  }
  //***************************************
  //Privatekey leer archivo
  File file6 = SPIFFS.open("/615f-private.pem.key", "r");
  if (!file6) {
    Serial.println("No se pudo abrir el archivo para leerlo");
    return;
  }
  Serial.println("privateKey contenido:");
  while (file6.available()) {
    Read_privatekey = file6.readString();
    Serial.println(Read_privatekey);
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

  Serial.println("================================================================================================");
  Serial.println("Certificados que pasan adjuntan al espClient");
  Serial.println();
  Serial.println("Root CA:");
  Serial.write(pRead_rootca);
  Serial.println("================================================================================================");
  Serial.println();
  Serial.println("Cert:");
  Serial.write(pRead_cert);
  Serial.println("================================================================================================");
  Serial.println();
  Serial.println("privateKey:");
  Serial.write(pRead_privatekey);
  Serial.println("================================================================================================");

  espClient.setCACert(pRead_rootca);
  espClient.setCertificate(pRead_cert);
  espClient.setPrivateKey(pRead_privatekey);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  //******************************************
  WiFi.macAddress(mac);
  snprintf(mac_Id, sizeof(mac_Id), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(mac_Id);
  //****************************************
  delay(2000);
 
}


void loop() {

  // ******************** SENSOR BMPx80 *********************************
  float h = bmp.readPressure();         // Lectura de presion
  float t = bmp.readTemperature();      // Lectura de humedad
  if (isnan(h) || isnan(t))
  {
    Serial.println("¡Error al leer del sensor BMP!");
    return;
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    //=============================================================================================
    String macIdStr = mac_Id;
    String Temprature = String(t);
    String Humidity = String(h);
    snprintf (payload, BUFFER_LEN, "{\"mac_Id\" : \"%s\", \"Temperatura\" : %s, \"Humedad\" : %s}", macIdStr.c_str(), Temprature.c_str(), Humidity.c_str());
    Serial.print("Publicando mensaje: ");
    Serial.print(count);
    Serial.println(payload);
    client.publish("$aws/things/Spiffs_test", payload);
    count = count + 1;
    //================================================================================================
  }
  digitalWrite(2, HIGH);   
  delay(1000);                      
  digitalWrite(2, LOW);   
  delay(1000);                       
}
