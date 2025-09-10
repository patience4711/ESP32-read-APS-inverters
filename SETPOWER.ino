int setMaxPower(int which) 
{
// if there is no coordinator, this command fails
// this function tries to program a throttle value in the inverter
// the value is in desiredThrottle[Inv]
// after that the inverter is queried ti determine the programmed value
// when the query succeeds this value is always written in preferences

int calibratedVal;
int Scaled;
char powCommand[85];
char ecu_id_reverse[13];
ECU_REVERSE().toCharArray(ecu_id_reverse, 13);

// calculate a checkval to determine whether the throttle succeeds
// can be used for both inverters
// calibratedVal = static_cast<int>(
//    ceil(desiredThrottle[which] * (1.0 + Inv_Prop[which].calib / 100.0)) 
// );
calibratedVal = desiredThrottle[which] + Inv_Prop[which].calib;
consoleOut ("user input calibrated = " + String(calibratedVal)); 

if(Inv_Prop[which].invType == 2)  // DS3
{
      consoleOut("sending the throttle command for DS3");
      // first we convert the scaled maxPower value to hex, ( msb and lsb )
      // and calculate the validation byte
      Scaled = (calibratedVal * 1659 + 50) / 100; // this rounds up instead of down
      //Scaled = calibratedVal * 16.59;
      uint8_t msb = (Scaled >> 8) & 0xFF; // 0x02
      uint8_t lsb = Scaled & 0xFF;        // 0x58
      // Compute validation byte
      uint8_t s = (msb + lsb) & 0xFF;
      uint8_t vv = (s - 0x29) & 0xFF;
      //Serial.println("for maxpower = " + String(desiredThrottle[which]) + " * 16.59, the values are ");
      //Serial.printf("MSB: %02X, LSB: %02X, VV: %02X\n", msb, lsb, vv);
      snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB06AA270000%02X%02X01%02XFEFE", Inv_Prop[which].invID, ecu_id_reverse, msb, lsb, vv);
      consoleOut("The raw powCommand for DS3 = " + String(powCommand));
       
  }  else
  {
     consoleOut("sending the throttle command for YC600 / QS1");
     Scaled = (calibratedVal * 2889 + 50) / 100; // this rounds up instead of down
     //Scaled = calibratedVal * 28.89;  // 
     char hexStr[5]; // Enough for "FFFFFFFF" + null terminator
     // Convert Scaled to hexadecimal string
     // %04X ensures it's always 4 hex digits, uppercase, zero-padded
     //Serial.println("Scaled = " + String(Scaled));
     snprintf(hexStr, sizeof(hexStr), "%04X", Scaled);
     //cout << "hexStr = " << hexStr << "\n<br>"<< endl;
      //Serial.println("hexStr = " + String(hexStr));
     //construct command
     powCommand[0]='\0'; // make sure its empty
     snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB061C8C02%s00", Inv_Prop[which].invID, ecu_id_reverse, hexStr);
         
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
     
  }    
  // if we are here. powCommand contains the raw throttle command
  // specific to each inverter type
  // we are going to send it and discard the response
    if(zigbeeUp == 1) sendZB(powCommand);
    //we discard the inMessage
    if( waitSerial2Available() ) {
          empty_serial2(); // clear the incoming data
    }
    // now we are going to send the dummy which is equal for each invertertype
     snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB06DE00000000000000FEFE", Inv_Prop[which].invID, ecu_id_reverse);
     consoleOut("the raw dummy command = " + String(powCommand));
    if(zigbeeUp == 1) sendZB(powCommand);
    //we also discard this response if there is any
    if( waitSerial2Available() ) {
          empty_serial2(); // clear the incoming data
    }
  // now we send the query comand which is also the same for both inverters
  snprintf(powCommand, sizeof(powCommand), "2401%s1414060001000F13%sFBFB06DE000000000000E4FEFE", Inv_Prop[which].invID, ecu_id_reverse);
  consoleOut("the raw query command = " + String(powCommand));
  // consoleOut("\nzigbeeUp value = " + String(zigbeeUp));
  #ifndef TEST
  if(zigbeeUp != 1) {
      Serial.println("no zigbee coordinator, fail"); 
      return 15;
    }
  sendZB(powCommand);
  #endif
  //we decode amd analize the query answer
  errorCode = decodeQueryAnswer(which, true);  // true means after a throttle   
  return errorCode;
  //if(errorCode == 0 || errorCode == 15 ) return true; else return false;
}
