HueAlarm
----------

Simple arduino sketch for a Wemos D1 Mini (ESP8266).
Monitors a pin attached though a relais to a static HIGH. If the pin switches to HIGH, a Hue light will be switched on for two minutes.
Used to turn on a lamp when an alarm comes in from a Swissphone ALGRA Expert pager station.

Requires the following libraries:
- ESP8266 platform (https://github.com/esp8266/Arduino)
- ESPHue (https://github.com/circuitsforfun/ESP_Hue)
- NTPClient (https://github.com/arduino-libraries/NTPClient)

The sketch needs to be configured with a `credentials.h` file that can be copied from `credentials-dummy.h` and filled with proper values.
For the ESP that I am currently using (WEMOS D1 Mini), select the board `LOLIN(WEMOS) D1 R2 & mini`.

How to find the IP address of the Hue Bridge: https://huetips.com/help/how-to-find-my-bridge-ip-address/
How to get the Hue API key: https://developers.meethue.com/develop/hue-api/7-configuration-api/
