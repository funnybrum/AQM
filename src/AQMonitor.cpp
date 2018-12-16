#include "AQMonitor.h"

void initSettings() {
    strcpy(settingsData.network.hostname, HOSTNAME);
}

SettingsData settingsData = SettingsData();
Logger logger = Logger();
Settings settings = Settings(logger, (void*)(&settingsData), sizeof(SettingsData), initSettings);

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

    if (settingsData.influxDB.enable) {
        systemCheck.stop();
        telemetryCollector.start();
    } else {
        telemetryCollector.stop();
        systemCheck.start();
    }

    delay(100);
}
