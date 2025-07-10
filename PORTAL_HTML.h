
/* the portal consists of 2 pages starting with portal homepage
 *  this page has 3 buttons that are hidden or shown when applicable
 *  it depends om event 101 or 100 how this page looks 
 *  the 2th page is the wifiform, when filled in and saved, we get the 
 *  1st page, the ok buttom is shown 
 *  when we click it we return to the 1st page with the close button shown
 *  and the ip info
 *  after clicking the close button, we get the last page, the confirmation that
 *  a restart is taking place.
 *  
 */

const char PORTAL_HOMEPAGE[] PROGMEM = R"=====( 
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
<div id="msect"><center>
<br><h2>HANSIART WIFI CONFIG</h2>
<div class="divstijl"><center>
<br><h3>device mac adres : {ma}</h3>
<form action='wifiForm' method='get'>
<div hadden><button class="bt_small">SETUP WIFI</button></div>
</form><br><br>
 
<div id='close' hidden><form action='wifiClose' method='get'>
<button class='bt_small'>CLOSE</button>
</form></div>
<br><br><div>
<div hodden><a style="width:30vw;" onclick="return confirm('erase all flash except the program. Are you sure?')" href="/ERASE_FLASH">ERASE FLASH</a></div>
<br><br>
<irame>
<br>

</body></html>
  )=====";

 

 

// below here was tested allright
 
// ******************************************************************************************
//                           THE WIFI FORM
// ******************************************************************************************
const char PORTAL_WIFIFORM[] PROGMEM = R"=====( 
<body>
<style>
.q{float: right;width: 64px; text-align: right;}
a{font-size:14px; text-decoration:none;}
.mi{height:30px;}
@media only screen and (max-width: 800px) {
.inp6 {width:180px; } }
</style>
<script>
function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}
function showdiv() { 
  document.getElementById('submitdiv').style.display = 'block';
  setTimeout(function(){ window.location.href='/wifiConfirm'; }, 3000 );
}
</script>
<div id="msect" style='background:lightblue;'>
<center>
<div id='submitdiv' style='opacity:1.0; color:white; position:fixed; top:0; left:0; height:100%; width:100%; background-color:rgba(100,100,100,0.75);z-index:1000; display:none;'>
<br><br><br><h1>trying to connect..</h1><br>
</div>

<H2>HANSIART CONFIG PORTAL </H2></center></div>
</div>
<div id="msect"><center>
  <div class="divstijl"><center>
  aplijst
  <br>
  <div style='border: 3px solid #f03467; background:lightyellow; width:99%'>
  <form method='get' action='wifiCon'>
  <center><table><tr><td style='width:35%;'>wifi network<td><input class="inp6" id='s' name='s' placeholder='SSID' required></input></td></tr>
  <tr><td>wifi passwd<td><input class="inp6" id='p' name='p' type='text' placeholder='password' required></input></tr>
  <tr><td>admin passwd<td><input  name='pw' class="inp6"  value='{pw}' pattern='.{4,10}' title='between 4 en 10 chars'></input></tr> 
  <tr><td>security level<td><input type='number' min='0' max="9" name='sl' value='{sl}'></input></tr> 
  </table><br/>
  <button style="background-color:#009933;color:#fff; font-size:30px; width:100%;" type='submit' onclick='showdiv()' style='float:left'>save</button>

  </form></div>
</div></body></html>
  )====="; 

const char PORTAL_CONFIRM[] PROGMEM = R"=====( 
<body style='font-size:16px;'>
<div id='msect'><center>
<h1>  CONNECT TO WIFI</h1><br><br><br>
<strong>please wait until the led has flashed and is on again, than click OK</strong><br><br>
<br><br>In case of troubles you can disconnect and reconnect from the AP now.<br><br>
<form action='/home' method='get'><button class='bt_small' type='submit'>OK</button></form>
<br><br>
</body></html>
  )=====";

const char PORTAL_LIST[] PROGMEM = R"=====( 
  <div class="mi"><a href="#p" onclick="c(this)">{v}</a>&nbsp;<span class="q {i}">{r}dBm</span></div>
  )=====";       
