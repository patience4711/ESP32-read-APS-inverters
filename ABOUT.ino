const char ABOUT [] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta http-equiv="refresh" content="180">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta charset='utf-8'>
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<title>ESP32-ECU</title>
<style>
table, th, td {border: 1px solid blue; width:font-size:12px; background-color: #ffffcc;border-collapse: collapse; }
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
char page[1600] = {0};
char temp[100]={0};
strcpy_P(page, ABOUT);

  int seconds = millis()/1000;
  int minutens = seconds/60;
  int urens = minutens/60;
  int dagen = urens/24;
 
  strcat(page, "<br><table><tr><TH colspan='2'> SYSTEM INFORMATION</th></tr>" );
  strcat(page, "<tr><td>firmware version<td>");strcat(page,VERSION);strcat(page,"</tr>");
  if ( timeRetrieved ) strcat(page,"<tr><td>time retrieved<td>yes</tr>"); else strcat(page,"<tr><td>time retrieved<td>n</tr>");
  sprintf(temp, "<tr><td>systemtime<td> %02d:%02d:%02d %04d-%02d-%02d " , hour(), minute(), second() , year(),month(),day());
  switch (dst) {
    case 1: strncat(temp, "summertime</td>", 19 ); break;
    case 2: strncat(temp, "wintertime</td>", 19 ); break;
    case 0: strncat(temp, "no dst set</td>", 19 ); 
  }
  strcat(page, temp);
  
  sprintf(temp, "<tr><td>system uptime <td>%d days %02d:%02d:%02d </td>", dagen, urens-dagen*24, minutens-urens*60,seconds%60);
  strcat(page, temp);
  sprintf(temp, "<tr><td>wifi signalstrength<td>%lddB</td>", WiFi.RSSI());
  strcat(page, temp);

    sprintf(temp, "<tr><td>ESP CHIP ID / MAC<td>%s / ...%06X</td>", getChipId(true).c_str(),getChipId(true).toInt() );
    strcat(page, temp);
    sprintf(temp, "<tr><td>Free heap<td>%ld KB (%ld bytes)</td>", esp_get_free_heap_size()/1024,esp_get_free_heap_size() );
    strcat(page, temp);
    sprintf(temp, "<tr><td>StackHighWaterMark<td>%ld KB (%ld bytes)</td>", uxTaskGetStackHighWaterMark(NULL)/1024,uxTaskGetStackHighWaterMark(NULL) );
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
    
    sprintf(temp,"<tr><td>polling from<td> %02d:%02d:%02d %04d-%02d-%02d", hour(switchonTime), minute(switchonTime),second(switchonTime),year(switchonTime),month(switchonTime),day(switchonTime) );
    strcat(page, temp);
    sprintf(temp,"<tr><td>polling to<td>%02d:%02d:%02d %04d-%02d-%02d", hour(switchoffTime), minute(switchoffTime),minute(switchoffTime), year(switchoffTime),month(switchoffTime),day(switchoffTime) );
    strcat(page, temp);
    
    sprintf(temp, "<tr><td>Auto Polling<td>%s</td>" , (Polling)?"true":"false" );
    strcat(page, temp);
    sprintf(temp, "<tr><td>Polling Mode<td>%s</td>" , (dayTime)?"Day":"Night" );
    strcat(page, temp);
    sprintf(temp, "<tr><td>Polling active<td>%s</td>" , (dayTime*Polling)?"true":"false" );
    strcat(page, temp);
    sprintf(temp, "<tr><td>Last Poll<td>%d sec / every %d sec</td>" , Polling*dayTime*(millis()-laatsteMeting)/1000,pollingtime );
    strcat(page, temp);

    sprintf(temp, "<tr><td>securityLevel<td>%d</td>" , securityLevel );
    strcat(page, temp);

    sprintf(temp, "<tr><td>ZB resetCounter<td>%d</td></table>" , resetCounter );
    strcat(page, temp);

    //Serial.println("page = " + String(page));
    Serial.println("length = " + String(strlen(page)));
 
    request->send(200, "text/html", page); //send the html code to the client
    memset(page, 0, sizeof(page));
}
