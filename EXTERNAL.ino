// we come her when an unautorized request is done

void handleNotFound(AsyncWebServerRequest *request) {
  
String serverargument = request->url().c_str();
//DebugPrintln("we are in handleNotFound");
//DebugPrint("serverargument = "); //DebugPrintln(serverargument);
// if the client is outside the network he is can do nothing

String addy = request->client()->remoteIP().toString();
bool intern = false;
String router=WiFi.gatewayIP().toString();
router = router.substring(0, 9);
//DebugPrint("router substring 0-9 = "); //DebugPrintln(router);

if ( addy.indexOf(router) == -1 ) { // indexOf geeft -1 als niet gevonden
  //DebugPrintln("the client comes form outside the network");
  intern = false; 
  } else {
  //DebugPrintln("the client comes form inside the network"); 
  intern = true; }

if ( intern ) {    //DebugPrintln("the request comes from inside the network");
             
        // POLL=;
        if ( serverargument.indexOf("POLL=") > -1) {
              if(Polling)
              {
                 request->send ( 200, "text/plain", "polling is automatic, skipping" ); //zend bevestiging
                 return; 
              }
              DebugPrintln("handleNotFound found POLL=");
              int x = serverargument.indexOf("POLL=");
              //DebugPrintln("serverargument = " + serverargument);
              //DebugPrintln("sub = " + serverargument.substring(6,7));
              int inv = serverargument.substring(6,7).toInt();
              //DebugPrintln("inv= " + String(inv));
              String teZenden = "polling inverternr " + String(inv);
              if(inv > inverterCount-1 || zigbeeUp == 0) {
                  String teZenden="ERROR " + serverargument + " zigbee down or inverter not exists !";
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

    else { // not intern
      //DebugPrint("\t\t\t\t unauthorized, not from inside the network : ");
      request->send ( 200, "text/plain", "ERROR you are not authorised, go back <--" );//send not found message
    }
}
