bool mqttConnect() {   // MQTT connection (documented way from AutoConnect : https://github.com/Hieromon/AutoConnect/tree/master/examples/mqttRSSI_NA)

// we are here because w'r not connected. Signal with the LED
    ledblink(2,70);    
    if ( Mqtt_Broker[0] == '\0' || Mqtt_Broker[0] == '0'  ) 
    {
      Mqtt_Format = 0; // we don't try again
      //DebugPrintln("no broker, cancel");
      return false;
    }
    if( diagNose != 0 )("mqtt try to connect to " + String(Mqtt_Broker));
    //DebugPrintln("connecting to mqtt");
    if (Mqtt_Port == "" ) Mqtt_Port = "1883";   // just in case ....
    uint8_t retry = 3;

    // We generate a unique name for this device to avoid mqtt problems 
    // in case if you have multiple RFlink-ESP devices
    String clientId = getChipId(false);
    
    while (!MQTT_Client.connected()) {

      if (MQTT_Client.connect(clientId.c_str() , Mqtt_Username.c_str(), Mqtt_Password.c_str())) 
      {
          if( diagNose != 0 ) consoleOut("MQTT connection Established with ID : " + clientId);
          //DebugPrintln(clientId);
          //connected, so we and subscribe to inTopic
         
          MQTT_Client.subscribe ( Mqtt_inTopic.c_str() ) ;   // 
          if( diagNose != 0 ) consoleOut("\nMQTT subscribed on topic " + Mqtt_inTopic);
          Update_Log("mqtt", "connected");
          return true;
      } 
      delay(500);
      if (!--retry) {
          //String term = "connection failed state: " + String(MQTT_Client.state());
          Update_Log("mqtt", "connection failed");
          if( diagNose != 0 ) consoleOut("MQTT connection failed"); 
          break;
       }
  }
  // if we are here, connection failed after 3 attempts
  // Mqtt_Enabled = false; // we don't try again
  return false;
}

// *************************************************************************
//                   process received mqtt
// *************************************************************************

void MQTT_Receive_Callback(char *topic, byte *payload, unsigned int length)
{
    //Serial.println("MQTT_message arrived in " + String(Mqtt_inTopic) );
    //Serial.println("length of the message = " + String(length));
    //String SerialRFcmd = "";
   
    String Payload = "";     // convert the payload to a String...
    for (int i = 0; i < length; i++)
    {
        Payload += (char)payload[i]; // convert to char, nodig???
    }
    
    if( diagNose != 0 ) consoleOut("mqtt received " + Payload);

    StaticJsonDocument<1024> doc;       // We use json library to parse the payload                         
   //  The function deserializeJson() parses a JSON input and puts the result in a JsonDocument.
    DeserializationError error = deserializeJson(doc, Payload); // Deserialize the JSON document

    if (error)            // Test if parsing succeeds.
    {
        if( diagNose != 0 ) consoleOut("mqtt no valid json ");
        return;
    } 
    
    // We check the kind of command format received with MQTT
    //now we have a payload like {"poll",1}    

    if( doc.containsKey("poll") )
    {
        if(!Polling)
        {
            int inv = doc["poll"].as<int>();  
            //DebugPrintln( "found {\"poll\" " + String(inv) + "}\"" );
            
            iKeuze = inv;
            if(iKeuze == 99) {
              if( diagNose != 0 ) consoleOut( "found {\"poll\" " + String(inv) + "}\"" );
              actionFlag = 48; // takes care for the polling of all inverters
              return;  
            }

            if ( iKeuze < inverterCount ) 
            { 
              actionFlag = 47; // takes care for the polling
              return;
            } else {
               if( diagNose != 0 ) consoleOut("mqtt error no inv " + String(iKeuze));
              return;         
            }
        }
        else
        {
          if( diagNose != 0 ) consoleOut("polling = automatic, skipping");
        }
    }
}
