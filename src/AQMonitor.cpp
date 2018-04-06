#include "AQMonitor.h"

void setup()
{ 
    Serial.begin(115200);
    while (! Serial) {
        delay(1);
    }
    ScanAndConnect();
    webServer.begin();
    aqSensors.begin();
}

void loop() {
    webServer.loop();
    aqSensors.loop();
    delay(100);
}
