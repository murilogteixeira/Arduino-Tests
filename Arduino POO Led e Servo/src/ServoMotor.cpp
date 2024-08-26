#include <Arduino.h>
#include <ServoMotor.h>

ServoMotor::ServoMotor(int pin, int updateInterval) {
    this->pin = pin;
    this->updateInterval = updateInterval;
    this->increment = 1;
}

void ServoMotor::attach() {
    servo.attach(pin);
}

void ServoMotor::detach() {
    servo.detach();
}

void ServoMotor::updatePos() {
    if(millis() - lastUpdate >= updateInterval) {
        lastUpdate = millis();
        pos += increment;
        servo.write(pos);
        Serial.println(pos);
        if((pos >= 180) || (pos <= 0)) {
            increment = -increment;
        }
    }
}