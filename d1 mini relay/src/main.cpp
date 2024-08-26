#include <Arduino.h>

int state = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(5, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  state = !state;
  digitalWrite(5, state);
  delay(2000);
}