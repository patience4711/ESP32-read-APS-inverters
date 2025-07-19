// //61888FC697221800004800221800000FB0000140600050F14005D8A3011B9780FBFB065E67875F27000001710158FEFEconst char GEOCONFIG[] PROGMEM = R"=====(

// const char POWERCONFIG[] PROGMEM = R"=====(
// <body>
// <div id='msect'>
// <div id='menu'>
//     <a href="#" class='close' onclick='cl();'>&times;</a>
//     <a href="#" id="sub" style='background:green; display: none' onclick='submitFunction()'>save</a><br>
// </div>
// <br><kop>MAX INVERTER POWER</kop><br><br>
//   <div class='divstijl' style='height:52vh;'>
//   <form id='formulier' method='get' action='submitform' oninput='showSubmit()'>
//   <center><table>
//   <tr><td style='width:140px;>INVERTER<td style="width:30px"><td style="width: 60px">max power</td></tr>  
//   <ireame>
  
//   </div></div><br></form>
// <br></body></html>
// )=====";

// void zendPagePowerconfig( AsyncWebServerRequest *request ) {

//     String webPage = FPSTR(HTML_HEAD);
//     webPage += FPSTR(POWERCONFIG);
//     String list = ""; 
//   // now we make a list of the inverters
//   for(int z=0;z<inverterCount; z++){ // 0 to 8 == 9 inverters
//     list+="<tr><td>inverter " + String(z) + "<td><input name=\'maxPower\' type=\'hidden\'></input><input name=\'maxP" + String(z) + "\' type='\number'\ value=\'{#val + String(z) + "\'}</input>
//     </tr>" 
//   }
//   // now we replace the {val0} with Inv_Prop[0].maxPower
//    if(Inv_Prop[0].maxPower) list.replace("{val0}", Inv_Prop[0].maxPower)
//   //so now each row would look like <tr><td> inverter 0<td><input name='inv0' type='number' value='{#val1}'></input></tr>
//   for (int i = 0; i < inverterCount; i++) {
//     String placeholder = "{inv" + String(i) + "}"; // would be {inv0}
//     list.replace(placeholder, Inv_prop[i].maxPower);
//     }
// //now we put it in place
//   webpage.replace("<irame>", list);
//   //
//   request->send(200, "text/html", webPage);
//   webPage="";

// }

void setMaxPower(int which) 
{

consoleOut("sending the trrottle command");
int Scaled = Inv_Prop[which].maxPower * 16.59;
char powCommand[85];
char ecu_id_reverse[13];
ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
// first we convert the scaled maxPower value to hex, ( msb and lsb )
// and calculate the validation byte
  uint8_t msb = (Scaled >> 8) & 0xFF; // 0x02
  uint8_t lsb = Scaled & 0xFF;        // 0x58
  // Compute validation byte
  uint8_t s = (msb + lsb) & 0xFF;
  uint8_t vv = (s - 0x29) & 0xFF;
  Serial.println("for maxpower = " + String(Inv_Prop[which].maxPower) + " * 16.59, the values are ");
  Serial.printf("MSB: %02X, LSB: %02X, VV: %02X\n", msb, lsb, vv);

  snprintf(powCommand, sizeof(powCommand), "2401%s1414060000050F14%sFBFB06AA270000%02X%02X01%02XFEFE", Inv_Prop[0].invID, ecu_id_reverse, msb, lsb, vv);
  Serial.println("The raw powCommand = " + String(powCommand));
  // now we can send it
  sendZB(powCommand);

  errorCode = decodeGeneralAnswer();     
  switch( errorCode )
    {
        case 0:
                 yield();
                 break;
        default:
              consoleOut("request has errorcode " + String(errorCode)); 
    }


}

