void multiPageLCD() {
  currentButtonLeft = debounce(lastButtonLeft, buttonLeft);
  currentButtonRight = debounce(lastButtonRight, buttonRight);

  // button prev page
  if (lastButtonLeft == LOW && currentButtonLeft == HIGH && enableChange == true) {
    lcd.clear();
    if (lcdPageCounter < 5) {  // Page counter never higher than 5(total of pages)
      lcdPageCounter++;
    } else {

      lcdPageCounter = 1;
    }
  }
  lastButtonLeft = currentButtonLeft;

  // button next page
  if (lastButtonRight == LOW && currentButtonRight == HIGH && enableChange == true) {
    lcd.clear();
    if (lcdPageCounter > 1) {
      lcdPageCounter--;
    } else {
      lcdPageCounter = 5;
    }
  }
  lastButtonRight = currentButtonRight;

  switch (lcdPageCounter) {
    case 1:
      {
        // baris 1
        lcd.setCursor(0, 0);
        lcd.print(myDate);
        lcd.setCursor(12, 0);
        lcd.print(myTime);
        // baris 2
        lcd.setCursor(0, 1);
        lcd.write(1);  // temp
        lcd.setCursor(2, 1);
        lcd.print(t);
        lcd.print((char)223);  // degree
        lcd.print("C");

        lcd.setCursor(11, 1);
        lcd.write(5);  // temp
        lcd.setCursor(13, 1);
        lcd.print(h);
        lcd.print("%");
        // baris 3
        lcd.setCursor(0, 2);
        lcd.write(3);  // pupuk
        lcd.setCursor(2, 2);
        lcd.print(fertilizerVolume);
        lcd.print("L");
        lcd.setCursor(11, 2);
        lcd.write(4);  // air
        lcd.setCursor(13, 2);
        lcd.print(waterVolume);
        lcd.print("L");
        // baris 4
        lcd.setCursor(0, 3);
        lcd.write(0);  // soil hum
        lcd.setCursor(2, 3);
        lcd.print(soilMoistPercen);
        lcd.print("%");

        lcd.setCursor(13, 3);
        lcd.print(intervalWatering/1000);
        lcd.print("s");
        
      }
      break;

    case 2:
      {
        lcd.setCursor(0, 0);
        lcd.print("TANGKI AIR");
        lcd.setCursor(12, 0);
        lcd.print(percenWaterDepth);
        lcd.print("%");
        lcd.setCursor(1, 1);
        lcd.print("Jarak:");
        lcd.setCursor(12, 1);
        lcd.print(waterDistance);
        lcd.print("cm");

        lcd.setCursor(1, 2);
        lcd.print("Kedalaman:");
        lcd.setCursor(12, 2);
        lcd.print(waterDepth);
        lcd.print("cm");

        lcd.setCursor(1, 3);
        lcd.print("Volume:");
        lcd.setCursor(12, 3);
        lcd.print(waterVolume);
        lcd.print("l");
      }
      break;

    case 3:
      {
        lcd.setCursor(0, 0);
        lcd.print("TANGKI PUPUK CAIR");
        lcd.setCursor(1, 1);
        lcd.print("Jarak:");
        lcd.setCursor(12, 1);
        lcd.print(fertilizerDistance);
        lcd.print("cm");

        lcd.setCursor(1, 2);
        lcd.print("Kedalaman:");
        lcd.setCursor(12, 2);
        lcd.print(fertilizerDepth);
        lcd.print("cm");

        lcd.setCursor(1, 3);
        lcd.print("Volume:");
        lcd.setCursor(12, 3);
        lcd.print(fertilizerVolume);
        lcd.print("l");
      }
      break;


    case 4:
      {
        lcd.setCursor(0, 0);
        lcd.print("JADWAL PUPUK");
        lcd.setCursor(0, 1);
        if (jadwalTanggal1 < 10) lcd.print("0");
        lcd.print(jadwalTanggal1);
        lcd.setCursor(2, 1);
        lcd.print("/");
        lcd.setCursor(3, 1);
        if (jadwalBulan1 < 10) lcd.print("0");
        lcd.print(jadwalBulan1);
        lcd.setCursor(7, 1);
        lcd.print(setJam);
        lcd.setCursor(9, 1);
        lcd.print(":");
        lcd.setCursor(10, 1);
        lcd.print(setMenit);
        lcd.setCursor(13, 1);
        lcd.print("WIB");

        lcd.setCursor(0, 2);
        if (jadwalTanggal2 < 10) lcd.print("0");
        lcd.print(jadwalTanggal2);
        lcd.setCursor(2, 2);
        lcd.print("/");
        lcd.setCursor(3, 2);
        if (jadwalBulan2 < 10) lcd.print("0");
        lcd.print(jadwalBulan2);

        lcd.setCursor(0, 3);
        if (jadwalTanggal3 < 10) lcd.print("0");
        lcd.print(jadwalTanggal3);
        lcd.setCursor(2, 3);
        lcd.print("/");
        lcd.setCursor(3, 3);
        if (jadwalBulan3 < 10) lcd.print("0");
        lcd.print(jadwalBulan3);
      }
      break;

    case 5:
      {
        lcd.setCursor(0, 0);
        lcd.print("Wi-Fi connected to:");
        lcd.setCursor(0, 1);
        lcd.print(WiFi.SSID());
        lcd.setCursor(0, 2);
        lcd.print("IP address: ");
        lcd.setCursor(0, 3);
        lcd.print(WiFi.localIP());
      }
      break;
  }

  // Backlight LCD
  unsigned long currentMillisBL = millis();
  if (currentMillisBL - previousMillisBL > intervalBL) {
    previousMillisBL = currentMillisBL;
    backlight = LOW;
    enableChange = false;
  }

  if (digitalRead(buttonLeft) == HIGH || digitalRead(buttonRight) == HIGH) {
    previousMillisBL = currentMillisBL;
    if (backlight == LOW) {
      backlight = HIGH;
    } else {
      enableChange = true;
    }
  }

  if (backlight == HIGH) {
    lcd.backlight();
    lcd.display();
  }
  if (backlight == LOW) {
    lcd.noBacklight();
    lcd.noDisplay();
  }
}