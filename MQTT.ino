bool mqttConnect() {   // 
/* this function checks if we are connected to the broker, if not connect anyway */  
   
    if( MQTT_Client.connected() ) {
    consoleOut("mqtt was connected");
    return true;
    }
     consoleOut(F("\nconnect mqtt"));
    // we are here because w'r not connected. Signal with the LED
    //ledblink(2,70);

    //if (Mqtt_Port == 0 ) { Mqtt_Port = 1883;}   // just in case ....
    uint8_t retry = 3;
    
    //char Mqtt_inTopic[11]={"ESP-ECU/in"};

    while (!MQTT_Client.connected()) {

      if ( MQTT_Client.connect( settings.Mqtt_Clientid, settings.Mqtt_Username, settings.Mqtt_Password) )
      {
         //connected, so subscribe to inTopic (not for thingspeak)
        if( settings.Mqtt_Format != 0 ) {
           //String sub = "ESP32-" + getChipId(true) + "/in"; // to get a intopic ESP32-234567/in
           if( MQTT_Client.subscribe ( settings.Mqtt_inTopic ) ) {
               consoleOut("subscribed to " + String(settings.Mqtt_inTopic));
           }
        }
         consoleOut(F("mqtt connected"));
         UpdateLog(3, "connected");
      
       return true;

    } else {
        //String term = "connection failed state: " + String(MQTT_Client.state());
        UpdateLog(3, "failed");
        if (!--retry) break; // stop when tried 3 times
        delay(500);
    }
  }
  // if we are here , no connection was made.

  consoleOut(F("mqtt connection failed"));
  return false;
}

// *************************************************************************
//                   process received mqtt
// *************************************************************************

void MQTT_Receive_Callback(char *topic, byte *payload, unsigned int length)
{
    // domoticz messages can be long so the buffer should be large enough (setup)
    int idx = 0;
    uint8_t nLevel = 0;
    consoleOut(F("received mqtt"));
    JsonDocument doc;       // We use json library to parse the payload 
    String jsonString = "";                        
   //  The function deserializeJson() parses a JSON input and puts the result in a JsonDocument.
   // DeserializationError error = deserializeJson(doc, Payload); // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload); // Deserialize the JSON document
    if (error)            // Test if parsing succeeds.
    {
       consoleOut("mqtt no valid json ");
        return;
    } 
    // the json contains a lot, a.o. idx and "svalue1" : "72" and "level":72,
    
    // We check the kind of command format received with MQTT
    //now we have a payload like {"switch",on}    
    if(!doc["idx"].isNull())
    {
      idx = doc["idx"].as<int>();
      //now lookup the device
      
      if(idx == settings.Mqtt_switchIDX) {
        consoleOut("there is recognized idx " + String(idx)) ;
      } else {
        consoleOut("wrong idx, abort");
        return;
      }
    }  //consoleOut("devNr " + String(dev)) ;

    if (!doc["Level"].isNull()) 
    {
      nLevel = doc["Level"].as<uint8_t>();
    } else {
      consoleOut("no nvalue, abort");
      return;
    }
    consoleOut("mqtt nLevel = " + String(nLevel));
    // switch the lamp without reporting to mqtt
    set_dim_level(nLevel);
    if(nLevel != 0)  
    { 
        UpdateLog(3, "dim command"); 
    } else  { 
        UpdateLog(3, "switched off");
        checkTimers(); // disarm a timer that is active
    }
}

bool mqttGeldig() {
    if (settings.Mqtt_Broker[0] == '\0' || settings.Mqtt_Format == 0) {
    return false;
    }
    if (settings.Mqtt_switchIDX == 0) { // geen geldige idx
    return false;   
    } else {
    return true; 
    }
}

void sendMqttswitch() 
{
  if ( !mqttGeldig() ){ 
    consoleOut("no valid mqtt address or not configured, skipping..");
    return;
  }    
// update switch state
   char out[124];
  JsonDocument doc;
  doc["command"] = "switchlight";
  doc["idx"] = settings.Mqtt_switchIDX;
  if(settings.Mqtt_Format == 1) {
  (dimmer_state == 0) ? doc["nvalue"] = 0 : doc["nvalue"] = 1;
  } else {
  (dimmer_state == 0) ? doc["state"] = "off" : doc["state"] = "on";  
  }
  int b =serializeJson(doc, out);
  MQTT_Client.publish (settings.Mqtt_outTopic, out );

  //{"command":"switchlight","idx":981,"switchcmd":"Set Level","level":64}'
  JsonDocument dac;
  dac["command"] ="switchlight";
  dac["idx"] = settings.Mqtt_switchIDX;
  dac["switchcmd"] = "Set Level";
  dac["level"] = current_duty;
  int c = serializeJson(dac, out);
  MQTT_Client.publish (settings.Mqtt_outTopic, out );
}