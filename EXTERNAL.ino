// find out if the request comes from inside the network
bool checkRemote(String url) {
//check if the first 9 characters of the router 192.168.0
    if(securityLevel == 0) return false; // never remote
    if ( url.indexOf(WiFi.gatewayIP().toString().substring(0, securityLevel)) == -1 ) return true; else return false;
}

// we come here when an unknown request is done
void handleNotFound(AsyncWebServerRequest *request) {
  
bool intern = false;
if(!checkRemote( request->client()->remoteIP().toString()) ) intern = true;

// **************************************************************************
//             R E S T R I C T E D   A R E A
// **************************************************************************
// access only from inside the network
if ( intern ) {    //DebugPrintln("the request comes from inside the network");
  String serverUrl = request->url().c_str();
  Serial.println("serverUrl = " + serverUrl);             

//  if ( serverUrl.indexOf("/CONSOLE")>-1 ) {
//    request->send_P(200, "text/html", CONSOLE_HTML);
//  }
//  if ( serverUrl.indexOf("/MENU")>-1 ) {
//  Serial.println("requestUrl = " + request->url() ); // can we use this
//  loginBoth(request, "admin");
//  toSend = FPSTR(HTML_HEAD);
//  toSend += FPSTR(MENUPAGE);
//  request->send(200, "text/html", toSend);
//  }

  //say we ha a request <ip of ecu>/TROTTLE?inv=1&val=500
  if ( serverUrl.indexOf("THROTTLE") > -1) 
  {

    int Inv = request->arg("inv").toInt() | 10;
    int throtVal = request->arg("val").toInt() | 800;
    Inv_Prop[Inv].maxPower = throtVal;
    if(Inv > 8 || Inv < 0 || throtVal > 500 || throtVal < 0 ){
      request->send ( 200, "text/plain", "invalid value(s)" );
      return;
    }
    actionFlag = 240+Inv;
    String term = "attempt throttling inverter " + String(Inv) + " to " + String(throtVal);
    request->send ( 200, "text/plain", term );
    return;
  }
  
  if ( serverUrl.indexOf("POLL=") > -1) {
      if(Polling)
      {
         request->send ( 200, "text/plain", "polling is automatic, skipping" ); //zend bevestiging
         return; 
      }
      if( diagNose != 0 ) consoleOut(F("handleNotFound found POLL="));
      int x = serverUrl.indexOf("POLL=");
      //DebugPrintln("serverUrl = " + serverUrl);
      //DebugPrintln("sub = " + serverUrl.substring(6,7));
      int inv = serverUrl.substring(6,7).toInt();
      //DebugPrintln("inv= " + String(inv));
      String teZenden = "polling inverternr " + String(inv);
      if(inv > inverterCount-1 || zigbeeUp != 1) {
          String teZenden="ERROR " + serverUrl + " zigbee down or inverter not exists !";
          request->send ( 200, "text/plain", teZenden ); 
          return;
      }               
                  
     iKeuze = inv;
     teZenden = "polled inverter " + String(inv);
     request->send ( 200, "text/plain", teZenden ); 
     actionFlag = 47; // takes care for the polling
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
