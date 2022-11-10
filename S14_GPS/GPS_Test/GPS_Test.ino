const int RX_PIN = 16;
const int TX_PIN = 17;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600,SERIAL_8N1,RX_PIN,TX_PIN);//GPS
}

void loop() {
  while(Serial2.available()){
    Serial.print(char(Serial2.read()));
  }

}
