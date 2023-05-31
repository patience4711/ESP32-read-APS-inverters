class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    handlePortalRoot();
    request->send(200, "text/html", toSend); //send the html code to the client
  }
};

void start_portal() {
// setup of configportal and next an infinitive loop
 
WiFi.mode(WIFI_OFF); // otherwise the scanning fails
delay(5000);

  Serial.println("scan start");
  scanWifi();
  Serial.println("result scan networksFound = " + String(networksFound));

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
dnsServer.start(DNS_PORT, "*", apIP);

  //String chipId = getChipId(); 

  //String temp = String(chipId);
  Serial.println("\ncould not connect with the last known wificredentials,  starting access point...");
  WiFi.softAPConfig(apIP, apIP, netMsk);
  const char* apNaam = getChipId(false).c_str();
  const char* apPasswd = "123456789";
  WiFi.softAP(apNaam, apPasswd);

  delay(500); // without delay tje IP address can be blanco
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */

  server.on("/redirect", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("/redirect requested");
  handlePortalRoot();    
  request->send(200, "text/html", toSend); //send the html code to the client
  });

  server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("/redirect requested");
  handlePortalRoot();    
  request->send(200, "text/html", toSend); //send the html code to the client
  });

  server.on("/Bback", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("/Bback requested");    
  handlePortalRoot();
  //sendHeaders();
  request->send(200, "text/html", toSend); //send the html code to the client
  });

  server.on("/Bback", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("/Bback requested");    
  handlePortalRoot();
  //sendHeaders();
  request->send(200, "text/html", toSend); //send the html code to the client
  });
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("/ requested");    
  handlePortalRoot();
  //sendHeaders();
  request->send(200, "text/html", toSend); //send the html code to the client
  });
 
  server.on("/wifiForm", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("/wifiForm requested");    
  handleForm();
  //sendHeaders();
  request->send(200, "text/html", toSend); //send the html code to the client
  });

  server.on("/PORTAL_STYLE", HTTP_GET, [](AsyncWebServerRequest *request) {
  //Serial.println("stylesheet requested");
    request->send_P(200, "text/css", PORTAL_STYLESHEET);
  });
// **********************************************************
//                   CONNECTING TO WIFI
// **********************************************************
  server.on("/wifiCon", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("/wifiCon requested");
  laatsteMeting = millis(); //om de timeout te verversen
  char ssid[33] = "";
  char pass[40] = "";

// process the data and try to connect
  strcpy( ssid, request->getParam("s")->value().c_str() );
  strcpy( pass, request->getParam("p")->value().c_str() );  
  strcpy( pswd, request->getParam("pw")->value().c_str() );
  securityLevel = request->arg("sl").toInt();  

    wifiConfigsave(); // save the admin passwd

// trying to connect now
// therefor we first go in wifi APSTA
  WiFi.mode(WIFI_AP_STA);
  Serial.println("Connecting to " + String(ssid));
  WiFi.begin(ssid, pass);  
  Serial.println("send confirm page  ");

if (connectWifi() == WL_CONNECTED) {
  Serial.println("youpy, connected");
  esp_task_wdt_reset();
    digitalWrite(led_onb, LED_UIT);
  event=101;
   
    } else {
  Serial.println("could not connect, try again");
  esp_task_wdt_reset();
    ledblink(10, 200);
    digitalWrite(led_onb, LED_AAN); // 
    event=100;
    }
//toSend = FPSTR(PORTAL_HEAD);
toSend = FPSTR(PORTAL_CONFIRM); 
if(event==100) {
toSend.replace("{text}", "connection has failed");
} else {
toSend.replace("{text}", "connection success");
}    
request->send(200, "text/html", toSend); //send the html code to the client
});


//  server.onNotFound(handlePortalNotFound);
  server.onNotFound([](AsyncWebServerRequest *request) {
  String message="file not found";
  AsyncWebServerResponse *response = request->beginResponse(404,"text/plain",message);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");
  request->send (response );
//  request->send(200, "text/html", toSend); //send the html code to the client 
   });

