/*
 * changed the order of the handlers
*/
void start_server() {
if( diagNose != 0 ) consoleOut("starting server");
//server.addHandler(&ws);
server.addHandler(&events);

// Handle Web Server Events
events.onConnect([](AsyncEventSourceClient *client){
  if(client->lastId()){
    Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
  }
});

// ***********************************************************************************
//                                     homepage
// ***********************************************************************************
server.on("/SW=BACK", HTTP_GET, [](AsyncWebServerRequest *request) {
    loginBoth(request, "both");
    request->redirect( String(requestUrl) );
});

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    loginBoth(request, "both");
    request->send_P(200, "text/html", ECU_HOMEPAGE);
});

server.on("/STYLESHEET", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", STYLESHEET);
});

server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("favicon requested");
    AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", FAVICON, FAVICON_len);
    request->send(response);
});

server.on("/details", HTTP_GET, [](AsyncWebServerRequest *request) {
iKeuze = atoi(request->arg("inv").c_str()) ;
//requestUrl = request->url();
strcpy( requestUrl, request->url().c_str() );
request->send_P(200, "text/html", DETAILSPAGE);
});
// ********************************************************************
//      very often called  X H T  R E Q U E S T S
//***********************************************************************
server.on("/get.General", HTTP_GET, [](AsyncWebServerRequest *request) {
// this link provides the general data on the frontpage
    char temp[15]={0};
    uint8_t remote = 0;
    if(checkRemote( request->client()->remoteIP().toString()) ) remote = 1; // for the menu link
    uint8_t night = 0; 
    if(!dayTime) { night = 1; }
    
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument root(160); //(160);
    //JsonObject root = doc.to<JsonObject>();
    root["cnt"] = inverterCount;    
    root["rm"] = remote;
    root["st"] = zigbeeUp;
    root["sl"] = night;    
    serializeJson(root, * response);
    request->send(response);
});
// this link provides the inverterdata on the frontpage
server.on("/get.Power", HTTP_GET, [](AsyncWebServerRequest *request) {
// what to do with unpolled inverters that have been polled earlyer this day
// we should show the energy value but 
    int i = atoi(request->arg("inv").c_str()) ;
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument root(128);
    if(Inv_Data[i].en_total > 0) { // only possible when was polled this day
        root["eN"] = round2(Inv_Data[i].en_total/(float)1000); // rounded
    } else {
        root["eN"] = "n/a" ;
    }
    //now populate the powervalues in an array "p":[p0, p1, p2, p3]
    for(int z = 0; z < 4; z++ ) 
    {
         //is the panel connected? if not put n/e
         if( ! Inv_Prop[i].conPanels[z] ) { root["p"][z] = "n/e";  }  
         // so the panel is connected, is the inverter polled?
         else if (polled[i]) 
         {
             //polled, we put a value
               root["p"][z] = round1(Inv_Data[i].power[z]) ;
         }   else {
            // not polled, we put n/a
               root["p"][z] = "n/a";
         }
     }

    serializeJson(root, * response);
    request->send(response);
});

// this link is called by the detailspage
server.on("/get.Inverter", HTTP_GET, [](AsyncWebServerRequest *request) { 
// this is used by the detailspage and for http requests      
// set the array into a json object
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  DynamicJsonDocument doc(768);
  JsonObject root = doc.to<JsonObject>();
  int i;
  if (request->hasArg("inv")) {
     i = (request->arg("inv").toInt()) ;
  } else {
     i = iKeuze;
  }
  if( i < inverterCount) {
    root["inv"] = i;
    root["name"] = Inv_Prop[i].invLocation;
    root["polled"] = polled[i];
    root["type"] = Inv_Prop[i].invType;
    root["serial"] = Inv_Prop[i].invSerial;
    root["sid"] = Inv_Prop[i].invID;
    root["freq"] = round1(Inv_Data[i].freq);
    root["temp"] = round1(Inv_Data[i].heath);
    root["acv"] = round1(Inv_Data[i].acv);
    root["sq"] = round1(Inv_Data[i].sigQ);     
    root["pw_total"] = round1(Inv_Data[i].pw_total);
    root["en_total"] = round2(Inv_Data[i].en_total/(float)1000); // rounded
      for(int z = 0; z < 4; z++ ) 
      {
         if(Inv_Prop[i].conPanels[z]) // is this panel connected?
         {
              root["dcv"][z] = round1(Inv_Data[i].dcv[z]);
              root["dcc"][z] = round1(Inv_Data[i].dcc[z]);
              root["pow"][z] = round1(Inv_Data[i].power[z]);
              root["en"][z] = round2(en_saved[i][z]); //rounded
         }   else {
              root["dcv"][z] = "n/e";
              root["dcc"][z] = "n/e";
              root["pow"][z] = "n/e";
              root["en"][z] = "n/e";
         }
      }
    serializeJson(root, * response);
    request->send(response);
     } else {
     String term = "unknown inverter " + String(i);
     request->send(200, "text/plain", term);
     }
});

