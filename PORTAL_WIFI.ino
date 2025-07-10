// {//test
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
  #define MAX_CLIENTS 4  // ESP32 supports up to 10 but I have not tested it yet
  #define WIFI_CHANNEL 6  // 2.4ghz channel 6 https://en.wikipedia.org/wiki/List_of_WLAN_channels#2.4_GHz_(802.11b/
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
  //const char* apPasswd = "123456789";
  const char* apPasswd = NULL;  
  WiFi.softAP(apNaam, apPasswd, WIFI_CHANNEL, 0, MAX_CLIENTS);
  // Disable AMPDU RX on the ESP32 WiFi to fix a bug on Android
  esp_wifi_stop();
  esp_wifi_deinit();
  wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();
  my_config.ampdu_rx_enable = false;
  esp_wifi_init(&my_config);
  esp_wifi_start();
  delay(500); // without delay tje IP address can be blanco
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
//  server.on("/redirect", HTTP_GET, [](AsyncWebServerRequest *request) {
//  consoleOut("/redirect requested");
//  handlePortalRoot();    
//  request->send(200, "text/html", toSend); //send the html code to the client
//  });
  server.on("/generate_204", [](AsyncWebServerRequest *request) { request->redirect("192.168.4.1"); });
  server.on("/redirect", [](AsyncWebServerRequest *request) { request->redirect("192.168.4.1"); });
  server.on("/hotspot-detect.html", [](AsyncWebServerRequest *request) { request->redirect("192.168.4.1"); });
  server.on("/canonical.html", [](AsyncWebServerRequest *request) { request->redirect("192.168.4.1"); });
  server.on("/ncsi.txt", [](AsyncWebServerRequest *request) { request->redirect("192.168.4.1"); });
  server.on("/ERASE_FLASH", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("/erase flash requested");
  flashErase(request);    
  });  
  server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("/redirect requested");
  handlePortalRoot();    
  request->send(200, "text/html", toSend); //send the html code to the client
  });

  server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request) {
  consoleOut("/home requested");    
  handlePortalRoot();
  //sendHeaders();
  request->send(200, "text/html", toSend); //send the html code to the client
  });

 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  consoleOut("/ requested");    
  handlePortalRoot();
  //sendHeaders();
  request->send(200, "text/html", toSend); //send the html code to the client
  });
 
  server.on("/wifiForm", HTTP_GET, [](AsyncWebServerRequest *request) {
  consoleOut("/wifiForm requested");    
  handleForm();
  //sendHeaders();
  request->send(200, "text/html", toSend); //send the html code to the client
  });

  server.on("/STYLESHEET", HTTP_GET, [](AsyncWebServerRequest *request) {
  //Serial.println("stylesheet requested");
    request->send_P(200, "text/css", STYLESHEET);
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
  Serial.println("wifiCon youpy, connected");
  ledblink(3, 200);
  esp_task_wdt_reset();
  digitalWrite(led_onb, LED_UIT);
  //pixelsAan(0);
  event=101;
   
    } else {
  Serial.println("could not connect, try again");
  esp_task_wdt_reset();
    ledblink(10, 200);
    digitalWrite(led_onb, LED_AAN); 
    //set_pwm(200);
    //pixelsAan(100); 

    event=100;
    }
toSend = FPSTR(HTML_HEAD);
toSend += FPSTR(PORTAL_CONFIRM); 
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

  
  server.on("/wifiClose", HTTP_GET, [](AsyncWebServerRequest *request) {
  Serial.println("wifiClose called");
  handlePortalClose(request);
  request->send(200, "text/html", toSend); //send the html code to the client 
   }); 

  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP

  server.begin(); // Web server start
  Serial.println("started async HTTP server for the portal");
  digitalWrite(led_onb, LED_AAN);  // led aan
   //pixelsAan(100);


// * * * * * * * *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//                               the connect portal loop
// * * * * * * * *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  // now we enter an infinitive loop that we leave only after 5 minutes, 
  // or when via server.handleClient something happens   
  //  bool Timed_Out = true;
  Serial.println("entering the infinitive loop with heartbeat");
  laatsteMeting = millis(); //voor de time-out

  // this is the infinitive  loop 
  static unsigned long heartbeat = 0;
  while (millis() < laatsteMeting + 300*1000UL) { // 5 minuten== 300 30 == 30sec
  
     if(actionFlag == 10) break; // set by portalClose 
     //DNS
     dnsServer.processNextRequest();
  } 
  // ************************ end while loop ******************************* 

   //we only are here after a timeout or a break.      
    Serial.println("portal timed out, resetting...");
    ESP.restart();
 
}

