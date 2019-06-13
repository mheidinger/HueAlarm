#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESPHue.h>

#include "credentials.h"

#define LIGHT_GROUP_NUM 1
#define LIGHT_DELAY 120000

#define UTC_OFFSET_SEC 3600

#define RELAIS_INPUT D1
#define RELAIS_OUTPUT D2

WiFiClient client;
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_SEC);
ESPHue hue(client, HUE_API_KEY, HUE_IP, HUE_PORT);

void setup() {
  pinMode(RELAIS_INPUT, INPUT_PULLUP);
  pinMode(RELAIS_OUTPUT, OUTPUT);
  digitalWrite(RELAIS_OUTPUT, LOW);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("==============================");
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
  Serial.println("==============================");

  timeClient.begin();
}

void loop() {
  if(digitalRead(RELAIS_INPUT) == LOW) {
    Serial.println("RelaisInput is LOW => Relais triggered");
    
    if (isDay()) {
      Serial.println("Detected day alarm, continue loop in 5 seconds");
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

    Serial.print("Turn off light group ");
    Serial.println(LIGHT_GROUP_NUM);
    hue.setGroupPower(LIGHT_GROUP_NUM, hue.OFF);

    Serial.println("==============================");
  }
}

#define NUM(off, mult) ((datetime[(off)] - '0') * (mult))

bool isDay() {
  Serial.println("Start check for day alarm");

  timeClient.update();
  int hours = timeClient.getHours();

  Serial.printf("Current hour is %d\n", hours);
  if (hours > 9 && hours < 17) {
    return true;
  }
  
  return false;
}
