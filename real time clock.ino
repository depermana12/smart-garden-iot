void checkTime()
{
  char bufdate[] = "DD/MM/YY";
  char buftime [] = "hh:mm:ss";
  DateTime now = rtc.now();
  hari = daysOfTheWeek[now.dayOfTheWeek()];
  tanggal = now.day();
  bulan = now.month();
  tahun = now.year();
  jam = now.hour();
  menit = now.minute();
  detik = now.second();
  suhu = rtc.getTemperature();
  myDate = now.toString(bufdate);
  myTime = now.toString(buftime);
}