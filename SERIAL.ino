void handle_Serial () {
      DebugPrintln(F("we are in handle serial"));  
      int SerialInByteCounter = 0;
      InputBuffer_Serial[0] = '\0'; // make it like empty
      byte SerialInByte;  
      delay(200); // wait untill maybe more data available
      DebugPrintln("normal serial: data available, number = " + String(Serial.available()) );
  
      while(Serial.available()) {
             SerialInByte=Serial.read(); 
             //Serial.print("+");
            
            if(isprint(SerialInByte)) {
              if(SerialInByteCounter<100) InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
            }    
            if(SerialInByte=='\n') {                                              // new line character
              InputBuffer_Serial[SerialInByteCounter]=0;
              DebugPrintln(F("found new line"));
             break; // serieel data is complete
            }
       }  
     DebugPrintln("InputBuffer_Serial = " + String(InputBuffer_Serial) );
     // evaluate the incomming data
     if (strlen(InputBuffer_Serial) > 6) {                                // need to see minimal 8 characters on the serial port
       if (strncmp (InputBuffer_Serial,"10;",3) == 0) {                 // Command from Master to RFLink
  
          if (strncasecmp(InputBuffer_Serial+3,"HELP",4) == 0) {
              scroll(4);
              Serial.println(F("*** AVAILABLE COMMANDS ***"));
              Serial.println(F("10;DIAG; (set diagNose for more serial output)"));
              Serial.println(F("10;POLL=x; (poll inverter nr x (9=all))"));                            
              Serial.println(F("10;ZBT=; (send zigbee message, e.g. 10;zbt=2101 (ping))"));
              //Serial.println(F("10;ERASE; (delete all inverter configfiles)"));
              Serial.println(F("10;DELETE=<file>; (delete a file)"));              
              Serial.println(F("10;HEALTH; (perform healthcheck zigbee)"));             
              Serial.println(F("10;INV_REBOOT; (reboot an unresponsive inverter)"));
              Serial.println(F("10;ZB_reset; (reset the cc2530 via its resetpin)"));
              Serial.println(F("10;EDIT=x-0xAABB; (fake pair an inverter)"));
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
      Serial.print(F("command = ")); Serial.println(String(tmp));
             
              testMessage();

               return;             
          
          } else 
          
          
          if (strncasecmp(InputBuffer_Serial+3,"REBOOT_INVERTER=",16) == 0) {

              int kz = String(InputBuffer_Serial[19]).toInt();
              Serial.print(F("\n\nreboot inverter ")); Serial.println(String(kz));
              if ( kz > inverterCount-1 ) 
              {
                 Serial.println(F("\n\nerror, non-excisting inverter"));
                 return;  
              }
                 inverterReset(kz);
              return;
          } else 

          if (strncasecmp(InputBuffer_Serial+3,"DIAG",4) == 0) {
              if(diagNose == 1) {
                diagNose = 0;
              } else { diagNose = 1; }
               
              Serial.println(F("\n\ndiagNose = ")); Serial.println(String(diagNose));
              return;
          } else

           if (strncasecmp(InputBuffer_Serial+3,"EDIT=",5) == 0) {
             int kz = String(InputBuffer_Serial[8]).toInt();
            Serial.print(F("received EDIT=")); Serial.println(String(kz) ); 
              if ( kz > inverterCount-1 ) {
              Serial.println(F("\n\nerror, no such inverter"));
              return;  
              }
              char invid[7];
              for(int i=10;  i<17; i++) { invid[i-10] = InputBuffer_Serial[i]; }
              Serial.print(F("\n\nedit inverter ")); Serial.println(String(kz));
              Serial.print(F("id = ")); Serial.println(String(invid));
              strncpy(Inv_Prop[kz].invID, invid, 6);
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
                  if(String(Inv_Prop[i].invID) != "0x0000") polling(i);
                } 
                return; } else {
                   Serial.print(F("\n\npoll inverter ")); Serial.println(String(kz));
                polling(kz);
                Serial.println(F("\n\npolling ready"));
                return; }
          } else
           
          if (strncasecmp(InputBuffer_Serial+3,"FORCE",5) == 0) {
            Serial.println(F("\n\nforce values in Inv_Data")); 
            int z=0;
            polled[z] = true; 
                sprintf(Inv_Prop[z].invID, "%s", "0xA1B2");
                
                strcpy(Inv_Data[z].acv, "220.1");
                strcpy(Inv_Data[z].heath, "16.2");
                strcpy(Inv_Data[z].sigQ, "79");
                strcpy(Inv_Data[z].freq, "50.1");
                Serial.println("check freq = " + String(Inv_Data[z].freq)); 
                // dtostrf(float_value, min_width, num_digits_after_decimal, where_to_store_string)             
             float number;                // now for all panels
             number=1.23; dtostrf(number,5, 2, Inv_Data[z].dcc[0]);
             number=30.11; dtostrf(number,5, 2, Inv_Data[z].dcv[0]);
             number=300.1; dtostrf(number,5, 1, Inv_Data[z].power[0]);
              
             number=1.24; dtostrf(number,5, 2, Inv_Data[z].dcc[1]);
             number=31.22; dtostrf(number,5, 2, Inv_Data[z].dcv[1]);
             number=310.2; dtostrf(number,5, 1, Inv_Data[z].power[1]);

             number=1.25; dtostrf(number,4, 2, Inv_Data[z].dcc[2]);
             number=32.33; dtostrf(number,5, 2, Inv_Data[z].dcv[2]);
             number=330.3; dtostrf(number,5, 1, Inv_Data[z].power[2]);

             number=1.26; dtostrf(number,4, 2, Inv_Data[z].dcc[3]);
             number=33.44; dtostrf(number,5, 2, Inv_Data[z].dcv[3]);
             number=340.4; dtostrf(number,5, 1, Inv_Data[z].power[3]);            
             Serial.println("check power = " + String(Inv_Data[z].power[3]));
             return;
          } else


//          if (strncasecmp(InputBuffer_Serial+3,"WIFICON=",8) == 0) {
//            int len = strlen(InputBuffer_Serial);  
//           for(int i=8; i<len; i++) 
//           {
//              tmp[i] = InputBuffer_Serial[i+7];
//           }
//          Serial.print(F("command = 10;WIFICON=")); Serial.println(String(tmp));
//          // we have a string like "ssid;passwd"
//          //extract the first part until#;
//           
//          } else


//           if (strncasecmp(InputBuffer_Serial+3,"PAIR",7) == 0) {  
//              Serial.println("going to setup zigbee coordinator 0");
//              pairing(0);
//              return;             
//          } else  

//           if (strncasecmp(InputBuffer_Serial+3,"UNPAIR=",6) == 0) {
//            // input can be 10;UNPAIR=0;
//              Serial.println("going to unpair the specified inverter");
//              int kz = String(InputBuffer_Serial[10]).toInt(); 
//              Serial.println("going to unpair inverter " + String(kz));
//              strcpy(Inv_Prop[kz].invID, "0x0000"); //unpair
//                writeStruct("/Inv_Prop" + String(kz) + ".str", kz); 
//              return;             
//          
//          } else 

                   

//           if (strncasecmp(InputBuffer_Serial+3,"DELETE=",7) == 0) {  
//              //input can be 10;DELETE=filename
//              String bestand="";
//              int z = sizeof(InputBuffer_Serial);
//              for(int i=10;  i<z; i++) {
//                bestand += String(InputBuffer_Serial[i]);
//              }
//              //Serial.println("input filename = " + bestand);
//              Serial.println("going to delete file " + bestand);
//              if (SPIFFS.exists(bestand)) { 
//                SPIFFS.remove(bestand);
//               Serial.println("file "+ bestand + " removed!"); 
//                } else {Serial.println("no such file");}
//              return;             
////          
//          } else
          
//           if (strncasecmp(InputBuffer_Serial+3,"ERASE",5) == 0) {  
//              Serial.println("going to delete all inverter files");
//              String bestand;
//              for(int i=0; i<50; i++) 
//              {
//                  String bestand = "/Inv_Prop" + String(i) + ".str";
//                  if (SPIFFS.exists(bestand)) 
//                  {
//                      SPIFFS.remove(bestand);
//                      Serial.println("removed file " + bestand);
//                  }
//
//              }
//              inverterCount = 0;
//              basisConfigsave(); // save inverterCount
//              Serial.println("done");
//              return;             
//          
//          } else          

           if (strncasecmp(InputBuffer_Serial+3,"INIT_N",6) == 0) {  
              Serial.println(F("\n\ninit the zb system for normal operation"));
              coordinator(true); // start coordinator for normal ops
              return;             
          } else
           
//           if (strncasecmp(InputBuffer_Serial+3,"INIT_P",6) == 0) {  
//              Serial.println("init the zb system for pairing");
//              coordinator(false); // start coordinator for pairing
//              //checkZigbeeRadio();
//              return;             
//          } else

           if (strncasecmp(InputBuffer_Serial+3,"HEALTH",6) == 0) {  
              Serial.println(F("\n\nchecking the ZB system"));
              healthCheck();
              return;             
          } else

           
//           if (strncasecmp(InputBuffer_Serial+3,"CC2530_CFG",10) == 0) {  
//              Serial.println("send general config cmds to zb radio ");
//              cc2530Config();
//              return;             
//          } else
           
          if (strncasecmp(InputBuffer_Serial+3,"DOMIDX=",6) == 0) {  
            //input can be 10;POLL=0; 
            //ws.textAll("received " + String( (char*)data) + "<br>"); 
              int kz = String(InputBuffer_Serial[10]).toInt()*100;
              kz+=String(InputBuffer_Serial[11]).toInt()*10;
              kz+=String(InputBuffer_Serial[12]).toInt();
 
              domIdx=kz; // max 450
              Serial.print(F("domIdx = ")); Serial.println(String(domIdx));
              
              return;
          } else 
           
           
           if (strncasecmp(InputBuffer_Serial+3,"ZB_reset",8) == 0) {  
              Serial.println(F("\n\nhard reset the cc2530"));
              ZBhardReset();
              return; 
           } else

//           if (strncasecmp(InputBuffer_Serial+3,"PAIRSIM",7) == 0) {  
//              strncpy(Inv_Prop[1].invID, "0xABCD",6);
//              Serial.print("Inv_Prop[1].invID = " + String(Inv_Prop[1].invID));
//              String bestand = "/Inv_Prop1.str"; // /Inv_Prop1.str
//              writeStruct("/Inv_Prop1.str", 1); // alles opslaan in SPIFFS
//              return;
//          } else

#ifdef TEST
      if (strncasecmp(InputBuffer_Serial+3, "TESTINV",7) == 0)  
      {
        DebugPrintln("command = " + String(InputBuffer_Serial) );  
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
