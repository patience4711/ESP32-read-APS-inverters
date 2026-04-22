// find out if the request comes from inside the network
bool checkRemote(String url) {
//check if the first 9 characters of the router 192.168.0
    if(securityLevel == 0) return false; // never remote
    if ( url.indexOf(WiFi.gatewayIP().toString().substring(0, securityLevel)) == -1 ) return true; else return false;
}

// we come here when an unknown request is done
void handleNotFound(AsyncWebServerRequest *request) 
{
  
   bool intern = false;
   if(!checkRemote( request->client()->remoteIP().toString()) ) intern = true;

  // **************************************************************************
  //             R E S T R I C T E D   A R E A
  // **************************************************************************
  // access only from inside the network
  if ( intern ) {    //DebugPrintln("the request comes from inside the network");
      String serverUrl = request->url().c_str();
      Serial.println("serverUrl = " + serverUrl);             
     
      if ( serverUrl.indexOf("SENDZIGBEE") > -1) 
      { 
          strcpy( txBuffer, request->arg("mes").c_str()); 
          diagNose = 10; // consoleOut logs the output 
          debugLog = "debug send zigbee command\n"; 
          actionFlag = 45; 
          String term = "sending zigbee message"; 
          if(diagNose == 10) term += "<br><br><a href=\"/readDebug\">view log</a>"; 
          request->send ( 200, "text/html", term ); 
          return; 
      } else
      // /SHOWFILE?file=/wificonfig.json
       if ( serverUrl.indexOf("SHOWFILES") > -1) 
      { 
      loginBoth(request, "admin"); // we need to be logged in
          //String file = request->arg("file").c_str();
          //const char* file = request->arg("file").c_str();
          diagNose = 3; // consoleOut logs the output 
          // debugLog = "debug send SHOWFILE\n"; 
          // //actionFlag = ; 
          String term = "printing all files in SPIFFS\n "; 
          // if( file_open_for_read(file) ) {
          //   consoleOut("read "+ String(file));
          // } else {
          //    consoleOut(String(file) + " not opened\n");
          // }
          SPIFFS_read();
          printInverters(); // show all the inverter files
          term += "<br><br><a href=\"/readDebug\">view log</a>"; 
          request->send ( 200, "text/html", term ); 
          return; 
      } else
 
 
      //say we have a request <ip of ecu>/TROTTLE?debug=1&inv=1&val=500
      //say we have a request ip_of_ecu/TROTTLE0=300
      if ( serverUrl.indexOf("THROTTLE") > -1) 
      {
          int Invert = request->arg("inv").toInt();
          int throtVal = request->arg("val").toInt();
          if (request->hasParam("debug")) diagNose = 10;
          
          debugLog = "debug throttlecommand\n";
          if (Invert > inverterCount) Invert = 10;
          if (throtVal == 0) throtVal = 800;
        
          Serial.println("inv = " + String(Invert));
          Serial.println("val = " + String(throtVal));
          //write the desired throttle value to preferences
          
          if(Invert > inverterCount || Invert < 0 || throtVal > 700 || throtVal < 20 )
            {
              request->send ( 200, "text/plain", "invalid value(s)" );
              return; 
            }
          desiredThrottle[Invert] = throtVal;
          actionFlag = 240+Invert;
          
          String term = "attempt throttling inverter " + String(Invert) + " to " + String(throtVal);
          if(diagNose==10) term += "<br><br>see the log <a href=\"/readDebug\">read log</a>";
          request->send ( 200, "text/html", term );
        
          return;
      } else
          ///QUERY?inv=0
          if ( serverUrl.indexOf("QUERY") > -1) 
      {
          int inv = request->arg("inv").toInt();
          diagNose = 10;
          debugLog = "debug query command inverter " + String(inv) + "\n<br>";
          if(inv > inverterCount || inv < 0 )
            {
              request->send ( 200, "text/plain", "invalid inverter" );
              return; 
            }
          actionFlag = inv + 210;
          
          String term = "<center>query inverter " + String(inv) + "\n<br>";
          term += "<br><br><a href=\"/readDebug\">view debug log</a>";
          request->send ( 200, "text/html", term );
          return;
      } else

           if ( serverUrl.indexOf("TESTMQTT") > -1) 
      {
          if (request->hasParam("debug")) diagNose = 10;
          
          if (diagNose == 10) debugLog = "debug test mosquitto\n<br>";
          actionFlag = 49;
          
          String term = "<center>test message mosquitto\n<br>";
          if (diagNose == 10) term += "<br><br><a href=\"/readDebug\">view debug log</a>";
          request->send ( 200, "text/html", term );
          return;
      } else
    
    
      if ( serverUrl.indexOf("readDebug") > -1) {
          debugLog += "\nlength log: " + String(debugLog.length());
          request->send(200, "text/html", "<pre>" + debugLog + "</pre>");
          debugLog="";
          return;
      } else

      if ( serverUrl.indexOf("INVERTERPOLL") > -1) 
      {   
          
          if (request->hasParam("debug")) diagNose = 3;
          debugLog = "debug poll command\n";
          int inv = request->arg("inv").toInt();
          
          if(Polling || !dayTime || inv > inverterCount-1)
          {
            request->send ( 200, "text/plain", "error check dayTime, Polling, inverter#" ); //zend bevestiging
            return; 
          }
        
          actionFlag = 220+inv; // takes care for the polling   
          String term = "polling inverter " + String(inv);
          if(diagNose==3) term += "<br><br><a href=\"/readDebug\">view the log</a>";
          request->send ( 200, "text/html", term );      
          return;
      } 

     // if we are here, no valid api was found    
     request->send ( 200, "text/plain", "ERROR this link is invalid, go back <--" );//send not found message
     }             
    else 
     { // not intern
      //DebugPrint("\t\t\t\t unauthorized, not from inside the network : ");
      request->send ( 200, "text/plain", "ERROR you are not authorised, go back <--" );//send not found message
     }
}
