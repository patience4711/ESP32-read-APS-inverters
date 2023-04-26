/* this was compiled with board version
 * latest mods tosend locally declared for info- and detailspages
 */
#include <WiFi.h>
#include <DNSServer.h> 

#include "OTA.H"
#include <Update.h>
#include <Hash.h>

#define VERSION  "ESP32-ECU_v0_2"

#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

#include "soc/soc.h" // ha for brownout
#include "soc/rtc_cntl_reg.h" // ha for brownout
#include <esp_task_wdt.h> // ha
#include "soc/rtc_wdt.h"

//#include <WebServer.h>
           
#include "SPIFFS.h"
#include "FS.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <Arduino.h>

//#include <AsyncTCP.h>
#include "Async_TCP.h" //we include the customized one

#include <ESPAsyncWebServer.h>
AsyncWebServer server(80); //

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

#define DEBUG
#ifdef  DEBUG
#define DebugPrint(x)     Serial.print (x)
#define DebugPrintDec(x)  Serial.print (x, DEC)
#define DebugPrintln(x)   Serial.println (x)
#else
#define DebugPrint(x)
#define DebugPrintDec(x)
#define DebugPrintln(x)
#endif

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
  char timezone[5] = "+120";  //+5.30 GMT
  bool zomerTijd = true;
  char static_ip[16] = "000.000.000.000";


