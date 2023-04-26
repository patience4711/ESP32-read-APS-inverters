const char INFOPAGE [] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta charset='utf-8'>
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<title>ESP32-ECU</title>
<style>
table, th, td {border: 1px solid blue; width:font-size:12px;} 
body {margin:10px 10px 0px 20px; font-family:'lato',Verdana,Sans-serif;font-size:12px;}
</style>
<script>

function loadTime() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
           console.log("answer = " + antwoord);
      var obj = JSON.parse(antwoord);
      var hr = obj.uur;
      var mn = obj.min;
      var tijd= hr + ":" + mn;
      document.getElementById('tijdveld').innerHTML=tijd;
      }
  };
  xhttp.open("GET", "get.currentTime", true);
  xhttp.send();
}

</script>
</head><body onload='loadTime()'><center>
)=====";

void handleInfo(AsyncWebServerRequest *request) {
DebugPrintln("we are at handleInfopage() ");

String pageSend = FPSTR(INFOPAGE);
//pageSend.replace("tieTel", swName );

pageSend += "<h2>Hansiart ECU STATUS INFORMATION</h2>";

pageSend += "<div style='position:fixed; text-align:center; width:100%;'<br><a href='/MENU'><button style='width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;'>close</button></a></div>"; 

String zt = "summertime";
switch (dst) {
  case 2: zt="wintertime"; break;
  case 0: zt="no dst set"; break;
}

pageSend += "<br><br><br><br>system time = <span style='font-size:20px;font-weight:bold' id='tijdveld'></span> hr.&nbsp&nbsp " + zt + "<br>";

pageSend += "firmware version : " + String(VERSION) + "<br>";

pageSend += "time retrieved today : "; if ( timeRetrieved ) { pageSend += "yes<br>"; } else { pageSend += "no<br>"; }

//
long rssi = WiFi.RSSI();
pageSend += "the wifi signalstrength (rssi) = " + String(rssi) + "<br>";

if ( Mqtt_Format != 0 ) { //bool == y en er is een mqtt adres, ja kijk dan of er een sensor is ingesteld
// check if connected
    //String clientId = "ESPClient#";
    //String clientId = "ESP32-ECU";
    String clientId = getChipId() ;
    pageSend += "de mqtt clientId = : " + clientId + "<br>";    
    if ( MQTT_Client.connected() ) {
       pageSend += "status mqtt : connected to " + Mqtt_Broker + "<br>";
       } else {
       pageSend += "status mqtt : not connected<br>";
       }
    } else {
   pageSend += "mosquitto not configured<br>";
   pageSend += "check the mosquitto settings<br>";  
}

  int minutens = millis()/60000;
  int urens = minutens/60;
  int dagen = urens/24;
  pageSend += "system up time: " + String(dagen) + " days " + String(urens-dagen*24) + " hrs " + String(minutens - urens*60) + " min.<br> ";
  pageSend += "current errorCode = " + String(errorCode) + "<br>"; 



pageSend += "<br><br><table><tr><TH> ESP INFORMATION</th></tr>";
pageSend += "<tr><td>ESP CHIP ID nr: <td>" + getChipId().substring(10);
pageSend += "<td>min. heap size<td>" +  String(esp_get_minimum_free_heap_size()) + " bytes</tr>";
pageSend += "<tr><td>Free heap<td>" +  String(esp_get_free_heap_size()) + " bytes<td>remote IP<td>" + request->client()->remoteIP().toString() + "</table>";

pageSend += "<h4>variables dump</h4>";
pageSend += "value=" + String(value)  + "  inverterCount=" + String(inverterCount) + "  zigbeeUp=" + String(zigbeeUp) + "<br>";
pageSend += "switchonTime=" + String(switchonTime)  + "  switchoffTime=" + String(switchoffTime)+ "<br>";
pageSend += "unixtime=" + String(now()) + "<br>";
pageSend += "polled = " + String(polled[0]) + String(polled[1]) + String(polled[2]) + String(polled[3]) + String(polled[4]) + String(polled[5]) + String(polled[6]) + String(polled[7]) + String(polled[8]) + "<br>";
pageSend += "ZB resetCounter = " + String(resetCounter);
pageSend += "  pollOffset = " + String(pollOffset) + "  Mqtt_Format = " + String(Mqtt_Format) + "<br>";
pageSend += "  Polling = " + String(Polling) + "<br>";
#ifdef TEST
pageSend += "  testCounter = " + String(testCounter) + " inv0 type = " + String(Inv_Prop[0].invType);
#endif
pageSend += "<h3>Content filesystem :</h3>";

File root = SPIFFS.open("/");
File file = root.openNextFile();
while (file) {
    pageSend += String(file.name()) + "   size ";
    pageSend += String(file.size()) + "<br>";
    file = root.openNextFile();
}

 //DebugPrintln("end infopage "); 
 //DebugPrint("de lengte van pageSend na de infopage = "); //DebugPrintln( pageSend.length() );
 request->send(200, "text/html", pageSend); //send the html code to the client
}
