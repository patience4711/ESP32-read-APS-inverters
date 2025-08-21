void querying(int which) {
    //polled[which]=false; //nothing is displayed on webpage

    consoleOut("query inverter " + String(which));
    char queryCommand[65] = {0};
    char ecu_id_reverse[13];
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);    
    //                                            2401  1414060000100F13  FBFB06DE000000000000E4FEFE
    snprintf(queryCommand, sizeof(queryCommand), "2401%s1414060001000F13%sFBFB06DE000000000000E4FEFE", Inv_Prop[which].invID, ecu_id_reverse);
    // put in the CRC at the end of the command done in sendZigbee
    consoleOut("raw queryCommand :" + String(queryCommand));
    #ifndef TEST
    if(zigbeeUp != 1) 
    {
      consoleOut(F("skipping query, coordinator down!")); //
      return;
    }
    #endif
    sendZB(queryCommand);

    // decodeQueryAnswer will read and analyze the answer 
    // the second arg tells that we query without throttle  
    errorCode = decodeQueryAnswer(which, false);     
    // in case of error there are 3 possibilities
    // the throttle and query succeeded, the actual throttle value is in preferences
    // only the query succeeded , the actual throttle value is in preferences
    // the throttle and query both failed, (coodinator down, inverter down or no answer) program -1 
    

    switch( errorCode )
    {
        case 0:
                 yield();
                 break;
         case 15:
                consoleOut("Throttle failed error 15");
                yield();
                break;
        default:
              String key = "maxPwr" + String(which);
              preferences.putUInt(key.c_str(), -1 );
              consoleOut("query failed with errorcode " + String(errorCode)); 
    }
}


