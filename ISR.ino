// *******************************************************************************************
//                  interrupt service routine
// *******************************************************************************************
// this routine is called when the button is pressed
// the first part is a debouncer and takes care for the case of grid failure
// when the program would think that a button has been pressed.
// next we check if the button was short or long pressed.

// *******************************************************************************************
//                  interrupt service routine
// *******************************************************************************************
// this routine is called when the button is pressed
// the first part is a debouncer and takes care for the case of grid failure
// when the program would think that a button has been pressed.
// next we check if the button was short or long pressed.

 IRAM_ATTR void isr() {
 actionFlag = 15;
 }

void buttonPressed() {
  int val = 0;
  detachInterrupt(knop); // prevent interrupts during the isr
  Serial.println("interrupt");
  //DebugPrintln(value);
  //we came here because the button was pressed. Now we check asfer a few
  //miliseconds if it still is, otherwise it was a not a manual press 
  unsigned long starttime = millis(); // save the current time in currentMillis
  unsigned long endtime = millis();
    while (endtime - starttime  <= 50) 
    { //2000 millis = 2 sec
      endtime = millis();
    }
  val = digitalRead(knop);
  if (val == 1) { // means the button is released
   //DebugPrintln("button was pressed too short, accidental spike ");
    attachInterrupt(digitalPinToInterrupt(knop), isr, FALLING);
    return; // jump out, the button is high so it was a coincidence
  }
//if we are here we passed the debounce
   digitalWrite(led_onb, HIGH); // led on
 //we wait again but longer 
   Serial.println("button still pressed, so not accidentally");
 // first the led is flashed
      while (endtime - starttime  <= 500) { //2000 millis = 2 sec
      endtime = millis();
      }
      digitalWrite(led_onb, LOW); // led off
      while (endtime - starttime  <= 1500) 
      { //2000 millis = 2 sec
      endtime = millis();
      }  

  // Now test the button again
  val = digitalRead(knop);
  if (val == 0)  // still pressed
  { 
     // we wait another time, somewhat longer  
     Serial.println("button still pressed");
     while (endtime - starttime  <= 6000) //2000 millis = 2 sec
     { 
     endtime = millis();
     }
    // wdt_disable(); //othersise it's going to reset
     //first test if the button is still pressed
     //otherwise it's going to reset after a clumpsy operation
     val = digitalRead(knop);
     if (val == 1) // means the button is released
         { 
           attachInterrupt(digitalPinToInterrupt(knop), isr, FALLING);
           return; // jump out, the button is high so it was a clumpsiness
         }
     digitalWrite(led_onb, HIGH); //de onboard led aan
     //Serial.println(F("button still pressed, onboard led on"));
     //next we wait some time again
     while (endtime - starttime  <= 9000) //2000 millis = 2 sec
     { 
         endtime = millis();
     }
           
         digitalWrite(led_onb, LOW); //the onboard led off
         // we test the button again, 
         val = digitalRead(knop);
         if (val == 0) // still pressed
         { 
               Serial.println(F("button still pressed, open accesspoint"));
               actionFlag = 11;
               //attachInterrupt(digitalPinToInterrupt(knop), isr, FALLING);
               return;
                      
          }
          else 
          {
              Serial.println("button finally released, reboot");
              ESP.restart(); // reboot
          }
  } 
 else 
  { //
  // the button was release after the seccond evaluation
  // so it was a switch command
  // if the switch is off it should go on or reversed
  // put here your switch logic
  //      if(some evaluation){ 
  //      // here we can put our switch off command
  //      ledsOffNow(true, true, "button");
  //     }
  //     else 
  //     { // ledState =  was 0 so switched off 
  //     // here we can put our switch on command
  //     ledsOnNow(true, false, "button");
  //     }
  // attach the interrupt again     
     attachInterrupt(digitalPinToInterrupt(knop), isr, FALLING);  
 }
}
