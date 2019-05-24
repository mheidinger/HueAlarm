#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPHue.h>
#include <ArduinoJson.h>

#include "credentials.h"

#define LIGHT_GROUP_NUM 1
#define LIGHT_DELAY 120000

#define RELAIS_INPUT D1
#define RELAIS_OUTPUT D2

WiFiClient client;
HTTPClient httpClient;
DynamicJsonDocument jsonDoc(JSON_OBJECT_SIZE(11) + 300);
ESPHue hue(client, HUE_API_KEY, HUE_IP, HUE_PORT);

void setup() {
  pinMode(RELAIS_INPUT, INPUT);
  pinMode(RELAIS_OUTPUT, OUTPUT);
  digitalWrite(RELAIS_OUTPUT, HIGH);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  //WiFi.config(); Can be used for a static IP
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  httpClient.setReuse(true);
}

void loop() {
  if(digitalRead(RELAIS_INPUT) == HIGH) {
    Serial.println("RelaisInput is HIGH");
    
    if (isTestAlarm()) {
      Serial.println("Detected TestAlarm, continue loop in 5 seconds");
      delay(5000);
      return;
    }

    Serial.print("No TestAlarm: Turn on light group ");
    Serial.println(LIGHT_GROUP_NUM);
    hue.setGroup(LIGHT_GROUP_NUM, hue.ON, 0, 254, 0);

    Serial.println("Start wait");
    unsigned long startMillis = millis();
    while (millis() - startMillis < LIGHT_DELAY) yield();
    Serial.println("Wait finished");

    Serial.print("Turn off light group");
    Serial.println(LIGHT_GROUP_NUM);
    hue.setGroupPower(LIGHT_GROUP_NUM, hue.OFF);
  }
}

#define NUM(off, mult) ((datetime[(off)] - '0') * (mult))

bool isTestAlarm() {
  Serial.println("Start check for test alarm");
  if (httpClient.begin(client, "http://worldtimeapi.org/api/timezone/Europe/Berlin")) {
    int httpCode = httpClient.GET();
    Serial.printf("Response is %d\n", httpCode);
    if (httpCode == 200) {
      String payload = httpClient.getString();
      DeserializationError error = deserializeJson(jsonDoc, payload);
      if (error) {
        Serial.print("Error deserializing payload: ");
        Serial.println(error.c_str());
        httpClient.end();
        return false;
      }
      
      int day_of_week = jsonDoc["day_of_week"];
      const char* datetime = jsonDoc["datetime"];
      Serial.print("Read datetime: ");
      Serial.println(datetime);

      int hour =   NUM(9, 10)   + NUM(10, 1);
      int minute = NUM(11, 10)  + NUM(12, 1);

      Serial.printf("Day of week is %d, hour is %d, minute is %d\n", day_of_week, hour, minute);
      if (day_of_week == 6 && ((hour == 11 && minute >= 55) || (hour == 12 && minute <= 5))) {
        httpClient.end();
        return true;
      } else {
        httpClient.end();
        return false;
      }
    } else if (httpCode < 0) {
      Serial.print("HTTP Client Error: ");
      Serial.println(httpClient.errorToString(httpCode).c_str());
    }
  }
  httpClient.end();
  return false;
}
