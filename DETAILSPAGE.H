const char DETAILSPAGE [] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<meta charset='utf-8'>
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<title>ESP32-ECU</title>
<style>
table, th, td {border: 1px solid blue; font-size:16px; text-align:center} 
body {margin:10px 10px 0px 20px; font-family:'lato',Verdana,Sans-serif;font-size:16px;}
tr {height:35px;}
.pol (display:none;}
</style>
<script>
function loadScript() {
  loadData();
  setInterval(function ld() {
    //console.log("setting interval");  
    loadData();
  },90000);
}  
function loadData() {
  console.log("function loadData");
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
      //console.log("answer = " + antwoord);
      var obj = JSON.parse(antwoord);

     // first display general data serial, shortid, type, 
      //var sid = obj.serial; 
      document.getElementById("ivn").innerHTML = obj.inv;
      document.getElementById("snr").innerHTML = obj.serial;
      var sid = obj.sid; //if 0x0000 not paired
      document.getElementById("sid").innerHTML = sid;
      var t = obj.type;
      var type ="YC600";
      if(t=="1"){type="QS1";}  if(t=="2") {type = "DS3";}
      document.getElementById("tp").innerHTML = type;      
      //console.log("sid = " + sid);
      if(sid != "0x0000" || sid == "") {
        
      var polled = obj.polled;
         if( polled=="1" ) { 
 
            document.getElementById("dcvc").style.display = "block"; 
            document.getElementById("npo").style.display = "none";

            document.getElementById("sq").innerHTML = obj.sq;
            document.getElementById("acv").innerHTML = obj.acv;
            document.getElementById("tmp").innerHTML = obj.temp;
            document.getElementById("fr").innerHTML = obj.freq;
            document.getElementById("v0").innerHTML = obj.dcv0;
            document.getElementById("v1").innerHTML = obj.dcv1;
            document.getElementById("v2").innerHTML = obj.dcv2;
            document.getElementById("v3").innerHTML = obj.dcv3;
            document.getElementById("c0").innerHTML = obj.dcc0;
            document.getElementById("c1").innerHTML = obj.dcc1;
            document.getElementById("c2").innerHTML = obj.dcc2;
            document.getElementById("c3").innerHTML = obj.dcc3;
                      
            } else {
            //display the not polled message
              document.getElementById("npo").style.display = "block";
              document.getElementById("dcvc").style.display = "none";
            } 
       } else {
        document.getElementById("npa").style.display = "block"; 
       }
     
    }    
  };
  xhttp.open("GET", "get.Inverter", true);
  xhttp.send();
}
</script>

</head><body onload='loadScript()'><center>
<h3>ESP-ECU DATA INVERTER&nbsp;<span id="ivn"></span></h3>
<table>
<colgroup><col span="1" style="background:#00cc66; width:100px;"><col span="1" style="background:#99ffcc; width:120px;"></colgroup>
<tr><td> Type <td id="tp">n/a</td></tr>
<tr><td> serialnr <td id="snr">n/a</td></tr>
<tr><td> ID <td id="sid">n/a</td></tr>
<tr><td>  signal quality <td id="sq">n/a</td></tr>
<tr><td>  ac voltage<td id="acv">n/a</td></tr>
<tr><td>  temperature<td id="tmp">n/a</td></tr>    
<tr><td>  frequency<td id="fr">n/a</td></tr>
</table>

<h4>INVERTER OUTPUT</h4>

<div id="dcvc" style="display:none;">
<table><tr style='background:#ccff33;'><td>unit</td><td>panel 0<td>panel 1<td>panel 2<td>panel 3</tr>
<colgroup><col span='1' style='background:#00cc66; width:80px;'><col span='4' style='background:#99ffcc; width:80px;'></colgroup>
<tr><td>dc voltage<td id="v0">n/a</td><td id="v1">n/a</td>
<td id="v2">n/a</td><td id="v3">n/a</td></tr>
<tr><td>dc voltage<td id="c0">n/a</td><td id="c1">n/a</td>
<td id="c2">n/a</td><td id="c3">n/a</td></tr>
</table></div>

<span id="npa" style="display:none;"><h2>this inverter is not paired!</h2></span>
<span id="npo" style="display:none;"><h2>this inverter is not polled!</h2></span>

<div style="position:fixed; text-align:center; width:100%;"><br><a href="/"><button style="width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;">close</button></a></div> 

