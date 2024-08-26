#include <Arduino.h>

const int ledPin = 13;
int ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
  delay(100);
}