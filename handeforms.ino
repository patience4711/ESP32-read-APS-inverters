void handleForms(AsyncWebServerRequest *request) {
     //every form submission is handled here
     // we find out which form with a parameter present 
     String serverUrl = request->url().c_str();
     Serial.println("serverUrl = " + serverUrl); // this is /submitform 

     if(request->hasParam("ecuid")) {
         // received form basisconfig
         strcpy(ECU_ID, request->arg("ecuid").c_str());
         strcpy(userPwd, request->arg("pw1").c_str());
         pollOffset = request->arg("offs").toInt();  
  
//BEWARE CHECKBOX
         if(request->hasParam("pL")) { Polling = true; } else { Polling = false;}
         basisConfigsave();  // alles opslaan
         return; 
     } else 

      if(request->hasParam("longi")) {
        // received the geoconfig form
        longi = request->getParam("longi")->value().toFloat();
        lati = request->getParam("be")->value().toFloat();
        strcpy(gmtOffset, request->getParam("tz")->value().c_str());
        // a checkbox has only a parameter when checked
        if(request->hasParam("ts")) zomerTijd = true;  else  zomerTijd = false;
        wifiConfigsave();
        actionFlag=25; // recalculate with these settings 
        return;
          
     } else
     if(request->hasParam("mqtAdres")) {
        // form mosquitto received
  strcpy( Mqtt_Broker  , request->getParam("mqtAdres")   ->value().c_str() );
  strcpy( Mqtt_Port    , request->getParam("mqtPort")    ->value().c_str() );
  strcpy( Mqtt_outTopic, request->getParam("mqtoutTopic")->value().c_str() );
  strcpy( Mqtt_Username, request->getParam("mqtUser")    ->value().c_str() );
  strcpy( Mqtt_Password, request->getParam("mqtPas")     ->value().c_str() );
  strcpy( Mqtt_Clientid, request->getParam("mqtCi")     ->value().c_str() );  
  Mqtt_stateIDX = request->arg("mqidx").toInt(); //values are 0 1 2
  Mqtt_Format = request->arg("fm").toInt(); //values are 0 1 2 3 4 5
        mqttConfigsave();  // 
        actionFlag=24; // reconnect with these settings
        return;
     } 
     
     // if we are here something was wrong, no parameters found
     request->send(200, "text/html", "no valid form found");
}    
