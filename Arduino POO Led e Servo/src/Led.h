using namespace std;

struct Led {
    private:
        int pin;
        int state;
        unsigned long lastUpdate;
        unsigned long onMillis;
        unsigned long offMillis;
        void setup();
        void setState();
    public: 
        Led(int pin, long onMillis, long offMillis);
        void blink();
};