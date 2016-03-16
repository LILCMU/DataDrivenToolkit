# A Data-Driven Toolkit


# Firmware Flasher
For Windows 32 bit : [Download](https://github.com/nodemcu/nodemcu-flasher/blob/master/Win32/Release/ESP8266Flasher.exe?raw=true).

For Windows 64 bit : [Download](https://github.com/nodemcu/nodemcu-flasher/blob/master/Win64/Release/ESP8266Flasher.exe?raw=true).

## Change Log

**0.6**

- Updated LED blinking (Wifi connection status).
  - LED on is connecting to wifi.
  - LED blinks quickly (every second) when wifi is connected.
  - LED flashes slowly (every 2 seconds) when wifi is not connected.

**0.5**

- First release.
- Features
  - Receiving the command packet form GoGo Board (http://www.gogoboard.org/)
  - Saving Wifi configuration and a data logging API key.
  - Logging datas into SD Card.
  - Logging datas to https://data.learninginventions.org.
  - NTP client
