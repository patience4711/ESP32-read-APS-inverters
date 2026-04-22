bool mqttConnect() {   // 
/* this function checks if we are connected to the broker, if not connect anyway */  
    if( MQTT_Client.connected() ) {
    consoleOut("mqtt was connected");
    return true;
    }
    // we are here because w'r not connected. Signal with the LED
    consoleOut("mqtt connecting");
    ledblink(2,70);

    if (Mqtt_Port[0] == '\0' ) strcpy(Mqtt_Port, "1883");   // just in case ....
    uint8_t retry = 3;
    
    //String Clientid = getChipId(false); 

    while (!MQTT_Client.connected()) {

      if ( MQTT_Client.connect( getChipId(false).c_str(), Mqtt_Username, Mqtt_Password) )
      {
         //connected, so subscribe to inTopic (not for thingspeak)
        if(Mqtt_Format != 5 ) {
        String clientid = getChipId(false) + "/in"; 
        if(  MQTT_Client.subscribe ( clientid.c_str() ) ) {
        //if(  MQTT_Client.subscribe ( Mqtt_inTopic ) ) { 
               //if(diagNose) ws.textAll("subscribed to " + String(Mqtt_inTopic ));
               consoleOut("subscribed to " + clientid);
           }
        }
         consoleOut(F("mqtt connected"));
         Update_Log(3, "connected");
      
       return true;

    } else {
        //String term = "connection failed state: " + String(MQTT_Client.state());
        Update_Log(3, "failed");
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
   
//    String Payload = "";     // convert the payload to a String...
//    for (int i = 0; i < length; i++)
//    {
//        Payload += (char)payload[i]; // convert to char, needed???
//    }
    
   // ws.textAll("mqtt received " + Payload);

    JsonDocument doc;       // We use json library to parse the payload                         
   //  The function deserializeJson() parses a JSON input and puts the result in a JsonDocument.
   // DeserializationError error = deserializeJson(doc, Payload); // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload); // Deserialize the JSON document
    if (error)            // Test if parsing succeeds.
    {
       consoleOut("mqtt no valid json ");
        return;
    } 
    consoleOut("Deserialized JSON:");
    serializeJson(doc, Serial);     // Print in one line
    Serial.println();
    // We check the kind of command format received with MQTT
      // if( doc["throttle"] != 0 )
    //if(doc.containsKey("throttle"))
    if (!doc["throttle"].isNull())
    {
       int Invert = doc["throttle"].as<int>(); 
       int throtVal = doc["val"].as<int>(); 
       String term = "mqtt got message {\"throttle\":" + String(Invert) + ",\"val\":" + String(throtVal) + "}";
       consoleOut(term);
      if(Invert > inverterCount || Invert < 0 || throtVal > 700 || throtVal < 20 )
         {
         consoleOut("invalid value(s), skipping");
         return; 
         }
      // write the desired throtval 
      desiredThrottle[Invert] = throtVal;
      //Inv_Prop[invert].maxPower = throtVal;
      actionFlag = 240 + Invert;  
    }  

    if (!doc["poll"].isNull())
    {
      //now we have a payload like {"poll",1} 
        int inv = doc["poll"].as<int>(); 
        consoleOut( "got message {\"poll\":" + String(inv) + "}" );

        if( !Polling && dayTime )
        {
             
            //ws.textAll( "found {\"poll\" " + String(inv) + "}\"" );
            
            iKeuze = inv;
            if(iKeuze == 99) {
                actionFlag = 48; // takes care for the polling of all inverters
                return;  
            }

            if ( iKeuze < inverterCount ) 
            { 
              actionFlag = 220+inv; // takes care for the polling
              return;
            } else {
               consoleOut("mqtt error no inv " + String(iKeuze));
              return;         
            }
        }
        else
        {
          consoleOut("autopolling set or nightTime, skipping");
        }
        consoleOut("nothing familiair found in mqtt");
    }
}
