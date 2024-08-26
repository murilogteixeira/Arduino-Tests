const int ledPin =  1;  // The blue LED on the ESP-01 module is connected to GPIO1 
                        // (which is also the TXD pin; so we cannot user Serial at the same time
 
int ledState = LOW;     
 
unsigned long previousMillis = 0;
const long interval = 1000;
 
void setup() {
  pinMode(ledPin, OUTPUT);
}
 
void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(ledPin, ledState);
  }
}
