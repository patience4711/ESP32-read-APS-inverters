#include <Arduino.h>

#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include "driver/ledc.h"  // ha mod

#include <WebServer.h>

#include <ESPmDNS.h>

WebServer server(80);

// variables for fading. These values are 0-100 and calculated by set_dim_level or setPower
int8_t current_duty  = 0;
int8_t last_duty = 50; // at boot this needs a value

#define DEFAULT_POWER_MODE   true
#define DEFAULT_DIMMER_LEVEL 50
const char *service_name = "PROV_1234";
const char *pop = "abcd1234";

bool dim_up = true; // direction of dimming when the button is pushed

// fedines of the pins
static int gpio_dimmer = 5;
static int button1 = 7;
static int onboard_button = 9;
bool dimmer_state = false;
#define LED_ON    LOW   
#define LED_OFF   HIGH
#define led_onb           8  // onboard led was 2

// The framework provides some standard device types like switch, lightbulb, fan, temperature sensor.
// But, you can also define custom devices using the 'Device' base class object, as shown here
static Device *my_device = NULL;



#include "PSACrypto.h"


#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

#include "soc/soc.h"           // ha for brownout
#include "soc/rtc_cntl_reg.h"  // ha for brownout
#include <esp_task_wdt.h>      // ha
#include <rtc_wdt.h>

#include "SPIFFS.h"
#include "FS.h"

#include <Preferences.h>  //#include <AsyncTCP.h>
Preferences prefs;

#include <PubSubClient.h>
WiFiClient espClient ;
PubSubClient MQTT_Client ( espClient ) ;

//
//#include "Async_TCP.h" //we include the customized one


//#include "esp_heap_caps.h"
/* this was compiled with board version
this software switches  small amount of white leds
by putting some gpio's in parallel
to source more current.
 */ 
           
// #include "SPIFFS.h"
// #include "FS.h"
// #include <EEPROM.h>
#include <ArduinoJson.h>

#include <PubSubClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
//const byte DNS_PORT = 53;
//DNSServer dnsServer;
#include "HTML.h"
#include "menu_html.h"
#include "home_html.h"
#include "timers_html.h"
//#include "eventSource.H"

#include "driver/ledc.h"
  // defines for the pwm
  #define LEDC_TIMER              LEDC_TIMER_0
  #define LEDC_MODE               LEDC_LOW_SPEED_MODE
  #define LEDC_OUTPUT_IO          (5) // Define the output GPIO
  #define LEDC_CHANNEL            LEDC_CHANNEL_0
  #define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
  #define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
  #define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz
  #define LEDC_MAX_DUTY           ((1 << 13) - 1) // 8191

  //char ssid[33] = ""; // was 33 
  char pass[64] = ""; // was 40
  //bool tryConnectFlag = false;

// variables concerning time and timers
  WiFiUDP Udp; 

// variables concerning clock and timers
  bool resyncFlag;
  //time_t dagbegintijd = 0;
  time_t switchOnTime[4] = {0};
  time_t switchOfTime[4] = {0};
  //time_t switchonMoment = 0;
  int dst;
 
  #define MODE_ABSOLUTE        0
  #define MODE_BEFORE_SUNRISE  1
  #define MODE_AFTER_SUNRISE   2
  #define MODE_BEFORE_SUNSET   3
  #define MODE_AFTER_SUNSET    4
  #define TIMERCOUNT  4

  time_t Sunset = 0;
  time_t Sunrise = 0; 
  int tKeuze = 0;
   bool timeRetrieved = false;
  
  typedef struct {
      bool    Active =false;
      uint8_t on_mode = 0; 
      uint8_t of_mode = 0; 
      uint8_t on_hh = 10; 
      uint8_t on_mm = 10;
      uint8_t of_hh = 10; 
      uint8_t of_mm = 10;
      bool dow[7];
      int     Level = 200;
  } myTimers;
  myTimers timers[4];  

  bool mustSwitch[4];
  bool hasSwitched[4];
  bool mustCalc[4] = {true, true, true, true}; //initial the timners are calculated
  
  uint8_t actionFlag = 0;

  char InputBuffer_Serial[50]; // need to be global

  char requestUrl[20] = {""}; // to remember from which webpage we came  

  struct mySettings 
  {
    char     passwd[11] = "0000";
    float    longi = 5.123;
    float    lati = 51.123;
    char     userPwd[11] = "1111";
    char     gmtOffset[5] = "+120";  //+5.30 GMT
    bool     DTS = true;
    uint8_t  securityLevel = 6; 
    int      default_duty = 50;
    char     dvName[21] = "ESP32-DIMMER";
    char     Mqtt_Broker[30] = "192.168.0.100";
    char     Mqtt_outTopic[26] = "domoticz/in"; // was 26
    char     Mqtt_inTopic[26] = "domoticz/out"; // was 26
    char     Mqtt_Username[26];
    char     Mqtt_Password[26];
    char     Mqtt_Clientid[26];
    int      Mqtt_Port = 1883;
    int      Mqtt_Format = 0;
    int      Mqtt_switchIDX = 0;
    bool     diagNose = false; // initial true but can be set false in settings
  }__attribute__((packed));
  mySettings settings;

  int   event = 0;
  long  mqtt_lastConnect = 0;

  //int dst;
  int iKeuze;
  uint8_t procesId = 1;
  int networksFound = 0; // used in the portal
  int datum = 0; //

  unsigned long previousMillis = 0;        // will store last temp was read
  static unsigned long laatsteMeting = 0; //wordt ook bij OTA gebruikt en bij wifiportal
  static unsigned long lastCheck = 0; //wordt ook bij OTA gebruikt en bij wifiportal

  #define LED_AAN    LOW   //sinc
  #define LED_UIT    HIGH
  #define knop              0  //

  //#define testPin           10
  
  
  // we use this string only to send webpages
  //String toSend = "";
  //int duty = 0;
  //int currentLevel = 0;
  //int backupDuty = 500;
  //int lampState = 0; 


  bool apFlag=false;
  // ******************************************************************************
  // *                                    LOGGING                                 *
  // ******************************************************************************
  #define Log_MaxEvents 18 
  typedef struct {
    char date[14] ;
    int  kind ; // zigbee, system, mqtt, pairing
    char  message[20] ;
  } logEvent;
  logEvent Log_list[Log_MaxEvents];
  bool Log_MaxReached = false;
  byte logNr = 0;

 // WiFiClient espClient ;
 // PubSubClient MQTT_Client ( espClient ) ;
 // int Mqtt_switchIDX = 123;

  //const int resolution = 8;
  const unsigned long HEALTH_INTERVAL = 10UL * 60UL * 1000UL;