server.on("/MENU", HTTP_GET, [](AsyncWebServerRequest *request) {
//Serial.println("requestUrl = " + request->url() ); // can we use this
if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );

loginBoth(request, "admin");
toSend = FPSTR(HTML_HEAD);
toSend += FPSTR(MENUPAGE);
request->send(200, "text/html", toSend);
});
server.on("/SECURITY", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", SECURITY);
});
server.on("/DENIED", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/html", REQUEST_DENIED);
});


server.on("/CONSOLE", HTTP_GET, [](AsyncWebServerRequest *request){
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    request->send_P(200, "text/html", CONSOLE_HTML);
  });

// ***********************************************************************************
//                                   basisconfig
// ***********************************************************************************
server.on("/BASISCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
loginBoth(request, "admin");
//requestUrl = request->url();// remember this to come back after reboot
strcpy( requestUrl, request->url().c_str() );
zendPageBasis(request);
//request->send(200, "text/html", toSend);
});

server.on("/submitform", HTTP_GET, [](AsyncWebServerRequest *request) {
handleForms(request);
confirm(); // puts a response in toSend
request->send(200, "text/html", toSend);
});

server.on("/IPCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  zendPageIPconfig();
  request->send(200, "text/html", toSend);
});

server.on("/IPconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
  handleIPconfig(request);
});

server.on("/MQTT", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  //requestUrl = request->url();
  strcpy( requestUrl, request->url().c_str() );
  zendPageMQTTconfig(request);
  //request->send(200, "text/html", toSend);
});

server.on("/GEOCONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  //requestUrl = request->url();
  strcpy( requestUrl, request->url().c_str() );
  zendPageGEOconfig(request);
  //request->send(200, "text/html", toSend);
});

server.on("/REBOOT", HTTP_GET, [](AsyncWebServerRequest *request) {
  loginBoth(request, "admin");
  actionFlag = 10;
  confirm(); 
  request->send(200, "text/html", toSend);
});

server.on("/STARTAP", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  String toSend = F("<!DOCTYPE html><html><head><script type='text/javascript'>setTimeout(function(){ window.location.href='/SW=BACK'; }, 5000 ); </script>");
  toSend += F("</head><body><center><h2>OK the accesspoint is started.</h2>Wait unil the led goes on.<br><br>Then go to the wifi-settings on your pc/phone/tablet and connect to ESP32-ECU");
  request->send ( 200, "text/html", toSend ); //zend bevestiging
  actionFlag = 11;
});

server.on("/ABOUT", HTTP_GET, [](AsyncWebServerRequest *request) {
Serial.println(F("/INFOPAGE requested"));
loginBoth(request, "both");
handleAbout(request);
});
server.on("/TEST", HTTP_GET, [](AsyncWebServerRequest *request) {
  loginBoth(request, "admin");
  actionFlag = 44;
  request->send( 200, "text/html", "<center><br><br><h3>checking zigbee.. please wait a minute.<br>Then you can find the result in the log.<br><br><a href=\'/LOGPAGE\'>click here</a></h3>" );
});

server.on("/LOGPAGE", HTTP_GET, [](AsyncWebServerRequest *request) {
  loginBoth(request, "both");
  //requestUrl = request->url();
  strcpy( requestUrl, request->url().c_str() );
  //handleLogPage(request);
  request->send_P(200, "text/html", LOGPAGE, putList);
});

server.on("/MQTT_TEST", HTTP_GET, [](AsyncWebServerRequest *request) {
loginBoth(request, "admin");
char Mqtt_send[26] = {0};
strcpy( Mqtt_send , Mqtt_outTopic);
if(Mqtt_send[strlen(Mqtt_send -1)] == '/') {
  strcat(Mqtt_send, String(Inv_Prop[0].invIdx).c_str());
}

String toMQTT = "{\"test\":\"" + String(Mqtt_send) + "\"}";
//DebugPrintln("MQTT_Client.publish the message : " + toMQTT);
MQTT_Client.publish ( Mqtt_send, toMQTT.c_str(), true );
toSend = "sent mqtt message : " + toMQTT;
request->send( 200, "text/plain", toSend  );
});
  
// ********************************************************************
//                    inverters
// ******************************************************************

