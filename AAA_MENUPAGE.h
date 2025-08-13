const char MENUPAGE[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP32-ECU</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/STYLESHEET">

<style>
#menu2 a:link, #menu2 a:visited {
  width: 98px;
}
</style>
<script type="text/javascript" src="SECURITY"></script>
</head><body>
<div id='msect'>
<div id='menu'>
    <a  href="/" class='close' >&times;</a>  
</div>
<br><br>
<kop>ESP32 ECU MENU</kop><br><br>
<div class='divstijl'><center><br>
<div id="menu2">
  <table><tr>
  <td style="width:100px"><a href='/ABOUT'>system info</a>
  </td><td style="width:60px"></td>
  <td style="width:100px"><a href='/GEOCONFIG'>time config</a></td></tr>

  <tr><td><a href='/LOGPAGE'>journal</a></td><td></td>
  <td><a href='/MQTT'>mosquitto</a></td></tr>
  
  <tr><td><a href='/INV_CONFIG'>inverters</a></td><td></td>
  <td><a href='/FWUPDATE'>fw update</a></td></tr>
  
  <tr><td><a href='/CONSOLE'>console</a></td><td></td>
  <td><a onclick="return confirm('are you sure?')" href='/REBOOT'>reboot</a></td></tr> 

   <tr><td><a href='/BASISCONFIG'>settings</a></td><td></td>
  <td><a onclick="return confirm('are you sure?')" href='/STARTAP'>start AP</a></td></tr> 
  </table>
</div></div></div>
</body></html>
  )=====";