// ***************************************************************************
//                             variables ECU
// ***************************************************************************
// ZIGBEE_globals
#define CC2530_MAX_SERIAL_BUFFER_SIZE 512
#define YC600_MAX_NUMBER_OF_INVERTERS 9
char inMessage[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
int readCounter = 0;
//char messageHead[5];
bool diagNose=false;
bool Polling = true; // when true we have automatic polling
int errorCode=10;
//int recovered = 0;

  int t_saved[YC600_MAX_NUMBER_OF_INVERTERS] = {0};
  float en_saved[YC600_MAX_NUMBER_OF_INVERTERS][4] = {0};
  
  char InputBuffer_Serial[50];

typedef struct{
  char invLocation[13] = "N/A";
  char invSerial[13]   = "000000000000";
  char invID[7]        = "0x0000";
  int  invType         = 0;
  int  invIdx          = 0;
  bool conPanels[4]    = {true,true,true,true};
} inverters; 
inverters Inv_Prop[9]; 

typedef struct{ 
char freq[5] = "00.0";
char sigQ[5] = "00.0";
char heath[6] = "000.0";
char acv[6] = "000.0";
char dcc[4][5] = {"00.0", "00.0", "00.0", "00.0"};              // ampere <100 
char dcv[4][5] = {"00.0", "00.0", "00.0", "00.0"};              // volt <100
char power[5][6]={"000.0", "000.0", "000.0", "000.0", "000.0"}; //watt < 1000
float en_total = 0;
} inverterdata;
inverterdata Inv_Data[9];
  
 
 bool polled[9]={false,false,false,false,false,false,false,false,false};
 uint8_t zigbeeUp = 11; // initial allways initializing, this changes to 1 = up or 0 not up after initial healthcheck
 int pollOffset = 0;
 int inverterKeuze=0;
 int inverterCount=0;
 char ECU_ID[13] = "D8A3011B9780";

 String requestUrl = ""; // to remember from which webpage we came  

// variables mqtt ********************************************
String  Mqtt_Broker=    "192.168.0.100";
String  Mqtt_inTopic =  "ECU/in";
String  Mqtt_outTopic = "domoticz/in";
String  Mqtt_Username;
String  Mqtt_Password;
String  Mqtt_Port =     "1883";
int     Mqtt_Format = 0;  
int event = 0;
long mqtt_lastConnect = 0;

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
// variables To record and display last events on the home page.
struct logEvent {
  String    Log_date ;
  String    Log_kind ;
  String    Log_message;
};
//bool Log = false;
static const int Log_MaxEvents = 20;    // Please avoid to make a too big value here, the past events will be stored in memory and displayed in the html home
bool Log_MaxReached = false;
byte Log_CurrentEvent = 0;
logEvent Log_EventList[Log_MaxEvents];  // To stock in memory the last x events (20 by default)

WiFiClient My_WifiClient ;
PubSubClient MQTT_Client ( My_WifiClient ) ;
int domIdx = 123;
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
  if (requestUrl.indexOf("/") == -1) requestUrl="/"; // vangnet
  DebugPrint("requestUrl = "); DebugPrintln(requestUrl);
  //DebugPrint("apFlag = " + String(apFlag) );
  start_wifi(); // start wifi and server

  getTijd(); // retrieve time from the timeserver
 
// reed inverter files
  for (int x=0; x < inverterCount+1; x++) 
  {
  String bestand = "/Inv_Prop" + String(x) + ".str";  
  DebugPrint("reading file "); DebugPrintln(bestand);
      if (!leesStruct(bestand)) 
      { 
        // if the file not exists we break
        DebugPrint("file " + bestand + "not exists"); //DebugPrintln(bestand);
        break;
      }
  }
  #ifdef DEBUG
  printInverters(); // show the inverter files
  #endif

  Update_Log("system", "boot up");
//  // ****************** mqtt init *********************
       MQTT_Client.setServer(Mqtt_Broker.c_str(), Mqtt_Port.toInt());
       MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
       //MQTT_Client.setKeepAlive( MQTT_KEEPALIVE );

  if ( Mqtt_Format != 0 ) 
  {
       Serial.println(F("setup: mqtt configure"));
       mqttConnect(); // mqtt connect
  } 
  else 
  {
       Update_Log("mqtt", "not enabled"); 
  }
//  initWebSocket();
    
  DebugPrintln("ATTENTION DEBUG DEFINED");
  Serial.println(F("booted up, checking coordinator"));
  Serial.println(WiFi.localIP());

  delay(1000);
  ledblink(3,500);
  #ifndef TEST
  Update_Log("zigbee","initial healthcheck");
          healthCheck(); // check the state of the zigbee system and if oke then poll the inverters
  if(zigbeeUp == 1) {
        Update_Log("zigbee","coordinator up");
          // we poll our inverters immediatly
          if(Polling)
          {
            poll_all();
          }
     }
  #endif
  resetCounter = 0;

//testPoll(0);
//  stoPPed = false;
//testPair(0);

#ifdef TEST
t_saved[0] = 36587;
zigbeeUp = 1;
force_values(); // now we have values for all inverters
#endif 

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
 
#ifndef TEST
   if(now() > switchonTime && now() < switchoffTime) 
    {
          if(!dayTime)  
          {
             dayTime = true;
             String term= "wake-up from nightmode";
             Update_Log("system", term);
             DebugPrintln(term);
             // reset the dayly energy at wakeup and send mqtt message
             resetValues(true, true);
            }
    } else {
         if(dayTime) 
         {
            dayTime = false;
            String term= "start nightmode";
            Update_Log("system", term);
            DebugPrintln(term);
            // clean memory
            memset( &inMessage, 0, sizeof(inMessage) ); //zero out the 
            delayMicroseconds(250);
            // we send null messages for each inverter
            resetValues(false, true); // make all values zero exept energy and send mqtt
            actionFlag = 301; // triggers the reset values and mqtt null message at midnight
         }
    }

// ******************************************************************
//              polling every 300 seconds
// ******************************************************************

  unsigned long nu = millis();  // the time the program is running
//   if (nu - laatsteMeting >= 1000UL * (300 / inverterCount)) 
   if (nu - laatsteMeting >= 1000UL * 300) 
   {
     DebugPrintln("300 secs passed, polling"); //
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

   nu = millis() + 1000ul*120; // 2 minutes later // must be one hour + 2 minutes = 
   if (nu - lastCheck >= 1000UL * 600) // =10min
   {
         lastCheck += 1000UL * 600;
         //we dont do healtcheck when stopped
         healthCheck(); // checks zb radio, mqtt and time, when false only message if error
   }
//// ******************************************************************
////              healthcheck every 60 minutes
//// ******************************************************************
//   nu = millis() + 1000ul*3720; // 2 minutes later // must be one hour + 2 minutes = 
//   if (nu - lastCheck >= 1000UL * 3600) // was 600=10min*6=3600
//   {
//         lastCheck += 1000UL * 3600;
//         //we dont do healtcheck when stopped
//         healthCheck(); // checks zb radio, mqtt and time, when false only message if error
//   }

#endif

  // we recalcultate the switchtimes for this day when there is a new date
  // if retrieve fails, day will not be datum, so we keep trying
  if (day() != datum && hour() > 2) // if date overflew and later then 2
  { 
          getTijd(); // retrieve time and recalculate the switch times
          //DebugPrintln("date overflew, retrieve time");
  }
 
  // check mqtt is connected when it should
  // once in 5 sec's this way it can't block the loop
  if(Mqtt_Format != 0) 
  {
      if (!MQTT_Client.connected()) {
        long now = millis();
        if (now - mqtt_lastConnect > 5000) {
          mqtt_lastConnect = now;
          // Attempt to connect
          if (mqttConnect()) {
             mqtt_lastConnect = 0;
          }
        }
      } else {
        // Client connected
        MQTT_Client.loop();
      }
  }
   
  //*********************************************************************
  //             send null data at midnight 
  // ********************************************************************
  if(hour() == 0 && timeRetrieved && actionFlag == 301)
  {
      if(second() > 0 ) 
      {
        resetValues(true, true); //set all values to zero and sent mqtt
        Update_Log("system", "all values reset");
        actionFlag = 0; // to prevent repetition
        //ZBhardReset(); // reset the zigbeemodule
        //healthCheck(); // this increases the resetcounter so we decrease it
        //resetCounter--; // so that this reset is not counted
       }
  }
  
  test_actionFlag();
  
  if( Serial2.available() ) {
    empty_serial2(); // clear unexpected incoming data
   }

//   ws.cleanupClients();
   yield(); // to avoid wdt resets

  // SERIAL: *************** kijk of er data klaar staat op de seriele poort **********************
  if(Serial.available()) {
       handle_Serial();
   }

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
    String ntpPool = "";
    bool AP = false;
  } data;

 strcpy( data.str, requestUrl.c_str() ); 
 data.haha = iKeuze;
 //data.AP = apFlag;
    EEPROM.put(0, data);
    EEPROM.commit();
}

