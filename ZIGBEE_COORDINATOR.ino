bool coordinator(bool normal) { // if true we send the extra command for normal operations
    DebugPrintln("start coordinator");  
    coordinator_init();
    if(normal) sendNO();
    // now check if running
    delay(1000); // to give the ZB the time to start
    empty_serial2(); //otherwise the check fails
    if ( checkCoordinator() == 0 ) // can be 0 1 or 2
     
    {
        Update_Log("zigbee" , "ZB coordinator started");
        DebugPrintln("ZB coordinator started");
        //busyState = 0;
        ledblink(5,100);
        return true;
      
    } else {
        Update_Log("zigbee" , "starting ZB coordinator failed");
        DebugPrintln("starting ZB coordinator failed");
        return false;  
    }
}

void coordinator_init() {

    /*
    * init the coordinator takes the following procedure
    * 1st we send a resetcommand 4 times Sent=FE0141000040
    * then we send the following commands
    *  0 Sent=FE03260503010321
    *  Received=FE0166050062
    *  1 Sent=FE0141000040
    *  Received=FE064180020202020702C2
    *  2 Sent=FE0A26050108FFFF80971B01A3D856
    *  Received=FE0166050062
    *  3 Sent=FE032605870100A6
    *  Received=FE0166050062
    *  4 Sent=FE 04 26058302 D8A3 DD  should be ecu_id the fst 2 bytes
    *  Received=FE0166050062
    *  5 Sent=FE062605840400000100A4
    *  Received=FE0166050062
    *  6 Sent=FE0D240014050F0001010002000015000020
    *  Received=FE0164000065
    *  7 Sent=FE00260026
    *  8 Sent=FE00670067
    *  Received=FE0145C0098D
    *  received FE00660066 FE0145C0088C FE0145C0098D F0F8FE0E670000FFFF80971B01A3D8000007090011
    *  now we can pair if we want to or else an extra command for retrieving data (normal operation)
    *  9 for normal operation we send cmd 9
    *  Finished. Heap=26712
    *  
    */

    DebugPrintln("init zb coordinator");
    zigbeeUp = 11; //initial it is initializing 11, 0=down 1=up
    yield();    
    char ecu_id_reverse[13]; //= {ECU_REVERSE()};
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
    char initCmd[254]={0};

    // commands for setting up coordinater
    char initBaseCommand[][254] = {
      "2605030103", // ok   this is a ZB_WRITE_CONFIGURATION CMD //changed to 01
      "410000",     // ok   ZB_SYS_RESET_REQ
      "26050108FFFF", // + ecu_id_reverse,  this is a ZB_WRITE_CONFIGURATION CMD
      "2605870100",  //ok 
      "26058302",  // + ecu_id.substring(0,2) + ecu_id.substring(2,4),
      "2605840400000100", //ok
      "240014050F00010100020000150000",  //AF_REGISTER register an application’s endpoint description
      "2600", //ok ZB_START_REQUEST
    };

    // we start with a hard reset of the zb module
    ZBhardReset();
    delay(500);

    // construct some of the commands
    // ***************************** command 2 ********************************************
    // command 2 this is 26050108FFFF we add ecu_id reversed
    strncat(initBaseCommand[2], ecu_id_reverse, sizeof(ecu_id_reverse)); 
    delayMicroseconds(250);
    DebugPrintln("initBaseCmd 2 constructed = " + String(initBaseCommand[2]));  // ok
    
    // ***************************** command 4 ********************************************
    // command 4 this is 26058302 + ecu_id_short 
    strncat(initBaseCommand[4], ECU_ID, 2);
    strncat(initBaseCommand[4], ECU_ID + 2, 2);
    delayMicroseconds(250);
    DebugPrintln("initBaseCmd 4 constructed = " + String(initBaseCommand[4]));

    // send the commands from 0 to 7
    for (int y = 0; y < 8; y++) 
    {
      //cmd 0 tm / 9 alles ok
      strcpy(initCmd, initBaseCommand[y]);
      DebugPrintln("cmd : " + String(y)); 
      char comMand[254];
      //first put the sLen in comMand and than add the command itself
      sprintf(comMand, "%02X", (strlen(initCmd) / 2 - 2));
      strcat(comMand, initCmd);
      // now we have comMand = len + initComd
      delayMicroseconds(250);
      //DebugPrintln("comMand after sLen" + String(y) + " = " + String(comMand));
      // CRC at the end of the command
      //strcat(comMand,checkSumString(comMand).c_str()) ; // do this in sendZigbee
      DebugPrintln("comMand inc len ex checkSum = " + String(comMand));
      
      delayMicroseconds(250);
      DebugPrintln("zb send cmd " + String(y));
  
      sendZigbee(comMand);
      ledblink(1,50);
      //check if anything was received
      waitSerial2Available();
      readZigbee();

      DebugPrintln("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
    }
    // now all the commands are send 
    //first clean (zero out) initCmd
    
    memset(&initCmd, 0, sizeof(initCmd)); //zero out all buffers we could work with "messageToDecode"
    delayMicroseconds(250);
    memset(&initBaseCommand, 0, sizeof(&initBaseCommand)); //zero out all buffers we could work with "messageToDecode"
    delayMicroseconds(250);    

}
// **************************************************************************************
//                the extra command for normal operations
// **************************************************************************************
void sendNO() {
    char noCmd[90] ={0} ;   //  we have to send the 10th command
    char ecu_id_reverse[13]; //= {ECU_REVERSE()};
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
  
    snprintf(noCmd, sizeof(noCmd), "2401FFFF1414060001000F1E%sFBFB1100000D6030FBD3000000000000000004010281FEFE", ecu_id_reverse);
    //Serial.println("noCmd = " + String(noCmd));    
  
    //add sln at the beginning
    char comMand[100];
    sprintf(comMand, "%02X", (strlen(noCmd) / 2 - 2));
    strcat(comMand, noCmd);
      
    //add the CRC at the end of the command is done by sendZigbee
    DebugPrintln("normal ops initCmd exc checksum = " + String(comMand));
    DebugPrintln("sending N.O. cmd");
  
    sendZigbee(comMand);
  
    //check if anything was received
    waitSerial2Available();
    readZigbee();
    if(readCounter != 0) {
       DebugPrintln("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
      } else {
       DebugPrintln("no answer"); 
    }
    DebugPrintln("zb initializing ready, now check");
    //zero out 
    memset(&comMand, 0, sizeof(comMand)); //zero out
    delayMicroseconds(250);    
    memset(&noCmd, 0, sizeof(noCmd)); //zero out
    delayMicroseconds(250);

}
