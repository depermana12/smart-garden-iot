void connectToMqtt()
{
  Serial.println(F("Connecting to MQTT..."));
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent)
{
  Serial.println(F("Connected to MQTT."));

  printSeparationLine();
  Serial.print(F("Session present: "));
  Serial.println(sessionPresent);
  // Subscribe
  uint16_t packetIdSub1 = mqttClient.subscribe(MQTT_SUB_REL1, 2); // override water pump
  uint16_t packetIdSub2 = mqttClient.subscribe(MQTT_SUB_REL2, 2); // overide selenoid valve
  uint16_t packetIdSub3 = mqttClient.subscribe(MQTT_SUB_REL3, 2); // overide fertilzer pump
  uint16_t packetIdSub4 = mqttClient.subscribe(MQTT_SUB_TRES, 2); // soil moisture setpoint
  uint16_t packetIdSub5 = mqttClient.subscribe(MQTT_SUB_MONTH1, 2); 
  uint16_t packetIdSub6 = mqttClient.subscribe(MQTT_SUB_MONTH2, 2);
  uint16_t packetIdSub7 = mqttClient.subscribe(MQTT_SUB_MONTH3, 2); 
  uint16_t packetIdSub8 = mqttClient.subscribe(MQTT_SUB_DAY1, 2);
  uint16_t packetIdSub9 = mqttClient.subscribe(MQTT_SUB_DAY2, 2);
  uint16_t packetIdSub10 = mqttClient.subscribe(MQTT_SUB_DAY3, 2);
  uint16_t packetIdSub11 = mqttClient.subscribe(MQTT_SUB_HTIME, 2); // schedule hour fertilizer
  uint16_t packetIdSub12 = mqttClient.subscribe(MQTT_SUB_DURA, 2); // duration fertilizer pump
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println(F("Disconnected from MQTT."));
  if (WiFi.isConnected())
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}
void onMqttPublish(uint16_t packetId)
{
  Serial.print(F("Publish acknowledged."));
  Serial.print(F("  packetId: "));
  Serial.println(packetId);
}
void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println(F("Subscribe acknowledged."));
  Serial.print(F("  packetId: "));
  Serial.println(packetId);
  Serial.print(F("  qos: "));
  Serial.println(qos);
}
void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  Serial.println(F("\n Publish received."));
  Serial.print(F("topic: "));
  Serial.println(topic);
  String messageTemp;
  for (int i = 0; i < len; i++)
  {
    messageTemp += (char)payload[i];
  }
  Serial.print(F("Message: "));
  Serial.println(messageTemp);

  if (String(topic) == "garden/relay/ch1") // water
  {
    Serial.println(F("Received garden/relay/ch1"));
    if (messageTemp == "ON")
    {
      overrideCh1 = true;
    }
    else if (messageTemp == "OFF")
    {
      overrideCh1 = false;
    }
  }

  if (String(topic) == "garden/relay/ch2") // selenoid
  {
    Serial.println(F("Received garden/relay/ch2"));
    if (messageTemp == "ON")
    { 
      overrideCh2 = true;
    }
    else if (messageTemp == "OFF")
    { 
      overrideCh2 = false;
    }
  }

  if (String(topic) == "garden/relay/ch3") // fertilizer
  {
    Serial.println(F("Received garden/relay/ch3"));
    if (messageTemp == "ON")
    {
      digitalWrite(fertilizerPump, LOW);
    }
    else if (messageTemp == "OFF")
    {
      digitalWrite(fertilizerPump, HIGH);
    }
  }

  if (String(topic) == "garden/soil/setPoint")
  {
    Serial.println(F("Received garden/soil/setPoint"));
    String triggerLevel;
    for (int i = 0; i < len; i++)
    {
      triggerLevel += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(triggerLevel);
    soilMoistSetpoint = triggerLevel.toInt();
    preferences.putInt("triggerLv", soilMoistSetpoint);
  }

  /////////////////////////////////////////////////
  if (String(topic) == "garden/time/fertM1")
  {
    Serial.println(F("Received garden/time/fertM1"));
    String setMonth1;
    for (int i = 0; i < len; i++)
    {
      setMonth1 += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(setMonth1);
    jadwalBulan1 = setMonth1.toInt();
    preferences.putInt("saveMonth1", jadwalBulan1);
  }

  if (String(topic) == "garden/time/fertM2")
  {
    Serial.println(F("Received garden/time/fertM2"));
    String setMonth2;
    for (int i = 0; i < len; i++)
    {
      setMonth2 += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(setMonth2);
    jadwalBulan2 = setMonth2.toInt();
    preferences.putInt("saveMonth2", jadwalBulan2);
  }

  if (String(topic) == "garden/time/fertM3")
  {
    Serial.println(F("Received garden/time/fertM3"));
    String setMonth3;
    for (int i = 0; i < len; i++)
    {
      setMonth3 += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(setMonth3);
    jadwalBulan3 = setMonth3.toInt();
    preferences.putInt("saveMonth3", jadwalBulan3);
  }
////////////////////////////////////////////////////////

if (String(topic) == "garden/time/fertD1")
  {
    Serial.println(F("Received garden/time/fertD1"));
    String setDay1;
    for (int i = 0; i < len; i++)
    {
      setDay1 += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(setDay1);
    jadwalTanggal1 = setDay1.toInt();
    preferences.putInt("saveDay1", jadwalTanggal1);
  }

  if (String(topic) == "garden/time/fertD2")
  {
    Serial.println(F("Received garden/time/fertD2"));
    String setDay2;
    for (int i = 0; i < len; i++)
    {
      setDay2 += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(setDay2);
    jadwalTanggal2 = setDay2.toInt();
    preferences.putInt("saveDay2", jadwalTanggal2);
  }

  if (String(topic) == "garden/time/fertD3")
  {
    Serial.println(F("Received garden/time/fertD3"));
    String setDay3;
    for (int i = 0; i < len; i++)
    {
      setDay3 += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(setDay3);
    jadwalTanggal3 = setDay3.toInt();
    preferences.putInt("saveDay3", jadwalTanggal3);
  }


/////////////////////////////////////////////////////////


if (String(topic) == "garden/time/fertH")
  {
    Serial.println(F("Received garden/time/fertH"));
    String setHour;
    for (int i = 0; i < len; i++)
    {
      setHour += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(setHour);
    setJam = setHour.toInt();
    preferences.putInt("saveJam", setJam);
  }

  if (String(topic) == "garden/time/duration")
  {
    Serial.println(F("Received garden/time/duration"));
    String durationPump;
    for (int i = 0; i < len; i++)
    {
      durationPump += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(durationPump);
    intervalFertilizer = durationPump.toInt();
    preferences.putInt("saveDura", intervalFertilizer);
    preferences.end();
  }
}
