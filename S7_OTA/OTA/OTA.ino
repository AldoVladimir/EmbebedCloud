#include "OTA.h"
#include <Credentials.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  setupOTA("Device_name", SSID, PASSWORD);
  // Your setup code
}

void loop() {
  ArduinoOTA.handle();

  // Your code here
}
