void checkTank()
{

  /*
  29ms are the shortest delay between pings.
  overlap 30ms between pings index 0 and 1
  */
  if (millis() - previousPing0 > 30)
  {
    waterDistance = sonar[0].ping_cm();
    if (waterDistance >= waterTankDepth || waterDistance == 0)
    {
      waterDistance = waterTankDepth; // check it does not go negative
    }
    waterDepth = waterTankDepth - waterDistance;
    // 22             5        = 17
    waterGap = waterTankDepth - gapSensor;
    waterMax = waterTankDepth - waterGap;
    waterMin = waterTankDepth;
    percenWaterDepth = map(waterDistance, waterMin, waterMax, 0, 100);
    percenWaterDepth = constrain(percenWaterDepth, 0, 100);
    waterVolume = (waterTankLength * waterTankWidth * waterDepth) / 1000;
    previousPing0 = millis();
  }

  if (millis() - previousPing1 > 60)
  {
    fertilizerDistance = sonar[1].ping_cm();
    if (fertilizerDistance >= fertilizerTankDepth || fertilizerDistance == 0){
      fertilizerDistance = fertilizerTankDepth;
    }
    fertilizerDepth = fertilizerTankDepth - fertilizerDistance;
    fertilizerVolume = (fertilizerTankLength * ferilizerTankWidth * fertilizerDepth) / 1000; 
    previousPing1 = millis();
  }
  /*
  // Serial console tanks
  Serial.println("Jarak air: " + String(waterDistance));
  Serial.println("Kedalaman air: " + String(waterDepth));
  Serial.println("Volume air: " + String(waterVolume));
  Serial.println("Presentase tangki air: " + String(percenWaterDepth));
  Serial.println("Jarak pupuk cair: " + String(fertilizerDistance));
  Serial.println("Kedalaman pupuk air: " + String(fertilizerDepth));
  Serial.println("Volume pupuk cair: " + String(fertilizerVolume));
  */
}

void waterTankFiller()
{

  if (percenWaterDepth <= 20)
  {
    
    selenoidOn = true;
  }
  else if (percenWaterDepth >= 100)
  {
   
    selenoidOn = false;
  }
  /*
   if(percenWaterDepth > 20 && percenWaterDepth <= 100 && selenoidOn == true){
      digitalWrite(selenoid, LOW);
   }
     else if(percenWaterDepth > 20 && percenWaterDepth <= 100 && selenoidOn == false){
       digitalWrite(selenoid, HIGH);
     } */
}