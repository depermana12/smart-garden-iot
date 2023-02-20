void checkSoilMoisture()
{
  soilMoistRaw = analogRead(soilMoisture);
  soilMoistPercen = map(soilMoistRaw, WaterValue, AirValue, 100, 0);
  soilMoistPercen = constrain(soilMoistPercen, 0, 100);
  //float soilMoistVolt = soilMoistRaw * (3.3 / 4095.0);

  /*
  // serial console soil moisture
  Serial.println("Soil moisture analog value: " + String(soilMoistRaw));
  Serial.println("Soil moisture voltage: " + String(soilMoistVolt));
  Serial.println("Soil moisture percentage: " + String(soilMoistPercen));
  */
}