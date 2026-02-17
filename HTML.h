
// //<link rel="icon" type="image/x-icon" href="/favicon.ico" />
// const char HTML_HEAD[] PROGMEM = R"=====(
// <!DOCTYPE html><html><head><meta charset='utf-8'>
// <title>ESP32-C3-DIMMER</title>
// <meta name="viewport" content="width=device-width, initial-scale=1">
// <link rel="stylesheet" type="text/css" href="/STYLESHEET">
// <link href="data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAu29YA0Q8YALTd3gBm4eMA0+blAIHh4wCa2tsAdd/gALUUnQC24+IAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAARERERERERERJkZmRGZkZlEkhIhIhIhKUSSFmFmFmEpRJIVUVUVUSlEkhV3EXcRGUSSZQAAB1YpRJJlczMHVilEiGWIOAdWKESIZYgIh1aIRIhliHiIiIhEiIiIVYhYiUSIZohmaCgpRIgiiCIoiClEiJmImZmJmUREREREREREQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" rel="icon" type="image/x-icon"/>
// <script type='text/javascript'>

// function showSubmit() {
// document.getElementById("sub").style.display = "block";
// }
// function submitFunction() {
// document.getElementById('formulier').submit();
// }
// function cl() {
// window.location.href='/MENU';
// }
// </script>
// </head>
// )=====";

