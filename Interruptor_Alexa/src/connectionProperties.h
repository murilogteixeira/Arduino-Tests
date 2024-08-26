#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <OTA.h>
#include <Configuration.h>

char uploadHtmlString[] = "<form method=\"post\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"name\"><input class=\"button\" type=\"submit\" value=\"Upload\"></form>";
ESP8266WebServer server(81);
OTA ota;
Configuration config;

String name;
WiFiManager wifi;
char ssid[] = "MURILO";
char password[] = "94658243";

void setupServer();

void setupConnection() {
    config.begin();
    name = config.getEspName();

    MDNS.begin(name);
    wifi.connectStation(ssid, password, []() {
        setupServer();
        MDNS.addService("http", "tcp", 80);
        Serial.printf("MDNS: http://%s.local:81\n", name.c_str());
    });
}

void setupServer() {
    // OTA
    server.on("/ota/update", HTTP_GET, []() { server.send(200, "text/html", uploadHtmlString); });
    server.on("/ota/update", HTTP_POST, []() { ota.updateHandler(&server); }, []() { ota.uploadHandler(&server); });
    
    // CONFIG
    server.on("/config/updateName", HTTP_GET, []() { config.updateNameHandler(&server); });

    // 404
    server.onNotFound([]() { server.send(404, "text/plain", "404: Not Found"); });

    server.begin();
}