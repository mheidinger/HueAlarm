HueAlarm
----------

Simple arduino sketch for a Wemos D1 Mini (ESP8266).
Monitors a pin attached though a relais to a static HIGH. If the pin switches to HIGH, a Hue light will be switched on for two minutes.
Used to turn on a lamp when an alarm comes in from a Swissphone ALGRA Expert pager station.

Requires the following libraries:
- ESP8266 platform
- ESPHue
- ArduinoJson

The sketch needs to be configured with a `credentials.h` file that can be copied from `credentials-dummy.h` and filled with proper values.
