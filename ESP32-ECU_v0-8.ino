#include <AsyncEventSource.h>
#include <AsyncJson.h>
#include <AsyncWebSocket.h>
#include <AsyncWebSynchronization.h>
#include <ESPAsyncWebSrv.h>
#include <SPIFFSEditor.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>

//#include "esp_heap_caps.h"
/* this was compiled with board version
 * latest mods tosend locally declared for info- and detailspages
 *  edit readZigbee
 *  made processIncomingByte redundant
 *  moved crc and slen to sendZigbee
 *  added a debugmessage to sendzigbee when (diag)
 *  removed the line 336 memset crashed the zigbee
 *  testing sending via sendZB 
 *  changed the notation of the invID, saved as was extracted so inverting not needed anymore
 *  experimenting with extractValue() )AAA-DECODE
 *  used arduinojson to make the jsons
 *  made all inverterdata float()
 *  introduced events. // when do we need to refresh
 *  when new data (after a poll, sleep/wakeup midnight)
 *  compressed webpages and scripts combined them to one file
 *  changed the order of handles in the server, the most popular first
 */ 
 
#include <WiFi.h>
#include <DNSServer.h> 

#include "OTA.H"
#include <Update.h>
#include <Hash.h>

#define VERSION  "ESP32-ECU_v0_8"

#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

#include "soc/soc.h" // ha for brownout
#include "soc/rtc_cntl_reg.h" // ha for brownout
#include <esp_task_wdt.h> // ha
#include "soc/rtc_wdt.h"
           
#include "SPIFFS.h"
#include "FS.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
//#include "AsyncJson.h"
#include <Arduino.h>

//#include <AsyncTCP.h>
#include "Async_TCP.h" //we include the customized one

//#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
AsyncEventSource events("/events"); 
AsyncWebSocket ws("/ws");

#include <PubSubClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
const byte DNS_PORT = 53;
DNSServer dnsServer;
#include "HTML.H"
//#include "AAA_CONFIG_INVERTERS.H"
#include "AAA_MENUPAGE.H"
#include "PORTAL_HTML.H"
#include "DETAILSPAGE.H"
#include "eventSource.H"
/*
 * if we define TEST we have a special environment to test the decoding of a string
 * for the different invertertypes. We only test inv0. If we set this up as a DS3
 * a ds3 string will be analyzed.
 * - no automatic polling
 * - no automatic healthcheck
 * - no start of zigbee at boot\
 * - in the console an option to test inv 0 '10;TESTINV'
 * - more debug information in the console
 */

//#define TEST

#ifdef TEST
int testCounter = 0;
#endif

//#define DEBUG
//#ifdef  DEBUG
//#define DebugPrint(x)     Serial.print (x)
//#define DebugPrintDec(x)  Serial.print (x, DEC)
//#define DebugPrintln(x)   Serial.println (x)
//#else
//#define DebugPrint(x)
//#define DebugPrintDec(x)
//#define DebugPrintln(x)
//#endif

//float calliBration = 1.66;
  //bool stoPPed = false;
  bool USB_serial = true;
  char ssid[33] = ""; // was 33 
  char pass[64] = ""; // was 40
  bool tryConnectFlag = false;
// variables concerning time and timers
  WiFiUDP Udp; 
  //time_t daybeginTime = 0;
  time_t switchoffTime = 0;
  time_t switchonTime = 0;
  bool dayTime=true;

//  byte mDay = 0;
//  String  maan = "";
  uint8_t actionFlag = 0;

 // variables wificonfig
  char pswd[11] = "0000";
  char userPwd[11] = "1111";  
  float longi = 5.123;
  float lati = 51.123;
  char gmtOffset[5] = "+120";  //+5.30 GMT
  bool zomerTijd = true;
  char static_ip[16] = "000.000.000.000";
  uint8_t securityLevel = 6;


