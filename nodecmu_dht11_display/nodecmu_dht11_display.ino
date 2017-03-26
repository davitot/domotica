#include <espsha1.h>
#include <espmysql.h>
#include <dht11.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>


#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);
Connector my_conn;
char* MySql_user = "xxxxx";
char* MySql_password = "xxxxxx";
// IP del host MySql
IPAddress MySql_IP(192, 168, 1, 107);
char query[2048];
const char INSERT_DATA[] = "INSERT INTO raspnet.temperaturas (tempDormitorio, humedadDormitorio) VALUES (%s,%s)";
char tempMysql[6];
char humMysql[6];
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

dht11 DHT11;

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

bool estatus = false;
const char* ssid = "xxxxx";
const char* password = "xxxxx";
int displayTime();
//int inicio = 0;

void setup()
{
  // setup serial port
  Serial.begin(9600);
  DHT11.attach(D4); //now you can just type the printed pin number
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
  float lux = testLux();
//  int estado = digitalRead(pinPulsador);  
  display.clearDisplay();
  datosWifi();
  displayTemp();
  display.display();
  insertMysql();  
  delay(900000);
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

void mysql_conexion() {
  if (!my_conn.is_connected()) {
    my_conn.disconnect();
    Serial.println("Conectando a MySql...");
    if (my_conn.mysql_connect(MySql_IP, 3306, MySql_user, MySql_password)) {
      Serial.println("Conexion a MySql OK!");
    }
    else {
      Serial.println("Conexion a MySql fallida.");
    }
  }
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
  int chk = DHT11.read();
  float temperatura = (float)DHT11.temperature;
  float humedad = (float)DHT11.humidity;
  switch (chk)
  {
    case 0:
      display.setCursor(10, 16);
      display.print("Tmp: ");
      display.print(temperatura, 2);
      display.println(" C");
      display.setCursor(10, 36);
      display.print("Hum: ");
      display.print(humedad, 2);
      display.println(" %");      
      break;
    case -1: display.println("Checksum error"); break;
    case -2: display.println("Time out error"); break;
    default: display.println("Error desconocido"); break;
  }
  display.display();
  delay(1000);
}

void insertMysql() {
  int chk = DHT11.read();
  float temperatura = (float)DHT11.temperature;
  float humedad = (float)DHT11.humidity;
  switch (chk)
  {
    case 0:
      mysql_conexion();      
      dtostrf(temperatura, 4, 2, tempMysql);
      dtostrf(humedad, 4, 2, humMysql);
      query[0] = '\0';
      sprintf(query, INSERT_DATA, tempMysql, humMysql);
      my_conn.cmd_query(query);
      Serial.println("INSERCION MYSQL REALIZADA!\n");
      my_conn.disconnect();
      break;
    case -1: display.println("Checksum error"); break;
    case -2: display.println("Time out error"); break;
    default: display.println("Error desconocido"); break;
  }
  delay(1000);
}


