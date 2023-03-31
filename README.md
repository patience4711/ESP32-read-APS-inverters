# ESP32 read APS inverters
This project is derived from the project [ESP-ECU](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3). The reason to try this on a more powerfull platform is that the ESP8266 was running on the top of its capabilities,
especially when multiple inverters were connected. Due to the massive webpages, the webserver crashed the system. Hopefully the ESP32 has more juice to handle all the requests.
Moreover, the ESP32 has a 2nd UART that is used for the zigbee communication. Now we have the 1st UART available for debugging on the serial monitor via usb.

##status##
So far i tested the system with a zigbee module connected. The coordinator is brought up without problems. I tested also the construction and send inf the relative zigbee commands. All worked fine.

The system is meant for reading APS Systems inverters. The program can pair and poll YC600 QS1 and DS3 inverters, up to 9 pieces. The read values are displayed on a web page and sent via mosquitto in a Json format.

Please see the <a href='https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/wiki'>WIKI</a> for information on building it, the working, etc. 

## downloads
march 31 2023: There is a new experimental version [ESP32-ECU_v0_0b](https://github.com/patience4711/ESP32-read-APS-inverters/blob/main/ESP32-ECU_v0-0b.bin) available. Relative to the ESP there is a new feature: in the frontpage we have a link to each inverter that shows us the other details like dc voltage, temperature, etc.
![frontpage](https://user-images.githubusercontent.com/12282915/229179248-28a2a888-a3b0-48db-9f67-9818206739f7.jpg)
![details](https://user-images.githubusercontent.com/12282915/229179300-c010226c-65e7-42ef-8b96-59570bae542f.jpg)

## features
- Simply to connect to your wifi
- Easy add, delete and pair inverters
- automatic polling or on demand via mqtt or http
- data can be requested via http and mosquitto
- There are 4 different mqtt json formats
- Fast asyc webserver
- a very smart on-line console to send commands and debugging
- Smart timekeeping
- A lot of system info on the webpage

## the hardware
It is nothing more than an esp32 device and a prepared cc2530, cc2531 zigbee module. And a powersupply.
The zigbeemodule should be flashed with a firmware that can be found here https://github.com/Koenkk/zigbee2mqtt/issues/4221. Here you can find much more information on this project.
The most recent link to a lua implementation (including firmware) by kadszol :https://github.com/Koenkk/zigbee2mqtt/files/6797510/discord-11-7-2021.zip

For info on how to build and use it, please see the <a href='https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/wiki'>WIKI</a>

## how does it work
APS works with their own zigbee implementation. The ESP-ECU sends zigbee commands (wireless) to the inverters and analyzes the answers, extracting the values. 
The ESP communicates with the zigbee module through the alternative serial port (wired).
The ESP-ECU starts a coordinator (an entity that can start a zigbee network). The coordinator binds the inverters and sends the poll requests to them.
The interesting values are send via mqtt and displayed on the main page.

![frontpage](https://user-images.githubusercontent.com/12282915/138686152-3b065a7c-88f8-49d6-bf89-c512f82fd562.jpg)
![graph2](https://user-images.githubusercontent.com/12282915/139062602-71e92216-9703-4fc4-acc6-fabf544c4ffd.jpg)

## get started ##
The wiring of the zigbeemodule to the esp32:<br>
P02  --->  TX2 (pin 17)<br>
P03  --->  RX2 (pin 16)<br>
rst  --->  pin 14<br>
vcc  --->  3.3V<br>
gnd  --->  gnd<br>
Once you flashed the binary and powered up you'l notice the blue led is on. <br> This means you should connect it to your wifi.
Visit the wiki of the ESP8266 project for more [details](https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/wiki)

## changelog ##
version ESP32-ECU_V0_0b:
Relative to the esp8266 implementation the frontpage is more handy, we have links for each inverter to enter the other details like dc voltage, temperature etc.
