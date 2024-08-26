#include <Arduino.h>
#include <Led.h>

Led::Led(int pin, long onMillis, long offMillis) {
    this->pin = pin;
    this->onMillis = onMillis;
    this->offMillis = offMillis;
    Led::setup();
}

void Led::setup() {
    lastUpdate = millis();
    state = LOW;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state);
}

void Led::blink() {
    switch (state) {
    case LOW:
        if (millis() - lastUpdate >= offMillis)
            setState();
        break;
    case HIGH:
        if (millis() - lastUpdate >= onMillis)
            setState();
        break;
    default: break;
    }
}

void Led::setState() {
    state = !state;
    lastUpdate = millis();
    digitalWrite(pin, state);
}