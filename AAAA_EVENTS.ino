/* we can push data to the website using an event for that 
 *  question is when is this needed.
 *  if the webpage is opened or gets focus 
 *  when state has changed
 *  when there is new data
 *  
 *  say there is new data but no client. should we send? no
 *  So we only send when there is a client
 */


//void generalEvent ()
//{
//// this link provides the general data on the frontpage
//    char temp[15]={0};
//    //char * temp;
//    char json[200]={0};
//    uint8_t remote = 0;
//    //if(checkRemote( request->client()->remoteIP().toString()) ) remote = 1; // for the menu link
//    uint8_t night = 0; 
//    if(!dayTime) { night = 1; }
//
//// {"ps":"05:27 hr","pe":"21:53 hr","cnt":3,"rm":0,"st":1,"sl":1}  length 62
//
//     snprintf(json, sizeof(json), "{\"count\":\"%d\",\"remote\":\"%d\",\"state\":\"%d\",\"sleep\":\"%d\"," ,inverterCount, remote, zigbeeUp, night );    
//     // add the polling times
//     if(minute(switchonTime) < 10 ) {
//       sprintf(temp,"\"ps\":\"0%d:0%d hr\",", hour(switchonTime), minute(switchonTime) );
//     } else {
//       sprintf(temp,"\"ps\":\"0%d:%d hr\",", hour(switchonTime), minute(switchonTime) );
//     }
//     strcat(json, temp); 
//     if( minute(switchoffTime) < 10 ) {
//        sprintf(temp,"\"pe\":\"0%d:0%d hr\",", hour(switchoffTime), minute(switchoffTime) );
//     } else {
//        sprintf(temp,"\"pe\":\"0%d:%d hr\",", hour(switchoffTime), minute(switchoffTime) );
//     }
//    strcat(json, temp);   
//
//    Serial.println("length temp = " + String(strlen(temp)));
//    Serial.println("length json = " + String(strlen(json)));
//    Serial.println("json = " + String(json));
//    //request->send(200, "text/json", json);
//    events.send( json, "general" );
//}
// 