// ********************************************************************
//                 de homepagina van het portal
// ********************************************************************
void handlePortalRoot() {
    //  always as we are here, portalstart is updated, so when there is activity in the
    //  webinterface we won't time out. 
    //  sendHeaders();
    laatsteMeting = millis(); // update portalstart
    Serial.println("handlePortalRoot, event = " + String(event));
    toSend = FPSTR(HTML_HEAD); // the normal head
    toSend += FPSTR(PORTAL_HOMEPAGE);
    //toSend.replace("{haha}" , "if (window.location.hostname != \"192.168.4.1\") {window.location.href = 'http://192.168.4.1'};");
    toSend.replace("{ma}", String(WiFi.macAddress()) );

    // The value of event decides what is shown on this page
    // when event = 101 (we are connected) we adapt the page with new state
    if (event == 101) {  // the 2e time that we are here we are connected 101 or not 100
        toSend.replace("hadden", "hidden"); // hide the setup button
        toSend.replace("hodden", "hidden"); // hide the "eraseflash" link
        //toSend.replace("close' hidden", "close'"); // show the close button
        String page = "";
        page += F("<div class=\"msg\">");
        page += "connected to <strong>" + WiFi.SSID() + "</strong><br><br>";
        page += "The IP address is <strong>http://" + WiFi.localIP().toString() + "</strong><br><br>";
        page += F("<h3 style='color:#FF0000';>Note down the ip-address and click \"close\"!</h3>");
        toSend.replace("<irame>" , page);
        // show the closebutton afsluitknop
        toSend.replace("close' hidden", "close'");
    }
    if (event==100) {  // niet verbonden
        String page = "";
        page+=F("Connection has failed!<br><br>");
        page+=F("Retry, click setup wifi or erase flash first!"); 
        toSend.replace("<irame>" , page);
    }

}


// *********************************************************************
//                          pagina voor invoeren credentials
// *********************************************************************
void handleForm() {
   laatsteMeting = millis();
// als we de eerste keer hierkomen beginnen we met een netwerkscan

toSend = FPSTR(HTML_HEAD);
toSend = FPSTR(PORTAL_WIFIFORM);
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
  //checkFixed(); // set static ip if configured

  return connRes;
}
// *********************************************************************
//                         Closing the  portal
// *********************************************************************
void handlePortalClose(AsyncWebServerRequest *request  ) {
  laatsteMeting = millis();
  String toSend = F("<!DOCTYPE html><html><head>");
  toSend += F("<script type='text/javascript'>setTimeout(function(){ window.location.href='/'; }, 1000 ); </script>");
  toSend += F("</head><body><center><h2>The esp is going to restart.!</h2>Do not forget to note the ip address!!<br>");
  toSend += F("<h2>The ip address is : ");
  toSend += WiFi.localIP().toString();
  toSend += F("</h2></body></html>");
  request->send(200, "text/html", toSend);
  actionFlag = 10; // ends the infinity loop and restarts
  //ESP.restart();
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
  for (int i = 0; i < 8; i++) { //max 8 items

        String item = FPSTR(PORTAL_LIST);
        String rssiQ;
        //int quality = (WiFi.RSSI(i));
        int quality = WiFi.RSSI(indices[i]);
        Serial.print("quality = "); Serial.println(quality);
        // -50 is greather than -80
        if (quality < -99){ continue; } // skip if smaller than  -65 p.e. -66
        item.replace("{v}", WiFi.SSID(indices[i]));        
        item.replace("{r}", String(quality));
        item.replace("{i}", "");
        lijst += item;
  }
return lijst;  
}
  