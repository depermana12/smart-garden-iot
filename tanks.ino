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

}


