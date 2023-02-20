

void checkTime()
{
  char bufdate[] = "DD/MM/YY";
char buftime [] = "hh:mm:ss";
  DateTime now = rtc.now();
  hari = daysOfTheWeek[now.dayOfTheWeek()];
  tanggal = now.day(), DEC;
  bulan = now.month(), DEC;
  tahun = now.year(), DEC;
  jam = now.hour(), DEC;
  menit = now.minute(), DEC;
  detik = now.second(), DEC;
  suhu = rtc.getTemperature();

  
myDate = now.toString(bufdate);
myTime = now.toString(buftime);

}