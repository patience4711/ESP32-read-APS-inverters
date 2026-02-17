

void printStruct( String bestand ) {
//input String bestand = "/Inv_Prop" + String(x) + ".str";
//input String bestand = "/timerProp" + String(x) + ".str";
      //String bestand = bestand + String(i) + ".str"
      //leesStruct(bestand); is done at boottime
      int timerNr = bestand.substring(10,11).toInt();
      Serial.println("timers[" + String(timerNr) + "].Active = " + String(timers[timerNr].Active ));
      Serial.println("timers[" + String(timerNr) + "].on_mode = " + String(timers[timerNr].on_mode )); 
      Serial.println("timers[" + String(timerNr) + "].of_mode = " + String(timers[timerNr].of_mode ));    
      Serial.println("timers[" + String(timerNr) + "].on_hh = " + String(timers[timerNr].on_hh));
      Serial.println("timers[" + String(timerNr) + "].of_hh = " + String(timers[timerNr].of_hh));
      Serial.println("timers[" + String(timerNr) + "].on_mm = " + String(timers[timerNr].on_mm));
      Serial.println("timers[" + String(timerNr) + "].of_mm = " + String(timers[timerNr].of_mm));  
      Serial.println("timers[" + String(timerNr) + "].Level = " + String(timers[timerNr].Level));
      Serial.print("timers[" + String(timerNr) + "].dow = :");
      
      for (int i = 0; i < 7; i++) {Serial.print(timers[timerNr].dow[i] ? '1' : '0');  }
      Serial.println("");
      Serial.println("****************************************");
}


void saveSettings() {
  prefs.begin("app-config", false);
  // We slaan de hele struct op onder de key "config"
  prefs.putBytes("config", &settings, sizeof(mySettings));
  prefs.end();
  Serial.println("settings saved with prefs!");
  actionFlag = 20; //reconnect mosquitto and getTime
}

void loadSettings() {
  prefs.begin("app-config", true); // true = read-only
  // We halen de data op en stoppen het terug in de 'settings' struct
  prefs.getBytes("config", &settings, sizeof(mySettings));
  prefs.end();
  Serial.println("settings loaded with prefs");
}

void saveTimers() {
  prefs.begin("app-config", false);
  // Gebruik de naam van de array (is al een pointer) 
  // en bereken de totale grootte van de 4 timers
  prefs.putBytes("timers", timers, sizeof(timers)); 
  prefs.end();
  Serial.println("4 timers opgeslagen!");
}

void loadTimers() {
  prefs.begin("app-config", true); // true = read-only
  // Controleer of de sleutel bestaat en de juiste grootte heeft
  size_t writeSize = prefs.getBytes("timers", timers, sizeof(timers));
  prefs.end();

  if (writeSize == 0) {
    consoleOut("no timeredata found");
  } else {
    ("loadTimers read bytes: " + String(writeSize));
  }
}

