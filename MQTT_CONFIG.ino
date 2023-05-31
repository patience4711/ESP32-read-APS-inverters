const char MQTTCONFIG[] PROGMEM = R"=====(
<body>
<div id='msect'>
<div id='bo'></div>
  <div id='help'>
  <span class='close' onclick='sl();'>&times;</span><h3>MOSQUITTO HELP</h3>
   <b>json format:</b><br>
  f0: mosquitto disabled<br>  
  f1: {"inv_serial":"408000158215","idx":879,"nvalue":0,"svalue":"5.2"}<br>
  f2: {"inv_serial":"408000158215","temp":"19.3","p0":"0.0","p1":"5.2","energy":"en_total"}<br>
  f3: {"inv_serial":"408000158211","acv":68.2,"freq":50.0,"temp":18.0,"dcv":[36.8,37.0],"dcc":[4.3,3.0],"pwr":[123.4,123.5],"en":[174.35,178.44]}<br>
  f4: {"inv_serial":"408000158211","acv":68.2,"freq":50.0,"temp":18.0,"ch0":[36.8,4.3,123.4,174.35],"ch1":[37.0,3.0,123.5,178.44],"totals":[power,energy]}
  <br>Format 3 and 4 have the retainflag set.
  <br><br> 
  
  <b>send topic:</b><br> 
  <br>If topic ends with '/' the idx of the inverter is added.

  <br><br>
  <b>receive topic:</b><br>The topic where will be subscribed for incoming messages.<br><br>
  <b>mqtt_username en password</b><br>Optional, these can be left empty.
  <br><br>

  </div>
</div>
<div id='msect'>
  <kop>MOSQUITTO CONFIGURATION</kop>
</div>
<div id='msect'>
  <div class='divstijl'>
  <center>
  <form id='formulier' method='get' action='MQTTconfig' oninput='showSubmit();'><table>
  
  
   <tr><td>format:&nbsp<td><select name='fm' class='sb1' id='sel'>
    <option value='0' fm_0>disabled</option>
    <option value='1' fm_1>format 1</option>
    <option value='2' fm_2>format 2</option>
    <option value='3' fm_3>format 3</option>
    <option value='4' fm_4>format 4</option>
    </select>
  <tr><td >address<td><input class='inp6' name='mqtAdres' value='{mqttAdres}' size='31' placeholder='broker adres'></tr>
  <tr><td >port<td><input class='inp2' name='mqtPort' value='{mqttPort}' size='31' placeholder='mqtt port'></tr>
  <tr><td>receive topic:&nbsp<td><input class='inp6' name='mqtinTopic' value='{mqttinTopic}' placeholder='mqtt topic send' length='60'></tr>
  <tr><td>send topic:&nbsp<td><input class='inp6' name='mqtoutTopic' value='{mqttoutTopic}' placeholder='mqtt topic receive' length='60'></tr>
  <tr><td>username:&nbsp<td><input class='inp4' name='mqtUser' value='{mqtu}' size='4' length='4'></td></tr>
  <tr><td>password:&nbsp<td><input class='inp4' name='mqtPas' value='{mqtp}' size='4' length='4'></td></tr>
 
  </form>
  </td></table>
  </div><br>
</div>
<div id='msect'>
  <ul>
  <li id='sub'><a href='#' onclick='submitFunction("/SW=BACK")'>save</a></li>
  <li><a href='/MENU'>done</a>
  <li><a href='#' onclick='helpfunctie()'>help</a>
  <li><a href='/MQTT_TEST' >test</a></ul>
  <br>
</div>
</body></html>
)=====";


void zendPageMQTTconfig() {
   //DebugPrintln("we are at zendPageMQTTconfig");
//toSend = FPSTR(HTML_HEAD);
//toSend.replace("tieTel", swname );
toSend = FPSTR(HTML_HEAD);
toSend += FPSTR(MQTTCONFIG);  

  //altijd de mqtt gegevens terugzetten
 
//if (Mqtt_Enabled) { toSend.replace("#check", "checked");}
toSend.replace("{mqttAdres}", String(Mqtt_Broker));
toSend.replace("{mqttPort}", String(Mqtt_Port));
toSend.replace("{mqttinTopic}", String(Mqtt_inTopic));
toSend.replace("{mqttoutTopic}", String(Mqtt_outTopic));
toSend.replace("{mqtu}", String(Mqtt_Username) );
toSend.replace("{mqtp}", String(Mqtt_Password) );

switch (Mqtt_Format) {
 case 0:
    toSend.replace("fm_0", "selected");
    break;
 case 1:
    toSend.replace("fm_1", "selected");
    break;
 case 2:
    toSend.replace("fm_2", "selected");
    break;
 case 3:
    toSend.replace("fm_3", "selected");
    break;
 case 4:
    toSend.replace("fm_4", "selected");
    break;
  }
}

void handleMQTTconfig(AsyncWebServerRequest *request) {

  //collect serverarguments
  strcpy( Mqtt_Broker  , request->getParam("mqtAdres")   ->value().c_str() );
  strcpy( Mqtt_Port    , request->getParam("mqtPort")    ->value().c_str() );
  strcpy( Mqtt_inTopic , request->getParam("mqtinTopic") ->value().c_str() );
  strcpy( Mqtt_outTopic, request->getParam("mqtoutTopic")->value().c_str() );
  strcpy( Mqtt_Username, request->getParam("mqtUser")    ->value().c_str() );
  strcpy( Mqtt_Password, request->getParam("mqtPas")     ->value().c_str() );

  Mqtt_Format = request->arg("fm").toInt(); //values are 0 1 2

  if( diagNose != 0 ) consoleOut("saved mqttconfig");
  mqttConfigsave();  // 
  actionFlag=24; // reconnect with these settings
}
