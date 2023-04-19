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
DebugPrintln("init zb coordinator");
zigbeeUp = 11; //initial it is initializing 11, 0=down 1=up
yield();

// init the coordinator takes the following procedure
// 1st we send a resetcommand 4 times Sent=FE0141000040
// then we send the following commands
//34:  0 Sent=FE03260503010321
//35: Received=FE0166050062
//39:  1 Sent=FE0141000040
//40: Received=FE064180020202020702C2
//44:  2 Sent=FE0A26050108FFFF80971B01A3D856
//45: Received=FE0166050062
//49:  3 Sent=FE032605870100A6
//50: Received=FE0166050062
//54:  4 Sent=FE 04 26058302 D8A3 DD  should be ecu_id the fst 2 bytes
//55: Received=FE0166050062
//59:  5 Sent=FE062605840400000100A4
//60: Received=FE0166050062
//64:  6 Sent=FE0D240014050F0001010002000015000020
//65: Received=FE0164000065
//69:  7 Sent=FE00260026
//74:  8 Sent=FE00670067
//75:  Received=FE0145C0098D
// received FE00660066 FE0145C0088C FE0145C0098D F0F8FE0E670000FFFF80971B01A3D8000007090011
//     9 for normal operastion we send cmd 9
//79: Finished. Heap=26712
//now we can pair if we want to or else an extra command for retrieving data

char ecu_id_reverse[13]; //= {ECU_REVERSE()};
ECU_REVERSE().toCharArray(ecu_id_reverse, 13);
char initCmd[254]={0};
//char temp[8]={0};


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
//  "6700", // the checkZigbeeRadio we can skip this, instead do checkZigbeeRadio

// we start with a hard reset of the zb module
ZBhardReset();
delay(500);

// construct some of the commands
// ***************************** command 2 ********************************************
// command 2 this is 26050108FFFF we add ecu_id reversed
strncat(initBaseCommand[2], ecu_id_reverse, sizeof(ecu_id_reverse)); 
delayMicroseconds(250);
DebugPrintln("initCmd 2 constructed = " + String(initBaseCommand[2]));  // ok

// ***************************** command 4 ********************************************
// command 4 this is 26058302 + ecu_id_short 
strncat(initBaseCommand[4], ECU_ID, 2);
strncat(initBaseCommand[4], ECU_ID + 2, 2);
delayMicroseconds(250);
DebugPrintln("initCmd 4 constructed = " + String(initBaseCommand[4]));

// send the commands from 0 to 7
    for (int y = 0; y < 8; y++) 
    {
      //cmd 0 tm / 9 alles ok
      strcpy(initCmd, initBaseCommand[y]);
      DebugPrintln("cmd : " + String(y)); 
      //DebugPrintln(String(initCmd));
      //add sLen at the beginning
      //strcpy(temp, sLengte(initCmd).c_str() ) ;
      //strcpy(initCmd, strncat( temp, initCmd, sizeof(temp) + sizeof(initCmd)) ); 
      //DebugPrintln("initCmd after sLen" + String(y) + " = " + String(initCmd));
      char comMand[254];
      sprintf(comMand, "%02X", (strlen(initCmd) / 2 - 2));
      delayMicroseconds(250);    
      strcat(comMand, initCmd);
      // now we have comMand = len + initComd
      delayMicroseconds(250);
      //DebugPrintln("comMand after sLen" + String(y) + " = " + String(comMand));
      // CRC at the end of the command
      //we put the checksum in temp
      //strcpy(temp, checkSumString(comMand).c_str() ) ;      
      //strcpy(comMand, strncat(comMand, temp, sizeof(comMand) + sizeof(temp)));
      //strcat(comMand, temp);
      strcat(comMand,checkSumString(comMand).c_str()) ;
      DebugPrintln("comMand after len and checkSum = " + String(comMand));
      
      delayMicroseconds(250);
      DebugPrintln("zb send cmd " + String(y));
  
      sendZigbee(comMand);
      ledblink(1,50);
      //delay(1000); // give the inverter the chance to answer
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
  char initCmd[254] ={0} ;   //  we have to send the 10th command
  //char temp[128]={0};
  char ecu_id_reverse[13]; //= {ECU_REVERSE()};
  ECU_REVERSE().toCharArray(ecu_id_reverse, 13);

    char lastCmd[][100] = {
    "2401FFFF1414060001000F1E",
    "FBFB1100000D6030FBD3000000000000000004010281FEFE",       
    };
    //Serial.println("free heap after lastCmd : " + String(ESP.getFreeHeap()));
    //// ***************************** command 9 ********************************************
    //// now send command 9 this is "2401FFFF1414060001000F1E", + ecu_id_reverse + FBFB1100000D6030FBD3000000000000000004010281FEFE"
    strncat(lastCmd[0], ecu_id_reverse, sizeof(ecu_id_reverse));
    //Serial.println("initCmd 9 after ecu_reverse = " + String(initCmd));
    strncat(lastCmd[0], lastCmd[1], sizeof(lastCmd[1]));
    delayMicroseconds(250);
    //strcpy(initCmd, lastCmd[0]);
    DebugPrintln("initCmd NO constructed = " + String(initCmd));
    //add sln at the beginning
      char comMand[254];
      sprintf(comMand, "%02X", (strlen(lastCmd[0]) / 2 - 2));
      delayMicroseconds(250);
      Serial.print("len = "); Serial.println(String(comMand));     
      strcat(comMand, lastCmd[0]);
    
    //put in the CRC at the end of the command
    //strcpy(temp, checkSumString(comMand).c_str() ); 
    //DebugPrintln("temp after checksum  = " + String(temp));
    //strcpy( comMand, strncat(comMand, temp, sizeof(comMand) + sizeof(temp)) );
    //strcat(comMand, temp);
    strcat(comMand,checkSumString(comMand).c_str()) ;
    DebugPrintln("normal ops initCmd after len and checksum = " + String(comMand));
    delayMicroseconds(250);
    // send and read
//    if(Log) Update_Log("zb-out",initCmd);
    DebugPrintln("sending NO cmd");
//    Serial.flush();

    sendZigbee(comMand);
    //delay(1000); // give the inverter the chance to answer
    //check if anything was received
    waitSerial2Available();
    readZigbee();

    DebugPrintln("inMessage = " + String(inMessage) + " rc = " + String(readCounter));
//    if(readCounter != 0) {
//      //Serial.println("got answer");
//      //Serial.flush();
//      //Serial.println("free heap : " + String(ESP.getFreeHeap()));      
////      if(Log) Update_Log("zb-in", String(inMessage) );
//      DebugPrintln("zb received " + String(inMessage));
//     }
    DebugPrintln("zb normal ops initialized");
    //zero out 
    memset(&initCmd, 0, sizeof(initCmd)); //zero out all buffers we could work with "messageToDecode"
    delayMicroseconds(250);    
    memset(&lastCmd, 0, sizeof(lastCmd)); //zero out all buffers we could work with "messageToDecode"
    delayMicroseconds(250);

}
