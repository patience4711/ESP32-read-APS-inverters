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
    
    if(zigbeeUp != 1) 
    {
      consoleOut(F("skipping query, coordinator down!")); //
      return;
    }
    
    sendZB(queryCommand);

    // decodeQueryAnswer will read and analyze the answer   
    errorCode = decodeQueryAnswer(which);     
    switch( errorCode )
    {
        case 0:
                 //polled[which] = true;
                 //yield();
                 //mqttPoll(which); //
                 yield();
                 break;
        default:
              consoleOut("query failed with errorcode " + String(errorCode)); 
    }
}


// ******************************************************************
//                    decode query zigbee answer
// ******************************************************************
int decodeQueryAnswer(int welke)
{
    char messageToDecode[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
      char s_d[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
    uint8_t Message_begin_offset = 0;    
    char *payload;
    int fault=0; 
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
    // if(strstr(messageToDecode,  "FE01640100") == NULL) // answer to AF_DATA_REQUEST 00=success
    // {
    //  consoleOut( "AF_DATA_REQUEST failed" );
    //  fault = 10;    
    // } else
    // if (strstr(messageToDecode, "FE03448000") == NULL) //  AF_DATA_CONFIRM the 00 byte = success
    // {
    //   consoleOut("no AF_DATA_CONFIRM");
    //   fault = 11;
    // } else
    // if (strstr(messageToDecode, "FE0345C4") == NULL) //  ZDO_SRC_RTG_IND
    // {
    //   consoleOut("no route receipt");
    //   //return 12; // this command seems facultative
    // } else 
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
    // for test we give payload a value
    consoleOut("payload " + String(payload) );
    
    // we must handle the DS3 and YC600 differently 
    
    if(Inv_Prop[welke].invType != 2) {
    // for test we give payload a content
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
        int decimalValue = (int)strtol(before, NULL, 16) / 28.89; // convert from hex string to int
        //double result = decimalValue / 28.89;
        consoleOut("power value YC600 = " + String(decimalValue));
        // this should match with the set throttle value which is
        consoleOut("Inv_Prop[welke].maxPower = " + String(Inv_Prop[welke].maxPower));
        if(abs(decimalValue - Inv_Prop[welke].maxPower) > 2) return 15;
        } else {
        consoleOut("0x3B66 not found or not enough characters before it.");
        return 15;
       }
    }  else {// end if(invType != 2)
    
    // this is ds3
    consoleOut("decoding a DS3 ");
    //the payload looks like FBFB5CDDDE010426E20013BA14B413EC000A032000500003DD03A403200003E80000000000640003DD03A503350304012C060D03FF045F0E93140E3204890258001374136F125C0014032007D023A6031401BF03D9FFFFFFFFFFFF23A6C8FF1C01FEFEA2F6734E  
    // for test we give payload a conten
    //strcpy(payload, "FBFB5CDDDE0104 26E2  0013BA14B413EC000A032000500003DD03A403200003E80000000000640003DD03A503350304012C060D03FF045F0E93140E3204890258001374136F125C0014032007D023A6031401BF03D9FFFFFFFFFFFF23A6C8FF1C01FEFEA2F6734E");
     
     char powval[5]; // 4 chars + null terminator
     memcpy(powval, payload + 10, 4); // copy "26E2"
     int decimalValue = (int)strtol(powval, NULL, 16) / 16.59;
     String term="power value DS3 = " + String(powval) + " this is dec. " + String(decimalValue);
     consoleOut(term);
     consoleOut("Inv_Prop[welke].maxPower = " + String(Inv_Prop[welke].maxPower));
     if(abs(decimalValue - Inv_Prop[welke].maxPower) > 2) return 15;
    }
    
    return 0;
} 