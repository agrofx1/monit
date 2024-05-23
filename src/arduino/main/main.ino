#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "base64_utils.h"
#include <string.h>

char ssid[16] = "";
char password[16] = "";

char pc1code[32] = "1";
char pc2code[32] = "2";
char pc3code[32] = "3";

char pc1[20];
char pc2[20];
char pc3[20];

AsyncWebServer server(80);

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
void ServerSetup(void) {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    int count = request->params();

    bool auth = false;
    int id = 0;
    if (count != 0) {
      for (int param = 0; param < count; param++) {
        if (request->getParam(param)->name() == "code") {
          String code = request->getParam(param)->value();
          if (code == pc1code) {
            id = 1;
            auth = true;
          }
          if (code == pc2code) {
            id = 2;
            auth = true;
          }
          if (code == pc3code) {
            id = 3;
            auth = true;
          }
        }
        if (request->getParam(param)->name() == "str") {
          String received_param = request->getParam(param)->value();
          char decoded_string[30];
          b64_decode(decoded_string, (char *)received_param.c_str(), received_param.length());
          if (auth) {
            switch (id) {
              case 1:
                strcpy(pc1, decoded_string);
                break;
              case 2:
                strcpy(pc2, decoded_string);
                break;
              case 3:
                strcpy(pc3, decoded_string);
                break;
            }
          }
        }
      }
    } else {
      request->send_P(400, "text/html", "invalid data");
      return;
    }
    if (auth == false || id == 0) {
      request->send_P(400, "text/html", "invalid credentials");
      return;
    }
    char buf[2] = { id };
    request->send_P(200, "text/html", buf);
  });
  server.begin();
}
void setup() {
  Serial.begin(9600);
  WiFiConnect();
  TimeSync();
  lcd.init();
  lcd.backlight();
  ServerSetup();
}
void loop() {
  timeClient.update();
  lcd.setCursor(8, 0);
  lcd.print(String(timeClient.getHours()) + ":" + (timeClient.getMinutes() < 10 ? "0" + String(timeClient.getMinutes()) : String(timeClient.getMinutes())));
  lcd.setCursor(0, 1);
  lcd.print(String(pc1));
  lcd.setCursor(0, 2);
  lcd.print(String(pc2));
  lcd.setCursor(0, 3);
  lcd.print(String(pc3));
  delay(1000);
  lcd.clear();
}