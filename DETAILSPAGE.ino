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
</style>

</head><body><center>
)=====";

void sendPageDetails(int i) {
DebugPrintln("we are on DETAILSPAGE() ");
//polled[i] = true; // for testing
toSend = FPSTR(DETAILSPAGE);
//toSend.replace("tieTel", swName );

//DebugPrintln("infopage in de string gezet) ");
toSend += "<h3>ESP-ECU DATA INVERTER " + String(i) + "</h3>";


if(String(Inv_Prop[i].invID) != "0x0000" ) {
    String i_type="YC600";
    if(Inv_Prop[i].invType == 2) i_type="DS3";
    if(Inv_Prop[i].invType == 1) i_type="QS1";
    toSend += "<h4>INVERTER PROPERTIES</h4><table>";
    toSend += "<colgroup><col span='1' style='background:#00cc66; width:100px;'><col span='1' style='background:#99ffcc; width:120px;'></colgroup>";

    toSend += "<tr><td>Type<td>" + i_type + "</tr>";
    toSend +=" <tr><td> serialnr <td >" + String(Inv_Prop[i].invSerial) + "</tr>";
    toSend += "<tr><td>ID<td>" + String(Inv_Prop[i].invID) + "</tr>";
    
    if ( polled[i] ) {  // when polled we add the data  
        toSend += "<tr><td>  signal quality <td>" + String(Inv_Data[i].sigQ) + "%</tr>";
        toSend += "<tr><td>  ac voltage<td>" + String(Inv_Data[i].acv) + "</tr>";
        toSend += "<tr><td>  temperature<td>" + String(Inv_Data[i].heath) + "</tr>";    
        toSend += "<tr><td>  frequency<td>" + String(Inv_Data[i].freq)= "</tr></table>";    
         
        toSend += "<h4>INVERTER OUTPUT</h4>";
     
        toSend += "<table><tr style='background:#ccff33;'><td>unit</td><td>panel 0<td>panel 1<td>panel 2<td>panel 3</tr>";
    toSend += "<colgroup><col span='1' style='background:#00cc66; width:80px;'><col span='4' style='background:#99ffcc; width:80px;'></colgroup>";

        // the row for the voltage
        toSend += "<tr><td>dc voltage<td>" + String(Inv_Data[i].dcv[0]) + "<td>" + String(Inv_Data[i].dcv[1]);
        if(Inv_Prop[i].invType == 1) {  
          toSend += "<td>" + String(Inv_Data[i].dcv[2]) + "<td>" + String(Inv_Data[i].dcv[3]) + "</tr>";
          } else { toSend += "</tr>";}

        // the row for the current
        toSend += "<tr><td>dc current<td>" + String(Inv_Data[i].dcc[0]) + "<td>" + String(Inv_Data[i].dcc[1]);
        if(Inv_Prop[i].invType == 1) {  
          toSend += "<td>" + String(Inv_Data[i].dcc[2]) + "<td>" + String(Inv_Data[i].dcc[3]) + "</tr>";
          } else { toSend += "</tr>";}
        // end table
        toSend += "</table>";
         } else { // not polled, end table and display message
         toSend += "</table><br><br><br><h2>inverter " + String(i) + " not polled!</h2>";
         }
    } else { // when not paired there was no table
    toSend += "<br><br><br><h2>inverter " + String(i) +" not paired</h2>";
    }

  toSend += "<div style='position:fixed; text-align:center; width:100%;'><br><a href='/'><button style='width:150px; height:50px;text-align:center; font-weight:bold; font-size:20px; background:#db97b9;'>close</button></a></div>"; 
  toSend += "</body></html>";
}
