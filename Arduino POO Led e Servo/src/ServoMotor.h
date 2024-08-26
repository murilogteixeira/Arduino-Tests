#include <Arduino.h>
#include <Servo.h>

using namespace std;

struct ServoMotor {
    private:
        Servo servo;
        int pin;
        int pos;
        int increment;
        int updateInterval;
        unsigned long lastUpdate;
    public:
        ServoMotor(int pin, int updateInterval);
        void attach();
        void detach();
        void updatePos();
};