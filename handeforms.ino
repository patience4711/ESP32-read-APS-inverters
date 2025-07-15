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
  strcpy( Mqtt_Clientid, request->getParam("mqtCi")     ->value().c_str() );  
  Mqtt_stateIDX = request->arg("mqidx").toInt(); //values are 0 1 2
  Mqtt_Format = request->arg("fm").toInt(); //values are 0 1 2 3 4 5
        mqttConfigsave();  // 
        actionFlag=24; // reconnect with these settings
        return;
  } else

      if(request->hasParam("maxPower")) // name of the hidden input
      // because of the hidden input named maxPower we know we received the setPower form 
      Serial.println("found maxPower");
      // check thast the form has the right params ( should be maxPx and naxPower)
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
       
      {
          int add; // to determin which inverter we are editing
          // if(request->hasParam("maxP0")) {Inv_Prop[0].maxPower = request->getParam("maxP0")->value().toInt(); add=0;}
          // if(request->hasParam("maxP1")) {Inv_Prop[1].maxPower = request->getParam("maxP1")->value().toInt(); add=1;}
          // if(request->hasParam("maxP2")) {Inv_Prop[2].maxPower = request->getParam("maxP2")->value().toInt(); add=2;}
          // if(request->hasParam("maxP3")) {Inv_Prop[3].maxPower = request->getParam("maxP3")->value().toInt(); add=3;}
          // if(request->hasParam("maxP4")) {Inv_Prop[4].maxPower = request->getParam("maxP4")->value().toInt(); add=4;}
          // if(request->hasParam("maxP5")) {Inv_Prop[5].maxPower = request->getParam("maxP5")->value().toInt(); add=5;}
          // if(request->hasParam("maxP6")) {Inv_Prop[6].maxPower = request->getParam("maxP6")->value().toInt(); add=6;}     
          // if(request->hasParam("maxP7")) {Inv_Prop[7].maxPower = request->getParam("maxP7")->value().toInt(); add=7;}
          // if(request->hasParam("maxP8")) {Inv_Prop[8].maxPower = request->getParam("maxP8")->value().toInt(); add=8;}
          for (int i = 0; i <= 8; i++) {
              String paramName = "maxP" + String(i);
              if (request->hasParam(paramName)) {
                  Inv_Prop[i].maxPower = request->getParam(paramName)->value().toInt();
                  add = i;
                  break;
              }
          }
                
          Serial.println("Inv_Prop[add].maxPower set to = " + String(Inv_Prop[add].maxPower)); 
          actionFlag=240 + add; // save the settings and send zigbee to inverter
          Serial.println("actionFlag set to" + String(actionFlag));
          //Serial.println("setting the return url to /details?inv=");
          String toReturn = "/details?inv=" + String(add);
          strcpy(requestUrl, toReturn.c_str() ); 
          Serial.println("requestUrl = " + String(requestUrl));
          return;

     }

     // if we are here something was wrong, no parameters found
     request->send(200, "text/html", "no valid form found");
}    
