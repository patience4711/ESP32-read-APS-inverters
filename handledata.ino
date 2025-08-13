void handleDataRequests(AsyncWebServerRequest *request) 

  {
  //consoleOut"handleDataRequest");
  Serial.println("handleDataRequest the request is " + String(requestUrl));
  if( request->hasArg("Power") )
    {
     consoleOut("found POWER");  
     int i = atoi(request->arg("inv").c_str()) ;
     AsyncResponseStream *response = request->beginResponseStream("application/json");
     JsonDocument root;
     if(Inv_Data[i].en_total > 0) { // only possible when was polled this day
        root["eN"] = round2(Inv_Data[i].en_total/(float)1000); // rounded
     } else {
        root["eN"] = "n/a" ;
     }
    //now populate the powervalues in an array "p":[p0, p1, p2, p3]
     for(int z = 0; z < 4; z++ ) 
        {
            //is the panel connected? if not put n/e
            if( ! Inv_Prop[i].conPanels[z] ) { root["p"][z] = "n/e";  }  
            // so the panel is connected, is the inverter polled?
            else if (polled[i]) 
            {
                //polled, we put a value
                  root["p"][z] = round1(Inv_Data[i].power[z]) ;
            }   else {
                // not polled, we put n/a
                  root["p"][z] = "n/a";
            }
        }

      serializeJson(root, * response);
      request->send(response);
      return; 
    } else

      if( request->hasArg("General") )
    { 
    consoleOut("found General");
    char temp[15]={0};
    uint8_t remote = 0;
    if(checkRemote( request->client()->remoteIP().toString()) ) remote = 1; // for the menu link
    uint8_t night = 0; 
    if(!dayTime) { night = 1; }
    
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument root; //(160);
    //JsonObject root = doc.to<JsonObject>();
    root["cnt"] = inverterCount;    
    root["rm"] = remote;
    root["st"] = zigbeeUp;
    root["sl"] = night;    
    serializeJson(root, * response);
    request->send(response);
    return;
    } else 
// the get.Data request, send by detailspage    
    if(request->hasArg("Inverter"))  // this is the get.Data
    { 
    consoleOut("found Inverter");
    // this is used by the detailspage and for http requests      
    // set the array into a json object
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument doc;  // size was 768
    JsonObject root = doc.to<JsonObject>();
    int i;
    i = (request->arg("Inverter").toInt()) | iKeuze;
    if( i < inverterCount) { // check that this is a valid value
      root["inv"] = i;
      root["name"] = Inv_Prop[i].invLocation;
      root["polled"] = polled[i];
      root["type"] = Inv_Prop[i].invType;
      root["serial"] = Inv_Prop[i].invSerial;
      root["sid"] = Inv_Prop[i].invID;
      root["freq"] = round1(Inv_Data[i].freq);
      root["temp"] = round1(Inv_Data[i].heath);
      root["acv"] = round1(Inv_Data[i].acv);
      root["sq"] = round1(Inv_Data[i].sigQ);     
      root["pw_total"] = round1(Inv_Data[i].pw_total);
      root["en_total"] = round2(Inv_Data[i].en_total/(float)1000); // rounded
      root["pwMax"] = Inv_Prop[i].maxPower;
      if(Inv_Prop[i].throttled == true) root["throttled"] = 1; else root["throttled"] = 0;
      
      for(int z = 0; z < 4; z++ ) 
      {
         if(Inv_Prop[i].conPanels[z]) // is this panel connected?
         {
              root["dcv"][z] = round1(Inv_Data[i].dcv[z]);
              root["dcc"][z] = round1(Inv_Data[i].dcc[z]);
              root["pow"][z] = round1(Inv_Data[i].power[z]);
              root["en"][z] = round2(en_saved[i][z]); //rounded
         }   else {
              root["dcv"][z] = "n/e";
              root["dcc"][z] = "n/e";
              root["pow"][z] = "n/e";
              root["en"][z] = "n/e";
         }
     }
     serializeJson(root, * response);
     request->send(response);
     } else {
       String term = "unknown inverter " + String(i);
       request->send(200, "text/plain", term);
     }
    return;
    }
    // if we are here no maching request was found
       String term = "invalid request";
       request->send(200, "text/plain", term);
 }    
