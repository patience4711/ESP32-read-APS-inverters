// find out if the request comes from inside the network
bool checkRemote(String url) {
//check if the first 9 characters of the router 192.168.0
    if(settings.securityLevel == 0) return false; // never remote
    if ( url.indexOf(WiFi.gatewayIP().toString().substring(0, settings.securityLevel)) == -1 ) return true; else return false;
}

// we come here when an unknown request is done
void handleNotFound() {
  
bool intern = false;
if(checkRemote(server.client().remoteIP().toString()) ) intern = true;

// **************************************************************************
//             R E S T R I C T E D   A R E A
// **************************************************************************
// access only from inside the network
if ( intern ) {    //DebugPrintln("the request comes from inside the network");
     String serverUrl = server.uri().c_str();
     consoleOut("\nserverUrl = " + serverUrl);             

     if(serverUrl == "/SW=ON") {
      consoleOut("sw=on found");
      set_dim_level(last_duty);
      UpdateLog(5, "switched on");
      server.send ( 200, "text/plain", "switched on" );
      return; 
     }
     if(serverUrl == "/SW=OFF") {
      consoleOut("sw=off found");
      set_dim_level(0);
      UpdateLog(5, "switched off");
      server.send ( 200, "text/plain", "switched off" );
      checkTimers;
      return; 
     }
     // if we are here, no valid api was found    
     server.send ( 200, "text/plain", "ERROR this link is invalid, go back <--" );//send not found message
     }             
    else 
     { // not intern
      //DebugPrint("\t\t\t\t unauthorized, not from inside the network : ");
      server.send ( 200, "text/plain", "ERROR you are not authorised, go back <--" );//send not found message
     }
}