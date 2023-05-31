void polling(int which) {
    polled[which]=false; //nothing is displayed on webpage
    if(zigbeeUp != 1) 
    {
      if(diagNose != 0) consoleOut(F("skipping poll, coordinator down!")); //
      return;
    }
   
    char pollCommand[65] = {0};
    char ecu_id_reverse[13];
    ECU_REVERSE().toCharArray(ecu_id_reverse, 13);    
    
    snprintf(pollCommand, sizeof(pollCommand), "2401%s1414060001000F13%sFBFB06BB000000000000C1FEFE", Inv_Prop[which].invID, ecu_id_reverse);
    delayMicroseconds(250);
    // put in the CRC at the end of the command done in sendZigbee
    if(diagNose != 0) consoleOut("pollCommand ex checksum:" + String(pollCommand));
    //} else
    //if(diagNose == 2) ws.textAll ("pollCommand ex checksum:" + String(pollCommand));

    sendZB(pollCommand);

    // decodePollAnswer will read and analyze the answer   
    errorCode = decodePollAnswer(which);     
    switch( errorCode )
    {
        case 0:
                 polled[which] = true;
                 yield();
                 mqttPoll(which); //
                 yield();
                 break;
        default:
              if( diagNose != 0 ) consoleOut("polling failed with errorcode " + String(errorCode)); 
    }
}
