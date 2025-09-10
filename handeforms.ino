void handleForms(AsyncWebServerRequest *request) 
{
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
  //strcpy( Mqtt_Clientid, request->getParam("mqtCi")     ->value().c_str() );  
  Mqtt_stateIDX = request->arg("mqidx").toInt(); //values are 0 1 2
  Mqtt_Format = request->arg("fm").toInt(); //values are 0 1 2 3 4 5
        mqttConfigsave();  // 
        actionFlag=24; // reconnect with these settings
        return;
  } else
// the request is something like pMax=200 MPW=0
  if(request->hasParam("pMax")) // name of the hidden input
  {    // because of the hidden input named maxPower we know we received the setPower form 
      Serial.println("found pMax");
      procesId = 3;
      // check that the form has the right params ( should be pMax and MPW)
      int params = request->params();
      Serial.print("Number of params: ");
      Serial.println(params);   
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        Serial.print("Param name: ");
        Serial.println(p->name());
        Serial.print("Param value: ");
        Serial.println(p->value());
      }
       
       int Inv = request->arg("INV").toInt();
       Serial.println("the form is for inverter " + String(Inv));
       desiredThrottle[Inv] = request->getParam("pMax")->value().toInt();
       // write the desired throttlevalue in preferences
       //String key = "maxPwr" + String(welke);
       //preferences.putUInt(key.c_str(), maxPw);
       //desiredThrottle =
       Serial.println("throttle set to = " + String(desiredThrottle[Inv])); 
       actionFlag=240 + Inv; // save the settings and send zigbee to inverter
       Serial.println("actionFlag set to " + String(actionFlag));
       //Serial.println("setting the return url to /details?inv=");
       String toReturn = "/details?inv=" + String(Inv);
       strcpy(requestUrl, toReturn.c_str() ); 
       Serial.println("requestUrl = " + String(requestUrl));
       return;

  }

     // if we are here something was wrong, no parameters found
     request->send(200, "text/html", "no valid form found");
}    
