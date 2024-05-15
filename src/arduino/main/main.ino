#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

char ssid[16] = "";
char password[16] = "";

LiquidCrystal_I2C lcd(0x27, 20, 4);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void WiFiConnect(void) {
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected!");
}
void TimeSync(void) {
  timeClient.begin();
  timeClient.setTimeOffset(10800);
}
void setup() {
  Serial.begin(9600);
  WiFiConnect();
  TimeSync();
  lcd.init();
  lcd.backlight();
}
void loop() {
  timeClient.update();
  lcd.setCursor(0, 0);
  lcd.print(String(timeClient.getHours())+":"+String(timeClient.getMinutes()));
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(String(timeClient.getHours())+":"+String(timeClient.getMinutes()));
  delay(1000);
  lcd.clear(); 
}