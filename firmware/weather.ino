void checkTempRh()
{
  h = dht.readHumidity();
  t = dht.readTemperature();
}