void read_eeprom() {
    EEPROM.begin(24);

  struct { 
    char str[16] = "";
    int haha = 0;
    String ntpPool = "";
   // bool AP = false;
  } data;

EEPROM.get(0, data);
DebugPrintln("read value from EEPROM is " + String(data.str));
requestUrl=String(data.str);
iKeuze = data.haha; // inverterkeuze
//apFlag = data.AP;
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
     DebugPrintln(F("wifi disconnected"));
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
     DebugPrintln("rebooting");
     write_eeprom();
     ESP.restart();
  }
    if (actionFlag == 60) {
      actionFlag = 0; //reset the actionflag
      pairOnActionflag();
    }
//    if (actionFlag == 21) {
//      actionFlag = 0; //reset the actionflag
//      coordinator(true); // start normal operation mode
//    }
//    if (actionFlag == 22) {
//      actionFlag = 0; //reset the actionflag
//      coordinator(false); // start pairing mode
//    }
//#ifdef TEST
//    if (actionFlag == 122) {
//      actionFlag = 0; //reset the actionflag
//      if(testCounter > 1) { 
//        testCounter=0; // reset testcounter
//        // make all values null
//        resetValues(true, false);
//      }
//      testDecode(); // test a fake pollinganswer for the invType
//    }
//    if (actionFlag == 123) {
//      actionFlag = 0; //reset the actionflag
//      testDecode(); // test a fake pollinganswer for qs
//    }    
//#endif
    
    if (actionFlag == 24) { // set by the mqtt config page
        actionFlag = 0; //reset the actionflag
        MQTT_Client.disconnect();
       //reset the mqttserver
        MQTT_Client.setServer(Mqtt_Broker.c_str(), Mqtt_Port.toInt());
        MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
        if (Mqtt_Format != 0) mqttConnect(); // reconnect mqtt after change of settings
    }    
    if (actionFlag == 25) {
      actionFlag = 0; //reset the actionflag
      getTijd(); // recalculate time after change of settings
    }
//    if (actionFlag == 44) { //triggered by the webpage zbtest
//        actionFlag = 0; //reset the actionflag
//        healthCheck(); 
//    }
//    if (actionFlag == 45) { //triggered by the webpage zbtest
//        actionFlag = 0; //reset the actionflag
//        testMessage();
//    }
    // polling a single inverter
//    if (actionFlag == 47) { //triggered by the webpage zbtest and mqtt
//        actionFlag = 0; //reset the actionflag
//        polling(iKeuze); 
//    }
    //polling all inverters
//    if (actionFlag == 48) { //triggered by the webpage zbtest and mqtt
//        actionFlag = 0; //reset the actionflag
//            ledblink(1,100);
//            for(int i=0; i<inverterCount; i++)
//            {     
//              if(String(Inv_Prop[i].invID) != "0x0000") polling(i);
//            } 
//    }

}

 void poll_all() { 
 // poll all inverters, inbetweeen empty serial and feed wdt 
     for(int i=0; i<inverterCount; i++)
     {     
          if(String(Inv_Prop[i].invID) != "0x0000") {
                polling(i);
                delay(2000);
                empty_serial2();
                //ESP.wdtFeed();
          }
       }
 } 

 String getChipId() {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return "ESP32-ECU-" + String(chipId);
}          
