// *****************************************************************************
//                       length and checksum
// *****************************************************************************
String checkSumString(char Command[])
{
char bufferCRC[254] = {0};
char bufferCRCdiezweite[254] = {0};

    strncpy(bufferCRC, Command, 2); //as starting point perhaps called "seed" use the first two chars from "Command"
    delayMicroseconds(250);         //give memset a little bit of time to empty all the buffers

    for (uint8_t i = 1; i <= (strlen(Command) / 2 - 1); i++)
    {
        strncpy(bufferCRCdiezweite, Command + i * 2, 2); //use every iteration the next two chars starting with char 2+3
        delayMicroseconds(250);                          //give memset a little bit of time to empty all the buffers
        sprintf(bufferCRC, "%02x", StrToHex(bufferCRC) ^ StrToHex(bufferCRCdiezweite));
        delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
    }
    uint8_t iToUpper = 0;
    while (bufferCRC[iToUpper])
    {
        bufferCRC[iToUpper] = toupper(bufferCRC[iToUpper]);
        iToUpper++;
    }
    Serial.println("bufferCRC = " + String(bufferCRC));
    return String(bufferCRC);
}

// calculate and return the length of the message
String sLengte(char Command[])  
{

char bufferSln[3];

    sprintf(bufferSln, "%02x", (strlen(Command) / 2 - 2));
    delayMicroseconds(250); //give memset a little bit of time to empty all the buffers

    //convert to uppercase
    uint8_t iToUpper = 0;
    while (bufferSln[iToUpper])
    {
        bufferSln[iToUpper] = toupper(bufferSln[iToUpper]);
        iToUpper++;
    }
    // is this null terminated?
    Serial.println("bufferSln = " + String(bufferSln));
    //strcat(bufferSln, "\0");
    //Serial.println("bufferSln + null = " + String(bufferSln));
    return String(bufferSln);
}
// *****************************************************************************
//                 send to zigbee radio
// *****************************************************************************
void sendZigbee(char printString[])
{
//char bufferSend[254];
    char bufferSend[3];

    if (Serial2.availableForWrite() > (uint8_t)strlen(printString))
    {
        Serial2.write(0xFE); //we have to send "FE" at start of each command
        for (uint8_t i = 0; i <= strlen(printString) / 2 - 1; i++)
        {
         // we use 2 characters to make a byte
            strncpy(bufferSend, printString + i * 2, 2); 
            delayMicroseconds(250);                     //give memset a little bit of time to empty all the buffers
            Serial2.write(StrToHex(bufferSend));        //turn the two chars to a byte and send this
        }
            Serial2.flush(); //wait till the full command was sent

        // not good as it comes too fast after the other states so that only this one was visible
        // _actualState = 16; //we send a new string to the CC2530

//        _waiting_for_response = true;
    }
}

// *****************************************************************************
//                 read zigbee radio at swapped serial
// *****************************************************************************
// this function is somewhat unefficient but that seems an advantage due to the 
// slow reaction from the inverter
void readZigbee() {
readCounter = 0;
//fullIncomingMessage[0] = '\0'; //terminate otherwise all is appended
memset( &inMessage, 0, sizeof(inMessage) ); //zero out the 
delayMicroseconds(250);
        
        while (Serial2.available())
        {
// here we have the danger that when readcounter reaches 512, there are 1024 bytes processed 
// the length of a poll answer is usually not more than 223
            if (readCounter < CC2530_MAX_SERIAL_BUFFER_SIZE/2)
            {
                processIncomingByte(Serial2.read());
                readCounter += 1;
            }
            else
            {
                //Serial.read(); // we read from serial to empty the buffer but do not process
             //   ESP.wdtFeed();
                empty_serial2(); // remove all excess data in the buffer at once
            }
            if (Serial2.available() == 0) delay(120); // we wait if there comes more data
        }
            //if we come here there is no serial data anymore
            //if (Serial.available() == 0)  // the buffer is empty
            //{
                //Put lower case in the message to upper
                uint16_t iToUpper = 0; // has to be 16bit because the received message if the YC600 answers is longer then 255
                while (inMessage[iToUpper])
                {
                    inMessage[iToUpper] = toupper(inMessage[iToUpper]);
                    iToUpper++;
                }
            //}
        
        // now we should have catched inMessage
        if (Serial2.available() == 0)
        {
          //  _waiting_for_response = false;
          //  _ready_to_send = true;
        }
    cleanIncoming(); // check for F8 and remove it
}

// format the incoming byte and add it to inMessage
void processIncomingByte(const byte inByte)
{
char oneChar[10] = {0};
    sprintf(oneChar, "%02x", inByte);
    strncat(inMessage, oneChar, 2); // append 
} // end of processIncomingByte



