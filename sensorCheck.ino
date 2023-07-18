void checkSensors()
{
// check LCD2004
  int status;
  status = lcd.begin(20, 4);
  if (status)
  { // non zero status means it was unsuccesful
    hd44780::fatalError(status);
  }

  // Check if DHT22 is not detected
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //  Check RTC
  if (!rtc.begin())
  {
    Serial.println(F("RTC not detected"));
    Serial.flush();
    abort();
  }
}