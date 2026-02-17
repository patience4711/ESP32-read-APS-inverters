void ledblink(int i, int wacht) {
  for(int x=0; x<i; x++) {
    digitalWrite(led_onb, LED_ON);
    set_pwm(90);
    delay(wacht);
    digitalWrite(led_onb, LED_OFF);
    set_pwm(0);
    delay(wacht);
   }
}

// void set_pwm(int value) {
//   //consoleOut("set_pwm value = " + String(value));
//   ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, value);
//   // Update duty to apply the new value
//   ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
// }

// void fade_pwm(int value) {
//   // LEDC_MODE, LEDC_CHANNEL, target duty, time = set in ms
//   //consoleOut("fade_pwm value = " + String(value)); 
//   ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, value, 1000); 
//   ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
// }

void checkduty_not_Null() 
  {
   if(current_duty != 0) {
        dimmer_state = true; 
    }
 }

 // function to show debug info
void consoleOut(String toLog) {
// decide to log to serial or the console 
  if(settings.diagNose) 
  {
    Serial.println(toLog);
  } 
}


String getChipId(bool sec) {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  if(sec) return String(chipId); else return "ESP32-SWITCH-" + String(chipId);
}

bool loginBoth(String who) {
  const char* realm = "Login Required";
  bool authenticated = false;

  if (who == "admin") {
    realm = "Admin Login";
    authenticated = server.authenticate("admin", settings.passwd); 
  } 
  else if (who == "both") {
    realm = "User or Admin Login";
    // Check beide accounts
    authenticated = server.authenticate("admin", settings.passwd) || 
                    server.authenticate("user", settings.userPwd);
  }

  if (!authenticated) {
    // Toon de popup in de browser
    server.requestAuthentication(BASIC_AUTH, realm, "Login failed!");
    server.send(401, "text/html", "<html><body><h1>access denied</h1><p>Click <a href='/'>here</a> to go back.</p></body></html>");
    return false; // Toegang geweigerd
  }
  
  return true; // Toegang verleend
}
