const char ABOUT [] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta charset='utf-8'>
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<title>ESP32-ECU</title>
<style>
table, th, td {border: 1px solid blue; width:font-size:12px;} 
body {margin:10px 10px 0px 20px; font-family:'lato',Verdana,Sans-serif;font-size:12px;}
</style>
<script type="text/javascript" src="SECURITY"></script>
</head><body onload='loadTime()'><center>
<h2>ESP32-ECU SYSTEM INFO</h2>
<div style='position:fixed; text-align:center; width:97%;'>
<a href='/'>
<button style='width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;'>close</button>
</a></div>
)=====";

void handleAbout(AsyncWebServerRequest *request) {
char page[1536] = {0};
char temp[100]={0};
strcpy_P(page, ABOUT);

  int minutens = millis()/60000;
  int urens = minutens/60;
  int dagen = urens/24;
 
  strcat(page, "<br><br><br><br><table><tr><TH colspan='2'> SYSTEM INFORMATION</th></tr>" );
  strcat(page, "<tr><td>firmware version<td>ESP32-ECU-v0_3d</tr>");
  if ( timeRetrieved ) strcat(page,"<tr><td>time retrieved<td>yes</tr>"); else strcat(page,"<tr><td>time retrieved<td>n</tr>");
  sprintf(temp, "<tr><td>systemtime<td> %d:%d " , hour(), minute());
  switch (dst) {
    case 1: strncat(temp, "summertime</td>", 19 ); break;
    case 2: strncat(temp, "wintertime</td>", 19 ); break;
    case 0: strncat(temp, "no dst set</td>", 19 ); 
  }
  strcat(page, temp);
  
  sprintf(temp, "<tr><td>system uptime<td>%d d %d h %d m </td>", dagen, urens-dagen*24, minutens-urens*60);
  strcat(page, temp);
  sprintf(temp, "<tr><td>wifi signalstrength<td>%lddB</td>", WiFi.RSSI());
  strcat(page, temp);



  //strcat(page, "<br><br><br><br><br>firmware version : ESP32-ECU-v0_3d<br>");

//  sprintf(temp, "systemtime = %d:%d " , hour(), minute());
//  switch (dst) {
//    case 1: strncat(temp, "summertime<br>", 14 ); break;
//    case 2: strncat(temp, "wintertime<br>", 14 ); break;
//    case 0: strncat(temp, "no dst set<br>", 14 ); 
//  }
//  strcat(page, temp);

//  strcat(page, "time retrieved today : "); 
//  if ( timeRetrieved ) strcat(page, "yes<br>"); else strcat(page, "no<br>");
//
//    sprintf(temp, "wifi signalstrength = %ld<br>", WiFi.RSSI());
//    strcat(page, temp);

//    if ( Mqtt_Format != 0 ) { //bool == y en er is een mqtt adres, ja kijk dan of er een sensor is ingesteld
//    // check if connected
//    sprintf(temp,"The mqtt clientId = : %s<br>", getChipId(false).c_str());
//    strcat(page,temp);    
//    if ( MQTT_Client.connected() ) { //: add a dot
//       sprintf(temp, "status mqtt : connected to %s<br>", Mqtt_Broker.c_str());
//       } else {
//       sprintf(temp,"status mqtt : not connected<br>");
//       }
//    } else {
//        sprintf(temp, "mosquitto not configured<br>check the mosquitto settings<br>");
//    }
//    strcat(page,temp);
//    int minutens = millis()/60000;
//    int urens = minutens/60;
//    int dagen = urens/24;
//    sprintf(temp, "system up time: %d days %d hrs %d min. ", dagen, urens-dagen*24, minutens-urens*60);
//    strcat(page,temp);

    //Serial.println("page = " + String(page));
    //Serial.println("length = " + String(strlen(page)));

    //strcat(page , "<br><br><table><tr><TH colspan='2'> SYSTEM INFORMATION</th></tr>" );

    sprintf(temp, "<tr><td>ESP CHIP ID<td>%s</td>", getChipId(true).c_str() );
    strcat(page, temp);
    //sprintf(temp, "<tr><td>stack size<td>%u bytes</td>", ESP.getFreeContStack() );
    //strcat(page, temp);
    sprintf(temp, "<tr><td>Free heap<td>%ld bytes</td>", esp_get_free_heap_size() );
    strcat(page, temp);
    //toSEND += request->client()->remoteIP().toString() + F("</table>");
    if ( Mqtt_Format != 0 ) { //bool == y en er is een mqtt adres, ja kijk dan of er een sensor is ingesteld
     sprintf( temp,"<tr><td>mqtt clientId<td>%ld</td>", getChipId(false).c_str());
     strcat(page, temp);
    // check if connected
    if ( MQTT_Client.connected() ) { //: add a dot
       sprintf(temp, "<tr><td>mqtt connected<td>%s</td>", Mqtt_Broker.c_str());
       } else {
       sprintf(temp, "<tr><td>mqtt status<td>not connected</td>");
       }
   } else {
       sprintf(temp, "mqtt status<br>not configured<br>");
   }
   strcat(page, temp);
    
    Serial.println("page = " + String(page));
    Serial.println("length = " + String(strlen(page)));

    sprintf(temp, "<tr><td>value<td> %d <tr><td>inverterCount<td>%d  <tr><td>zigbeeUp<td>%d</td>" , value, inverterCount, zigbeeUp);
    strcat(page, temp);

    sprintf(temp, "<tr><td>switchonTime<td> %ld<tr><td>switchoffTime<td>%d</td>" , (long)switchonTime, (long)switchoffTime);
    strcat(page, temp);

    sprintf(temp, "<tr><td>autopolling<td> %d<tr><td>polled<td> %d%d%d%d%d%d%d%d%d</td>" , Polling, polled[0], polled[1], polled[2],polled[3], polled[4], polled[5], polled[6], polled[7], polled[8]);
    strcat(page, temp);

    sprintf(temp, "<tr><td>securityLevel<td>%d</td>" , securityLevel );
    strcat(page, temp);

    sprintf(temp, "<tr><td>ZB resetCounter<td>%d</td></table>" , resetCounter );
    strcat(page, temp);
//Serial.println("4");
//#ifdef TEST
//toSEND += "  testCounter = " + String(testCounter) + " inv0 type = " + String(Inv_Prop[0].invType);
//#endif

    Serial.println("page = " + String(page));
    Serial.println("length = " + String(strlen(page)));

 
 request->send(200, "text/html", page); //send the html code to the client
}