//  server.on("/LittleFS_ERASE", eraseFiles);
//  server.on("/STATIC_ERASE", resetStatic);   
  server.on("/STATIC_ERASE", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("static erase requested");
  String toSend = F("<!DOCTYPE html><html><head>");
  toSend += F("<script type='text/javascript'>setTimeout(function(){ window.location.href='/redirect'; }, 500 ); </script>");

  toSend += F("</head><body><h2>erase ip settings, please wait... !</h2></body></html>");

  static_ip[0] = '\0';
  static_ip[1] = '\0';
  wifiConfigsave();
  request->send(200, "text/html", toSend); //send the html code to the client 
   });
  
  server.on("/close", HTTP_GET, [](AsyncWebServerRequest *request) {
  handlePortalClose();
  request->send(200, "text/html", toSend); //send the html code to the client 
   }); 

  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP

  server.begin(); // Web server start
  Serial.println("started async HTTP server for the portal");
  digitalWrite(led_onb, LED_AAN);  // led aan

  // now we enter an infinitive loop that we leave only after 5 minutes, 
  // or when via server.handleClient something happens   
  //  bool Timed_Out = true;
  Serial.println("entering the infinitive loop with heartbeat");
  laatsteMeting = millis(); //voor de time-out

  // this is the infinitive  loop 
  static unsigned long heartbeat = 0;
  while (millis() < laatsteMeting + 300*1000UL) { // 5 minuten== 300 30 == 30sec
  if ( millis() > heartbeat + 10*1000UL ) {
    heartbeat = millis(); // elke 10 sec een heartbeat
    Serial.print("a ");
  }
//  // SERIAL: *************** see if there is data available **********************
//  if(Serial.available()) {
//       handle_Serial();
//   }
  
  if (tryConnectFlag) { // there are credentials provided
      wifiConnect(); // if connected we break out of this loop
  }
      //DNS
    //dnsServer.processNextRequest();
  } 
  // ************************ end while loop ******************************* 

  //we only are here after a timeout. If we click finish we restart      
//  if (Timed_Out == true) {
    Serial.println("portal timed out, resetting...");
    ESP.restart();
//    } 
}

// ********************************************************************
//                 de homepagina van het portal
// ********************************************************************
void handlePortalRoot() {
//  always as we are here, portalstart is updated, so when there is activity in the
// webinterface we won't time out. 
//sendHeaders();
laatsteMeting = millis(); // update portalstart
Serial.println("handlePortalRoot, event = " + String(event));
//toSend = FPSTR(PORTAL_HEAD);
toSend = FPSTR(PORTAL_PAGE);
//toSend.replace("{haha}" , "if (window.location.hostname != \"192.168.4.1\") {window.location.href = 'http://192.168.4.1'};");
toSend.replace("{ma}", String(WiFi.macAddress()) );

      // if event = 101 we adapt the page with new data
      if (event == 101) {  // the 2e time that we are here we are connected 101 or not 100

          toSend.replace("hadden", "hidden"); // verberg de configuratieknop
          toSend.replace("close' hidden", "close'"); // show the close button
          String page = "";
          page += F("<div class=\"msg\">");
          page += F("connected to network <strong> unset<a>");
          page.replace("unset", WiFi.SSID());
          page += F("</strong><br><br>");
          page += F("The IP address is <strong><a href=\"http://");
          page += WiFi.localIP().toString();
          page += F("\">");
          page += WiFi.localIP().toString();
          page += F("</a></strong><br><br>");
          page += F("<h3 style='color:#FF0000';>Note down the ip-address and click \"close\"!</h3>");
      toSend.replace("<strong>Currently not connected to a wifi network!</strong>" , page);
      // toon de afsluitknop
      toSend.replace("close' hidden", "close'");
      }
      if (event==100) {  // niet verbonden
          String page = "";
          page+=F("Your attempt to connect to the network has failed!<br><br>");
          page+=F("Try again, click on wifi configuration!"); 
          toSend.replace("Currently not connected to a  wifi network!" , page);
      }
      // hadden hudden hodden hedden
      // verberg de wis knop als gewist en wanneer verbonden
      if ( SPIFFS.exists("/basisconfig.json") && event!=101 ) { //show if file is present
       toSend.replace("erase' hidden", "erase'");
      }  
       //deze laten we zien als niet 101 en wel static ip
      if( static_ip[0] != '\0' && static_ip[0] != '0' && event!=101) { //show if static is present
       toSend.replace("static' hidden", "static'"); 
      }
//        sendHeaders();
//        request->send(200, "text/html", toSend); //send the html code to the client
//        delay(500);//wait half a second after sending the data 
//        event = 0; //voor de volgende ronde 
}


// *********************************************************************
//                          pagina voor invoeren credentials
// *********************************************************************
void handleForm() {
   laatsteMeting = millis();
// als we de eerste keer hierkomen beginnen we met een netwerkscan

//toSend = FPSTR(PORTAL_HEAD);
toSend = FPSTR(WIFI_PAGE);
toSend.replace("{pw}",  String(pswd) );
toSend.replace("{sl}",  String(securityLevel) );
if (networksFound == 0) {
      toSend.replace("aplijst" ,"WiFi scan not found networks. Restart configuration portal to scan again.");
    } else {
      Serial.println("make a list of found networks");
      String lijst = makeList(networksFound);
      Serial.println("lijst = "); Serial.println(lijst);
      toSend.replace("aplijst", lijst);  
    }

//  sendHeaders();
//  request->send(200, "text/html", toSend); //send the html code to the client
//  delay(500); //wait half a second after sending the data
 //deze pagina heeft een form met action="wifiCon", verwijst naar wifiConnect
}

