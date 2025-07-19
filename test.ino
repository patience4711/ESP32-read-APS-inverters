// we can send a zigbee message via serial console
void testMessage(bool console) 
{
      char sendCmd[100]={0};
     
      if(console) diagNose = 1; else diagNose = 2;
      int len;
      if(console) len = strlen( txBuffer );  else len = strlen( InputBuffer_Serial );

      //put all the bytes of inputBuffer_Serial ( or txBuffer) in sendCmd, starting at pos 7
      for(int i=0; i<len; i++) 
      {
         if(console) sendCmd[i] = txBuffer[i+7]; else sendCmd[i] = InputBuffer_Serial[i+7];
      }
      consoleOut("the message = " + String(sendCmd)); 
       //now we send this command
       sendZB(sendCmd);
       // find the answer
       
      decodeGeneralAnswer();
      delayMicroseconds(250);
       
       // cleanup
       memset(&sendCmd, 0, sizeof(sendCmd)); //zero out 
       delayMicroseconds(250);
}

void rawMessage(bool console) {
      char sendCmd[100]={0};
      char reCeived[254]={0};
      if(console) diagNose = 1; else diagNose = 2;
      int len;
      if(console) len = strlen( txBuffer );  else len = strlen( InputBuffer_Serial );

      //put all the bytes of inputBuffer_Serial ( or txBuffer) in sendCmd, starting at pos 7
      for(int i=0; i<len; i++) 
      {
         if(console) sendCmd[i] = txBuffer[i+11]; else sendCmd[i] = InputBuffer_Serial[i+11];
      }
      consoleOut("the message = " + String(sendCmd)); 
       //now we send this command
       sendRaw(sendCmd); // this adds the
       // find the answer
       
      decodeGeneralAnswer();
      
      delayMicroseconds(250);
       
       // cleanup
       memset(&sendCmd, 0, sizeof(sendCmd)); //zero out 
       delayMicroseconds(250);
}


void sendRaw( char printString[] )
{
    char bufferSend[254]={0};
    char byteSend[3]; // never more than 2 bytes 

    //strcpy(bufferSend, printString); // 

    empty_serial2();
    //if (Serial2.availableForWrite() > (uint8_t)strlen(bufferSend))
    if (Serial2.availableForWrite() > (uint8_t)strlen(printString))
    {
        //Serial2.write(0xFE); //we have to send "FE" at start of each command
        for (uint8_t i = 0; i <= strlen(printString) / 2 - 1; i++)
        {
         // we use 2 characters to make a byte
            strncpy(byteSend, printString + i * 2, 2); 
            delayMicroseconds(250);                     //

            Serial2.write(StrToHex(byteSend));        //turn the two chars to a byte and send this
        }
            
            Serial2.flush(); //wait till the full command was sent
            
    }
    
    consoleOut("ZB sent " + String(printString));
     
    //else if (diagNose == 2) ws.textAll("sendZB FE" + String(bufferSend));
}


