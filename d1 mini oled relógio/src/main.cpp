
#include <Wire.h>
#include "SSD1306Wire.h"
#include <WiFiManager.h>
#include <WiFiUdp.h>

WiFiUDP UDP;                     // Create an instance of the WiFiUDP class to send and receive

IPAddress timeServerIP;          // c.ntp.brNTP server address
const char* NTPServerName = "b.ntp.br";

const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message

char NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);
WiFiManager wifi;

char ssid[] = "Murilo";
char pass[] = "94658243";
int screenWidth = 64 - 1;
int screenHeight = 48 - 1;
int centerX = screenWidth / 2;
int centerY = screenHeight / 2;

unsigned long intervalNTP = 60000; // Request NTP time every minute
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = millis();
uint32_t timeUNIX = 0;

unsigned long prevActualTime = 0;

unsigned long lastDisplayTime = 0;

void setupDisplay();
void printDisplay(String string);
void startWiFi();
void startUDP();
uint32_t getTime();
void sendNTPpacket(IPAddress& address);
inline int getSeconds(uint32_t UNIXTime);
inline int getMinutes(uint32_t UNIXTime);
inline int getHours(uint32_t UNIXTime);

void setup() {
  Serial.begin(9600);
  setupDisplay();
  startWiFi();
  startUDP();

  if(!WiFi.hostByName(NTPServerName, timeServerIP)) { // Get the IP address of the NTP server
    Serial.println("DNS lookup failed. Rebooting.");
    Serial.flush();
    ESP.reset();
  }
  Serial.print("Time server IP:\t");
  Serial.println(timeServerIP);
  
  Serial.println("\r\nSending NTP request ...");
  sendNTPpacket(timeServerIP); 

  display.setFont(ArialMT_Plain_24);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - prevNTP > intervalNTP) { // If a minute has passed since last NTP request
    prevNTP = currentMillis;
    Serial.println("\r\nSending NTP request ...");
    sendNTPpacket(timeServerIP);               // Send an NTP request
  }

  uint32_t time = getTime();                   // Check if an NTP response has arrived and get the (UNIX) time
  if (time) {                                  // If a new timestamp has been received
    timeUNIX = time - 10800;
    Serial.print("NTP response:\t");
    Serial.println(timeUNIX);
    lastNTPResponse = currentMillis;
  } else if ((currentMillis - lastNTPResponse) > 3600000) {
    Serial.println("More than 1 hour since last NTP response. Rebooting.");
    Serial.flush();
    ESP.reset();
  }

  uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse)/1000;
  if (actualTime != prevActualTime && timeUNIX != 0) { // If a second has passed since last print
    prevActualTime = actualTime;
    String timeString;
    timeString = timeString + getHours(actualTime);
    timeString = timeString + ":" + getMinutes(actualTime);
    
    if (millis() - lastDisplayTime > 2000)
      printDisplay(timeString);

    timeString = timeString + ":" + getSeconds(actualTime);
    Serial.printf("\rUTC time:\t%s   ", timeString.c_str());
  }
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
  lastDisplayTime = millis();
}

void startUDP() {
  Serial.println("Starting UDP");
  UDP.begin(123);                          // Start listening for UDP messages on port 123
  Serial.print("Local port:\t");
  Serial.println(UDP.localPort());
  Serial.println();
}

void printDisplay(String string) {
  display.clear();
  display.drawStringMaxWidth(centerX, centerY, screenWidth, string);
  display.display();
}

uint32_t getTime() {
  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
  uint32_t UNIXTime = NTPTime - seventyYears;
  return UNIXTime;
}

void sendNTPpacket(IPAddress& address) {
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

inline int getSeconds(uint32_t UNIXTime) {
  return UNIXTime % 60;
}

inline int getMinutes(uint32_t UNIXTime) {
  return UNIXTime / 60 % 60;
}

inline int getHours(uint32_t UNIXTime) {
  return (UNIXTime / 3600 % 24);
}