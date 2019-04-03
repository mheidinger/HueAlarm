#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPHue.h>
#include <ArduinoJson.h>

#include "credentials.h"

#define LIGHT_NUM 1
#define LIGHT_DELAY 120000
#define TIMEZONE 1
#define SUMMERTIME true

int relaisInput = 12;
int relaisOutput = 13;

WiFiClient client;
HTTPClient httpClient;
DynamicJsonDocument jsonDoc(JSON_OBJECT_SIZE(11) + 300);
ESPHue hue(client, HUE_API_KEY, HUE_IP, HUE_PORT);

void setup() {
  pinMode(relaisInput, INPUT);
  pinMode(relaisOutput, OUTPUT);
  digitalWrite(relaisOutput, HIGH);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  //WiFi.config();
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if(digitalRead(relaisInput) == HIGH) {
    if (isTestAlarm()) {
      delay(5000);
      return;
    }
    
    hue.setLight(LIGHT_NUM, hue.ON, 0, 100, 0);

    unsigned long startMillis = millis();
    while (millis() - startMillis < LIGHT_DELAY);

    hue.setLightPower(LIGHT_NUM, hue.OFF);
  }
}

#define NUM(off, mult) ((datetime[(off)] - '0') * (mult))

bool isTestAlarm() {
  if (httpClient.begin(client, "http://worldtimeapi.org/api/timezone/Europe/Berlin")) {
    int httpCode = httpClient.GET();
    if (httpCode > 0) {
      String payload = httpClient.getString();
      DeserializationError error = deserializeJson(jsonDoc, payload);
      if (error) {
        httpClient.end();
        return false;
      }
      
      int day_of_week = jsonDoc["day_of_week"];
      const char* datetime = jsonDoc["datetime"];

      int hour =   NUM(9, 10)   + NUM(10, 1);
      int minute = NUM(11, 10)  + NUM(12, 1);

      if (day_of_week == 6 && ((hour == 11 && minute >= 55) || (hour == 12 && minute <= 5))) {
        httpClient.end();
        return true;
      } else {
        httpClient.end();
        return false;
      }
    }
  }
  httpClient.end();
  return false;
}
