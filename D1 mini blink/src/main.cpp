#include <Arduino.h>

int state = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
}

void loop() {
  state = !state;
  digitalWrite(2, state);
  delay(500);
}