</body></html>
)=====";


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
//
//</head><body><center>
//)=====";
//
//void sendPageDetails(AsyncWebServerRequest *request) {
//DebugPrintln("we are on DETAILSPAGE() ");
////polled[i] = true; // for testing
//String pageSend = FPSTR(DETAILSPAGE);
//
////DebugPrintln("infopage in de string gezet) ");
//pageSend += "<h3>ESP-ECU DATA INVERTER " + String(iKeuze) + "</h3>";
//
//
//if(String(Inv_Prop[iKeuze].invID) != "0x0000" ) {
//    String i_type="YC600";
//    if(Inv_Prop[iKeuze].invType == 2) i_type="DS3";
//    if(Inv_Prop[iKeuze].invType == 1) i_type="QS1";
//    pageSend += "<h4>INVERTER PROPERTIES</h4><table>";
//    pageSend += "<colgroup><col span='1' style='background:#00cc66; width:100px;'><col span='1' style='background:#99ffcc; width:120px;'></colgroup>";
//
//    pageSend += "<tr><td>Type<td>" + i_type + "</tr>";
//    pageSend +=" <tr><td> serialnr <td >" + String(Inv_Prop[iKeuze].invSerial) + "</tr>";
//    pageSend += "<tr><td>ID<td>" + String(Inv_Prop[iKeuze].invID) + "</tr>";
//    
//    if ( polled[iKeuze] ) {  // when polled we add the data  
//        pageSend += "<tr><td>  signal quality <td>" + String(Inv_Data[iKeuze].sigQ) + "%</tr>";
//        pageSend += "<tr><td>  ac voltage<td>" + String(Inv_Data[iKeuze].acv) + "</tr>";
//        pageSend += "<tr><td>  temperature<td>" + String(Inv_Data[iKeuze].heath) + "</tr>";    
//        pageSend += "<tr><td>  frequency<td>" + String(Inv_Data[iKeuze].freq)= "</tr></table>";    
//         
//        pageSend += "<h4>INVERTER OUTPUT</h4>";
//     
//        pageSend += "<table><tr style='background:#ccff33;'><td>unit</td><td>panel 0<td>panel 1<td>panel 2<td>panel 3</tr>";
//        pageSend += "<colgroup><col span='1' style='background:#00cc66; width:80px;'><col span='4' style='background:#99ffcc; width:80px;'></colgroup>";
//
//        // the row for the voltage
//        pageSend += "<tr><td>dc voltage<td>" + String(Inv_Data[iKeuze].dcv[0]) + "<td>" + String(Inv_Data[iKeuze].dcv[1]);
//        if(Inv_Prop[iKeuze].invType == 1) {  
//          pageSend += "<td>" + String(Inv_Data[iKeuze].dcv[2]) + "<td>" + String(Inv_Data[iKeuze].dcv[3]) + "</tr>";
//          } else { pageSend += "</tr>";}
//
//        // the row for the current
//        pageSend += "<tr><td>dc current<td>" + String(Inv_Data[iKeuze].dcc[0]) + "<td>" + String(Inv_Data[iKeuze].dcc[1]);
//        if(Inv_Prop[iKeuze].invType == 1) {  
//          pageSend += "<td>" + String(Inv_Data[iKeuze].dcc[2]) + "<td>" + String(Inv_Data[iKeuze].dcc[3]) + "</tr>";
//          } else { pageSend += "</tr>";}
//        // end table
//        pageSend += "</table>";
//         } else { // not polled, end table and display message
//         pageSend += "</table><br><br><br><h2>this inverter is not polled!</h2>";
//         }
//    } else { // when not paired there was no table
//    pageSend += "<br><br><br><h2>this inverter is not paired!</h2>";
//    }
//
//  pageSend += "<div style='position:fixed; text-align:center; width:100%;'><br><a href='/'><button style='width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;'>close</button></a></div>"; 
//  pageSend += "</body></html>";
//  request->send(200, "text/html", pageSend);
//}
//
//
////const char DETAILS [] PROGMEM = R"=====(
////<!DOCTYPE html><html><head>
////<meta name="viewport" content="width=device-width, initial-scale=1">
////<link rel="stylesheet" type="text/css" href="/STYLESHEET">
////<meta charset='utf-8'>
////<link rel="icon" type="image/x-icon" href="/favicon.ico" />
////<title>ESP32-ECU</title>
////<style>
////table, th, td {border: 1px solid blue; font-size:16px; text-align:center} 
////body {margin:10px 10px 0px 20px; font-family:'lato',Verdana,Sans-serif;font-size:16px;}
////tr {height:35px;}
////</style>
////
////</head><body><center>
////<h3>ESP-ECU DATA INVERTER<span id="ivn"></span></h3>
////<h4>INVERTER PROPERTIES</h4><table>
////<colgroup><col span='1' style='background:#00cc66; width:100px;'><col span='1' style='background:#99ffcc; width:120px;'></colgroup>
////<tr><td> Type <td id="tp"><n/a</td></tr>
////<tr><td> serialnr <td id="snr"></td></tr>
////<tr><td> ID <td id="sid"></td></tr>
////<tr><td>  signal quality <td id="sq"><n/a></td></tr>
////<tr><td>  ac voltage<td id="acv">n/a</td></tr>
////<tr><td>  temperature<td id="tmp">n/a</td></tr>    
////<tr><td>  frequency<td id="frq"></td></tr></table>
//////id's are tp, snr, sid, sq, tmp, frq
////<h4>INVERTER OUTPUT</h4>
////
////<table id="dcvc" style="display:none;"><tr style='background:#ccff33;'><td>unit</td><td>panel 0<td>panel 1<td>panel 2<td>panel 3</tr>
////<colgroup><col span='1' style='background:#00cc66; width:80px;'><col span='4' style='background:#99ffcc; width:80px;'></colgroup>
////<tr><td>dc voltage<td id="v0">n/a</td><td id="v1">n/a</td>
////<td id="v2">n/a</td><td id="v3">n/a</td></tr>
////<tr><td>dc voltage<td id="c0">n/a</td><td id="c1">n/a</td>
////<td id="c2">n/a</td><td id="c3">n/a</td></tr>
////</table>
////
////<span id="npa" style="display:none;"><br><br><br><h2>this inverter is not paired</h2></span>
////<span id="npo" style="display:none;"><br><br><br><h2>this inverter is not polled!</h2></span>
////
////<div style='position:fixed; text-align:center; width:100%;'><br><a href='/'><button style='width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;'>close</button></a></div> 
////
////<script>
////// put all data in place
////function getData(invnr) {
////  console.log("term = " + term);
////  var xhttp = new XMLHttpRequest();
////  xhttp.onreadystatechange = function() {
////    if (this.readyState == 4 && this.status == 200) {
////      var antwoord = this.responseText;
////      console.log("answer = " + antwoord);
////      var obj = JSON.parse(antwoord);
////
//// //vars are polled, sid, type, sq, acv, tm, fr, v0, v1, v2, v3, c0, c1, c2, c3 
//// //id's are tp, snr, sid, sq, tmp, frq    
////      var polled = obj.polled;
////      if(polled=="1") { document.getElementById("dcvc").style.display = "block"; }
////      document.getElementById("sn").innerHTML = obj.snr;
////      var sid = obj.sid; //if 0x0000 not paired
////      document.getElementById("sid").innerHTML = sid;
////      //var type = obj.type;
////      document.getElementById("tp").innerHTML = obj.tp;      
////      //var sq = obj.sq;
////      document.getElementById("sq").innerHTML = obj.sq;
////      var acv = obj.acv;
////      document.getElementById("acv").innerHTML = obj.acv;
////      //var tm = obj.tm; //temperature
////      document.getElementById("tmp").innerHTML = obj.tmp;
////      var fr = obj.fr;
////      document.getElementById("fr").innerHTML = obj.fr;
////      var v0 = obj.v0;
////      document.getElementById("v0").innerHTML = obj.v0;
////      var v1 = obj.v1;
////      document.getElementById("v1").innerHTML = obj.v1;
////      var v2 = obj.v2;
////      document.getElementById("v2").innerHTML = obj.v2;
////      var v3 = obj.v3;
////      document.getElementById("v3").innerHTML = obj.v3;
////      var c0 = obj.c0;
////      document.getElementById("c0").innerHTML = obj.c0;
////      var c1 = obj.c1;
////      document.getElementById("c1").innerHTML = obj.c1;
////      var c2 = obj.c2;
////      document.getElementById("c2").innerHTML = obj.c2;
////      var c3 = obj.c3;
////      document.getElementById("c3").innerHTML = obj.c3;
////      // first check if polled           
////      if(polled != "1") {
////        document.getElementById("npo").style.display = "block";
////      } 
////      if(sid == "0x0000" || sid == "") {
////       document.getElementById("npa").style.display = "block";
////       } 
////    }    
////  }
////  xhttp.open("GET", get.Details, true);
////  xhttp.send();
////}
////</body></html>
////)=====";
//
