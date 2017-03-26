
#include <DHT.h>
#include <espsha1.h>
#include <espmysql.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>


#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);
Connector my_conn;
char* MySql_user = "xxxxxx";
char* MySql_password = "xxxxxxx";
// IP del host MySql
IPAddress MySql_IP(192, 168, 1, 107);
char query[2048];
const char INSERT_DATA[] = "INSERT INTO raspnet.temperaturasDormitorio (tempDormitorio, humedadDormitorio) VALUES (%s,%s)";
char tempMysql[6];
char humMysql[6];
const int sleepTimeS = 1;
/*
  static const uint8_t D0   = 16;
  static const uint8_t D1   = 5;
  static const uint8_t D2   = 4;
  static const uint8_t D3   = 0;
  static const uint8_t D4   = 2;
  static const uint8_t D5   = 14;
  static const uint8_t D6   = 12;
  static const uint8_t D7   = 13;
  static const uint8_t D8   = 15;
  static const uint8_t D9   = 3;
  static const uint8_t D10  = 1;*/
//#define pinPulsador 12
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

bool estatus = false;
const char* ssid = "xxxxxx";
const char* password = "xxxxxx";
int ledPin = 12;
int DHTPin = 14;
int val = 0;
DHT dht(DHTPin, DHTTYPE);

void setup()
{
  // setup serial port
  Serial.begin(9600);
  resetPines();
  pinMode(ledPin, OUTPUT);  
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);   // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.clearDisplay();   // clears the screen and buffer
  delay(1000);
  // setup WiFi
  setup_wifi();
  delay(1000);
}

void loop()
{
  //Show the time and date
  //float lux = testLux();
  //  int estado = digitalRead(pinPulsador);
  display.clearDisplay();
  datosWifi();
  displayTemp();  
  display.display();  
  //ESP.deepSleep(sleepTimeS * 60 * 1000000,WAKE_RF_DEFAULT);
  delay(895000);  
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  formatDisplay(1);
  display.setCursor(0, 0);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  delay(1000);
}

void turnOffOn(int pin, int valor) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, valor);
}

int datosWifi() {
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Conectado a ");
  display.print(ssid);
  display.setCursor(10, 16);
}

int formatDisplay(int tam) {
  display.setTextSize(tam);
  display.setTextColor(WHITE);
}

float testLux() {
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  return voltage;
}

void displayTemp() {
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) { 
    Serial.println("Failed to read from DHT sensor!"); 
    return; 
  } 
  display.setCursor(25, 20);
  display.print("Tmp: ");
  display.print(t);
  display.println(" *C");
  display.setCursor(25, 40);
  display.print("Hum: ");
  display.print(h);
  display.println(" %");  
  insertMysql(t, h);
  turnOffOn(ledPin, 1);
  delay(2000);
  turnOffOn(ledPin, 0);
  delay(500);
}

void resetPines() {
  turnOffOn(14, 0);
}

void mysql_conexion() {
  if (!my_conn.is_connected()) {
    my_conn.disconnect();    
    if (!my_conn.mysql_connect(MySql_IP, 3306, MySql_user, MySql_password)) {          
      Serial.println("Conexion a MySql fallida.");
    }
  }
}

void insertMysql(float t, float h) {
  mysql_conexion();
  dtostrf(t, 4, 2, tempMysql);
  dtostrf(h, 4, 2, humMysql);
  query[0] = '\0';
  sprintf(query, INSERT_DATA, tempMysql, humMysql);
  my_conn.cmd_query(query);
  Serial.println("INSERCION MYSQL REALIZADA!\n");
  my_conn.disconnect();
  delay(1000);
}

