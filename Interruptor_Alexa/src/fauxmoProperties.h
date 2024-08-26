#include <Arduino.h>
#include <fauxmoESP.h>

fauxmoESP fauxmo;
bool currentState;

void onStateChange();

void setupFauxmo() {
    fauxmo.createServer(true);
    fauxmo.setPort(80);
    fauxmo.enable(true);

    fauxmo.addDevice(name.c_str());

    fauxmo.onSetState([](unsigned char device_id, const char *device_name, bool state, unsigned char value) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
        if ( (strcmp(device_name, name.c_str()) == 0) ) {
            currentState = state;
            onStateChange();
        }
    });
}