// ***************************************************************************
//                             variables ECU
// ***************************************************************************
// ZIGBEE_globals
#define CC2530_MAX_SERIAL_BUFFER_SIZE 512
#define YC600_MAX_NUMBER_OF_INVERTERS 9
char inMessage[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
int readCounter = 0;
//char messageHead[5];
int diagNose = 1; // initial true but after a successful healthcheck false
bool Polling = true; // when true we have automatic polling
int errorCode=10;
//int recovered = 0;
  char txBuffer[50];

  int t_saved[YC600_MAX_NUMBER_OF_INVERTERS] = {0};
  float en_saved[YC600_MAX_NUMBER_OF_INVERTERS][4] = {0};
  
  char InputBuffer_Serial[50]; // need to be global

typedef struct{
  char invLocation[13] = "N/A";
  char invSerial[13]   = "000000000000";
  char invID[5]        = "0000";
  int  invType         = 0;
  int  invIdx          = 0;
  bool conPanels[4]    = {true,true,true,true};
} inverters; 
inverters Inv_Prop[9]; 

typedef struct{ 
float freq = 0.0;
float sigQ = 0.0;
float heath = 0.0;
float acv = 0.0;
float dcc[4] = {0.0, 0.0, 0.0, 0.0};              // ampere <100 
float dcv[4] = {0.0, 0.0, 0.0, 0.0};              // volt <100
float power[4] = {0.0, 0.0, 0.0, 0.0};       //watt < 1000
float pw_total = 0.0;
float en_total = 0;
} inverterdata;
inverterdata Inv_Data[9];
  
 
 bool polled[9]={false,false,false,false,false,false,false,false,false};
 uint8_t zigbeeUp = 11; // initial allways initializing, this changes to 1 = up or 0 not up after initial healthcheck
 int pollOffset = 0;
 int inverterKeuze=0;
 int inverterCount=0;
 char ECU_ID[13] = "D8A3011B9780";

char requestUrl[12] = {""}; // to remember from which webpage we came  

// variables mqtt ********************************************
  char  Mqtt_Broker[30]=    {"192.168.0.100"};
//  char  Mqtt_inTopic[16] =  {"ESP-ECU/in"};
  char  Mqtt_outTopic[26] = {"domoticz/in"}; // was 26
  char  Mqtt_Username[26] = {""};
  char  Mqtt_Password[26] = {""};
  char  Mqtt_Clientid[26] = {""};
  char  Mqtt_Port[5] =  {"1883"};
  int   Mqtt_Format = 0; 
int   event = 0;
long  mqtt_lastConnect = 0;

  int dst;
  int iKeuze;
//  int inverterTopoll = 0;
  bool timeRetrieved = false;
  int networksFound = 0; // used in the portal
  int datum = 0; //

  unsigned long previousMillis = 0;        // will store last temp was read
  static unsigned long laatsteMeting = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  static unsigned long lastCheck = 0; //wordt ook bij OTA gebruikt en bij wifiportal

#define LED_AAN    HIGH   //sinc
#define LED_UIT    LOW
#define knop              0  //
#define led_onb           2  // onboard led was 2
#define ZB_RESET          5 // D5
//#define ZB_TX             17 // D8

// we use this string only to send webpages
String toSend = "";
 
int value = 0; 

//int aantal = 0;
int resetCounter=0;
bool apFlag=false;
// *******************************  log *************************************
//// variables To record and display last events on the home page.
//struct logEvent {
//  String    Log_date ;
//  String    Log_kind ;
//  String    Log_message;
//};
// *******************************  log *************************************
// variables To record and display last events on the home page.
#define Log_MaxEvents 18 
 
 typedef struct {
  char date[14] ;
  int  kind ; // zigbee, system, mqtt, pairing
  char  message[13] ;
} logEvent;
logEvent Log_Events[Log_MaxEvents];
bool Log_MaxReached = false;
byte logNr = 0;

WiFiClient espClient ;
PubSubClient MQTT_Client ( espClient ) ;
int Mqtt_stateIDX = 123;
bool getallTrigger = false;
bool reloadTrigger = false;
// *****************************************************************************
// *                              SETUP
// *****************************************************************************
#define RXD2 16
#define TXD2 17
void setup() {
  Serial.begin(115200);

  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println(F("Serial Txd is on pin: 17"));
  Serial.println(F("Serial Rxd is on pin: 16"));

  
  pinMode(knop, INPUT_PULLUP); // de knop
  pinMode(led_onb, OUTPUT); // onboard led
 // pinMode(ZB_TX, OUTPUT);
 // digitalWrite(ZB_TX, LOW); //
  pinMode(ZB_RESET, OUTPUT);// resetpin cc2530   
  digitalWrite(ZB_RESET, HIGH);
  pinMode(4, OUTPUT); // 
  digitalWrite(4, LOW); //
  
  ledblink(1, 800);

   attachInterrupt(digitalPinToInterrupt(knop), isr, FALLING);

  SPIFFS_read();
 // now we know the number of inverters we can find an interval between pollings
 int pollintervall = 300/inverterCount;  
// takes care for the return to the last webpage after reboot
  read_eeprom(); // put the value of requestUrl back
  if (requestUrl[0] != '/' ) strcpy(requestUrl, "/");  // vangnet
  consoleOut("requestUrl = " + String(requestUrl));
  //DebugPrint("apFlag = " + String(apFlag) );
  start_wifi(); // start wifi and server

// reed inverter files
  for (int x=0; x < inverterCount+1; x++) 
  {
  String bestand = "/Inv_Prop" + String(x) + ".str";  
  if( diagNose != 0 ) consoleOut("reading file " + bestand);
      if (!leesStruct(bestand)) 
      { 
        // if the file not exists we break
        if( diagNose != 0 ) consoleOut("file " + bestand + "not exists"); //DebugPrintln(bestand);
        break;
      }
  }
  #ifdef DEBUG
  printInverters(); // show the inverter files
  #endif
  getTijd(); // retrieve time from the timeserver
  Update_Log(1, "boot up");

  // ****************** mqtt init *********************
  MQTT_Client.setKeepAlive(150);
  MQTT_Client.setServer(Mqtt_Broker, atoi(Mqtt_Port));
  MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;

  if ( Mqtt_Format != 0 ) 
  {
       Serial.println(F("setup: mqtt configure"));
       mqttConnect(); // mqtt connect
  } 
  else 
  {
       Update_Log(3, "not enabled"); 
  }
  initWebSocket();
    
  Serial.println(F("booted up, checking coordinator"));
  Serial.println(WiFi.localIP());

  delay(1000);
  ledblink(3,500);

  Update_Log(1,"healthcheck");
  healthCheck(); // check the state of the zigbee system and if oke then poll the inverters
  if(zigbeeUp == 1) {
        Update_Log(2,"running");
          // we poll our inverters immediatly
          if(Polling)
          {
            poll_all();
          }
     }

  resetCounter = 0;
  //events.send( "reload", "message"); //getGeneral and getAll triggered
  eventSend(0);

} // end setup()

//****************************************************************************
//*                         LOOP
//*****************************************************************************
void loop() {



// ***************************************************************************
//                       day or night mode
// ***************************************************************************
#ifdef TEST
// always daytime to be able to test
 dayTime = true;
#endif
 
   if(now() > switchonTime && now() < switchoffTime) 
    {
          if(!dayTime)  
          {
             dayTime = true;
             Update_Log(1, "woke up");
             if( diagNose != 0 ) consoleOut("woke-up");
             // reset the dayly energy at wakeup and send mqtt message
             resetValues(true, true);
             //events.send( "reload", "message"); // refresh the data and state
             eventSend(1);
            }
    } else {
         if(dayTime) 
         {
            dayTime = false;
            //String term= "nightmode";
            Update_Log(1, "nightmode");
            if( diagNose != 0 ) consoleOut("nightmode");
            // clean memory
            //memset( &inMessage, 0, sizeof(inMessage) ); //zero out the 
            //delayMicroseconds(250);
            // we send null messages for each inverter
            resetValues(false, true); // make all values zero exept energy and send mqtt
            //events.send( "reload", "message"); // refresh the data and state
            eventSend(0);
            actionFlag = 250; // triggers the reset values and mqtt null message at midnight
            
         }
    }

// ******************************************************************
//              polling every 300 seconds
// ******************************************************************

  unsigned long nu = millis();  // the time the program is running

   if (nu - laatsteMeting >= 1000UL * 300) // 300 sec
   {
     if(diagNose) Serial.println("300 secs passed, polling" + String(millis()) ); //
        laatsteMeting += 1000UL * 300 ; // increases each time with (300/inverterCount * miliseconds);
        if(dayTime && Polling) // we only poll at day and when Polling = true 
           { 
              ledblink(1,100);
              poll_all(); //if inverterCount = 9 than we have inverters 0-8
           } 
 }

// ******************************************************************
//              healthcheck every 10 minutes
// ******************************************************************

   nu = millis() + 1000UL*120; // 2 minutes later // 
   if (nu - lastCheck >= 1000UL * 600) // =10min
   {
   Serial.println("600 secs passed, healthcheck" + String(millis()) );
         lastCheck += 1000UL * 600;
         //we dont do healtcheck when stopped
         healthCheck(); // checks zb radio, mqtt and time, when false only message if error
   }

  // we recalcultate the switchtimes for this day when there is a new date
  // if retrieve fails, day will not be datum, so we keep trying by healthcheck
  if (day() != datum && hour() > 2) // if date overflew and later then 2
  { 
          getTijd(); // retrieve time and recalculate the switch times
          //delay(500);
          //ESP.restart();
          //DebugPrintln("date overflew, retrieve time");
  }
 
// ***************************************************************************
//                       m q t t
// ***************************************************************************
       // before each transmission the connection is tested
       // so we don't do this in the loop
       if(Mqtt_Format != 0 ) MQTT_Client.loop(); //looks for incoming messages
    
  //*********************************************************************
  //             send null data at midnight 
  // ********************************************************************
  if(hour() == 0 && timeRetrieved && actionFlag == 250)
  {
      if(second() > 0 ) 
      {
        resetValues(true, true); //set all values to zero and sent mqtt
        Update_Log(1, "values wipe");
        actionFlag = 0; // to prevent repetition
        //events.send( "getall", "message"); // refresh the data
        eventSend(2);
       }
  }
  
  test_actionFlag();
  
  if( Serial2.available() ) {
    empty_serial2(); // clear unexpected incoming data
   }

   ws.cleanupClients();
   yield(); // to avoid wdt resets

  // SERIAL: *************** kijk of er data klaar staat op de seriele poort **********************
  if(Serial.available()) {
       handle_Serial();
   }

//  if(getallTrigger) {
//    getallTrigger = false;
//    events.send( "getall", "message", millis());
//  }
//  if(reloadTrigger) {
//    reloadTrigger = false;
//    events.send( "reload", "message", millis());
//   }
}
//****************  End Loop   *****************************

// flash the led
void ledblink(int i, int wacht) {
  for(int x=0; x<i; x++) {
    digitalWrite(led_onb, LED_AAN);
    delay(wacht);
    digitalWrite(led_onb, LED_UIT);
    delay(wacht);
   }
}


void empty_serial() { // remove any remaining data in serial buffer
  while(Serial.available()) {
    Serial.read();
  }
}
void empty_serial2() { // remove any remaining data in serial buffer
  while(Serial2.available()) {
    Serial2.read();
  }
}

// ****************************************************************
//                  eeprom handlers
//*****************************************************************
void write_eeprom() {
    EEPROM.begin(24);
  //struct data
  struct { 
    char str[16] = "";
    int haha = 0;
  } data;

 strcpy( data.str, requestUrl ); 
 data.haha = iKeuze;
    EEPROM.put(0, data);
    EEPROM.commit();
}

void read_eeprom() {
    EEPROM.begin(24);

  struct { 
    char str[16] = "";
    int haha = 0;
  } data;

EEPROM.get(0, data);
if( diagNose != 0 ) consoleOut("read value from EEPROM is " + String(data.str));
strcpy(requestUrl, data.str);
iKeuze = data.haha; // inverterkeuze

EEPROM.commit();
}

// all actions called by the webinterface should run outside the async webserver environment
// otherwise crashes will occure.
    void test_actionFlag() {
  // ******************  reset the nework data and reboot in AP *************************
    if (actionFlag == 11 || value == 11) { // 
     //DebugPrintln("erasing the wifi credentials, value = " + String(value) + "  actionFlag = " + String(actionFlag));
     delay(1000); //reserve time to release the button
     //eraseWifiFlash();
     WiFi.disconnect();
//    WiFi.end();
     // we write a flag in EEPROM
     if( diagNose != 0 ) consoleOut(F("wifi disconnected"));
//we try to overwrite the wifi creentials     
const char* ssid = "dummy";
const char* password = "dummy";
    WiFi.begin(ssid, password);
    Serial.println(F("\nConnecting to dummy network"));
    int teller = 0;
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(F("wipe wifi credentials\n"));
        delay(100);         
        teller ++;
        if (teller > 2) break;
    }
     ESP.restart();
  }  

    if (actionFlag == 10) { // the button was pressed a long time, start ap
     delay(2000); // give the server the time to send the confirm
     if( diagNose != 0 ) consoleOut("rebooting");
     write_eeprom();
     ESP.restart();
  }
// interrrupt button test
if (actionFlag == 15) {
  actionFlag = 0;
  buttonPressed();
}
    
    if (actionFlag == 60) {
      actionFlag = 0; //reset the actionflag
      pairOnActionflag();
    }
    if (actionFlag == 21) {
      actionFlag = 0; //reset the actionflag
      coordinator(true); // start normal operation mode
    }
// mosquitto test **********************************************    
    if (actionFlag == 24) { // set by the mqtt config page
        actionFlag = 0; //reset the actionflag
        MQTT_Client.disconnect();
       //reset the mqttserver
        MQTT_Client.setServer(Mqtt_Broker, atoi(Mqtt_Port) );
        MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
        if (Mqtt_Format != 0) mqttConnect(); // reconnect mqtt after change of settings
    }    
    if (actionFlag == 25) {
      actionFlag = 0; //reset the actionflag
      getTijd(); // recalculate time after change of settings
    }
    if (actionFlag == 43) { //triggered by the console
        actionFlag = 0; //reset the actionflag
        inverterReboot(iKeuze);
    }
    if (actionFlag == 44) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        healthCheck(); 
    }
    if (actionFlag == 45) { //triggered by the webconsole
        actionFlag = 0; //reset the actionflag
        //Serial.println("someone made actionFlag 45 !!"); the uin8_t doesn't allow 301
        testMessage(true); // the bool decides where to find the input
    }
    
    if (actionFlag == 46) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        showDir(); 
    }
    
    // polling a single inverter
    if (actionFlag == 47) { //triggered by the webpage zbtest and mqtt
        actionFlag = 0; //reset the actionflag
        polling(iKeuze);
        //events.send( "getall", "message");
        eventSend(2); 
    }
    //polling all inverters
    if (actionFlag == 48) { //triggered by the webpage zbtest and mqtt
        actionFlag = 0; //reset the actionflag
            ledblink(1,100);
             poll_all(); 
    }

    if (actionFlag == 49) { //triggered by console testmqtt
        actionFlag = 0; //reset the actionflag
        ledblink(1,100);
        // always first drop the existing connection
        MQTT_Client.disconnect();
        ws.textAll("dropped connection");
        delay(100);
        char Mqtt_send[26] = {0};
       
        if(mqttConnect() ) {
        String toMQTT=""; // if we are connected we do this
        
        strcpy( Mqtt_send , Mqtt_outTopic);
        
        if(Mqtt_send[strlen(Mqtt_send -1)] == '/') strcat(Mqtt_send, String(Inv_Prop[0].invIdx).c_str());
        toMQTT = "{\"test\":\"" + String(Mqtt_send) + "\"}";
        
        if(Mqtt_Format == 5) toMQTT = "field1=12.3&field4=44.4&status=MQTTPUBLISH";
        
        if( MQTT_Client.publish (Mqtt_outTopic, toMQTT.c_str() ) ) {
            ws.textAll("sent mqtt message : " + toMQTT);
        } else {
            ws.textAll("sending mqtt message failed : " + toMQTT);    
        }
      } 
     // the not connected message is displayed by mqttConnect()
    }


} // end test actionflag


void poll_all() { 
 // poll all inverters, inbetweeen empty serial and feed wdt 
     for(int i=0; i<inverterCount; i++)
     {     
          if(String(Inv_Prop[i].invID) != "0000") {
                polling(i);
                delay(2000);
                empty_serial2();
                //ESP.wdtFeed();
          }
       }
     //events.send( "getall", "message");
     eventSend(2);
 } 

String getChipId(bool sec) {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  if(sec) return String(chipId); else return "ESP32-ECU-" + String(chipId);
}

void showDir() {
    char temp[50]={0};
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      sprintf(temp, "%s  size %u<br>" , file.name(), file.size() );
      if (diagNose != 0) consoleOut(String(temp));
      delay(100);
      file = root.openNextFile();
    }
  } 

void eventSend(byte what) {
  if (what == 1) {
      events.send( "general", "message"); //getGeneral triggered            
  } else 
     if (what == 2) {
     events.send( "getall", "message"); //getAll triggered
  } else {  
     events.send( "reload", "message"); // both triggered
  }
}
