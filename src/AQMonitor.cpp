#include "AQMonitor.h"

void initSettings() {
    strcpy(settingsData.network.hostname, HOSTNAME);
}

float lastPushedTVOC = -1;
float lastCollectedTVOC = -1;

void collectData(InfluxDBCollector* collector) {
    lastCollectedTVOC = sgp30.getTVOC();

    // Avoid false positivive initial push condition.
    if (lastPushedTVOC < 0) {
        lastPushedTVOC = lastCollectedTVOC;
    }

    collector->append("eco2", sgp30.geteCO2());
    collector->append("tvoc", sgp30.getTVOC());
    collector->append("temperature", bme280.getTemperature(), 2);
    collector->append("humidity", bme280.getHumidity(), 1);
    collector->append("pressure", bme280.getPressure());
    collector->append("absolute_humidity", bme280.getAbsoluteHimidity());
    collector->append("free_heap", ESP.getFreeHeap());
}

void onPush() {
    lastPushedTVOC = lastCollectedTVOC;
}

/**
 * Push the collected data if the current IAQ vs the last pushed IAQ is with difference at least 20
 * and at least 20% at the same time.
 */
bool shouldPush() {
    if (lastCollectedTVOC < 0) {
        return false;
    }

    return lastPushedTVOC/lastCollectedTVOC <= 0.8f || lastPushedTVOC/lastCollectedTVOC >= 1.25f;
}

SettingsData settingsData = SettingsData();
Logger logger = Logger();
Settings settings = Settings(&logger, (void*)(&settingsData), sizeof(SettingsData), initSettings);
WiFiManager wifi = WiFiManager(&logger, &settingsData.network);
SystemCheck systemCheck = SystemCheck(&logger);
InfluxDBCollector telemetryCollector = InfluxDBCollector(
    &logger, &wifi, &settingsData.influxDB, &settingsData.network, collectData, shouldPush, onPush);

WebServer webServer = WebServer(&settingsData.network, &logger, &systemCheck);
BME280 bme280 = BME280(1000, &settingsData.aqSensor.temperatureOffset);
SGP30 sgp30 = SGP30();

void setup()
{ 
    Serial.begin(115200);
    while (! Serial) {
        delay(1);
    }
    settings.begin();
    wifi.begin();
    led.begin();
    webServer.begin();
    systemCheck.begin();
    telemetryCollector.begin();
    bme280.begin();
    sgp30.begin();

    wifi.connect();
}

void loop() {
    wifi.loop();
    webServer.loop();
    settings.loop();
    led.loop();
    systemCheck.loop();
    telemetryCollector.loop();
    bme280.loop();
    sgp30.loop();

    if (settingsData.influxDB.enable) {
        systemCheck.stop();
        telemetryCollector.start();
    } else {
        telemetryCollector.stop();
        systemCheck.start();
    }

    delay(100);
}
