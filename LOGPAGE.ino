//const char HTML_LOGPAGE[] PROGMEM = R"=====(
//<!DOCTYPE html><html><head><meta charset='utf-8'>
//<title>ESP32-ECU</title>
//<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
//<link rel="icon" type="image/x-icon" href="/favicon.ico"/>
//<link rel="stylesheet" type="text/css" href="/STYLESHEET">
//<style>
//#lijst {
//  font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
//  border-collapse: collapse;
//  width: 88vw;
//  font-size:2vw;
//}
//#lijst td, #customers th {
//  border: 1px solid #ddd;
//  padding: 6px;
//}
//#lijst tr:nth-child(even){background-color: #f2f2f2;}
//
//#lijst th {
//  padding-top: 5px;
//  padding-bottom: 5px;
//  padding-left: 10px;
//  text-align: left;
//  background-color: #4CAF50;
//  color: white;
//}
//tr {height:20px;}
//
//@media only screen and (max-width: 600px) {
//#lijst{ font-size:12px; }
//tr {height:10px;}
//#lijst td, #customers th { padding: 2px;}
//}
//</style>
//<script type="text/javascript" src="SECURITY"></script>
//</head>
//<body>
//<center>
//<ul>
//<li><a href='/'>close</a></li>
//<li><a href='/ABOUT'>info</a></li>
//<li><a onclick="return confirm('are you sure?')" href='/CLEAR_LOG'>clear</a></li>
//</ul>
//<br><kop>ECU LOG</kop>
//<div class='divstijl'>
//<table><tr><td style='width:240px;'>Last refresh : !@@!<td>
//
//<button onClick='window.location.reload();' style='width: 100px' value='0'>Refresh</button></table><br>
//
//<table id='lijst'>
//<tr><th style='width:16vw;'>Time</th>
//<th style='width:16vw;'>Type</th>
//<th style='width:56vw;'>Command</th></tr> 
//<cont> 
//   
//</table></center></div>
//</div>
//</body>
//</html>
//)=====";
////</tr><tr><td>date </td><td>system</td><td>192.168.0.aaa.sss.ddd </td></tr>
//
//void zendPageLog( AsyncWebServerRequest *request ) {
//
// String uur = String(hour());
// if(hour() < 10) { 
//        uur = "0" + String(hour());
// } 
//  String minuten = String(minute());
//  if(minute() < 10) { 
//       minuten = "0" + String(minute());
// }
//
//String cont = "";
//cont += uur + " : " + minuten + " hr.";
//  char page[1536] = {0};
//  char temp[100]={0};
//  strcpy_P(page, ABOUT); 
//char toSEND=FPSTR(HTML_LOGPAGE);
//
////toSEND.replace("HANSIART" , String(swName));
//
//toSEND.replace("!@@!", cont);
//
////DebugPrintln(" zendlogpage :build eventlist");  
//  byte Log_Count = 0;
//  Log_MaxReached ? Log_Count = Log_MaxEvents : Log_Count = logNr;  // determine if the max number of event is already reached
//
//  int j = logNr;
//  String content = "";
//  for ( int i = 1; i <= Log_Count; i++ ) {
// //Serial.println("een regel van de lijst, nummer i = "); //Serial.println(i);    
//  j--; //  we get the index of the last record in the array
// //Serial.println("een regel van de lijst, nummer j = "); //Serial.println(j);
//  if (j ==-1) j = Log_MaxEvents - 1; // if we are under the first index of the array ,we go to the last
//  ////////////////// One table line ///////////////////
//sprintf(temp,"<tr><td>%s</td><td>%s</td><td>%s</td>", Log_list[j].Log_date, Log_list[j].Log_kind, Log_list[j].Log_message);
//
//  ////////////////// One table line ///////////////////
//  }
//  //Serial.print(content);
//  toSEND.replace("<cont>", content);
//  request->send(200, "text/html", toSEND); 
//}
//
// void //Update_log(String what, String message) {
//  String nu;
//        //DebugPrintln("updating the log");
//        if(what != "clear") {
//        nu = String(day()) + "-" + String(hour()) + ":" + String(minute()) + ":" + String(second());
//        } else { 
//          nu = "";
//          what = "";}
//        //DebugPrint("nu = "); DebugPrintln(nu);
//        Log_list[logNr].Log_date = nu;
//        Log_list[logNr].Log_kind = what;
//        //Log_list[logNr].Log_issued = who;
//        Log_list[logNr].Log_message = message;
//        logNr++;
//        if (logNr >= Log_MaxEvents)
//        {
//            logNr = 0;//start again
//            Log_MaxReached = true;
//        }
//}
////void //Update_log(String what, String message) {
////  String nu;
////        //DebugPrintln("updating the log");
////        if(what != "clear") {
////        nu = String(day()) + "-" + String(hour()) + ":" + String(minute()) + ":" + String(second());
////        } else { 
////          nu = "";
////          what = "";}
////        //DebugPrint("nu = "); DebugPrintln(nu);
////        Log_list[logNr].Log_date = nu;
////        Log_list[logNr].Log_kind = what;
////        //Log_list[logNr].Log_issued = who;
////        Log_list[logNr].Log_message = message;
////        logNr++;
////        if (logNr >= Log_MaxEvents)
////        {
////            logNr = 0;//start again
////            Log_MaxReached = true;
////        }
////}
//
//void Clear_Log() {
//    //Serial.println("clearing the log");
//    if(logNr != 0) {
//      String nu="";
//      String what="";
//      String message="";
//            for (int i=0; i <= Log_MaxEvents; i++) {
//            Update_Log("clear", "");
//            }
//         logNr = 0;//start again
//         Log_MaxReached = false;     
//        //Serial.println("log cleared");   
//    }
//}
