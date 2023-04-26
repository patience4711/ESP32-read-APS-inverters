//const char DETAILSPAGE [] PROGMEM = R"=====(
//<!DOCTYPE html><html><head>
//<meta name="viewport" content="width=device-width, initial-scale=1">
//<meta charset='utf-8'>
//<link rel="icon" type="image/x-icon" href="/favicon.ico" />
//<title>ESP32-ECU</title>
//<style>
//table, th, td {border: 1px solid blue; width:font-size:12px;} 
//body {margin:10px 10px 0px 20px; font-family:'lato',Verdana,Sans-serif;font-size:12px;}
//</style>
//<script type="text/javascript" src="DETAILSSCRIPT"></script>
//</head><body onload='loadData()'><center>
//<h3>ESP-ECU DATA INVERTER<span id="ivn"></span></h3>
//<h4>INVERTER PROPERTIES</h4>
//</body></html>
//)=====";
//
//const char DETAILS_SCRIPT[] PROGMEM = R"=====(
//
//function loadData() {
//  console.log("function loadData");
//  var xhttp = new XMLHttpRequest();
//  xhttp.onreadystatechange = function() {
//    if (this.readyState == 4 && this.status == 200) {
//      var antwoord = this.responseText;
//      console.log("answer = " + antwoord);
//      var obj = JSON.parse(antwoord);
//
//      var polled = obj.polled;
//      if(polled=="1") { document.getElementById("dcvc").style.display = "block"; }
//      var sid = obj.sid; 
//      document.getElementById("sid").innerHTML = sid;
//    }    
//  };
//  xhttp.open("GET", "get.Details", true);
//  xhttp.send();
//}
//)=====";
//const char DETAILSPAGE [] PROGMEM = R"=====(
//<!DOCTYPE html><html><head>
//<meta name="viewport" content="width=device-width, initial-scale=1">
//<link rel="stylesheet" type="text/css" href="/STYLESHEET">
//<meta charset='utf-8'>
//<link rel="icon" type="image/x-icon" href="/favicon.ico" />
//<title>ESP32-ECU</title>
//<style>
//table, th, td {border: 1px solid blue; font-size:16px; text-align:center} 
//body {margin:10px 10px 0px 20px; font-family:'lato',Verdana,Sans-serif;font-size:16px;}
//tr {height:35px;}
//</style>
//<script>
//
//setInterval(function ld() {
//console.log("setting interval");  
//getDeta();
//},15000);
//  
//function loadData() {
//  console.log("function loadData");
//  var xhttp = new XMLHttpRequest();
//  xhttp.onreadystatechange = function() {
//    if (this.readyState == 4 && this.status == 200) {
//      var antwoord = this.responseText;
//      console.log("answer = " + antwoord);
//      var obj = JSON.parse(antwoord);
//
//      var polled = obj.polled;
//      if(polled=="1") { document.getElementById("dcvc").style.display = "block"; }
//      var sid = obj.sid; 
//      document.getElementById("sid").innerHTML = sid;
//    }    
//  };
//  xhttp.open("GET", "get.Details", true);
//  xhttp.send();
//}
//</script>
//
//</head><body onload='loadData()'><center>
//<h3>ESP-ECU DATA INVERTER<span id="ivn"></span></h3>
//<h4>INVERTER PROPERTIES</h4><table>
//<colgroup><col span="1" style="background:#00cc66; width:100px;"><col span="1" style="background:#99ffcc; width:120px;"></colgroup>
//<tr><td> Type <td id="tp"><n/a</td></tr>
//<tr><td> serialnr <td id="snr"></td></tr>
//<tr><td> ID <td id="sid"></td></tr>
//<tr><td>  signal quality <td id="sq"><n/a></td></tr>
//<tr><td>  ac voltage<td id="acv">n/a</td></tr>
//<tr><td>  temperature<td id="tmp">n/a</td></tr>    
//<tr><td>  frequency<td id="frq"></td></tr></table>
//
//<h4>INVERTER OUTPUT</h4>
//
//<table id="dcvc" style="display:none;"><tr style='background:#ccff33;'><td>unit</td><td>panel 0<td>panel 1<td>panel 2<td>panel 3</tr>
//<colgroup><col span='1' style='background:#00cc66; width:80px;'><col span='4' style='background:#99ffcc; width:80px;'></colgroup>
//<tr><td>dc voltage<td id="v0">n/a</td><td id="v1">n/a</td>
//<td id="v2">n/a</td><td id="v3">n/a</td></tr>
//<tr><td>dc voltage<td id="c0">n/a</td><td id="c1">n/a</td>
//<td id="c2">n/a</td><td id="c3">n/a</td></tr>
//</table>
//
//<span id="npa" style="display:none;"><br><br><br><h2>this inverter is not paired</h2></span>
//<span id="npo" style="display:none;"><br><br><br><h2>this inverter is not polled!</h2></span>
//
//<div style="position:fixed; text-align:center; width:100%;"><br><a href="/"><button style="width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;">close</button></a></div> 
//
//</body></html>
//)=====";

 //vars are polled, sid, type, sq, acv, tm, fr, v0, v1, v2, v3, c0, c1, c2, c3 
 //id's are tp, snr, sid, sq, tmp, frq 
 //{"polled":"1","snr":"408000158218","sid":"0x0000","type":"0","sq":"44.3%","acv":"221.1v","tm":"11.0°C","fr":"50.0hz","v0":"34.7","c0":"0.8","v1":"34.3","c1":"0.8","v2":"n/e","c2":"n/e","v3":"n/e","c3":"n/e"}
 


