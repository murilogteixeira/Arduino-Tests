#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

#include <connectionProperties.h>
#include<fauxmoProperties.h>

#define RELAY_PIN LED_BUILTIN

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nBooting...");

  setupConnection();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
}

void loop() {
  wifi.refreshConnection();
}