#include "AQMonitor.h"

void setup()
{ 
    Serial.begin(115200);
    while (! Serial) {
        delay(1);
    }
    settings.begin();

    wifi.begin();
    led.begin();
    aqSensors.begin();
    webServer.begin();
    systemCheck.begin();
    telemetryCollector.begin();

    wifi.connect();
}

void loop() {
    wifi.loop();
    webServer.loop();
    aqSensors.loop();
    settings.loop();
    led.loop();
    systemCheck.loop();
    telemetryCollector.loop();
    delay(100);
}
