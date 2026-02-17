//<link rel="stylesheet" type="text/css" href="/STYLESHEET">
const char menu_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP32-C3-DIMMER</title>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>
body {font-family:arial; font-size: 12px;}
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
<script>
function close() {
window.location.href='/';
}
</script>

<body>
<div class='container'>
<div class='nav'>
<span class='close'><a href="/">&times;</a></span>
<br><br></div>
<center>
<br><br>
<h2>ESP32 DIMMER MENU</h2><br><br>
<div class="nav">

<table><tr>
<td style="width:120px"><a href='/TIMERCONFIG'>timers</a></td><td style="width:60px"></td><td style="width:120px">
<a href='/LOGPAGE'>journal</a></<td></tr>
<tr><tr>
<tr><td><a href='/SETTINGS'>settings</a></td><td></td>
<td><a href='/ABOUT'>info</a></td></tr>
<tr><tr>
<tr><td><a onclick="return confirm('are you sure?')" href='/REBOOT'>reboot</a></td><td></td>
<td><a onclick="return confirm('reset wifi, are you sure?')" href='/STARTAP?'>reset wifi</a></td><tr>

</table>
</div></div></div>
</body></html>

  )rawliteral";