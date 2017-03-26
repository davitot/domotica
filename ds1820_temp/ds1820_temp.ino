
#include <DHT.h>
#include <espsha1.h>
#include <espmysql.h>
#include <ESP8266WiFi.h>

Connector my_conn;
char* MySql_user = "xxxxxx";
char* MySql_password = "xxxxx";
// IP del host MySql
IPAddress MySql_IP(192, 168, 1, 107);
char query[2048];
const char INSERT_DATA[] = "INSERT INTO raspnet.temperaturasDormitorio (tempDormitorio, humedadDormitorio) VALUES (%s,%s)";
char tempMysql[6];
char humMysql[6];
const int sleepTimeS = 1;
/*
  static const uint8_t D2   = 2;
*/

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

const char* ssid = "xxxxxx";
const char* password = "xxxxxx";
int DHTPin = 2;
DHT dht(DHTPin, DHTTYPE, 11);

void setup()
{
  // setup serial port

  Serial.begin(9600);
  dht.begin();
  delay(2000);
  // setup WiFi
  setup_wifi();
  delay(1000);
}

void loop()
{
  datosWifi();
  displayTemp();
  //ESP.deepSleep(sleepTimeS * 60 * 1000000,WAKE_RF_DEFAULT)  
  delay(895000);
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  delay(1000);
}

int datosWifi() {
  Serial.print("Conectado a ");
  Serial.println(ssid);
}

void displayTemp() {
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Tmp: ");
  Serial.print(t);
  Serial.println(" *C");
  Serial.println("");
  Serial.print("Hum: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.println("");
  insertMysql(t, h);
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



