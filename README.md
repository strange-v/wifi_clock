# WiFi Clock
One more WiFi clock based on DS1339U and ESP8266. Requires 12V 1.5A power source.

## Functionality
* 24 hours time format
* Automatic time sync (NTP server and interval can be configured)
* Auto brightness
* DST support
* Display options (on/off leading zero, on/off column blinking, do not blink in a specific period of time)
* Over-the-Air update

## Main components
- ESP8266 - microcontroller 
- DS1339U - real time clock
- PCA9685 x 2 - LED controller
- BH1750FVI-TR - light sensor
- MT2492 - step-down  converter
- LM1117IMP-3.3 - voltage regulator
