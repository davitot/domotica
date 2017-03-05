#include <ESP8266WiFi.h>
const char* ssid = "WLAN_DAV";
const char* password = "Qup_4_mcax";

void setup() {
  Serial.begin(9600);
  setup_wifi(); 

}

void loop() {

}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {    
    Serial.print(".");
    delay(1000);
  }  
  datosWifi();
}

int datosWifi(){
  Serial.println(""); 
  Serial.print("Conectado a ");
  Serial.println(ssid); 
  Serial.print("IP: "); 
  Serial.println(WiFi.localIP());  
}