// ******************************************************************
//                    decode query zigbee answer
// ******************************************************************
int decodeQueryAnswer(int welke, bool throTTle)
{
// if throttle == true we do this query after a throttle attempt
// which mens that we need to determine desired and programmed
    char messageToDecode[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
      char s_d[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
    uint8_t Message_begin_offset = 0;    
    char *payload;
    int fault=0; 
    int programmedVal;
    //float calibrateFactor;
    consoleOut("decoding inverter " + String(welke));
    //retrieve the answer
    strcpy(messageToDecode, readZB(s_d));
    // for testing we used these fake answers
    #ifdef TEST
    if(Inv_Prop[welke].invType != 2) {
    strcpy(messageToDecode, "FE0164010064FE0345C43A1000A8FE034480001400D3FE0345C43A1000A8FE6E4481000006013A101414006900C3B77000005A408000158215FBFB4DDE041105440FE5020F32B003CF05440FE5020F32B0066604CC0EA3A804D70214050C100FD80ED07A0F32B0056A054F019000641F3FE480068ACE8ACE000130103030190604001D21DB3B6600000000FEFE3A100E00");
    readCounter = 120;
    } else {
    readCounter = 120;
    strcpy(messageToDecode, "FE0164010064FE0345C4A2F600D6FE034480001400D3FE0345C4A2F600D6FE0345C4A2F600D6FE7D448100000601A2F61414008000CC73F7000069704000202594FBFB5CDDDE010426E20013BA14B413EC000A032000500003DD03A403200003E80000000000640003DD03A503350304012C060D03FF045F0E93140E3204890258001374136F125C0014032007D023A6031401BF03D9FFFFFFFFFFFF23A6C8FF1C01FEFEA2F6734E");
    }
    #endif
    
    if (readCounter == 0) {
        consoleOut(F("decodeQueryAnswer: no answer on request"));  
        return 50; //no answer
      }          

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
    
   // if we are here we should have a string containing 4481, we analyse it defferently for YC600 an DS3
    payload = split(messageToDecode, "FBFB"); // remove the 0000 as well   
     //   */
    // for test we gave payload a value
    consoleOut("payload " + String(payload) );
    
    // we must handle the DS3 and YC600 differently 
    
    float calibrateFactor =  1.0 + Inv_Prop[welke].calib / 100.0; //eg 1.12
    String key = "maxPwr" + String(welke);
    consoleOut("preferences key = " + key);
    // **************************************************************
    //                          DECODING YC600  / QS1
    //***************************************************************
    if(Inv_Prop[welke].invType != 2) 
    {
        consoleOut("decoding a YC600 / QS1 ");
        // we know the tail is
        //FBFB4DDE041105440FE5020F32B003CF05440FE5020F32B0066604CC0EA3A804D70214050C100FD80ED07A0F32B0056A054F0>
        //FE480068ACE8ACE000130103030190604001D1B153B6600000000FEFE3A100E5D
        // the powervalue is right behind 2B66
        char target[5] = {"3B66"};

        char *ptr = strstr(payload, target); // find "3B66"
        if (ptr != NULL && ptr - payload >= 4) {
            char before[5];
            strncpy(before, ptr - 4, 4);
            before[4] = '\0'; 
            // before is the hex value of the throttle setting
            // convert from hex string to int
            int decimalValue = (int)strtol(before, NULL, 16) / 28.89; 
            // we calulate the original value 
            //double result = decimalValue / 28.89;
            
            programmedVal = floor(decimalValue / calibrateFactor);
            consoleOut("power value in YC600 = " + String(programmedVal));
            // we always write this value in preferences
            preferences.putInt(key.c_str(), programmedVal );
            Serial.println("preference written = " + String(preferences.getInt(key.c_str(), 0)));
            // if we are here, a valid query is done and we have the throtvalue written
            // if throttled, we determine wheter the throttle succeeded, this is only the 
            // case when desired and programmed match     
            if(throTTle) 
                {
                    consoleOut("the wanted throttle = " + String(desiredThrottle[welke]));
                    if(abs(programmedVal - desiredThrottle[welke]) > 2) 
                    {
                      consoleOut("desired and programmed throttlevalue mismatch"); 
                      // to indicate that there was a failure 
                      return 15;
                    }
                }

        } 
    }  else {// end if(invType != 2)
    // *******************************************************************************
    //                          DECODING DS3
    // ***************************************************************
    consoleOut("decoding a DS3 ");
    //the payload looks like FBFB5CDDDE010426E20013BA14B413EC000A032000500003DD03A403200003E80000000000640003DD03A503350304012C060D03FF045F0E93140E3204890258001374136F125C0014032007D023A6031401BF03D9FFFFFFFFFFFF23A6C8FF1C01FEFEA2F6734E  
    // for test we give payload a conten
    //strcpy(payload, "FBFB5CDDDE0104 26E2  0013BA14B413EC000A032000500003DD03A403200003E80000000000640003DD03A503350304012C060D03FF045F0E93140E3204890258001374136F125C0014032007D023A6031401BF03D9FFFFFFFFFFFF23A6C8FF1C01FEFEA2F6734E");
     
     char powval[5]; // 4 chars + null terminator
     memcpy(powval, payload + 10, 4); // copy "26E2"
     int decimalValue = (int)strtol(powval, NULL, 16) / 16.59;
     programmedVal = floor(decimalValue / calibrateFactor);
     consoleOut("throttle value in DS3 = " + String(programmedVal)) ;
     // we always write this value in preferences
     preferences.putInt(key.c_str(), programmedVal );
     Serial.println("preference written = " + String(preferences.getInt(key.c_str(), 0)));
     // if we are here, a valid query is done and we have the throtvalue written
     // if throttled, we determine wheter the throttle succeeded, this is only the 
     // case when desired and programmed match
     
        if(throTTle) 
        {
            consoleOut("the wanted throttle = " + String(desiredThrottle[welke]));
            if(abs(programmedVal - desiredThrottle[welke]) > 2) {
               consoleOut("desired and programmed throttlevalue mismatch"); 
               // to indicate that there was a failure 
              return 15;
             }
        }
    // if we are here the desired is equal to the present so all oke 
         
    }
    
    return 0;
} 