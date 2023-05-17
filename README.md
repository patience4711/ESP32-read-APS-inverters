# ESP32 read APS inverters
This project is derived from the project [ESP-ECU](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3). The reason to try this on a more powerfull platform is that the ESP8266 was running on the top of its capabilities,
especially when multiple inverters were connected. Due to the massive webpages, the webserver crashed the system. Hopefully the ESP32 has more juice to handle all the requests. Comparison in free heap between esp and esp32 show that this is more than 7x higher for the esp32.<br>
Moreover, the ESP32 has a 2nd UART that is used for the zigbee communication. Now we have the 1st UART available for debugging on the serial monitor via usb.

See it in action on [YouTube](https://youtu.be/WKFVQ6d8KhQ)

## status ##
I tested the system with a zigbee module connected. The coordinator is brought up without problems. Pairing an inverter worked at one shot. The data of my inverter are nicely displayed and mqtt-ed. So it is working fine.

## purpose ##
The system is meant for reading APS Systems inverters. The program can pair and poll YC600 QS1 and DS3 inverters, up to 9 pieces. The read values are displayed on a web page and sent via mosquitto in a Json format.

Please see the <a href='https://github.com/patience4711/ESP32-read-APS-inverters/wiki'>WIKI</a> for information on building it, the working, etc. 

## downloads
may 11 2023: There is a new version [ESP32-ECU_v0_3b](https://github.com/patience4711/ESP32-read-APS-inverters/blob/main/ESP32_ECU_v0_3b.zip) available. Please see changelog.<br> Important: If you want to upgrade from an older verson: Please read [here](https://github.com/patience4711/ESP32-read-APS-inverters/issues/4#issuecomment-1539977508) first.
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
- There are 4 different mqtt json formats
- Fast asyc webserver
- a serial- and a web console to send commands and debugging
- Smart timekeeping
- A lot of system info on the webpage

## the hardware
It is nothing more than an esp32 device and a prepared cc2530, cc2531 zigbee module. And a powersupply.
The zigbeemodule should be flashed with a firmware that is developped by kadsol : [CC25xx_firmware](https://github.com/Koenkk/zigbee2mqtt/files/10193677/discord-09-12-2022.zip). The firmware is also available [here](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/blob/main/cc25xx_firmware.zip). Much more info as to the development of this software can be found here https://github.com/Koenkk/zigbee2mqtt/issues/4221.

For info on how to build and use it, please see the <a href='https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/wiki'>WIKI</a>

## how does it work
APS works with their own zigbee implementation. The ESP-ECU sends zigbee commands (wireless) to the inverters and analyzes the answers, extracting the values. 
The ESP communicates with the zigbee module through the alternative serial port (wired).
The ESP-ECU starts a coordinator (an entity that can start a zigbee network). The coordinator binds the inverters and sends the poll requests to them.
The interesting values are send via mqtt and displayed on the main page.
<br><br> example of a sensor in Domoticz:<br>
![graph2](https://user-images.githubusercontent.com/12282915/139062602-71e92216-9703-4fc4-acc6-fabf544c4ffd.jpg)

## get started ##
The wiring of the zigbeemodule to the esp32:<br>
P02  --->  TX2 (pin 17)<br>
P03  --->  RX2 (pin 16)<br>
rst  --->  pin D5<br>
vcc  --->  3.3V<br>
gnd  --->  gnd<br>
Once you flashed the binary and powered up you'l notice the blue led is on. <br> This means you should connect it to your wifi.
Visit the wiki of the ESP8266 project for more [details](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/wiki)

## firmware update ##
This works slightly different. When the fil is uploaded you have to refert back to the menupage and reboot. (or press the reset button or power cycle)

## changelog ##
version ESP32-ECU_V0_3b:
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