server.on("/INVSCRIPT", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", INV_SCRIPT);
});

server.on("/INV_CONFIG", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );  
  iKeuze=0;
  strcpy( requestUrl, request->url().c_str() ); 
  inverterForm(); // prepare the page part with the form
  request->send_P(200, "text/html", INVCONFIG_START, processor);
});

server.on("/inverterconfig", HTTP_GET, [](AsyncWebServerRequest *request) {
handleInverterconfig(request);
});

server.on("/PAIR", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  loginBoth(request, "admin");
  //requestUrl = request->url();
  strcpy( requestUrl, request->url().c_str() );
  //DebugPrintln(F("pairing requested"));
  handlePair(request);
});

server.on("/INV_DEL", HTTP_GET, [](AsyncWebServerRequest *request) {
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
  handleInverterdel(request);
});

server.on("/INV", HTTP_GET, [](AsyncWebServerRequest *request) {
    strcpy( requestUrl, request->url().c_str() );
    //bool nothing = false;
    int i = atoi(request->arg("welke").c_str()) ;
    iKeuze = i;
    consoleOut("?INV iKeuze at enter = " + String(iKeuze));
    if( iKeuze == 99 ) {
        iKeuze = inverterCount; //indicate this is an adition
        inverterCount += 88;
        }
     String bestand = "/Inv_Prop" + String(iKeuze) + ".str";
     consoleOut("iKeuze = " + String(iKeuze));
     if (!SPIFFS.exists(bestand)) Inv_Prop[iKeuze].invType = 2;
     inverterForm(); // prepare the form page
     request->send_P(200, "text/html", INVCONFIG_START, processor); //send the html code to the client
});

//server.on("/CONFIRM_INV", HTTP_GET, [](AsyncWebServerRequest *request) {
//    toSend = FPSTR(CONFIRM_INV); // prepare the page
//    request->send(200, "text/html", toSend); //send the html code to the client
//});
// ********************************************************************
//                    X H T  R E Q U E S T S
//***********************************************************************

server.on("/get.Paired", HTTP_GET, [](AsyncWebServerRequest *request) {     
// set the array into a json object
  String json="{";
  json += "\"invID\":\"" + String(Inv_Prop[iKeuze].invID) + "\"";
  json += "}";
  request->send(200, "text/json", json);
  json = String();
});

// ***************************************************************************************
//                           Simple Firmware Update
// ***************************************************************************************                                      
  server.on("/FWUPDATE", HTTP_GET, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    strcpy(requestUrl, "/");
    if (!request->authenticate("admin", pswd) ) return request->requestAuthentication();
    request->send_P(200, "text/html", otaIndex); 
    });
  server.on("/handleFwupdate", HTTP_POST, [](AsyncWebServerRequest *request){
    if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    Serial.println("FWUPDATE requested");
    if( !Update.hasError() ) {
    toSend="<br><br><center><h2>UPDATE SUCCESS !!</h2><br><br>";
    toSend +="click here to reboot<br><br><a href='/REBOOT'><input style='font-size:3vw;' type='submit' value='REBOOT'></a>";
    } else {
    toSend="<br><br><center><kop>update failed<br><br>";
    toSend +="click here to go back <a href='/FWUPDATE'>BACK</a></center>";
    }
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", toSend);
    response->addHeader("Connection", "close");
    request->send(response);
  
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    //Serial.println("filename = " + filename);
    if(filename != "") {
    if(!index){
      //#ifdef DEBUG
        Serial.printf("start firmware update: %s\n", filename.c_str());
      //#endif
      //Update.runAsync(true);
      if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
        //#ifdef DEBUG
          Update.printError(Serial);
        //#endif
      }
    }
    } else {
      if( diagNose != 0 ) consoleOut("filename empty, aborting");
//     Update.hasError()=true;
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
          Serial.println("update failed with error: " );
          Update.printError(Serial);
      }
    }
    if(final){
      if(Update.end(true)){
        Serial.printf("firmware Update Success: %uB\n", index+len);
      } else {
        Update.printError(Serial);
      }
    }
  });


// if everything failed we come here
server.onNotFound([](AsyncWebServerRequest *request){
  //Serial.println("unknown request");
  handleNotFound(request);
});

server.begin(); 
}

void confirm() {
toSend="<html><body onload=\"setTimeout(function(){window.location.href='";
toSend+=String(requestUrl);
toSend+="';}, 3000 );\"><br><br><center><h3>processing<br>your request,<br>please wait</html>";
Serial.println(toSend);
}

double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
double round1(double value) {
   return (int)(value * 10 + 0.5) / 10.0;
}
