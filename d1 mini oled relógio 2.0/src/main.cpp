#include <Wire.h>
#include "SSD1306Wire.h"
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
// #include <time.h>

WiFiManager wifi;
WiFiUDP udp;
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

int timeOffSet = -10800;
int updateInterval = 6000;
NTPClient timeClient(udp, "a.ntp.br", timeOffSet, updateInterval);

char ssid[] = "MURILO";
char pass[] = "94658243";

int screenWidth = 64;
int screenHeight = 48;
int centerX = screenWidth / 2;
int centerY = screenHeight / 2;


int lastMinute = -1;
int lastSecond = -1;
String formattedDateDisplay;
String formattedTimeDisplay;
String months[12] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"};

void setupDisplay();
void printDisplay(String string);
void startWiFi();
void startUDP();
void drawTime();
void drawDate();

void setup() {
  Serial.begin(9600);
  setupDisplay();
  startWiFi();
  startUDP();
  timeClient.begin();
}

void loop() {
  timeClient.update();

  String formattedTime = timeClient.getFormattedTime();
  Serial.printf("\rFormatted Time: %s", formattedTime.c_str());

  display.clear();
  drawTime();
  drawDate();
  display.display();
}

void setupDisplay() {
  display.init();
  display.setBrightness(128);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
}

void startWiFi() {
  printDisplay("Conectando wifi...");
  wifi.connectStation(ssid, pass);
  printDisplay("Conectado à rede " + String(ssid) + "!");
  delay(2000);
}

void startUDP() {
  Serial.println("\nStarting UDP");
  udp.begin(123);                          // Start listening for UDP messages on port 123
  Serial.print("Local port:\t");
  Serial.println(udp.localPort());
  Serial.println();
}

void printDisplay(String string) {
  display.clear();
  display.drawStringMaxWidth(centerX, 0, screenWidth, string);
  display.display();
}

void drawTime() {
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();

  char string[6];
  sprintf(string, "%02d:%02d", hour, minute);
  formattedTimeDisplay = String(string);

  display.setFont(ArialMT_Plain_24);
  display.drawStringMaxWidth(centerX, 0, screenWidth, formattedTimeDisplay);
}

void drawDate() {
  unsigned long epochTime = timeClient.getEpochTime();

  time_t seconds = time_t(epochTime);
  struct tm *currentTime = localtime(&seconds);
  int day = currentTime->tm_mday;
  String month = months[currentTime->tm_mon];

  char string[30];
  sprintf(string, "%02d %s", day, month.c_str());
  formattedDateDisplay = String(string);

  display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(centerX, centerY, screenWidth, formattedDateDisplay);
}
