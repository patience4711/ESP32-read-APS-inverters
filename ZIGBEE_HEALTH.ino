// *************************************************************************
//                   system healtcheck 
//**************************************************************************

void healthCheck() {
    #ifdef TEST
    return;
    #endif
    if(Mqtt_Format != 0 && Mqtt_Format != 5 && Mqtt_stateIDX != 0) {
       if (mqttConnect() ) {
            char Mqtt_send[26];
            strcpy( Mqtt_send, Mqtt_outTopic) ;
            if( Mqtt_send[strlen(Mqtt_send)-1] == '/' ) {
                 strcat(Mqtt_send, "heap");
            }
      
        char toMQTT[50]={0}; // when i make this bigger a crash occures
        
        sprintf( toMQTT, "{\"idx\":%d,\"svalue\":\"%ld\",\"heap\":%ld}", Mqtt_stateIDX, esp_get_free_heap_size(), esp_get_free_heap_size() );  
        //must be like {"idx":901,"svalue":"22968", "heap":22968}
    
        consoleOut("mqtt publish heap, mess is : " + String(toMQTT) );
        MQTT_Client.publish ( Mqtt_send, toMQTT, false);  
        
        memset(&toMQTT, 0, sizeof(&toMQTT)); //zero out 
        delayMicroseconds(250);
       }  
    }

 
        if(!timeRetrieved) { 
          getTijd();
          eventSend(1);
        }
        
        // reset the errorcode so that polling errors remain
        if(errorCode >= 3000) errorCode -= 3000;
        if(errorCode >= 200) errorCode -= 200;
        if(errorCode >= 100) errorCode -= 100;
        
        // if there are no inverters, we don't start the coordinator
        if( inverterCount < 1 ) {
          if (diagNose) consoleOut(F("skipping, no inverters"));
          //Update_log( 2, "no inverter");
          zigbeeUp = 0;
          return;
        }
        
        switch(checkCoordinator() ) // send the 2700 command 
           {
            case 0:
                  zigbeeUp = 1; // all oke
                  diagNose = 0; // reset diagNose as this costs cpu
                  break;
    
            case 2:
                  //zigbeeUp = 0;
                  //String term = "zb down";
                  //Update_log("zigbee", "zb down" );
                   consoleOut("zb down");
                  resetCounter += 1;
                  resetValues(false, false); // reset all values, no mqtt
                  // try to start the coordinator
                  Serial.println("hc starting coordinator");
                  if (coordinator(true) ) zigbeeUp = 1; else zigbeeUp = 0;
                  //events.send( "reload", "message");
                  eventSend(1);
            }      
    
}

int checkCoordinator() {
// this is basically the 2700 command  
// the answer can mean that the coordinator is up, not yet started or no answer
// we evaluate that
// first empty serial2, comming from coordinator this is necessary;
    //empty_serial2(); is done in the loop
    
    char ecu_id_reverse[13];
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
    char * tail;

    // the response = 67 00, status 1 bt, IEEEAddr 8bt, ShortAddr 2bt, DeviceType 1bt, Device State 1bt
    //  FE0E 67 00 00 FFFF 80971B01A3D8 0000 0709001
    // status = 00 means succes, IEEEAddress= FFFF80971B01A3D8, ShortAdr = 0000, devicetype=07 bits 0 to 2
    
    //Device State 09 started as zigbeecoordinator     
    
    if (diagNose !=0) consoleOut("checkZigbeeRadio");
    //if(log) Update_Log("zigbee", "checking zb module");
    //check the radio, send FE00670067
    // when ok the returned string = FE0E670000FFFF + ECU_ID REVERSE + 00000709001
    // so we check if we have this

    char checkCommand[10]; // we send 2700 to the zb
    strncpy(checkCommand, "2700", 5);
    
    char reCeived[254]={0}; // a buffer for the received message
    char s_d[100]={0};
    // now we do this 3 times
    for (int x=1; x<4; x++)
    {
  
      sendZB( checkCommand );
      
      s_d[0]='\0';  
      // now read the answer if there is one
      strcpy(reCeived, readZB(s_d));
      delayMicroseconds(250);
      
      //if ( waitSerial2Available() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read
      //if(diagNose == 1) Serial.println("cc inMessage = " + String(inMessage) + " rc = " + String(readCounter));

  
      // we get this : FE0E670000 FFFF80971B01A3D8 0000 07090011 or
      //    received : FE0E670000 FFFF80971B01A3D6 0000 0709001F when ok
  
      //check if ecu_id_reverse is in the string, then split it there + 2 bytes
      if( strstr(reCeived, ecu_id_reverse) )
      {
          tail = split(reCeived, ecu_id_reverse + 4);
          // the tail should contain 0709
          if( strstr(tail, "0709") ) 
            {
              if( diagNose != 0 ) consoleOut("found 0709, running oke");
              zigbeeUp = 1;
              return 0;            
            } 
       }     
   delay(700);
   reCeived[0] = '\0';
   
   if( diagNose != 0 )  consoleOut("retrying.."); 
   }
   // if we come here 3 attempts failed       
    return 2;
}


//void ZigbeePing() {
//    // if the ping command failed then we have to restart the coordinator
//    //Update_Log("zigbee", "check serial loopback");
//    // these commands already have the length 00 and checksum 20 resp 26
//    char pingCmd[5]={"2101"}; // ping
//    char s_d[200] = {0}; 
//    if(diagNose !=0 ) consoleOut(F("send zb ping"));
//
//    sendZB( pingCmd ); // answer should be FE02 6101 79 07 1C
////    if ( waitSerial2Available() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read
////    DebugPrintln("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
//    char reCeived[254]={0};  
//    strcpy(reCeived, readZB(s_d));
//    
//    
//    if (strstr(reCeived, "FE026101" ) == NULL) 
//    {
//        consoleOut(F("no ping answer"));
//    } else {
//        consoleOut(F("ping ok"));
//    }
//    // we ignore the answer
//}

//bool ZigbeeLoopBack() {
//    // if the ping command failed then we have to restart the coordinator
//    //Update_Log("zigbee", "check serial loopback");
//    // these commands already have the length 00 and checksum 20 resp 26
//    char loopCmd[11]={"2710AABBCC"}; // ping
//    char s_d[200] = {0}; 
//    if(diagNose !=0 ) consoleOut(F("send zb loopback"));
//
//    sendZB( loopCmd ); // answer should be FE02 6101 79 07 1C
////    if ( waitSerial2Available() ) { readZigbee(); } else { readCounter = 0;} // when nothing available we don't read
////    DebugPrintln("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
//
//    char reCeived[254]={0};  
//    strcpy(reCeived, readZB(s_d));
//    
//    if(readCounter > 10) return false;
//    
//    if (strstr(reCeived, "AABBCC" ) == NULL) 
//    {
//        consoleOut(F("no loopback answer"));
//       return false;
//    } else {
//        consoleOut(F("loopback ok"));
//       return true;
//    }
//}
// *************************************************************************
//                          hard reset the cc25xx
// *************************************************************************
void ZBhardReset() 
    {
    digitalWrite(ZB_RESET, LOW);
    delay(500);
    digitalWrite(ZB_RESET, HIGH);
    //char term[20] = {"ZBmodule hard reset"} ;
    Update_Log(2, "hard reset");
     consoleOut("ZB module hard reset");
    delay(2000); //wait for the cc2530 to reboot
    }


    
