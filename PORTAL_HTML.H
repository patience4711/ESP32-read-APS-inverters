
const char PORTAL_STYLESHEET[] = R"=====(
.table td{padding:.5em;text-align:left}
.table tbody>:nth-child(2n-1){background:#ddd}
.btn,h2{font-size: 20px}
.btn{width:300px;height:60px;background:#0066FF; outline: none;}
.btn2{width:260px;height:60px;background:#CC00FF; outline: none; font-size: 14px}
#together {position: absolute; display: inline-block; margin;left:10px; width:330px;} 

.c{text-align: center;} div,input{padding:5px;font-size:1em;} 
input{width:95%;} 
body{ text-align: center; font-family:verdana;} 
button{border:0;border-radius:0.3rem;background-color:#009933;color:#fff;line-height:2.4rem;font-size:30px;width:100%;}  
.q{float: right;width: 64px;text-align: right;} 
.l{background: }
.btn a{text-decoration: none}
  )=====";

//const char PORTAL_HEAD[] PROGMEM = R"=====(
//<!DOCTYPE html><html><html lang=\"en\"><head><meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
//<title>hansiart wificonfig</title>
//<style>
//.table td{padding:.5em;text-align:left}
//.table tbody>:nth-child(2n-1){background:#ddd}
//.btn,h2{font-size: 20px}
//.btn{width:300px;height:60px;background:#0066FF; outline: none;}
//.btn2{width:260px;height:60px;background:#CC00FF; outline: none; font-size: 14px}
//#together {position: absolute; display: inline-block; margin;left:10px; width:330px;} 
//
//.c{text-align: center;} div,input{padding:5px;font-size:1em;} 
//input{width:95%;} 
//body{ text-align: center; font-family:verdana;} 
//button{border:0;border-radius:0.3rem;background-color:#009933;color:#fff;line-height:2.4rem;font-size:30px;width:100%;}  
//.q{float: right;width: 64px;text-align: right;} 
//.l{background: }
//.btn a{text-decoration: none}
//</style>
//</head>
//  )=====";

const char PORTAL_PAGE[] PROGMEM = R"=====( 
<!DOCTYPE html><html><html lang=\"en\"><head><meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<title>hansiart wificonfig</title>
<link rel="stylesheet" type="text/css" href="/PORTAL_STYLE">
</head>
<body><script>
function c(l){
  document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();
  }
</script>
<script>
if (window.location.hostname != "192.168.4.1") {
   window.location.href = 'http://192.168.4.1'; 
} 
</script>
<h2>HANSIART WIFI CONFIG</h2>
<h3>device mac adres : {ma}<h3>
<form action='wifiForm' method='get'>
<div hadden><button class="btn">WIFI CONFIGURATION</button></div>
</form><br><br>
<div id='close' hidden><form action='close' method='get'>
<button class='btn'>CLOSE</button>
</form><br>
<br><br></div>
<strong>Currently not connected to a wifi network!</strong><br>
<br><br>
<div id='erase' hidden><a onclick="return confirm('all files are erased, are you sure?')" href='/SPIFFS_ERASE'><button class='btn2'>delete files</button></a></div>
<br><br>
<div id='static' hidden><a onclick="return confirm('erase static ip settings, are you sure?')" href='/STATIC_ERASE'><button class='btn2'>delete static ip</button></a></div>
</body></html>
  )=====";
// ******************************************************************************************
//                                the page with the wifi form
// ******************************************************************************************
const char WIFI_PAGE[] PROGMEM = R"=====( 
<!DOCTYPE html><html><html lang=\"en\"><head><meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<title>hansiart wificonfig</title>
<link rel="stylesheet" type="text/css" href="/PORTAL_STYLE">
</head>
<body>
<script>
function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}
function showdiv() { document.getElementById('submitdiv').style.display = 'block';}
</script>

<div id='submitdiv' style='opacity:1.0; color:white; position:fixed; top:0; left:0; height:100%; width:100%; background-color:rgba(100,100,100,0.75);z-index:1000; display:none;'>
<br><br><br><h1>please wait while trying to connect..</h1></div>
<div style='background:lightblue; width:380px;'><H3 style='text-align: center;'> HANSIART CONFIG PORTAL </h3></div>
<div id='together' style='top:88px; overflow-y:scroll; height:300px; left:35px; border: 3px solid #ff3467;'><h3>wifi networks</h3>
aplijst
</div>
<div id='together' style='top:410px; left:35px; border: 3px solid #f03467; background:lightyellow;'>
<form method='get' action='wifiCon'>
<table><tr><td style='width:120px;'>wifi netwerk<td><input id='s' name='s' length=32 placeholder='SSID' required></input></td></tr>
<tr><td>wifi passwd<td><input id='p' name='p' length=64 type='text' placeholder='password' required></input></tr>
<tr><td>admin passwd<td><input  name='pw' length='11' placeholder='max. 10 tekens' value='{pw}' pattern='.{4,10}' title='tussen 4 en 10 karakters'></input></tr> 
</table><br/>
<button type='submit' onclick='showdiv()' style='float:left'>save</button>
</form></div></br></body></html>
  )=====";

//<script type='text/javascript'>setTimeout(function(){ window.location.href='/CONCLUDE'; }, 3000 ); </script>
const char PORTAL_CONFIRM[] PROGMEM = R"=====( 
<!DOCTYPE html><html><html lang=\"en\"><head><meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<title>hansiart wificonfig</title>
<link rel="stylesheet" type="text/css" href="/PORTAL_STYLE">
</head>
<body style='font-size:16px;'>
<h1>  CONNECT TO WIFI</h1><br><br>
<strong>{text}</strong><br><br>
click on OK<br><br><br>
<form action='/' method='get'><button class='btn'>OK</button></form>
<br><br>
</body></html>
  )=====";  

const char PORTAL_LIST[] PROGMEM = R"=====( 
  <div><a href="#p" onclick="c(this)">{v}</a>&nbsp;<span class="q {i}">{r}dBm</span></div>
  )====="; 
