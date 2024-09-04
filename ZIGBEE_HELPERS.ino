// *****************************************************************************
//                            read zigbee
// *****************************************************************************

char * readZB( char inMess[] ) {
    readCounter = 0;
    String term="";
    //wait a while untill something available
    //if not within the reactiontime we return NULL
    if( !waitSerial2Available() ) {
        inMess[0]='\0';
        term = " readZB nothing to read";
        consoleOut(F(" readZB nothing to read"));
        return inMess;
    }
    
    char oneChar[10] = {0};
    //fullIncomingMessage[0] = '\0'; //terminate otherwise all is appended
    //memset( &inMessage, 0, sizeof(inMessage) ); //zero out the 
    //delayMicroseconds(250);
        
    while (Serial2.available())
        {
        //Serial.print("#");
        // here we have the danger that when readcounter reaches 512, there are 1024 bytes processed 
        // the length of a poll answer is usually not more than 223
        if (readCounter < CC2530_MAX_SERIAL_BUFFER_SIZE/2)
            {
                sprintf( oneChar, "%02X", Serial2.read() ); // always uppercase
                strncat(inMess, oneChar, 2); // append 
                readCounter += 1;
            }
            else
            {
                empty_serial2(); // remove all excess data in the buffer at once
            }
            if (Serial2.available() == 0) delay(120); // we wait if there comes more data
        }
        // now we should have catched inMessage
        if(readCounter == 0) inMess[0]='\0';
        // with swaps we get F8 sometimes, this removes it
        if(inMess[0] == 'F' && inMess[1] == '8') {
           Serial.println("found F8");
           strcpy(inMess, &inMess[2]);
        }   

        consoleOut("readZB " + String(inMess) + "  rc=" + String(readCounter) + "\n");  
        
           //if(diagNose == 1) Serial.println(term); else if(diagNose == 2) ws.textAll(term);
        //}
        delayMicroseconds(250); // give it some time
        return inMess;
}

// *****************************************************************************
//                 send to zigbee radio
// *****************************************************************************
void sendZB( char printString[] )
{
    char bufferSend[254]={0};
    char byteSend[3]; // never more than 2 bytes 
    sprintf(bufferSend, "%02X", (strlen(printString) / 2 - 2)); //now contains a hex representation of the length 
    //first add length and the checksum
    strcat(bufferSend, printString); // now put slen and the rest together
    delayMicroseconds(250);

    strcat(bufferSend,checkSumString(bufferSend).c_str()) ;
    
    //until here this works!
    empty_serial2();
    if (Serial2.availableForWrite() > (uint8_t)strlen(bufferSend))
    {
        Serial2.write(0xFE); //we have to send "FE" at start of each command
        for (uint8_t i = 0; i <= strlen(bufferSend) / 2 - 1; i++)
        {
         // we use 2 characters to make a byte
            strncpy(byteSend, bufferSend + i * 2, 2); 
            delayMicroseconds(250);                     //

            Serial2.write(StrToHex(byteSend));        //turn the two chars to a byte and send this
        }
            
            Serial2.flush(); //wait till the full command was sent
            
    }
    
    consoleOut("  sendZB FE" + String(bufferSend));
     
    //else if (diagNose == 2) ws.textAll("sendZB FE" + String(bufferSend));
}

// *****************************************************************************
//                  return checksum as a string
// *****************************************************************************
String checkSumString(char Command[])
{
char bufferCRC[254] = {0};
char bufferCRC_2[254] = {0};

    strncpy(bufferCRC, Command, 2); //as starting point perhaps called "seed" use the first two chars from "Command"
    delayMicroseconds(250);         //give memset a little bit of time to empty all the buffers

    for (uint8_t i = 1; i <= (strlen(Command) / 2 - 1); i++)
    {
        strncpy(bufferCRC_2, Command + i * 2, 2); //use every iteration the next two chars starting with char 2+3
        delayMicroseconds(250);                          //give memset a little bit of time to empty all the buffers
        sprintf(bufferCRC, "%02X", StrToHex(bufferCRC) ^ StrToHex(bufferCRC_2));
        delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
    }
    return String(bufferCRC);
}


