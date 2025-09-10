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
//Serial.println("details url = " + String(requestUrl));
request->send_P(200, "text/html", DETAILSPAGE);
});
// ********************************************************************
// very often called  XHT REQUESTS handled by handleDataRequests()
//***********************************************************************
server.on("/get.Data", HTTP_GET, [](AsyncWebServerRequest *request) {
  strcpy( requestUrl, request->url().c_str() );
  //consoleOut("get.Data url = " + String(requestUrl));
  handleDataRequests(request);
});



server.on("/MENU", HTTP_GET, [](AsyncWebServerRequest *request) {
//Serial.println("requestUrl = " + request->url() ); // can we use this
if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );

loginBoth(request, "admin");
//toSend = FPSTR(HTML_HEAD);
//toSend += FPSTR(MENUPAGE);
request->send_P(200, "text/html", MENUPAGE);
});
server.on("/SECURITY", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/css", SECURITY);
});
server.on("/DENIED", HTTP_GET, [](AsyncWebServerRequest *request) {
   request->send_P(200, "text/html", REQUEST_DENIED);
});


server.on("/CONSOLE", HTTP_GET, [](AsyncWebServerRequest *request){
  if(checkRemote( request->client()->remoteIP().toString()) ) request->redirect( "/DENIED" );
    diagNose=1;
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
  strcpy( requestUrl, "/");
  procesId = 3;
  confirm(); 
  //strcpy( requestUrl, "/");
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

// server.on("/MQTT_TEST", HTTP_GET, [](AsyncWebServerRequest *request) {
// loginBoth(request, "admin");
// char Mqtt_send[26] = {0};
// strcpy( Mqtt_send , Mqtt_outTopic);
// if(Mqtt_send[strlen(Mqtt_send -1)] == '/') {
//   strcat(Mqtt_send, String(Inv_Prop[0].invIdx).c_str());
// }

// String toMQTT = "{\"test\":\"" + String(Mqtt_send) + "\"}";
// //DebugPrintln("MQTT_Client.publish the message : " + toMQTT);
// MQTT_Client.publish ( Mqtt_send, toMQTT.c_str(), true );
// toSend = "sent mqtt message : " + toMQTT;
// request->send( 200, "text/plain", toSend  );
// });
  
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
    //strcpy( requestUrl, request->url().c_str() );
    //bool nothing = false;
    int i = atoi(request->arg("welke").c_str()) ;
    iKeuze = i;
    String toReturn = "/INV?welke=" + String(iKeuze);
    strcpy(requestUrl, toReturn.c_str() ); 
    consoleOut("requestUrl = " + String(requestUrl));
       
    consoleOut("INV iKeuze at enter = " + String(iKeuze));
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

//void confirm() {
//toSend="<html><body onload=\"setTimeout(function(){window.location.href='";
//toSend+=String(requestUrl);
//int waitTime = 3000*procesId;
//toSend += "';}, " + String(waitTime) + " );\"><br><br><center><h3>processing<br>your request,<br>please wait</html>";
//}
void confirm() {
toSend  = "<html><head><script>";
toSend += "let waitTime=" + String(3000*procesId) + ";";
toSend += "function redirect(){";
toSend += " let counter=document.getElementById('counter');";
toSend += " let secs=waitTime/1000;";
toSend += " counter.textContent=secs;";
toSend += " let timer=setInterval(function(){";
toSend += "   secs--; counter.textContent=secs;";
toSend += "   if(secs<=0){ clearInterval(timer); window.location.href='" + String(requestUrl) + "'; }";
toSend += " },1000);";
toSend += "}";
toSend += "</script></head>";
toSend += "<body onload='redirect()'>";
toSend += "<br><br><center><h3>processing<br>your request,<br>please wait<br><br>";
toSend += "Redirecting in <span id='counter'></span> seconds...</h3></center>";
toSend += "</body></html>";
}



double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
double round1(double value) {
   return (int)(value * 10 + 0.5) / 10.0;
}