/*
      document.getElementById("sn").innerHTML = obj.snr;
      var sid = obj.sid; //if 0x0000 not paired
      document.getElementById("sid").innerHTML = sid;
      //var type = obj.type;
      document.getElementById("tp").innerHTML = obj.tp;      
      //var sq = obj.sq;
      document.getElementById("sq").innerHTML = obj.sq;
      //var acv = obj.acv;
      document.getElementById("acv").innerHTML = obj.acv;
      //var tm = obj.tm; //temperature
      document.getElementById("tmp").innerHTML = obj.tmp;
      //var fr = obj.fr;
      document.getElementById("fr").innerHTML = obj.fr;
      //var v0 = obj.v0;
      document.getElementById("v0").innerHTML = obj.v0;
      //var v1 = obj.v1;
      document.getElementById("v1").innerHTML = obj.v1;
      //var v2 = obj.v2;
      document.getElementById("v2").innerHTML = obj.v2;
      //var v3 = obj.v3;
      document.getElementById("v3").innerHTML = obj.v3;
      //var c0 = obj.c0;
      document.getElementById("c0").innerHTML = obj.c0;
      //var c1 = obj.c1;
      document.getElementById("c1").innerHTML = obj.c1;
      //var c2 = obj.c2;
      document.getElementById("c2").innerHTML = obj.c2;
      //var c3 = obj.c3;
      document.getElementById("c3").innerHTML = obj.c3;
      // first check if polled           
      if(polled != "1") {
        document.getElementById("npo").style.display = "block";
      } 
      if(sid == "0x0000" || sid == "") {
       document.getElementById("npa").style.display = "block";
      }
 */ 

// function loadTime() {
//  var xhttp = new XMLHttpRequest();
//  xhttp.onreadystatechange = function() {
//    if (this.readyState == 4 && this.status == 200) {
//      var antwoord = this.responseText;
//      var obj = JSON.parse(antwoord);
//      var hr = obj.uur;
//      var mn = obj.min;
//      var tijd= hr + ":" + mn;
//      document.getElementById('tijdveld').innerHTML=tijd;
//      }
//  };
//  xhttp.open("GET", "get.currentTime", true);
//  xhttp.send();
//}

//function getDeta() {
//  console.log("function getData");
//  var xhttp = new XMLHttpRequest();
//  xhttp.onreadystatechange = function() {
//    if (this.readyState == 4 && this.status == 200) {
//      var antwoord = this.responseText;
//      console.log("answer = " + antwoord);
//      var obj = JSON.parse(antwoord);
//
//      var polled = obj.polled;
//      if(polled=="1") { document.getElementById("dcvc").style.display = "block"; }
//      var sid = obj.sid; 
//      document.getElementById("sid").innerHTML = sid;
//    }    
//  };
//  //xhttp.open("GET", "get.Details", true);
//  xhttp.open("GET", "get.currentTime", true);
//  xhttp.send();
//}
