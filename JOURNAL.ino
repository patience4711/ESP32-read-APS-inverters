void handleLogPage( ) 
{
    String uur = String(hour());
    if(hour() < 10) { 
          uur = "0" + String(hour());
    } 
    String minuten = String(minute());
    if(minute() < 10) { 
          minuten = "0" + String(minute());
    }

    String cont = "";
    cont += uur + " : " + minuten + " hr.";
    //char page[1536] = {0};
    char temp[256]={0};
    //strcpy_P(page, ABOUT); 
    String toZend=FPSTR(HTML_LOGPAGE);

    toZend.replace("!@@!", cont);
    // this should be displayed with the last event above
    //DebugPrintln(" zendlogpage :build eventlist");  
    byte Log_Count = 0;
    Log_MaxReached ? Log_Count = Log_MaxEvents : Log_Count = logNr;  // determine if the max number of event is already reached

    int j = logNr;
    String content = "";
    char what[14];
    for ( int i = 1; i <= Log_Count; i++ ) 
    {
        consoleOut("one row of the list, nummer i = " + String(i)); //Serial.println(i);    
        j--; //  we get the index of the last record in the array
        //Serial.println("een regel van de lijst, nummer j = "); //Serial.println(j);
        if (j ==-1) j = Log_MaxEvents - 1; // if we are under the first index of the array ,we go to the last
        ////////////////// One table line ///////////////////
        //consoleOut("crash here?");
        //sprintf(temp,"<tr><td>%s</td><td>%s</td><td>%s</td>", Log_list[j].date, Log_list[j].kind, Log_list[j].message);
      
        switch(Log_list[j].kind)
        {
            case 1:
              strcpy(what, "system");
              break;
            case 3:
              strcpy(what,"mqtt");
              break;
             case 4:
              strcpy(what,"voice");
              break;
            case 5:
              strcpy(what,"webui");
              break;
            case 6:
            strcpy(what,"button");
              break;
            case 7:
              strcpy(what,"clear");
              break;           
            default:
              String toPaste = "timer " + String(Log_list[j].kind-20);
              strcpy(what, toPaste.c_str());
              break;
        }
     //consoleOut("update log with " + String(what));
     //consoleOut("lognr = " + String(logNr)); 
   
        // have only chars to sprintf, what is the converted kind
        snprintf(temp, sizeof(temp),"<tr><td>%s</td><td>%s</td><td>%s</td>",
          Log_list[j].date,
          what,
          Log_list[j].message
          );
        //consoleOut("temp = " + String(temp));
        content += String(temp);
        temp[0]='\0';
        ////////////////// One table line ///////////////////
    }
    //consoleOut("content=" + content);
    toZend.replace("<cont>", content);
    server.send(200, "text/html", toZend); 
}

void UpdateLog(int who, String message) 
{

    String date = String(day()) + "-" + String(hour()) + ":" + String(minute()) + ":" + String(second());
      //what = "";}
    strcpy(Log_list[logNr].date, date.c_str());
    Log_list[logNr].kind = who;
    strcpy(Log_list[logNr].message, message.c_str());
    consoleOut("log after udate");
    consoleOut("date = " + String(Log_list[logNr].date));
    consoleOut("kind = " + String(Log_list[logNr].kind));
    consoleOut("message = " + String(Log_list[logNr].message));
    logNr++;
    if (logNr >= Log_MaxEvents)
    {
        logNr = 0;//start again
        Log_MaxReached = true;
    }
}      

// void clearLog() {
//    //Serial.println("clearing the log");
//    if(logNr != 0) {
//      String nu="";
//      String what="";
//      String message="";
//            for (int i=0; i <= Log_MaxEvents; i++) {
//            UpdateLog(7, "");
//            }
//         logNr = 0;//start again
//         Log_MaxReached = false;     
//         consoleOut("log cleared");   
//    }
// }

// void handleLogClear()
// {
//    clearLog();
//    confirm("/LOGPAGE");
// }
