

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
  uint16_t packetIdSub5 = mqttClient.subscribe(MQTT_SUB_DTIME, 2); // schedule day fertilizer
  uint16_t packetIdSub6 = mqttClient.subscribe(MQTT_SUB_HTIME, 2); // schedule hour fertilizer
  uint16_t packetIdSub7 = mqttClient.subscribe(MQTT_SUB_MTIME, 2); // schedule minute fertilizer
  uint16_t packetIdSub8 = mqttClient.subscribe(MQTT_SUB_DURA, 2); // duration fertilizer pump
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

  if (String(topic) == "garden/time/fertD")
  {
    Serial.println(F("Received garden/time/fertD"));
    String setDay;
    for (int i = 0; i < len; i++)
    {
      setDay += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(setDay);
    setHari = setDay.toInt();
    preferences.putInt("saveHari", setHari);
  }

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

  if (String(topic) == "garden/time/fertM")
  {
    Serial.println(F("Received garden/time/fertM"));
    String setMin;
    for (int i = 0; i < len; i++)
    {
      setMin += (char)payload[i];
    }
    Serial.print(F("Message: "));
    Serial.println(setMin);
    setMenit = setMin.toInt();
    preferences.putInt("saveMenit", setMenit);
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
