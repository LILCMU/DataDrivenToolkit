# A Data-Driven Toolkit

## Getting Started

**Firmware Flasher** (NodeMCU Firmware Programmer)
  - For Windows 32 bit : [Download](https://github.com/nodemcu/nodemcu-flasher/blob/master/Win32/Release/ESP8266Flasher.exe?raw=true).
  - For Windows 64 bit : [Download](https://github.com/nodemcu/nodemcu-flasher/blob/master/Win64/Release/ESP8266Flasher.exe?raw=true).

**The latest firmware**
  - Go to [Latest release page](https://github.com/LILCMU/DataDrivenToolkit/releases/latest) then download the file named "*ddt_firmware_v(x).bin*"
  - or [Download](https://github.com/LILCMU/DataDrivenToolkit/releases/download/v0.6/ddt_firmware_v0_6.bin) a firmware.

**Updating firmware**
  1. Open NodeMCU Firmware Programmer (ESP8266Flasher.exe)
      ![Program](https://raw.githubusercontent.com/LILCMU/DataDrivenToolkit/gh-pages/images/Operation.png)
  2. Select COM Port that connected to the board.
  2. Go to "**Config**" Tab.
  3. Enter firmware filepath or select a firmware file ( click the icon <img src="https://raw.githubusercontent.com/LILCMU/DataDrivenToolkit/gh-pages/images/Setting.png" width="28" height="28"> ) at the first line.
      ![Select a firmware](https://raw.githubusercontent.com/LILCMU/DataDrivenToolkit/gh-pages/images/Config.png)
  4. Back to "**Operation**" Tab and then click "**Flash**" button.
      ![Select a firmware](https://raw.githubusercontent.com/LILCMU/DataDrivenToolkit/gh-pages/images/Success.png)

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
