const char ABOUT [] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta http-equiv="refresh" content="180">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta charset='utf-8'>
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<title>ESP32-ECU</title>
<style>
table, th, td {border: 1px solid blue; width:font-size:12px; background-color: #ffffcc; }
th, td { padding-left: 4px; padding-right: 4px;}
th {background-color: #22ffb3;}
body {font-size:12px;} tr {height:26px;} 
</style>
<script type="text/javascript" src="SECURITY"></script>
<script> function cl() { window.location.href='/MENU'; }</script>
</head><body>
<div id='msect'><div id='menu'><a href="/MENU" class='close'>&times;</a></div>

<kop>ESP-ECU SYSTEM DATA</kop>

)=====";

void handleAbout(AsyncWebServerRequest *request) {
char page[1536] = {0};
char temp[100]={0};
strcpy_P(page, ABOUT);

  int minutens = millis()/60000;
  int urens = minutens/60;
  int dagen = urens/24;
 
  strcat(page, "<br><table><tr><TH colspan='2'> SYSTEM INFORMATION</th></tr>" );
  strcat(page, "<tr><td>firmware version<td>ESP32-ECU-v1_1</tr>");
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

    sprintf(temp, "<tr><td>ESP CHIP ID<td>%s</td>", getChipId(true).c_str() );
    strcat(page, temp);
    sprintf(temp, "<tr><td>Free heap<td>%ld bytes</td>", esp_get_free_heap_size() );
    strcat(page, temp);
    if ( Mqtt_Format != 0 ) { //bool == y en er is een mqtt adres, ja kijk dan of er een sensor is ingesteld
     sprintf( temp,"<tr><td>mqtt clientId<td>%ld</td>", getChipId(false).c_str());
     strcat(page, temp);
    // check if connected
    if ( MQTT_Client.connected() ) { //: add a dot
       sprintf(temp, "<tr><td>mqtt connected<td>%s</td>", Mqtt_Broker );
       } else {
       sprintf(temp, "<tr><td>mqtt status<td>not connected</td>");
       }
   } else {
       sprintf(temp, "<tr><td>mqtt status<td>not configured</td>");
   }
   strcat(page, temp);
    
 if(minute(switchonTime) < 10 ) {
       sprintf(temp,"<tr><td>polling from<td> 0%d:0%d hr", hour(switchonTime), minute(switchonTime) );
     } else {
       sprintf(temp,"<tr><td>polling from<td> 0%d:%d hr", hour(switchonTime), minute(switchonTime) );
     }

     if( minute(switchoffTime) < 10 ) {
        sprintf(temp,"<tr><td>polling to<td>%d:0%d hr", hour(switchoffTime), minute(switchoffTime) );
     } else {
        sprintf(temp,"<tr><td>polling to<td>%d:%d hr", hour(switchoffTime), minute(switchoffTime) );
     }

    sprintf(temp, "<tr><td>securityLevel<td>%d</td>" , securityLevel );
    strcat(page, temp);

    sprintf(temp, "<tr><td>ZB resetCounter<td>%d</td></table>" , resetCounter );
    strcat(page, temp);

    //Serial.println("page = " + String(page));
    Serial.println("length = " + String(strlen(page)));
 
    request->send(200, "text/html", page); //send the html code to the client
    memset(page, 0, sizeof(page));
}