// **************************************************************************
//                               data converters
// **************************************************************************

// calculate and return the length of the message
char *sLen(char Command[])  
{
    char bufferSln[9]; // why is this so big 254
    sprintf(bufferSln, "%02X", (strlen(Command) / 2 - 2));
    delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
    return bufferSln;
}


// convert a char to Hex ******************************************************
int StrToHex(char str[])
{
    return (int)strtol(str, 0, 16);
}

// reverse the ecu id **********************************************************
String ECU_REVERSE() {
   String ecu_id = String(ECU_ID);
   String reverse = ecu_id.substring(10,12) + ecu_id.substring(8,10) + ecu_id.substring(6,8) + ecu_id.substring(4,6) + ecu_id.substring(2,4) + ecu_id.substring(0,2);
   return reverse;
}

// ******************************************************************************
//                   reboot an inverter
// *******************************************************************************
// ******************************************************************************
//                   reboot an inverter
// *******************************************************************************
void inverterReboot(int which) {
    char ecu_id_reverse[13];  
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
    if(zigbeeUp != 1) {
       ws.textAll("skip inverter reboot, zigbee down");
       return; 
    }

//swap_to_usb ();
//    Serial.println("sending the reboot message");
    
    //char inv_id[7];
    //strncpy(inv_id, Inv_Prop[which].invID, strlen(Inv_Prop[which].invID));
    char rebootCmd[57]={0};
    char s_d[200]={0};
    
      char command[][50] = {
        "2401",
        "1414060001000F13",
        "FBFB06C1000000000000A6FEFE",
        };
//Serial.println("constructing command");
      strncpy( rebootCmd, command[0], sizeof(command[0]) );
      strncat( rebootCmd, Inv_Prop[which].invID, 4 ); // ad the 2nd byte of inv_id
      strncat( rebootCmd, command[1], sizeof(command[1]) );
      strncat( rebootCmd, ecu_id_reverse, sizeof(ecu_id_reverse) );
      strncat( rebootCmd, command[2], sizeof(command[2]) );
//Serial.println("command: " + String(rebootCmd));
      ws.textAll("the rebootCmd = " + String(rebootCmd));

       //2401 BBAA 1414060001000F13 80971B01A3D6 FBFB06C1000000000000A6FEFE
      //should be 2401 103A 1414060001000F13 80 97 1B 01 A3 D6 FBFB06C1000000000000A6FEFE
      // got      1414060001000F1380971B01A3D6FBFB06C1000000000000A6FEFE
      //          2401 BBAA 1414060001000F13 80971B01A3D6 1414060001000F13
      //  2401 3A10 1414060001000F13 80971B01B3D6 FBFB06C1000000000000A6FEFE
      // put sln at the beginning and CRC at the end done by sendZigbee()
//swap_to_zb();    

      sendZB(rebootCmd);
      delay(1000);
      //char s_d[150]={0};
      readZB(s_d);

//      if(readCounter == 0) {
//      } 
//      ws.textAll("received : " + String(inMess) );
}

// ******************************************************************************
//                    reset all values and send mqtt
// ******************************************************************************
void resetValues(bool energy, bool mustSend) {
      for(int z=0; z<inverterCount; z++) 
      { 
         for(int y=0; y<5; y++ ) Inv_Data[z].power[y] = 0.0; // make powervalues null
         //DebugPrintln("reset power values");
         if(energy) 
         {
            Inv_Data[z].en_total = 0; // make energy null
            //DebugPrintln("reset energy values");
         }
         if(mustSend) mqttPoll(z); // send the null values
      }  
}

bool waitSerial2Available() // wait untill something 's available
{
  //We start with a minimum delay
  //delay(800);
  unsigned long wait = millis();
  while ( !Serial2.available() )
      {
      if ( millis() - wait > 2000) return false; // return false after 2000 milis time out
      }
  // if we are here there is something available
   delay(500);
   return true;
}

char *split(char *str, const char *delim)
{
    char *p = strstr(str, delim);

    if (p == NULL)
        return NULL; // delimiter not found

    *p = '\0';                // terminate string after head
    return p + strlen(delim); // return tail substring
}
