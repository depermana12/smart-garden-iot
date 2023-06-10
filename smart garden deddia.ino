// Deddia Permana
// Changelog History
//
// 19.11.2022
// replaced broken ic ds3231SN RTC
// adjusting calibrating ds3231SN using syncroTime software
// removed poor charging circuit R 201 in RTC board
// 20.11.2022
// fixed relay D2 and D4 active low, setup to high after initialization
// 21.11.2022
// tweaking soil moisture v1.2 into 480khz
// replaced ne555 ic into tlc555 original TI
// 22.11.2022
// created custom charachter icon lcd
// DHT22 set to 2000 millis because hardware low sampling
// 23.11.2022
// Replacing Liquid crystal I2C library with hd44780 library
// added timing for lcd backlight on and off
// added depth and distance calculation of water tank and fertilizer tank
// constrain 1-100 percentage soil humidity
// 24.11.2022
// added multiple screen page in lcd
// 25.11.2022
// integrating nodered
// 8.12.2022
// switching to preferences library, EEPROM deprecated
// 13.12.2022
// added nodes persist to save latest set value
// touch up css dashboard
// 14.12.2022 - 27.12.2022
// refactor

#include <SPI.h>
#include <Preferences.h>
#include <NewPing.h>
#include "DHT.h"
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include "RTClib.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>




#define SONAR_NUM 2
#define DHTPIN 33
#define DHTTYPE DHT22
//#define WIFI_SSID "Kodedroid"
//#define WIFI_PASSWORD "aryamandaka"

/*
Define MQTT Host
for local host use IPAddress(x,x,x,x)
for public with domain use "domain.com"
*/

#define MQTT_HOST IPAddress(159, 223, 57, 199)
#define MQTT_USERNAME "depermana"
#define MQTT_PASSWORD "gundam123"
#define MQTT_PORT 1883

// Publish Topics
#define MQTT_PUB_TEMP "garden/dht22/temperature"
#define MQTT_PUB_HUM "garden/dht22/humidity"
#define MQTT_PUB_SOIL "garden/soil/soilHum"
#define MQTT_PUB_WATER "garden/hcsr04/water"
#define MQTT_PUB_WATERK "garden/hcsr04/waterK"
#define MQTT_PUB_WATERJ "garden/hcsr04/waterJ"
#define MQTT_PUB_WATERP "garden/hcsr04/waterP"
#define MQTT_PUB_FERT "garden/hcsr04/fertilizer"
#define MQTT_PUB_FERTK "garden/hcsr04/fertilizerK"
#define MQTT_PUB_FERTJ "garden/hcsr04/fertilizerJ"

// Subscribe Topics
#define MQTT_SUB_REL1 "garden/relay/ch1" // water
#define MQTT_SUB_REL2 "garden/relay/ch2" // selenoid valve
#define MQTT_SUB_REL3 "garden/relay/ch3" // fertilizer
#define MQTT_SUB_TRES "garden/soil/setPoint"
#define MQTT_SUB_MONTH1 "garden/time/fertM1"
#define MQTT_SUB_MONTH2 "garden/time/fertM2"
#define MQTT_SUB_MONTH3 "garden/time/fertM3"

#define MQTT_SUB_DAY1 "garden/time/fertD1"
#define MQTT_SUB_DAY2 "garden/time/fertD2"
#define MQTT_SUB_DAY3 "garden/time/fertD3"
#define MQTT_SUB_HTIME "garden/time/fertH"
#define MQTT_SUB_MTIME "garden/time/fertMin"
#define MQTT_SUB_DURA "garden/time/duration"


  #define _ESPASYNC_WIFIMGR_LOGLEVEL_    3
  #define DISPLAY_STORED_CREDENTIALS_IN_CP        false
//For ESP32, To use ESP32 Dev Module, QIO, Flash 4MB/80MHz, Upload 921600
  #include <esp_wifi.h>
  #include <WiFiClient.h>
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;
  #define USE_LITTLEFS    true
  #define USE_SPIFFS      false

  #if USE_LITTLEFS
    #include "FS.h"
    #include <LittleFS.h>       // https://github.com/espressif/arduino-esp32/tree/master/libraries/LittleFS

      FS* filesystem =      &LittleFS;
      #define FileFS        LittleFS
      #define FS_Name       "LittleFS"
   
    #endif

 
  //////

  #define ESP_getChipId()   (ESP.getChipId())

// You only need to format the filesystem once
//#define FORMAT_FILESYSTEM       true
#define FORMAT_FILESYSTEM         false
#define MIN_AP_PASSWORD_SIZE    8
#define SSID_MAX_LEN            32
#define PASS_MAX_LEN            64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

