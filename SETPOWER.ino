

bool setMaxPower(int which) 
{
// if there is no coordinator, this command fails


int Scaled;
char powCommand[85];
char ecu_id_reverse[13];
ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
// first we convert the scaled maxPower value to hex, ( msb and lsb )

if(Inv_Prop[which].invType == 2)  // DS3
{// and calculate the validation byte
      consoleOut("sending the throttle command for DS3");
      Scaled = Inv_Prop[which].maxPower * 16.59;
      uint8_t msb = (Scaled >> 8) & 0xFF; // 0x02
      uint8_t lsb = Scaled & 0xFF;        // 0x58
      // Compute validation byte
      uint8_t s = (msb + lsb) & 0xFF;
      uint8_t vv = (s - 0x29) & 0xFF;
      Serial.println("for maxpower = " + String(Inv_Prop[which].maxPower) + " * 16.59, the values are ");
      Serial.printf("MSB: %02X, LSB: %02X, VV: %02X\n", msb, lsb, vv);
      snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB06AA270000%02X%02X01%02XFEFE", Inv_Prop[0].invID, ecu_id_reverse, msb, lsb, vv);
      consoleOut("The raw powCommand for DS3 = " + String(powCommand));
      // now we send the command immediately and discard the response
      if(zigbeeUp == 1) sendZB(powCommand);
      //we discard the inMessage
      if( waitSerial2Available() ) {
          empty_serial2(); // clear the incoming data
      }
       //now we send a nonsense command this should look like
      //24013A101414060000050F1380971B01B3D6FBFB06DE00000000000000FEFE
      snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB06DE00000000000000FEFE", Inv_Prop[0].invID, ecu_id_reverse);
      if(zigbeeUp == 1) sendZB(powCommand);
      //we discard this inMessage also
      if( waitSerial2Available() ) {
          empty_serial2(); // clear the incoming data
    }
    // now we should send the inverterquery to find out whether the command succeeded or not.
    snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB06DE000000000000E4FEFE", Inv_Prop[which].invID, ecu_id_reverse); 
  
  }  else
  {
   consoleOut("sending the throttle command for YC600 / QS1");
   Scaled = Inv_Prop[which].maxPower * 28.89;
     char hexStr[5]; // Enough for "FFFFFFFF" + null terminator
     // Convert Scaled to hexadecimal string
     // %04X ensures it's always 4 hex digits, uppercase, zero-padded
     Serial.println("Scaled = " + String(Scaled));
     snprintf(hexStr, sizeof(hexStr), "%04X", Scaled);
     //cout << "hexStr = " << hexStr << "\n<br>"<< endl;
      Serial.println("hexStr = " + String(hexStr));
     //construct command
     powCommand[0]='\0'; // make sure its empty
     snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB061C8C02%s00", Inv_Prop[0].invID, ecu_id_reverse, hexStr);
         
     // now we calculate the checksum( the last 2 bytes)
     int getal=0;
     char bytes2summerize[15]={"061C8C02"};
     // add the trottle value
     strncat(bytes2summerize, hexStr, sizeof(bytes2summerize) - strlen(bytes2summerize));
     strncat(bytes2summerize, "00", sizeof(bytes2summerize) - strlen(bytes2summerize));
    //
     Serial.println("the last 7 bytes are " + String(bytes2summerize));
     for(int x=0; x < strlen(bytes2summerize); x += 2) {
        // Convert two hex chars into a byte
        char hexByte[3] = { bytes2summerize[x], bytes2summerize[x + 1], '\0' };
        //cout <<"hexByte = " << hexByte << "\n<br>" << endl;
        int value = strtol(hexByte, NULL, 16); // convert hex string to int
        getal += value;
    }
    char hexString[10];  // Enough space for hex + null terminator
    // Convert int to hex and store in hexString
    snprintf(hexString, sizeof(hexString), "%04X", getal);
    //append to sendCmd
    strncat(powCommand, hexString,  sizeof(powCommand) - strlen(powCommand) - 1);
    strncat(powCommand, "FEFE",  sizeof(powCommand) - strlen(powCommand) - 1);
    consoleOut("The raw powCommand for YC600 / QS1 = " + String(powCommand));
    // we can send it not waiting for response
    if(zigbeeUp == 1) sendZB(powCommand);
    //we discard the inMessage
    if( waitSerial2Available() ) {
          empty_serial2(); // clear the incoming data
    }
    //now we send a nonsense command this should look like
    //24013A101414060000050F1380971B01B3D6FBFB06DE00000000000000FEFE
    snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB06DE00000000000000FEFE", Inv_Prop[0].invID, ecu_id_reverse);
    if(zigbeeUp == 1) sendZB(powCommand);
    //we discard this inMessage also
    if( waitSerial2Available() ) {
          empty_serial2(); // clear the incoming data
    }
    // now we should send the inverterquery to find out whether the command succeeded or not.
    snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB06DE000000000000E4FEFE", Inv_Prop[which].invID, ecu_id_reverse); 
  }    
  
  // now we can send the
  // consoleOut("\nzigbeeUp value = " + String(zigbeeUp));
  if(zigbeeUp != 1) {
      Serial.println("no zigbee coordinator, fail"); 
      return false;
    }
  
  sendZB(powCommand);
  // it seems that the response is the same as from the query command so we decode query answer
  errorCode = decodeQueryAnswer(which);     
  
  if(errorCode == 0) return true; else return false;

}
