#include <Arduino.h>
#include <Led.h>
#include <ServoMotor.h>

Led led1(13, 1000, 1000);
Led led2(10, 100, 100);
Led led3(8, 200, 600);

ServoMotor servo(6, 0);

void setup() {
  Serial.begin(9600);
  servo.attach();
  Serial.println("Setup finalizado.");
}

void loop() {
  led1.blink();
  led2.blink();
  led3.blink();

  servo.updatePos();
}