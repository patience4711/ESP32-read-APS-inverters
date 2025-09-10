// flash the led
void ledblink(int i, int wacht) {
  for(int x=0; x<i; x++) {
    digitalWrite(led_onb, LED_AAN);
    delay(wacht);
    digitalWrite(led_onb, LED_UIT);
    delay(wacht);
   }
}


void empty_serial() { // remove any remaining data in serial buffer
  while(Serial.available()) {
    Serial.read();
  }
}
void empty_serial2() { // remove any remaining data in serial buffer
  while(Serial2.available()) {
    Serial2.read();
  }
}

void delayedReset() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  SPIFFS.end();       // Recommended before formatting
  SPIFFS.format();
  ESP.restart();
}

void flashErase(AsyncWebServerRequest *request) {
// call this from the portal to erase the wifi and the flash
     procesId = 2;
     confirm();
     request->send(200, "text/html", toSend);
     resetTicker.once(3.0, delayedReset); // 1-second delay
}

void eventSend(byte what) {
  if (what == 1) {
      events.send( "general", "message"); //getGeneral triggered            
  } else 
     if (what == 2) {
     events.send( "getall", "message"); //getAll triggered
  } else {  
     events.send( "reload", "message"); // both triggered
  }
}

int readInverterfiles() {
  // we must try to read from 0 to 8 because in some situations there can be a gap
  int count =0;
  for (int x=0; x < 9; x++) 
  {
  String bestand = "/Inv_Prop" + String(x) + ".str";  
  //consoleOut("reading file " + bestand);
      if (!readStruct(bestand)) 
      { 
        // if the file not exists we break
        consoleOut("no file " + bestand); 
        //break;
      } else { count += 1; }
  }
  return count;
}

    void test_actionFlag() {
    if(actionFlag == 0) return;
    
    consoleOut("testing actionFlag = " + String(actionFlag));
    
    //if(actionFlag != 0) Serial.println("test_actionFlag 1 val = " + String(actionFlag));  
    // ******************  reset the nework data and reboot in AP *************************
    if (actionFlag == 11 || value == 11) 
    { // 
     //DebugPrintln("erasing the wifi credentials, value = " + String(value) + "  actionFlag = " + String(actionFlag));
     delay(1000); //reserve time to release the button
     //eraseWifiFlash();
     WiFi.disconnect();
//    WiFi.end();
     // we write a flag in EEPROM
     consoleOut(F("wifi disconnected"));
//we try to overwrite the wifi creentials     
     const char* ssid = "dummy";
    const char* password = "dummy";
    WiFi.begin(ssid, password);
    Serial.println(F("\nConnecting to dummy network"));
    int teller = 0;
      while(WiFi.status() != WL_CONNECTED){
          Serial.print(F("wipe wifi credentials\n"));
          delay(100);         
          teller ++;
          if (teller > 2) break;
      }
     ESP.restart();
    }  

    if (actionFlag == 10) { // the button was pressed a long time, start ap
     delay(2000); // give the server the time to send the confirm
     consoleOut("rebooting");
     String key = "req";
     preferences.putString(key.c_str(), requestUrl);
     ESP.restart();
  }
    
    //Serial.println("test_actionFlag 2 val = " + String(actionFlag));
    // interrrupt button test
    if (actionFlag == 15) {
      actionFlag = 0;
      buttonPressed();
    }
     //Serial.println("test_actionFlag 3 val = " + String(actionFlag));
    if (actionFlag == 60) {
      actionFlag = 0; //reset the actionflag
      pairOnActionflag();
    }
     //Serial.println("test_actionFlag 4 val = " + String(actionFlag));
    if (actionFlag == 21) {
      actionFlag = 0; //reset the actionflag
      coordinator(true); // start normal operation mode
    }
 //Serial.println("test_actionFlag 5 val = " + String(actionFlag));
// mosquitto test **********************************************    
    if (actionFlag == 24) { // set by the mqtt config page
        actionFlag = 0; //reset the actionflag
        MQTT_Client.disconnect();
       //reset the mqttserver
        MQTT_Client.setServer(Mqtt_Broker, atoi(Mqtt_Port) );
        MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
        if (Mqtt_Format != 0) mqttConnect(); // reconnect mqtt after change of settings
    }    
 //Serial.println("test_actionFlag 6 val = " + String(actionFlag));
    if (actionFlag == 25) {
      actionFlag = 0; //reset the actionflag
      getTijd(); // recalculate time after change of settings
    }

   // *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
   //                               T H R O T T L I N G                  
   // *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
    if (actionFlag > 239 && actionFlag < 249) {
      int whichInv = actionFlag - 240; // with 248 this would be 8  0 1 2 3 4 5 6 7 8 is in range
      if(diagNose == 10) diagNose = 3;
      consoleOut("inside actionFlag: inverter is " + String(whichInv));
      actionFlag = 0; //reset the actionflag
      int errorCode = setMaxPower(whichInv);
      if(errorCode == 0) {
         //Inv_Prop[whichInv].throttled = true;
          String term= "throttle inv " + String(whichInv) + " success";
          Update_Log(2, term.c_str());
      } else {
        // the setPower command failed, if not 15 we set -1
        if(errorCode != 15) 
           { 
             String key = "maxPwr" + String(whichInv);
             preferences.putInt(key.c_str(), -1 );
           }
        String term= "throttle inv " + String(whichInv) + " failed";
        consoleOut("throttle failed inv " + String(whichInv));
        Update_Log(2, term.c_str());
      }
     // if we throttled via an api and had debug set we do this
     if (diagNose == 3) diagNose = 0;  
    
    eventSend(0);
    procesId = 1; // for the confirm
    #ifdef TEST
       testCounter += 1;
    #endif
    //String bestand = "/Inv_Prop" + String(whichInv) + ".str"; // /Inv_Prop0.str
    //consoleOut("going to write " + bestand );
    //writeStruct(bestand, whichInv); // alles opslaan in SPIFFS
    }

    if (actionFlag == 43) { //triggered by the console
        actionFlag = 0; //reset the actionflag
        inverterReboot(iKeuze);
    }
    if (actionFlag == 44) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        healthCheck(); 
    }
    if (actionFlag == 45) { //triggered by the api (external)
        actionFlag = 0; //reset the actionflag
        //Serial.println("someone made actionFlag 45 !!"); the uin8_t doesn't allow 301
        testMessage(); // the bool decides where to find the input
        if(diagNose == 3) diagNose = 1;
    }
    //     if (actionFlag == 55) { //triggered by the webconsole
    //     actionFlag = 0; //reset the actionflag
    //     //Serial.println("someone made actionFlag 45 !!"); the uin8_t doesn't allow 301
    //     rawMessage(true); // the bool decides where to find the input
    // }
    if (actionFlag == 46) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        showDir(); 
    }
    
   // *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
   //                               P O L L I N G                  
   // *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *
    if (actionFlag > 219 && actionFlag < 229) {
      int whichInv = actionFlag - 220; // with 248 this would be 8  0 1 2 3 4 5 6 7 8 is in range
      consoleOut("inside actionFlag: inverter is " + String(whichInv));
      actionFlag = 0; //reset the actionflag
      polling(whichInv);
      // if we polled via an api and had debug set we do this
      if (diagNose == 3) diagNose = 0;  
      eventSend(2);
    }
    
    if (actionFlag > 209 && actionFlag < 219) {
            int whichInv = actionFlag - 210;
            actionFlag = 0;
            if(diagNose == 10) diagNose = 3;
            consoleOut("inside actionFlag: inverter is " + String(whichInv));
            querying(whichInv);
        //events.send( "getall", "message");
             if (diagNose == 3) diagNose = 0;
    }

    // query a single inverter
    // if (actionFlag == 57) { //triggered by QUERY?inv=0
    //     actionFlag = 0; //reset the actionflag
    //     querying(iKeuze);
    //     //events.send( "getall", "message");
    //     if (diagNose == 3) diagNose = 0;
    // }
    //polling all inverters
    if (actionFlag == 48) { //triggered by the webpage zbtest and mqtt
        actionFlag = 0; //reset the actionflag
            ledblink(1,100);
            poll_all(); 
    }

    if (actionFlag == 49) { //triggered by console testmqtt
        actionFlag = 0; //reset the actionflag
        ledblink(1,100);
        // always first drop the existing connection
        if(diagNose == 10) diagNose = 3;
        MQTT_Client.disconnect();
        consoleOut("mqtt forced drop connection");
        delay(100);
        char Mqtt_send[26] = {0};
       
        if(mqttConnect() ) {
        String toMQTT=""; // if we are connected we do this
        
        strcpy( Mqtt_send , Mqtt_outTopic);
        
        if(Mqtt_send[strlen(Mqtt_send -1)] == '/') strcat(Mqtt_send, String(Inv_Prop[0].invIdx).c_str());
        toMQTT = "{\"test\":\"" + String(Mqtt_send) + "\"}";
        
        if(Mqtt_Format == 5) toMQTT = "field1=12.3&field4=44.4&status=MQTTPUBLISH";
        
        if( MQTT_Client.publish (Mqtt_outTopic, toMQTT.c_str() ) ) {
            consoleOut("sent mqtt message : " + toMQTT);
        } else {
            consoleOut("sending mqtt message failed : " + toMQTT);    
        }
      } 
     // the not connected message is displayed by mqttConnect()
    }


} // end test actionflag


void poll_all() { 
 // poll all inverters, inbetweeen empty serial and feed wdt 
     for(int i=0; i<inverterCount; i++)
     {     
          if(String(Inv_Prop[i].invID) != "0000") {
                polling(i);
                delay(2000);
                empty_serial2();
                //ESP.wdtFeed();
          }
       }
     //events.send( "getall", "message");
     eventSend(2);
 } 

String getChipId(bool sec) {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  if(sec) return String(chipId); else return "ESP32-ECU-" + String(chipId);
}

void showDir() {
    char temp[50]={0};
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      sprintf(temp, "%s  size %u<br>" , file.name(), file.size() );
      consoleOut(String(temp));
      delay(100);
      file = root.openNextFile();
    }
  }

  // function to 
void consoleOut(String toLog) {

 switch (diagNose) 
    { case 0:  
          break;
      case 1:       
          delay(100); // otherwise the socket cannot keep up
          ws.textAll(toLog);
          break;
      case 2: 
          Serial.println(toLog);
          break;
      case 3:
           toLog += "\n";
          debugLog += toLog;   
    }
}