const char settings_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Dimmer Config</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: sans-serif; background: #f0f2f5; padding: 10px; }
    
    .container { max-width: 500px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
       
    input[type=text], input[type=number], select { 
       width: 100%; padding: 10px; margin-top: 4px; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; 
    }
  th { background-color: lightblue; padding: 20px}
   
  .btn { background: #28a745; color: white; padding: 12px; border: none; width: 100%; margin-top: 25px; border-radius: 4px; cursor: pointer; font-size: 1em; font-weight: bold; }
  .btn:hover { background: #218838; }
  .nav { background: #eee; padding: 10px; }
  .close a {
color:red; 
text-decoration: none; 
float:right;   
font-size:40px;
font-weight: bold; 
}
</style>
</head>
<body>
<div class="container">
<div class="nav">
<span class='close'><a href="/">&times;</a></span>   
<br><br></div>
<form action="/saveSettings" method="POST">
      
<table style="width:100%">
<th colspan="2">Device & Security
<tr><td>device name<td><input type="text" name="dvName" value="%DVNAME%" maxlength="20">
<tr><td>admin passwd<td><input type="text" name="passwd" value="%PASSWD%" maxlength="10">
<tr><td>user passwd<td><input type="text" name="userPwd" value="%USERPWD%" maxlength="10">
<tr><td>security level<td><input type="number" name="secLvl" value="%SECLVL%" min="0" max="255">
<tr><td>default dim level<td><input type="number" name="duty" value="%DUTY%" min="0" max="1023"><tr><td>diagnose<td><input type="checkbox" name="diag" %DIAG_CHK%>

<tr><th colspan="2">Location and Time</th>
<tr><td>longitude<td><input type="number" name="longi" value="%LONGI%" step="0.001">
<tr><td>latitude <td><input type="number" name="lati" value="%LATI%" step="0.001">
<tr><td>GMT Offset<td><input type="text" name="gmt" value="%GMT%" maxlength="4">
<tr><td>summertime (DTS)<td><input type="checkbox" name="DTS" %DTS_CHK%>

<tr><th colspan="2">MQTT Settings</th>
<tr><td>broker address<td><input type="text" name="mqBrk" value="%MQBRK%" maxlength="29">
<tr><td>port<td><input type="number" name="mqPrt" value="1883">
<tr><td>in topic<td><input type="text" name="mqIn" value="%MQIN%" maxlength="25">
<tr><td><label>out topic<td><input type="text" name="mqOut" value="%MQOUT%" maxlength="25">
<tr><td>data format<td><input type="number" name="mqFmt" value="%MQFMT%" min="0" max="5">
<tr><td>domoticz idx<td><input type="text" name="mqIdx" maxlength="4" pattern="\[0-9]{1,4}" inputmode="numeric" value="%MQIDX%" >
<tr><td>Username<td><input type="text" name="mqUsr" value="%MQUSR%" maxlength="25">
<tr><td>Password<td><input type="text" name="mqPwd" value="%MQPWD%" maxlength="25">
<tr><td>client id<td><input type="text" name="mqCID" value="%MQCID%" maxlength="25">


</tr>
<tr><td colspan="2">
      <button type="submit" class="btn">SAVE SETTINGS </button></td></tr>
    </form></table>
  </div>
</body>
</html>
)rawliteral";

const char HTML_LOGPAGE[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP32-DIMMER</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
#lijst {
 font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
 border-collapse: collapse;
 width: 96%;
 font-size:18px;
}
#lijst td, #customers th {
 border: 1px solid #ddd;
 padding: 2px;
}
#lijst tr:nth-child(even){background-color: #f2f2f2;}

#lijst th {
 padding-top: 5px;
 padding-bottom: 5px;
 padding-left: 10px;
 text-align: left;
 background-color: #4CAF50;
 color: white;
}
#lijst tr {height: 20px;}
@media only screen and (max-width: 800px) {
#lijst{ font-size:12px; }
#lijst td, #lijst tr { padding:2px; height: 16px;}
}
.nav { background: #eee; padding: 10px; }
.nav a { margin-right: 10px; cursor: pointer; color: blue; text-decoration: none; font-size:20px;}
.container { max-width: 500px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
tr {height: 20px;}

.close a {
color:red; 
text-decoration: none; 
float:right;   
font-size:40px;
font-weight: bold; 
}
</style>
<script type="text/javascript" src="SECURITY"></script>
</head>
<body>
<div class='container'><center>
<div class='nav'>
<span class='close'><a href="/">&times;</a></span>
<br><br></div>
<h2>ESP32-DIMMER LOG</h2>
<table><tr><td style='width:240px;'>Last refresh : 18 : 31 hr.<td>
<button onClick='window.location.reload();' style='width: 100px' value='0'>Refresh</button></table><br>
<table id='lijst'>
<tr><th style='width:26%;'>Time</th>
<th style='width:26%;'>Type</th>
<th style='width:48%;'>Command</th></tr> 
<cont> 
  
</table></center></div>
</div>
</body>
</html>
)=====";

const char TIMERCONFIG_START[] PROGMEM = R"=====(

<body>

<div id='msect'>
<div id='bo'></div>
<div id="menu">
<a href="#" class='close' onclick='cl();'>&times;</a>
<a href="#" id="sub" onclick='submitFunction()'>save</a></div>
</div>
</div>

<div id='msect'>
  <center><h2>ESP32 DIMMER TIMERS </h2></center><br>
</div>

<div id='msect'>
<center><div class='divstijl'>

<form id='formulier' method='get' action='submitform' oninput='showSubmit()'>
  <center><irame name>
  </div>
<div><br></form>
<div id='msect'>
<div id="menu">
<b>timers</b>
<a href='/TIMER?welke=0' > 0</a>
<a href='/TIMER?welke=1' > 1</a>
<a href='/TIMER?welke=2' > 2</a>
<a href='/TIMER?welke=3' > 3</a>
</div>
</div>
</body></html>
 )=====";
                                                                        
//  const char TIMER_GENERAL[] PROGMEM = R"=====(
// <H1 style='color:#e207fa;'>TIMER {nr} </h1>
// <center><table>
// <tr><td style='width:180px;'>active?<td><input type='checkbox' name='ta' tActive>
// <tr><td>level (max 8192)<td><input class ='inp3' type='number' name='lev' value="{lev}" min="10" max="8192">
// </table>

// <table>
// <tr><td>on:<td><input class='inp4' type='time' name='inw' value='{onX}' title='hh:mm'>
// <td><select name='zonattaan' class='sb1'>
// <option value='0' zonattaan_0>absolute</option>
// <option value='1' zonattaan_1>before sunrise</option>
// <option value='2' zonattaan_2>after sunrise</option>
// <option value='3' zonattaan_3>before sunset</option>
// <option value='4' zonattaan_4>after sunset</option>

// </select></td></tr>
// <tr><td>off:<td><input class='inp4' name='uitw' type='time' value='{ofX}' title='hh:mm' >
// <td><select name='zonattuit' class='sb1'>
// <option value='0' zonattuit_0>absolute</option>
// <option value='1' zonattuit_1>before sunrise</option>
// <option value='2' zonattuit_2>after sunrise</option>
// <option value='3' zonattuit_3>before sunset</option>
// <option value='4' zonattuit_4>after sunset</option>

// </select></td></tr></table>
// <br><table>
// <thead><tr style='height:20px;'>
// <th> su </th>
// <th> mo </th>
// <th> tu </th>
// <th> we </th>
// <th> th </th>
// <th> fr </th>
// <th> sa </th>
// </tr></thead>
// <tr>
// <td><input type='checkbox' name='su' selzo>
// <td><input type='checkbox' name='mo' selma>
// <td><input type='checkbox' name='tu' seldi>
// <td><input type='checkbox' name='we' selwo>
// <td><input type='checkbox' name='th' seldo>
// <td><input type='checkbox' name='fr' selvr>
// <td><input type='checkbox' name='sa' selza>
// </table>
// </center><br>
//  )=====";