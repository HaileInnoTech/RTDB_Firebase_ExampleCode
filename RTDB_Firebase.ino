#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "paPOCOF3"
#define WIFI_PASSWORD "12345678"

#define DHTPIN 2       // Pin connected to DHT11 data pin
#define DHTTYPE DHT11   // DHT sensor type (DHT11 or DHT22)

#define API_KEY "AIzaSyAOmVngCIDg_WpMykay6NeKttHJPDu0pHc"
#define DATABASE_URL "smart-farming-system-f9435-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "hai28022002@gmail.com"
#define USER_PASSWORD "Hoanghai___2002"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
    fbdo.setBSSLBufferSize(4096, 1024);
    // Connect to RTDB
    Firebase.begin(&config, &auth);
    Firebase.reconnectNetwork(true);
}

void loop()
{
    if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) 
    {
        sendDataPrevMillis = millis();
        storeDHTData();
    }
}

void storeDHTData() {
  float temperature = dht.readTemperature();    // Read temperature in Celsius
  float humidity = dht.readHumidity();          // Read humidity
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  unsigned long currentTime = millis();

  Firebase.RTDB.pushFloat(&fbdo,"/Temp-Humid/temperature", temperature);
  Firebase.RTDB.pushFloat(&fbdo,"/Temp-Humid/humidity", humidity);
  Firebase.RTDB.pushString(&fbdo,"/Temp-Humid/timestamp", currentTime);
}
