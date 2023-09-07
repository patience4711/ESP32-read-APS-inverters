void handle_Serial () {
      //DebugPrintln(F("we are in handle serial"));  
      int SerialInByteCounter = 0;
      InputBuffer_Serial[0] = '\0'; // make it like empty
      byte SerialInByte;  
      delay(200); // wait untill maybe more data available
      Serial.println("serial data available: " + String(Serial.available()) );
  
      while(Serial.available()) {
             SerialInByte=Serial.read(); 
            
            if(isprint(SerialInByte)) {
              if(SerialInByteCounter<100) InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
            }    
            if(SerialInByte=='\n') {                                              // new line character
              InputBuffer_Serial[SerialInByteCounter]=0;
              break; // serieel data is complete
            }
       }  
     Serial.println("InputBuffer_Serial = " + String(InputBuffer_Serial) );
     diagNose = 1; // direct the output to serial
     // evaluate the incomming data
     if (strlen(InputBuffer_Serial) > 6) {                                // need to see minimal 8 characters on the serial port
       if (strncmp (InputBuffer_Serial,"10;",3) == 0) {                 // Command from Master to RFLink
  
          if (strncasecmp(InputBuffer_Serial+3,"HELP",4) == 0) {
              scroll(4);
              Serial.println(F("*** AVAILABLE COMMANDS ***"));
              Serial.println(F("10;DIAG=x; (if x = 1 set diagNose for serial debug)"));
              Serial.println(F("10;POLL=x; (poll inverter nr x (9=all))"));                            
              Serial.println(F("10;ZBT=; (send zigbee message, e.g. 10;zbt=2101 (ping))"));
              Serial.println(F("10;DELETE=<file>; (delete a file)"));              
              Serial.println(F("10;HEALTH; (perform healthcheck zigbee)"));             
              Serial.println(F("10;INV_REBOOT; (reboot an unresponsive inverter)"));
              Serial.println(F("10;ZB_reset; (reset the cc2530 via its resetpin)"));
              Serial.println(F("10;EDIT=x-AABB; (edit the id of an inverter)"));
              #ifdef TEST
              Serial.println(F("10;TESTINV; (decode a testanswer for inv 0)"));
              #endif
              return;
          } else 

          if (strncasecmp(InputBuffer_Serial+3,"INV_REBOOT",10) == 0) {
            scroll(2);
              scroll(4);
              Serial.println(F("\nYou can reboot an inverter when it stopped working."));
              Serial.println(F("Characteristics: not responsive, (slow blinking red led)."));
              Serial.println(F("\ntype REBOOT_INVERTER=x (x=inverternumber 0, 1 etc.)"));
              Serial.println(F("\nDISCLAIMER: THIS HAS NOT BEEN TESTED, USE AT YOUR OWN RISK!"));
              return;
          } else
              
// // ********************** zigbee test new*****************************          
           if (strncasecmp(InputBuffer_Serial+3,"ZBT=",4) == 0) { 
            char tmp[128]={0}; 
            int len = strlen(InputBuffer_Serial); 
              Serial.println("\n\nsend a zigbee command, len=" + String(len));
      for(int i=0; i<len; i++) 
      {
        tmp[i] = InputBuffer_Serial[i+7];
      }
      //Serial.print("command = " + String(tmp));
      testMessage(false);
      return;             
          
      } else 
          
           if (strncasecmp(InputBuffer_Serial+3,"EDIT=",5) == 0) {
             int kz = String(InputBuffer_Serial[8]).toInt();
            Serial.print(F("received EDIT=")); Serial.println(String(kz) ); 
              if ( kz > inverterCount-1 ) {
              Serial.println(F("\n\nerror, no such inverter"));
              return;  
              }
              char invid[5];
              for(int i=10;  i<15; i++) { invid[i-10] = InputBuffer_Serial[i]; }
              Serial.print(F("\n\nedit inverter ")); Serial.println(String(kz));
              Serial.print(F("id = ")); Serial.println(String(invid));
              strncpy(Inv_Prop[kz].invID, invid, 4);
              String bestand = "/Inv_Prop" + String(kz) + ".str"; // /Inv_Prop0.str
              writeStruct(bestand, kz); // save in SPIFFS 
              return;
          } else          
          
          if (strncasecmp(InputBuffer_Serial+3,"POLL=",5) == 0) {
            //input can be 10;POLL=0;  
              int kz = String(InputBuffer_Serial[8]).toInt();
              if ( kz > inverterCount ) {
              Serial.println(F("\n\nerror, non-excisting inverter"));
              return;  
              }
              if(kz==9) {
                Serial.println(F("\n\npolling all inverters"));
                ledblink(1,100);
                for(int i=0; i<inverterCount; i++)
                {     
                  if(String(Inv_Prop[i].invID) != "0000") polling(i);
                } 
                return; } else {
                   Serial.print(F("\n\npoll inverter ")); Serial.println(String(kz));
                polling(kz);
                Serial.println(F("\n\npolling ready"));
                return; }
          } else
          
          
          // simulate a polled inverter
          if (strncasecmp(InputBuffer_Serial+3,"FORCE=",6) == 0) {
              Serial.println(F("\n\nforce values in Inv_Data for all ")); 
              for (int z=0; z<inverterCount; z++) {
                   polled[z] = true; 
                   sprintf(Inv_Prop[z].invID, "%s", "A1B2");
                      
                   Inv_Data[z].acv = 220.1;
                   Inv_Data[z].heath = 16.2;
                   Inv_Data[z].sigQ = 79;
                   Inv_Data[z].freq = 50.1;
                     // now for all panels
                   Inv_Data[z].dcc[0] = 1.23;
                   Inv_Data[z].dcv[0] = 30.11;
                   Inv_Data[z].power[0]=300.1;
                                 
                   Inv_Data[z].dcc[1] = 1.24;
                   Inv_Data[z].dcv[1] = 31.22;
                   Inv_Data[z].power[1] = 310.2;
      
                   Inv_Data[z].dcc[2] = 1.25;
                   Inv_Data[z].dcv[2] = 32.23;
                   Inv_Data[z].power[2] = 330.3;
      
                   Inv_Data[z].dcc[3] = 1.26;
                   Inv_Data[z].dcv[3] = 33.44;
                   Inv_Data[z].power[3] = 340.3;            
                   
                   Inv_Data[z].en_total=1300.1;
             }
             eventSend(2); // inform the webpage
             return;
          } else


//          if (strncasecmp(InputBuffer_Serial+3,"WIFICON=", 7) == 0) {
//          char * pass;
//          char * ssid;
//          char * tmp;
//          int len = strlen(InputBuffer_Serial);
//          Serial.println("bufferlength = " + String(len) );  
////           for(int i=11; i<len; i++) 
////           {
////              strcat(tmp, InputBuffer_Serial[i] );
////           }
//          Serial.print(F("command = 10;WIFICON=")); Serial.println(String(tmp));
//          // we have a string like "ssid;passwd"
//          //extract the first part until#;
//            tmp = strtok(InputBuffer_Serial, "=");        // store SSID
//            ssid = strtok(NULL, ",");        // store SSID
//            pass = strtok(NULL, ",");
//            Serial.println("ssid/pass=" + String(ssid) + "/" + String(pass) );
//            event=0;
//            while (WiFi.status() != WL_CONNECTED) {
//              delay(500);
//              Serial.print("*");
//              WiFi.begin();
//              event+=1;
//              if (event==10) {break;}
//              }
//              if (event == 10) Serial.println("wifi failed");
//              return;
//          
//          } else

           if (strncasecmp(InputBuffer_Serial+3,"FILES",5) == 0) {  
              showDir();
              return;  
          } else

           if (strncasecmp(InputBuffer_Serial+3,"INIT_N",6) == 0) {  
              Serial.println(F("\n\ninit the zb system for normal operation"));
              coordinator(true); // start coordinator for normal ops
              return;             
          } else
           
           if (strncasecmp(InputBuffer_Serial+3,"HEALTH",6) == 0) {  
              Serial.println(F("\n\nchecking the ZB system"));
              healthCheck();
              return;             
          } else
  
           if (strncasecmp(InputBuffer_Serial+3,"ZB_reset",8) == 0) {  
              Serial.println(F("\n\nhard reset the cc2530"));
              ZBhardReset();
              return; 
           } else

// ************************ test mosquitto *****************************
            if (strncasecmp(InputBuffer_Serial+3,"TESTMQTT",8) == 0) {  
              ws.textAll("test mosquitto");
              actionFlag=49; // perform the healthcheck
              diagNose=true;
              return;             

          } else 

#ifdef TEST
      if (strncasecmp(InputBuffer_Serial+3, "TESTINV",7) == 0)  
      {
        Serial.println("command = " + String(InputBuffer_Serial) );  
      if(testCounter > 1) { 
        testCounter=0; // reset testcounter
        // make all values null
        resetValues(true, false);
      }
      testDecode(); // test a fake polling answer for the invType
    //}
      return;
      } else
#endif    
          {
          // if we are here no command was reconnized
          Serial.print( String(InputBuffer_Serial)); Serial.println(F(" INVALID COMMAND" ));     
          }
       } // end if if (strncmp (InputBuffer_Serial,"10;",3) == 0)
    
    } //  end if strlen(InputBuffer_Serial) > 6
    Serial.print(String(InputBuffer_Serial)); Serial.println(F(" UNKNOWN COMMAND"));
  // the buffercontent is not making sense so we empty the buffer
  empty_serial();
} // 

  void scroll(int aantal) {
    for (int x=0;x<aantal; x++) { Serial.println(F("\n")); }
  }
