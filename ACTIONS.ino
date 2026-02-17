void check_actionFlag()
{

  if (actionFlag == 20)
  // reconnect mosquitto if something has changed
  {
      actionFlag = 0; //reset the actionflag
      // reset mosquitto if format not NULL
      if(settings.Mqtt_Format != 0)
      {
          MQTT_Client.disconnect();
          //reset the mqttserver
          MQTT_Client.setServer(settings.Mqtt_Broker, settings.Mqtt_Port);
          MQTT_Client.setCallback ( MQTT_Receive_Callback ) ;
          mqttConnect(); // reconnect mqtt after change of settings
      }
             
       getTijd(); //retrieve time and recalculate timers
  }  



}