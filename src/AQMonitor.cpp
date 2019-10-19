#include "AQMonitor.h"

void initSettings() {
    strcpy(settingsData.network.hostname, HOSTNAME);
}

float lastPushedIAQ = -1;
float lastCollectedIAQ = -1;

void collectData(InfluxDBCollector* collector) {
    lastCollectedIAQ = aqSensors.getIAQ();
    if (lastPushedIAQ < 0) {
        lastPushedIAQ = lastCollectedIAQ;
    }
    collector->append("iaq", aqSensors.getIAQ());
    collector->append("iaq_static", aqSensors.getStaticIAQ());
    collector->append("temperature", aqSensors.getTemp(), 2);
    collector->append("humidity", aqSensors.getHumidity(), 1);
    collector->append("resistance", aqSensors.getGasResistance());
    collector->append("pressure", aqSensors.getPressure());
    collector->append("accuracy", aqSensors.getAccuracy());
    // collector->append("free_heap", ESP.getFreeHeap());
}

void onPush() {
    lastPushedIAQ = lastCollectedIAQ;
}

/**
 * Push the collected data if the current IAQ vs the last pushed IAQ is with difference at least 20
 * and at least 20% at the same time.
 */
bool shouldPush() {
    if (lastCollectedIAQ < 0) {
        return false;
    }

    if (abs(lastPushedIAQ - lastCollectedIAQ) >= 20 &&
        (lastPushedIAQ/lastCollectedIAQ <= 0.8f || lastPushedIAQ/lastCollectedIAQ >= 1.25f)) {
            return true;
        }

    return false;
}

SettingsData settingsData = SettingsData();
Logger logger = Logger();
Settings settings = Settings(&logger, (void*)(&settingsData), sizeof(SettingsData), initSettings);
WiFiManager wifi = WiFiManager(&logger, &settingsData.network);
SystemCheck systemCheck = SystemCheck(&logger);
InfluxDBCollector telemetryCollector = InfluxDBCollector(
    &logger, &wifi, &settingsData.influxDB, &settingsData.network, collectData, shouldPush, onPush);

WebServer webServer = WebServer(&settingsData.network, &logger, &systemCheck);

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
