//<script type="text/javascript" src="SECURITY"></script>

const char LOGPAGE [] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP32-ECU</title>
<meta http-equiv="refresh" content="60">
<meta name="viewport" content="width=device-width, initial-scale=1">

<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<style>
#lijst {
  font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
  border-collapse: collapse;
  width: 380px;
  font-size:14px;
  border: 1px solid;
  text-align: left;
}
#lijst td  {
padding-left: 10px;
border: 1px solid;
}

#lijst tr:nth-child(even){background-color: #f2f2f2;
  border: 1px solid #ddd;
  padding: 6px;
}
#lijst th {
  padding-top: 5px;
  padding-bottom: 5px;
  padding-left: 10px;
  text-align: left;
  background-color: #4CAF50;
  color: white;
}
.th1 { width:30%%; }
.th2 { width:20%%; }
.th3 { width:50%%; }
tr {height:20px;}

@media only screen and (max-width: 600px) {
#lijst{ font-size:12px; width: 320px;}
tr {width:94vw;}
.th1 { width:25%%; }
.th2 { width:15%%; }
}
</style>
<script type="text/javascript" src="SECURITY"></script>
<script>function cl() {window.location.href='/MENU';}</script>
</head>
<body><center>
<div id='msect'>
<div id='menu'><a href="/MENU" class='close'>&times;</a></div>

<kop>ESP-ECU JOURNAL</kop>

<div class='divstijl'><center>
<br>
<table id='lijst'><tr><th class='th1'>Time</th><th class='th2'>Type</th><th class='th3'>Description</th></tr>
%rows%
</table></center></div></body></html>
)=====";

// ************************************************************************************
//                      U P D A T E    L O G
// ************************************************************************************
 void Update_Log(int what, const char* message) {
  char nu[14];
  // when the log is full we start overwriting with the first row 
        sprintf(nu,"%d-%d:%d:%d ", day(), hour(), minute(), second());
        
        strcpy( Log_Events[logNr].date, nu );
                              
        Log_Events[logNr].kind = what;
        
        strcpy( Log_Events[logNr].message, message );

        logNr++;

        if (logNr >= Log_MaxEvents)
        {
            logNr = 0;//start again
            Log_MaxReached = true;
        }
}


//void Clear_Log(AsyncWebServerRequest *request) {
//
//    if(!checkRemote( request->client()->remoteIP().toString()) ) {
//        if(logNr != 0) {
////        for (int i=0; i <= Log_MaxEvents; i++) {
////           Log_date[20][0] = '\0';
////           Log_kind[20][0] = '\0';
////           Log_message[20][0] = '\0'; 
////        }
//        logNr = 0;//start again
//        Log_MaxReached = false;     
//        //Serial.println("log cleared");   
//        }
//    } 
//}



String putList(const String& var)
{

  if(var == "rows") 
  {
    Serial.println("found rows, logNr = " + String(logNr));
 
   char content[1536] = {0};
   char temp1[80]={0}; // 14 + 
   char temp2[8]={0};
   //char temp2[13];
   byte Log_Count = 0;
   Log_MaxReached ? Log_Count = Log_MaxEvents : Log_Count = logNr;  // determine if the max number of event is reached
   int j = logNr;
   // the rows 0-logNr are the recent updates, are printed from logNr to 0
   // so first we print the recent from logNr -> null (j=logNr) 
   // and next the old ones from maxnr -> logNr 
   for ( int i = 1; i <= Log_Count; i++ ) {
      j--; //  this is the index of the newest record in the array
      if (j ==-1) j = Log_MaxEvents - 1; // if we are below the first index of the array ,we start at the last
   
   switch ( Log_Events[j].kind ) {
      case 1:
         strncpy( temp2, "system\0",  7 ) ;
         break;
      case 2:
         strncpy( temp2, "zigbee\0",  7 ) ;
         break;
      case 3:
         strncpy( temp2, "mqtt\0",    5 ) ;
         break;
      case 4:
         strncpy( temp2, "pairing\0", 8 ) ;
        }  
      // One table line
      sprintf(temp1,"<tr><td>%s</td><td>%s</td><td>%s</td>", Log_Events[j].date, temp2, Log_Events[j].message );
      strcat(content, temp1);
      }

   return content;
  }
return String();
}
