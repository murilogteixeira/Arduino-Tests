#include <Arduino.h>

#define LED_PIN 8
#define BUTTON_PIN 7

bool ledStatus = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Setup finalizado...");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    ledStatus = !ledStatus;
    digitalWrite(LED_PIN, ledStatus);
    while (digitalRead(BUTTON_PIN) == LOW)
      delay(100);
  }
}