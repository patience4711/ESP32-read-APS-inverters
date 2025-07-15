//<script type="text/javascript" src="SECURITY"></script>
//<link rel="icon" type="image/x-icon" href="/favicon.ico" />
const char DETAILSPAGE [] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<meta charset='utf-8'><title>ESP-ECU</title>
<style>
.l_s table, .l_s th, .l_s td {border: 1px solid blue; font-size:14px; text-align:center} 
.l_s tr {height:30px;} 
.l_s .pol {display:none;}  
@media only screen and (max-width: 800px) {.l_s .l_s th, .l_s td { width:60px; font-size:12px; }} </style>
<script type="text/javascript">
function showSubmit() {
document.getElementById("sub").style.display = "block";
}
function submitFunction(a) {
console.log("submit throttle form");
document.getElementById('formulier').submit();
}
function loadScript(){loadData(),setInterval((function(){loadData()}),9e4)}
function loadData(){
console.log("function loadData");
var e=new XMLHttpRequest;
e.onreadystatechange=function(){
  if(4==this.readyState&&200==this.status){
    var e=this.responseText,n=JSON.parse(e);
    var pinput= document.getElementById("pMax");
    pinput.name="maxP" + n.inv;
    console.log("name changed to " + pinput.name );
    pinput.value = n.pwMax;
    console.log("value changed to " + pinput.value );
    document.getElementById("ivn").innerHTML=n.inv,document.getElementById("nm").innerHTML="<strong>"+n.name+"</strong>",document.getElementById("snr").innerHTML=n.serial;var t=n.sid;document.getElementById("sid").innerHTML=t;var d=n.type,c="YC600";if("1"==d&&(c="QS1"),"2"==d&&(c="DS3"),document.getElementById("tp").innerHTML=c,"0000"!=t||""==t)if("1"==n.polled){document.getElementById("dcvc").style.display="block",document.getElementById("npo").style.display="none",document.getElementById("sq").innerHTML=n.sq+" %",document.getElementById("acv").innerHTML=n.acv+" V",document.getElementById("tmp").innerHTML=n.temp+" &#8451;",document.getElementById("fr").innerHTML=n.freq+" Hz";for(let e=0;e<4;e++)"n/e"!=n.dcv[e]&&"n/a"!=n.dcv[e]&&(n.dcv[e]=n.dcv[e].toFixed(1)),"n/e"!=n.dcc[e]&&"n/a"!=n.dcc[e]&&(n.dcc[e]=n.dcc[e].toFixed(1));document.getElementById("v0").innerHTML=n.dcv[0],document.getElementById("v1").innerHTML=n.dcv[1],document.getElementById("v2").innerHTML=n.dcv[2],document.getElementById("v3").innerHTML=n.dcv[3],document.getElementById("c0").innerHTML=n.dcc[0],document.getElementById("c1").innerHTML=n.dcc[1],document.getElementById("c2").innerHTML=n.dcc[2],document.getElementById("c3").innerHTML=n.dcc[3],paintCells()}else document.getElementById("npo").style.display="block",document.getElementById("dcvc").style.display="none";else document.getElementById("npa").style.display="block"}},
    e.open("GET","get.Data?Inverter=0",!0),e.send()}
  
  function paintCells(){for(let e=0;e<4;e++)v="v"+e,c="c"+e,"n/e"==document.getElementById(v).innerHTML&&(document.getElementById(v).style="background-color:#a6a6a6"),"n/e"==document.getElementById(c).innerHTML&&(document.getElementById(c).style="background-color:#a6a6a6")}
</script><script type="text/javascript" src="SECURITY"></script>
</head><body onload='loadScript()'>
<div class="l_s">
<div id='msect'> 
<div id='menu'>
<a href='/' class='close'>&times;</a>
<a href="#" id="sub" style='background:green; display: none' onclick='submitFunction()'>save</a><br>
</div> 
<h3>ESP-ECU INVERTER :&nbsp; <span id="ivn"></span></h3>
<table><tr><td colspan="2" id="nm"></tr> <colgroup><col span="1" style="background:#00cc66; width:100px;"><col span="1" style="background:#99ffcc; width:120px;"></colgroup>
<tr><td> Type <td id="tp">n/a</td></tr> 
<tr><td> serialnr <td id="snr">n/a</td></tr> 
<tr><td> ID <td id="sid">n/a</td></tr> 
<tr><td> signal quality <td id="sq">n/a</td></tr>
<tr><td> ac voltage<td id="acv">n/a</td></tr>
<tr><td> temperature<td id="tmp">n/a</td></tr> 
<tr><td> frequency<td id="fr">n/a</td></tr>
<tr><td> throttle<td><form id='formulier' method='get' action='submitform'><input id='pMax' name='dummy' value="" type='number' min='0' max='500' step='100' class="inp3" oninput='showSubmit()'></input><input type='hidden'name='maxPower' value="111"></input></tr></form>
</table>

<h4>INVERTER OUTPUT</h4><div id="dcvc" style="display:none;"><table>
<tr style='background:#ccff33;'><td>unit</td><td>panel 0<td>panel 1<td>panel 2<td>panel 3</tr>
<colgroup><col span='1' style='background:#00cc66; width:80px;'><col span='4' style='background:#99ffcc; width:80px;'></colgroup>
<tr><td>dc voltage<td id="v0">n/a</td><td id="v1">n/a</td><td id="v2">n/a</td><td id="v3">n/a</td></tr>
<tr><td>dc current<td id="c0">n/a</td><td id="c1">n/a</td><td id="c2">n/a</td><td id="c3">n/a</td></tr>
</table></div><span id="npa" style="display:none;"><h2>this inverter is not paired!</h2></span><span id="npo" style="display:none;"><h2>this inverter is not polled!</h2></span><br><br></div></body></html> 
)=====";

