# ESP32 read APS inverters
This project is derived from the project [ESP-ECU](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3). The reason to try this on a more powerfull platform is that the ESP8266 was running on the top of its capabilities,
especially when more than 5 inverters were connected. Comparison in free heap between esp and esp32 show that this is more than 7x higher for the esp32.<br>
The ESP32 has big advantage over the ESP8266, the presence of a second uart. We can use one uart for the zigbee moduele and we have the other UART available for debugging on the serial monitor via usb. This way it is easyer to debug. I can use this project to test things out that could lead to improvements for the ESP8266 project.

See it in action on [YouTube](https://youtu.be/WKFVQ6d8KhQ)

## status ##
The system has been tested in practice with a YC600, QS1 and DS3 inverters and it works fine. 

## purpose ##
The system is intended for reading APS Systems inverters. The program can pair and poll YC600 QS1 and DS3 inverters, up to 9 pieces. The read values are displayed on a web page and sent via mosquitto in a Json format.

Please see the <a href='https://github.com/patience4711/ESP32-read-APS-inverters/wiki'>WIKI</a> for information on building it, the working, etc. 

## downloads
Nov 08 2023: There is a new version [ESP32-ECU_v0_8](https://github.com/patience4711/ESP32-read-APS-inverters/blob/main/ESP32-ECU_v0-8.bin) Please see changelog.

<br><br>
The frontpage:<br>
![frontpage](https://user-images.githubusercontent.com/12282915/229239150-05f6d29d-7620-4363-94fc-787b09d11fad.jpg)
<br><br>The details page:<br>
![details](https://user-images.githubusercontent.com/12282915/229239148-fc0c345e-5291-49b7-a36c-70f452333a61.jpg)

## features
- Simply to connect to your wifi
- Easy add, delete and pair inverters
- automatic polling or on demand via mqtt or http
- data can be requested via http and mosquitto
- There are 5 different mqtt json formats
- Fast asyc webserver
- a serial- and a web console to send commands and debugging
- Smart timekeeping
- A lot of system info on the webpage
- Easy firmware update "Over The Air"

## the hardware
It is nothing more than an esp32 device and a prepared cc2530, cc2531 zigbee module. And a powersupply.
The zigbeemodule should be flashed with a firmware that is developped by kadsol : [CC25xx_firmware](https://github.com/Koenkk/zigbee2mqtt/files/10193677/discord-09-12-2022.zip). The firmware is also available [here](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/blob/main/cc25xx_firmware.zip). Much more info as to the development of this software can be found here https://github.com/Koenkk/zigbee2mqtt/issues/4221.

For info on how to build and use it, please see the [wiki](https://github.com/patience4711/ESP32-read-APS-inverters/wiki)

## how does it work
APS works with their own zigbee implementation. The ESP-ECU sends zigbee commands (wireless) to the inverters and analyzes the answers, extracting the values. 
The ESP communicates with the zigbee module through the alternative serial port (wired).
The ESP-ECU starts a coordinator (an entity that can start a zigbee network). The coordinator binds the inverters and sends the poll requests to them.
The interesting values are send via mqtt and displayed on the main page. The ecu sends a message that there is new data and the webpage reacts by requesting
the new data.
<br><br> example of a sensor in Domoticz:<br>
![graph2](https://user-images.githubusercontent.com/12282915/139062602-71e92216-9703-4fc4-acc6-fabf544c4ffd.jpg)

## changelog ##
version ESP32-ECU_V0_7:
* fixed a bug related to the working of the button
  
version ESP32-ECU_V0_7:
* more efficient communication browser/server (events driven)
* minimized all webpages and javascripts
* improved menu and browsing on the ecu website
  
version ESP32-ECU_V0_5:
* more efficient use of the memory
* Use of arduinoJson
  
version ESP32-ECU_V0_4:
* Banned all string operations in main processes
* Some webpages improved.

version ESP32-ECU_V0_3b:
* some security updates (maintenance from outside the own network)
* fine-tuned the pairing process
* redesigned the important processes to gain more free heap.
* some cosmetics and small bugs

version ESP32-ECU_V0_1d:
* replaced elegantOta with my own implentation
* fixed a bug in the pairing proces
* solved system crashed due to string operations
* fixed a bug in the pairing proces

version ESP32-ECU_V0_1a:
Relative to the esp8266:
* new frontpage with buttons to inverter details 
* removed the websocket console to relieve the webserver
* added a serial console to issue commands
