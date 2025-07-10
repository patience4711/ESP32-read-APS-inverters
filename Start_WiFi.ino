// ************************************************************************************
// *                        START wifi
// ************************************************************************************
void start_wifi() {
 WiFi.softAPdisconnect(true);
 WiFi.mode(WIFI_STA);
 Serial.println("starting wifi ");
 delay(1000);
//Serial.println("start wifi 3");
   WiFi.setHostname(getChipId(false).c_str()); 

//Serial.println("start wifi 4");

// WiFi.mode(WIFI_STA); // geen ap op dit moment 

// we gaan 10 pogingen doen om te verbinden
// met de laatst gebruikte credentials
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print("*");
     WiFi.begin();
     event+=1;
     if (event==10) {break;}
  }
// als het verbinden is mislukt gaan we naar het configportal  
  if (event>9) {
     event=0;
     Serial.println("\nWARNING connection failed");
     start_portal();
      } else {
     Serial.print("\nconnection success, ip = ");
     Serial.println(WiFi.localIP());
     }
   Serial.print("# connection attempts = ");  //Serial.println(event);
   event=0; // we kunnen door naar de rest
   //checkFixed();

   start_server();
}
//// *************************************************************************
////                      START THE SERVER 


//  // ********************************************************************
// //             check if there must come a static ip
// // ********************************************************************
// void checkFixed() {
//   // we come here only when wifi connected
//     char GATE_WAY[16]="";
//     IPAddress gat=WiFi.gatewayIP();
//     sprintf(GATE_WAY, "%d.%d.%d.%d", gat[0], gat[1], gat[2], gat[3]);
//     //DebugPrint("GATE_WAY in checkFixed = nu: "); //DebugPrintln(String(GATE_WAY));
//     //DebugPrint("static_ip in checkFixed = nu: "); //DebugPrintln(String(static_ip));

//     if (static_ip[0] != '\0' && static_ip[0] != '0') {
//       //DebugPrintln("we need s static ip  Custom STA IP/GW/Subnet");
//       IPAddress _ip,_gw,_sn(255,255,255,0); // declare 
//       _ip.fromString(static_ip);
//       _gw.fromString(GATE_WAY);//  if (ssid != "") {
//       WiFi.config(_ip, _gw, _sn);
//       //DebugPrintln(WiFi.localIP());
//   } else {
//       //DebugPrintln("trying to get rid of wificonfig");
//       WiFi.config(0u, 0u, 0u);     
//   }
// }

void loginBoth(AsyncWebServerRequest *request, String who) {
  String authFailResponse = "<h2>login failed <a href='/'>click here</a></h2>";
  if (who == "admin" ){
  const char* www_realm = "login as administrator."; 
    if (!request->authenticate("admin", pswd)) return request->requestAuthentication();
  }
  if (who == "both" ){
  const char* www_realm = "login as administrator or user."; 
    if (!request->authenticate("admin", pswd) && !request->authenticate("user", userPwd)) return request->requestAuthentication();
  }
}