typedef struct
{
  String wifi_ssid;
  String wifi_pw;
}  WiFi_Credentials_String;

#define NUM_WIFI_CREDENTIALS      5


typedef struct
{
  WiFi_Credentials  WiFi_Creds [NUM_WIFI_CREDENTIALS];
  uint16_t checksum;
} WM_Config;

WM_Config         WM_config;

#define  CONFIG_FILENAME              F("/wifi_cred.dat")
//////

bool initialConfig = false;
#define USE_AVAILABLE_PAGES         false
#define USE_STATIC_IP_CONFIG_IN_CP  false
#define USE_ESP_WIFIMANAGER_NTP     false
#define USE_CLOUDFLARE_NTP          false
#define USING_CORS_FEATURE          false

////////////////////////////////////////////

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
  // Force DHCP to be true
  #if defined(USE_DHCP_IP)
    #undef USE_DHCP_IP
  #endif
  #define USE_DHCP_IP     true
#else
  // You can select DHCP or Static IP here
  #define USE_DHCP_IP     true
  //#define USE_DHCP_IP     false
#endif

#if ( USE_DHCP_IP )
  // Use DHCP

  #if (_ESPASYNC_WIFIMGR_LOGLEVEL_ > 3)
    #warning Using DHCP IP
  #endif

  IPAddress stationIP   = IPAddress(0, 0, 0, 0);
  IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
  IPAddress netMask     = IPAddress(255, 255, 255, 0);

#endif

////////////////////////////////////////////


#define USE_CONFIGURABLE_DNS      false

IPAddress dns1IP      = gatewayIP;
IPAddress dns2IP      = IPAddress(8, 8, 8, 8);

#define USE_CUSTOM_AP_IP          false

IPAddress APStaticIP  = IPAddress(192, 168, 100, 1);
IPAddress APStaticGW  = IPAddress(192, 168, 100, 1);
IPAddress APStaticSN  = IPAddress(255, 255, 255, 0);

#include <ESPAsync_WiFiManager.h>               //https://github.com/khoih-prog/ESPAsync_WiFiManager


// SSID and PW for Config Portal
String ssid = "smart garden";
String password = "smartgarden123";

// SSID and PW for your Router
String Router_SSID;
String Router_Pass;


WiFi_AP_IPConfig  WM_AP_IPconfig;
WiFi_STA_IPConfig WM_STA_IPconfig;

unsigned long previousMillisBL = 0;
unsigned long intervalBL = 60000;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long interval1 = 2000;
unsigned long interval2 = 5000;
unsigned long intervalPolling = 1000;
unsigned long previousMillisFert = 0;
unsigned long previousMillisCheckFert = 0;
unsigned long previousPing0 = 0;
unsigned long previousPing1 = 0;

const int LCD_COLS = 20;
const int LCD_ROWS = 4;
const int waterPump = 16;
const int selenoid = 4;
const int fertilizerPump = 2;
const int PingPin1 = 13;
const int EchoPin1 = 27;
const int PingPin2 = 25;
const int EchoPin2 = 26;
const int MAX_DISTANCE = 23;
const int waterTankDepth = 22;
const int waterTankLength = 19;
const int waterTankWidth = 19;
const int fertilizerTankDepth = 22;
const int fertilizerTankLength = 19;
const int ferilizerTankWidth = 19;
const int buttonLeft = 18;
const int buttonRight = 19;
const int soilMoisture = 32;
const int AirValue = 3409;
const int WaterValue = 1666;

int tanggal, bulan, tahun, jam, menit, detik, suhu, t, h;
int jadwalBulan1, jadwalBulan2, jadwalBulan3, jadwalTanggal1, jadwalTanggal2, jadwalTanggal3, setJam, setMenit;
int soilMoistRaw, soilMoistPercen, soilMoistSetpoint;
int waterTank;
int gapSensor = 5;
int percenWaterDepth;
int intervalFertilizer;
int lcdPageCounter = 1;

float waterMax, waterMin, waterGap;
float waterVolume, fertilizerVolume, waterDistance, fertilizerDistance, waterDepth, fertilizerDepth;

String hari, myDate, myTime, myTemp;
String waterPumpStatus;
String selenoidStatus = "OFF";
String waterFertilizerStatus;

// Custom LCD Character
byte soilHum[] = {B00100, B01110, B11111, B11111, B10101, B00100, B11111, B11111};
byte tempHum[] = {B00100, B01010, B01010, B01110, B01110, B11111, B11111, B01110};
byte dateClock[] = {B00000, B01110, B10101, B10101, B10111, B10001, B01110, B00000};
byte pupuk[] = {B01100, B01100, B01000, B00000, B10001, B11111, B11111, B11111};
byte air[] = {B00100, B01110, B01010, B00000, B10001, B11111, B11111, B11111};
byte kelembapan[] = {B00000, B00100, B01110, B11111, B11111, B11111, B01110, B00000};

// 0 minggu, 1 senin, 2 selasa, 3 rabu, 4 kamis, 5 jumat, 6 sabtu
char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};

NewPing sonar[SONAR_NUM] = {
    // Sensor object array.
    NewPing(PingPin1, EchoPin1, MAX_DISTANCE),
    NewPing(PingPin2, EchoPin2, MAX_DISTANCE)};

// Instance
Preferences preferences;
DHT dht(DHTPIN, DHTTYPE);
hd44780_I2Cexp lcd;
RTC_DS3231 rtc;
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

// DEBOUNCE
boolean currentButtonLeft = LOW;
boolean lastButtonLeft = LOW;
boolean lastButtonRight = LOW;
boolean currentButtonRight = LOW;
boolean enableChange = true;
boolean backlight = HIGH;
boolean overrideCh1 = false;
boolean overrideCh2 = false;
boolean overrideCh3 = false;
boolean fertilizerPumpOn = false;
boolean selenoidOn;
boolean waterPumpOn;
// Debounce buttton counter page
boolean debounce(boolean last, int pin)
{
  boolean current = digitalRead(pin);
  if (last != current)
  {
    delay(10);
    current = digitalRead(pin);
  }
  return current;
}

void setup()
{
  preferences.begin("smTres", false);
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

  Serial.begin(115200);
  dht.begin();
  pinMode(waterPump, OUTPUT);
  pinMode(selenoid, OUTPUT);
  pinMode(fertilizerPump, OUTPUT);
  /*
    for relay that use low level trigger,
    relay pin must set to high to avoid active, vice versa.
  */
  digitalWrite(waterPump, HIGH);
  digitalWrite(selenoid, HIGH);
  digitalWrite(fertilizerPump, HIGH);
  // LCD custom character
  lcd.createChar(0, soilHum);
  lcd.createChar(1, tempHum);
  lcd.createChar(2, dateClock);
  lcd.createChar(3, pupuk);
  lcd.createChar(4, air);
  lcd.createChar(5, kelembapan);
  lcd.home();

  // MQTT calback function
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  WiFi.onEvent(WiFiEvent);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
   mqttClient.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);
  connectToWifi();
  setupWifiManager();
}

void loop()
{

  waterTankFiller();
  multiPageLCD();

  // Publish sensor data every 2 second
  unsigned long currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 >= interval1)
  {
    checkTempRh();
    checkSoilMoisture();
    checkTank();

    // Publish an MQTT message on topic garden/dht22/temperature
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEMP, 1, true, String(t).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_TEMP, packetIdPub1);
    Serial.printf("Message: %.2f \n", t);

    // Publish an MQTT message on topic garden/dht22/humidity
    uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_HUM, 1, true, String(h).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_HUM, packetIdPub2);
    Serial.printf("Message: %.2f \n", h);

    // Publish an MQTT message on topic garden/soil/temperature
    uint16_t packetIdPub3 = mqttClient.publish(MQTT_PUB_SOIL, 0, true, String(soilMoistPercen).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_SOIL, packetIdPub3);
    Serial.printf("Message: %.2f \n", soilMoistPercen);

    previousMillis1 = currentMillis1;
  }

  // Publish sensor every 5 second
  unsigned long currentMillis2 = millis();
  if (currentMillis2 - previousMillis2 >= interval2)
  {

    // Publish an MQTT message on topic garden/hcsr04/water
    uint16_t packetIdPub4 = mqttClient.publish(MQTT_PUB_WATER, 1, true, String(waterVolume).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_WATER, packetIdPub4);
    Serial.printf("Message: %.2f \n", waterVolume);

    // Publish an MQTT message on topic garden/hcsr04/water
    uint16_t packetIdPub5 = mqttClient.publish(MQTT_PUB_WATERK, 1, true, String(waterDepth).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_WATERK, packetIdPub5);
    Serial.printf("Message: %.2f \n", waterDepth);

    // Publish an MQTT message on topic garden/hcsr04/water
    uint16_t packetIdPub6 = mqttClient.publish(MQTT_PUB_WATERJ, 1, true, String(waterDistance).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_WATERJ, packetIdPub6);
    Serial.printf("Message: %.2f \n", waterDistance);

    // Publish an MQTT message on topic garden/hcsr04/waterp
    uint16_t packetIdPub7 = mqttClient.publish(MQTT_PUB_WATERP, 1, true, String(percenWaterDepth).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_WATERP, packetIdPub7);
    Serial.printf("Message: %.2f \n", percenWaterDepth);

    // Publish an MQTT message on topic garden/hcsr04/fertilizer
    uint16_t packetIdPub8 = mqttClient.publish(MQTT_PUB_FERT, 1, true, String(fertilizerVolume).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_FERT, packetIdPub8);
    Serial.printf("Message: %.2f \n", fertilizerVolume);

    // Publish an MQTT message on topic garden/hcsr04/fertilizer
    uint16_t packetIdPub9 = mqttClient.publish(MQTT_PUB_FERTK, 1, true, String(fertilizerDepth).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_FERTK, packetIdPub9);
    Serial.printf("Message: %.2f \n", fertilizerDepth);
    /**/
    // Publish an MQTT message on topic garden/hcsr04/fertilizer
    uint16_t packetIdPub10 = mqttClient.publish(MQTT_PUB_FERTJ, 1, true, String(fertilizerDistance).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_FERTJ, packetIdPub10);
    Serial.printf("Message: %.2f \n", fertilizerDistance);

    previousMillis2 = currentMillis2;
  }

  if (millis() - previousMillisCheckFert >= intervalPolling)
  {
    checkTime();
   
    // First schedule
if (bulan == jadwalBulan1 && tanggal == jadwalTanggal1 && jam == setJam && menit == setMenit && detik == 0 && fertilizerPumpOn == false)
  {
    fertilizerPumpOn = true;
      previousMillisFert = millis();
      digitalWrite(fertilizerPump, LOW); // pompa on
      Serial.println(F("Memompa pupuk cair"));
      waterFertilizerStatus = "ON";
}
 
// Second schedule
else if (bulan == jadwalBulan2 && tanggal == jadwalTanggal2 && jam == setJam && menit == setMenit && detik == 0 && fertilizerPumpOn == false)
    {
    fertilizerPumpOn = true;
      previousMillisFert = millis();
      digitalWrite(fertilizerPump, LOW); // pompa on
      Serial.println(F("Memompa pupuk cair"));
      waterFertilizerStatus = "ON";
    }

// Third schedule
else if (bulan == jadwalBulan3 && tanggal == jadwalTanggal3 && jam == setJam && menit == setMenit && detik == 0 && fertilizerPumpOn == false)
    {
    fertilizerPumpOn = true;
      previousMillisFert = millis();
      digitalWrite(fertilizerPump, LOW); // pompa on
      Serial.println(F("Memompa pupuk cair"));
      waterFertilizerStatus = "ON";
    }
	
	 // Turn off the relay after 5 seconds
  if (fertilizerPumpOn == true && (millis() - previousMillisFert) >= intervalFertilizer) {
    fertilizerPumpOn = false;
    digitalWrite(fertilizerPump, HIGH); // Turn off relay
  }
  }

/*
 OVERRIDE RELAY CONTROL
 override only for turning on and off when relay's state is off
 not inteded yet to do the opposite
*/
  // override water pump
  if (waterPumpOn == true || overrideCh1 == true)
  {
    digitalWrite(waterPump, LOW);
  }
  else
  {
    digitalWrite(waterPump, HIGH);
  }

  // override selenoid valve
  if (selenoidOn == true || overrideCh2 == true)
  {
    digitalWrite(selenoid, LOW);
    //lcd.clear();
      //selenoidStatus = "ON";
  }
  else
  {
    digitalWrite(selenoid, HIGH);
    //lcd.clear();
      //selenoidStatus = "OFF";
  }

  // Soil moisture trigger water pump
  if (soilMoistPercen <= soilMoistSetpoint)
  {
    waterPumpStatus = "ON";
    waterPumpOn = true;
  }
  else
  {
    waterPumpStatus = "OFF";
    waterPumpOn = false;
  }

  // Read preferences value. if key failed to retrieve value, then set to 0
  soilMoistSetpoint = preferences.getInt("triggerLv", 0);

  setJam = preferences.getInt("saveJam", 0);
  setMenit = preferences.getInt("saveMenit", 0);
  intervalFertilizer = preferences.getInt("saveDura", 0);

  jadwalBulan1 = preferences.getInt("saveMonth1", 0);
  jadwalBulan2 = preferences.getInt("saveMonth2", 0);
  jadwalBulan3 = preferences.getInt("saveMonth3", 0);


  jadwalTanggal1 = preferences.getInt("saveDay1", 0);
  jadwalTanggal2 = preferences.getInt("saveDay2", 0);
  jadwalTanggal3 = preferences.getInt("saveDay3", 0);

loopWifiManager();
}