// ******************************************************************
//                    decode zigbee answer
// ******************************************************************
int decodeGeneralAnswer()
{
    char messageToDecode[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
  
    char s_d[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
    uint8_t Message_begin_offset = 0;    
  
    //retrieve the answer
    strcpy(messageToDecode, readZB(s_d));
    if (readCounter == 0) {
        consoleOut(F("decodeGeneralAnswer: no answer on request"));  
        return 50; //no answer
      }


    char *tail;
    int fault=0;               


    if(strstr(messageToDecode,  "FE01640100") == NULL) // answer to AF_DATA_REQUEST 00=success
    {
     consoleOut( "AF_DATA_REQUEST failed" );
     fault = 10;    
    } else
    if (strstr(messageToDecode, "FE03448000") == NULL) //  AF_DATA_CONFIRM the 00 byte = success
    {
      consoleOut("no AF_DATA_CONFIRM");
      fault = 11;
    } else
    if (strstr(messageToDecode, "FE0345C4") == NULL) //  ZDO_SRC_RTG_IND
    {
      consoleOut("no route receipt");
      //return 12; // this command seems facultative
    } else 
    if (strstr(messageToDecode, "4481") == NULL)
    {
      consoleOut("no  AF_INCOMING_MSG"); // this is the real answer
      fault=13;
    }
    if(fault > 9 ) {
       memset(&messageToDecode, 0, sizeof(messageToDecode)); //zero out 
       delayMicroseconds(250); 
      return fault;
    }
   
    consoleOut("received response " + String(messageToDecode) );
         
    // so now we have a message containing    
    // shorten the message by removing everything before 4481

    tail = split(messageToDecode, "44810000"); // remove the 0000 as well
    //tail = after removing the 1st part
    // in tail at offset 14, 2 bytes with signalQuality reside   
    consoleOut("tail " + String(tail) );
  
    return 0;
} 

#ifdef TEST
void testDecode() {
    // we feed the decoding of a polling answer with an artificial polling answer
    // we always test inverter 0, depending on the type we test the right string
    // So if we want to test a ds3 we have to define inverter0 as ds3
    int type = Inv_Prop[0].invType; //
    // we define an inmessage first
    
    switch(type) {
     case 0: // yc600
       strncpy(inMessage, "FE0164010064FE034480001401D2FE0345C43A1000A8FE724481000006013A101414007100B57CFA00005E408000158215FBFB51B103D40F4117000074CF00000076706A73D06B0496000000000000000172072D88017862E8201F00030555073F0303030100000100000000000000000000000000000000000000000000000000000000000000FEFE3A100E76",300);
       break;
     case 1: // test qs1 johan
       strncpy(inMessage, "FE0164010064FE034480001401D2FE72448100000601C0051414005E00905D5B00005E801000085070FBFB51B103EB0F419300CAF069D9F068C7C068C1206804B868E0000006A80001BB38134D01CCE90E0A01FD1E052201D967D0641F0003055400000000000000000000000000000000000000000000000000002B2A0000FEFEC0050E55",300);
       break;
     case 2: // ds3
       if(testCounter == 0) {
    // from npeters tracefile at 9:53 and 9:57
       strncpy(inMessage, "FE0164010064FE034480001401D2FE0345C4226C00CCFE0345C4226C00CCFE0345C43A1000A8FE724481000006013A101414007100B57CFA00005E703000021300fbfb5cbbbb2000fc0001ffff000000000000000006e506ee00E200EA036e13882D5F01480026ffff052508430049F8C40048C77C00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3896fefe",300);   
    //                                                                                                                                           >                                                   26                      tttt                    100     108       
       } else { //we increased energy values to be able to calc the power
       strncpy(inMessage, "FE0164010064FE034480001401D2FE0345C4226C00CCFE0345C4226C00CCFE0345C43A1000A8FE724481000006013A101414007100B57CFA00005E703000021300fbfb5cbbbb2000fc0001ffff000000000000000006e506ee015900EF010313882E8B01480026ffff052508430050BF39004F849C00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3896fefe",300);   
    //                                                                                                                                           >                                                   26                      tttt                    100     108       
       }
    break;
    }
    //703000021300 FB FB 5C BB BB 20 00 FC 00 01 FF FF 00 00 00 00 00 00 00 00 06 83 06 8A 02 31 02 3A 03 68 13 87   2C 9B  01 
    //703000021300 fb fb 5c bb bb 20 00 02 00 e6 ff ff 00 00 00 00 00 00 00 00 06 f5 06 f9 00 2e 00 34 03 60 13 8a | 17 a7 |00 
    //0             6  7  8  9 10 11 12 13 14 15                21 22                      30                      | time  |40                            50       53 54 55
    
    //BB 00 2A FF FF 05 6B 08 D8 01 1C 4E 79 01 1D BD 26 00 FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF3830FEFE226C70E
    //24 00 1f ff ff 05 42 06 90 00 16 f6 2b 00 18 e4 51 ff ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff3969fefe
    //41          45             50             55  
    
    //DebugPrintln("going to test for invType: " + String(type) + "\n");
    decodePollAnswer(0);
    
    polled[0]=true;
    testCounter += 1; // for the next round we have new string
    mqttPoll(0);
}
#endif
