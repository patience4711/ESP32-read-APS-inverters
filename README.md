# read APS inverters
This project is derived from the project The reason to try this on a more powerfull platform is that the ESP8266 was running on the top of its capabilities,
especially when multiple inverters were connected. Due to the massive webpages, the webserver crashed the system. Hopefully the ESP32 has more juice to handle all the requests.
Moreover, the ESP32 has a 2nd UART that is used for the zigbee communication. Now we have the 1st UART available for debugging on the serial monitor via usb.

The system is meant for reading APS Systems inverters. The program can pair and poll YC600 QS1 and DS3 inverters, up to 9 pieces. The read values are displayed on a web page and sent via mosquitto in a Json format.
The program has a lot of smart features. All settings can be done via the webinterface. It has a console that can be used for debugging and sending/receiving zigbee messages.<br>
See the <a href='https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/wiki'>WIKI</a> for information on building it, the working, etc. 



## downloads
march 31 2023: There is a new experimental version [ESP32-ECU_v0_0a](https://github.com/patience4711/ESP32-read-APS-inverters/blob/main/ESP32-ECU_v0-0b.bin) available. This version has output of all inverters together in the frontpage. Look [here]( https://github.com/patience4711/read-APSystems-YC600-QS1-DS3/issues/89) for more info.

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

![minhw](https://user-images.githubusercontent.com/12282915/138685751-98112dfd-8ed8-4185-9de7-c2e2e8f005a9.jpg)
## how does it work
APS works with their own zigbee implementation. The ESP-ECU sends zigbee commands (wireless) to the inverters and analyzes the answers, extracting the values. 
The ESP communicates with the zigbee module through the alternative serial port (wired).
The ESP-ECU starts a coordinator (an entity that can start a zigbee network). The coordinator binds the inverters and sends the poll requests to them.
The interesting values are send via mqtt and displayed on the main page.

![frontpage](https://user-images.githubusercontent.com/12282915/138686152-3b065a7c-88f8-49d6-bf89-c512f82fd562.jpg)
![graph2](https://user-images.githubusercontent.com/12282915/139062602-71e92216-9703-4fc4-acc6-fabf544c4ffd.jpg)

## changelog ##
version ESP-ECU_V9_6c:

