/*
 * Arduino Geliştirme Platformu
 * Facebook: https://www.facebook.com/groups/Arduino.Pic
 * Youtube : https://www.youtube.com/ArduinoPicGelistirmePlatformu
 * Web Site: http://arduproje.blogspot.com.tr
 * ESP8266 4x8 Dot matrix Display
 * Yasin EKEN
 * 03/06/2017
 */
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "WundergroundClient.h"
#include "TimeClient.h"
const float UTC_OFFSET = 2;
const boolean IS_METRIC = true;
const String WUNDERGRROUND_API_KEY   = "";        // Buraya WUNDERGRROUND Api Key'inızı Yazınız Yoksa Çalışmaz
const String WUNDERGRROUND_LANGUAGE  = "TR";      
const String WUNDERGROUND_COUNTRY    = "TR";
const String WUNDERGROUND_CITY       = "Izmir";   // Buraya Bulunduğunuz İli İlk Harf Büyük Olacak Şekilde Türkçe Karakter Kullanmadan Yazınız
TimeClient timeClient(UTC_OFFSET);
WundergroundClient wunderground(IS_METRIC);
/* 
 * NODEMCU PIN SPI
 * CLK D5
 * DIN D7
 * CS  D8
 */
#define CS     	15
#define MAXS 	   4
char ssid[] = "SSID";
char pass[] = "PASS";
unsigned short maxX = MAXS * 8 - 1;
unsigned short LED[MAXS][8];
unsigned short LMAX[MAXS * 8];
unsigned short HMAX[MAXS * 8];
unsigned short stringtoshort ( String s );
void stringToDisplay ( String s , int x, int y, int t);
void max7219_init(void);
void max7219_set_br(unsigned short br);
void led_init(void);
void clear_Display(void);
void refresh_display(void);
void char2Arr(unsigned short ch, int PosX, short PosY);
//**************************************************************************************************
void connect_to_WiFi() {  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    for (byte i = 0; i < 133; i++) {
      delay(10);
      stringToDisplay ("Connect to " + String(ssid) , i, 0, 800);
    }
  }
}
void setup() {
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  Serial.begin(115200);
  Serial.println();
  SPI.begin();
  led_init();
  max7219_init();
  max7219_set_br(10); // 0 to 15 Ekran Parlaklığı
  clear_Display();
  connect_to_WiFi();
  wunderground.updateConditions(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  wunderground.updateForecast(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  timeClient.updateTime();
  Serial.print("wunderground.getDate()= "); Serial.println(wunderground.getDate());
  IPAddress IP = WiFi.localIP();
  for (byte i = 0; i < 218; i++) {
    delay(10);
    stringToDisplay ("IP address: " + String(IP[0]) + "." + String(IP[1]) + "." + String(IP[2]) + "." + String(IP[3]) , i, 0, 800);
  }
}
//**************************************************************************************************
int x = 0;
int y = 0;
byte oldh1;
byte oldh2;
byte oldm1;
byte oldm2;
byte h1, h2, m1, m2 = 0;
void loop() {
  while (true) {
    delay(1);
    String text = "  SICAKLIK:" + wunderground.getCurrentTemp() +
                  " '  NEM:"    + wunderground.getHumidity()    +
                  "  BASINC:"   + wunderground.getPressure()    +
                  "  RUZGAR:"   + wunderground.getWindSpeed()   +
                  "  "          + wunderground.getWeatherText() +
                  "  "          + wunderground.getDate()        +
                  "  "          + wunderground.getForecastTitle(0);

    byte h  = timeClient.getFormattedTime().substring(0, 2).toInt() + 1;
    byte m  = timeClient.getFormattedTime().substring(3, 5).toInt();
    byte s  = timeClient.getFormattedTime().substring(6, 8).toInt();
    if ( h > 23 ) h = 0;

    stringToDisplay (text , x, 0, 400);

    char2Arr (stringtoshort(String(h / 10)), 29 + x, h1);
    char2Arr (stringtoshort(String(h % 10)), 23 + x, 0);
    if (timeClient.getFormattedTime().substring(6, 8).toInt() % 2 == 0 )
      char2Arr (stringtoshort(timeClient.getFormattedTime().substring(2, 3)), 16 + x, 0);
    else
      char2Arr (stringtoshort(" "), 16 + x, 0);
    char2Arr (stringtoshort(String(m / 10)), 12 + x, 0);
    char2Arr (stringtoshort(String(m % 10)),  6 + x, 0);

    refresh_display();
    if ( s > 0 && s < 5 || x < 0 || x > 0 ) {
      x++;
    }
    if (x > ((int)text.length() * 6) + 40) x = -30;
  }
}