void cleanIncoming() {
// with swaps we get F8 sometimes, this removes it
   if(inMessage[0] == 'F' && inMessage[1] == '8') {
       strcpy(inMessage, &inMessage[2]);
   }
}

// **************************************************************************
//                               data converters
// **************************************************************************

//// calculate and return the length of the message
//char *sLen(char Command[])  
//{
//char bufferSln[254];
//    sprintf(bufferSln, "%02x", (strlen(Command) / 2 - 2));
//    delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
//
//    uint8_t iToUpper = 0;
//    while (bufferSln[iToUpper])
//    {
//        bufferSln[iToUpper] = toupper(bufferSln[iToUpper]);
//        iToUpper++;
//    }
//    // is this null terminated?
//    //DebugPrintln("terminated with ");
//    return bufferSln;
//}
//
//// calculate and return the checksum of the message ****************************
//char *checkSum(char Command[])
//{
//char bufferCRC[254] = {0};
//char bufferCRCdiezweite[254] = {0};
//
//    strncpy(bufferCRC, Command, 2); //as starting point perhaps called "seed" use the first two chars from "Command"
//    delayMicroseconds(250);         //give memset a little bit of time to empty all the buffers
//
//    for (uint8_t i = 1; i <= (strlen(Command) / 2 - 1); i++)
//    {
//        strncpy(bufferCRCdiezweite, Command + i * 2, 2); //use every iteration the next two chars starting with char 2+3
//        delayMicroseconds(250);                          //give memset a little bit of time to empty all the buffers
//        sprintf(bufferCRC, "%02x", StrToHex(bufferCRC) ^ StrToHex(bufferCRCdiezweite));
//        delayMicroseconds(250); //give memset a little bit of time to empty all the buffers
//    }
//    uint8_t iToUpper = 0;
//    while (bufferCRC[iToUpper])
//    {
//        bufferCRC[iToUpper] = toupper(bufferCRC[iToUpper]);
//        iToUpper++;
//    }
//    return bufferCRC;
//}

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
void inverterReset(int which) {
char ecu_id_reverse[13];  
ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
char inv_id[7];
strncpy(inv_id, Inv_Prop[which].invID, strlen(Inv_Prop[which].invID));
char resetCmd[80];
char temp[8]={0};
char command[][50] = {
  "2401",
  "1414060001000F13",
  "FBFB06C1000000000000A6FEFE",
  };
strncpy( resetCmd, command[0], sizeof(command[0]) );
strncat( resetCmd, inv_id + 4, 2 ); // ad the 2nd byte of inv_id
strncat( resetCmd, inv_id + 2, 2 );     // ad the 1st byte of inv_id  
strncat( resetCmd, command[1], sizeof(command[1]) );
strncat( resetCmd, ecu_id_reverse, sizeof(ecu_id_reverse) );
strncat( resetCmd, command[2], sizeof(command[2]) );
DebugPrintln("the resetCmd = " + String(resetCmd));

//should be 2401 103A 1414060001000F13 80 97 1B 01 A3 D6 FBFB06C1000000000000A6FEFE
// got      1414060001000F1380971B01A3D6FBFB06C1000000000000A6FEFE


// calculate and prefix length 
strcpy(temp, sLengte(resetCmd).c_str() ) ;
strcpy(resetCmd, strncat(temp, resetCmd, sizeof(temp) + sizeof(resetCmd))); //build command plus sln at the beginning
// put in the CRC at the end of the command
strcpy(temp, checkSumString(resetCmd).c_str() ) ; 
strcpy( resetCmd, strncat(resetCmd, temp, sizeof(resetCmd) + sizeof(temp)) );

////commented out while testing
//swap_to_zb(); // set serial to the cc2530
sendZigbee(resetCmd);
delay(1000);
////waitSerial(); //check if anything was received
readZigbee();
////read_ZB();
//swap_to_usb(); // set serial back to usb

    if(readCounter == 0) {
      Serial.println("nothing received");
    } 
      Serial.println("readCounter = " + String(readCounter));
      DebugPrintln("received : " + String(inMessage) );
}

// ******************************************************************************
//                    reset all values and send mqtt
// ******************************************************************************
void resetValues(bool energy, bool mustSend) {
      for(int z=0; z<inverterCount; z++) 
      { 
         for(int y=0; y<5; y++ ) strcpy(Inv_Data[z].power[y], "0.0"); // make powervalues null
         DebugPrintln("reset power values");
         if(energy) 
         {
            Inv_Data[z].en_total = 0; // make energy null
            DebugPrintln("reset energy values");
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
