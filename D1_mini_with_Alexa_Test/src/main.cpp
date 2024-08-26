#include <Arduino.h>
#include <connectionProperties.h>
#include <fauxmoProperties.h>
#include <LittleFS.h>

#define RELAY_PIN LED_BUILTIN

unsigned long lastFreeHeap = millis();

void setup() {
    Serial.begin(9600);
    Serial.println("\n\nBooting...");

    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    setupConnection();
    setupFauxmo(name);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);
}

void loop() {
    server.handleClient();
    MDNS.update();
    wifi.refreshConnection();
    fauxmo.handle();

    if (millis() - lastFreeHeap > 5000) {
        lastFreeHeap = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }
}

void onStateChange() {
    Serial.println("RELAY 1 switched by Alexa");
    if (currentState) {
        digitalWrite(RELAY_PIN, LOW);
    } else {
        digitalWrite(RELAY_PIN, HIGH);
    }
}