//**********************************************************************
//      process the provided data and try to connect
// **********************************************************************
void wifiConnect() {
  // we are here because bool tryConnectFlag was true in the loop
      digitalWrite(led_onb, LED_UIT);
       tryConnectFlag=false;
       laatsteMeting = millis();

      Serial.println("we are in wifiConnect");

      WiFi.mode(WIFI_AP_STA);
      delay(500);
     
      Serial.println("Connecting to " + String(ssid));
      Serial.println("password =  " + String(pass));

      if (connectWifi() == 1) {
        Serial.println("youpy, connected");
         ledblink(3, 500);
         event=101;
       } else {
         Serial.println("could not connect, try again");
         digitalWrite(led_onb, LED_AAN); // 
         event=100;
        } 
      // must jump out of the loop
 }

void handlePortalNotFound(AsyncWebServerRequest *request) {
  String message="file not found";
  AsyncWebServerResponse *response = request->beginResponse(404,"text/plain",message);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");
  request->send (response );  
}

// ************************************************************************
//          try to connect with new credentials
// ************************************************************************

//int connectWifi(String ssid, String pass) {
int connectWifi() {  
  Serial.println("try connect with the new credentials");
  Serial.println(ssid);
  Serial.println(pass);
  
  WiFi.begin(ssid, pass);// Start Wifi with new values.
  
  int connRes = WiFi.waitForConnectResult();
  Serial.print("Connection result is : " + connRes);
  Serial.println(WiFi.localIP());
  checkFixed(); // set static ip if configured

  return connRes;
}
// *********************************************************************
//                         Closing the  portal
// *********************************************************************
void handlePortalClose() {
   laatsteMeting = millis();
  String toSend = F("<!DOCTYPE html><html><head>");
  toSend += F("<script type='text/javascript'>setTimeout(function(){ window.location.href='/'; }, 1000 ); </script>");
  toSend += F("</head><body><h2>The esp is going to restart.!</h2>Do not forget to note the ip address!!<br>");
  toSend += F("<h2>The ip address is : ");
  toSend += WiFi.localIP().toString();
  toSend += F("</h2></body></html>");

  ESP.restart();
}

void eraseFiles() {
  Serial.println("LittleFS wipe and format");
  String toSend = F("<!DOCTYPE html><html><head>");
  toSend += F("<script type='text/javascript'>setTimeout(function(){ window.location.href='/redirect'; }, 1000 ); </script>");
#ifdef DUTCH
  toSend += F("</head><body><h2>bestandssysteem wissen, even wachten... !</h2></body></html>");
#else
  toSend += F("</head><body><h2>erasing the filesystem, please wait... !</h2></body></html>");
#endif  
//  portalserver.send ( 200, "text/html", toSend ); //zend bevestiging
  delay(500); //wait half a second after sending the data 
  delay(1);
     if (SPIFFS.exists("/wifiConfig.conf")) { SPIFFS.remove("/wificonfig.json"); } 
     if (SPIFFS.exists("/wificonfig.json")) { SPIFFS.remove("/wificonfig.json"); }
     if (SPIFFS.exists("/basisconfig.json")) { SPIFFS.remove("/basisconfig.json"); }
     if (SPIFFS.exists("/timerconfig.json")) { SPIFFS.remove("/timerconfig.json"); }
     SPIFFS.format();
     Serial.println("We gaan LittleFS wissen");
}

void resetStatic() {
   Serial.println("static ip wissen");
  String toSend = F("<!DOCTYPE html><html><head>");
  toSend += F("<script type='text/javascript'>setTimeout(function(){ window.location.href='/redirect'; }, 500 ); </script>");
#ifdef DUTCH
  toSend += F("</head><body><h2>ip instelling wissen, even wachten... !</h2></body></html>");
#else
 toSend += F("</head><body><h2>erasing ip settings, please wait... !</h2></body></html>");
#endif
//request->send ( 200, "text/html", toSend ); //zend bevestiging
  delay(500); //wait half a second after sending the data 
  delay(1);
  static_ip[0] = '\0';
  static_ip[1] = '\0';  
}

void scanWifi() {
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      //Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  networksFound = n;
  //return n; // # of found networks
  }

//// ********************************************************
////      zet de gevonden netwerken in een string
//// ********************************************************
String makeList(int aantal) { // aantal is het aantal gevonden netwerken

// FIRST sort
      int indices[aantal];
      for (int i = 0; i < aantal; i++) {
        indices[i] = i;
      }
      // RSSI SORT
      for (int i = 0; i < aantal; i++) {
        for (int j = i + 1; j < aantal; j++) {
          if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
            std::swap(indices[i], indices[j]);
          }
        }
      }   
   
  String lijst="";
  for (int i = 0; i < aantal; i++) {

        String item = FPSTR(PORTAL_LIST);
        String rssiQ;
        //int quality = (WiFi.RSSI(i));
        int quality = WiFi.RSSI(indices[i]);
        Serial.print("quality = "); Serial.println(quality);
        // -50 is groter dan -80
        if (quality < -99){ continue; } // skip als kleiner dan -65 bijv -66
//        item.replace("{v}", WiFi.SSID(i));
        item.replace("{v}", WiFi.SSID(indices[i]));        
        item.replace("{r}", String(quality));
        item.replace("{i}", "");
        lijst += item;
  }
return lijst;  
}
  
