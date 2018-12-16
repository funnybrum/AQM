#include "AQMonitor.h"

void initSettings() {
    strcpy(settingsData.network.hostname, HOSTNAME);
}

void collectData(InfluxDBCollector* collector) {
    collector->append("iaq", aqSensors.getIAQ());
    collector->append("iaq_static", aqSensors.getStaticIAQ());
    collector->append("iaq_calculated", aqSensors.getCalculatedIAQ());
    collector->append("temperature", aqSensors.getTemp(), 2);
    collector->append("humidity", aqSensors.getHumidity(), 2);
    collector->append("resistance", aqSensors.getGasResistance());
    collector->append("pressure", aqSensors.getPressure());
    collector->append("accuracy", aqSensors.getAccuracy());
    collector->append("free_heap", ESP.getFreeHeap());
}

SettingsData settingsData = SettingsData();
Logger logger = Logger();
Settings settings = Settings(&logger, (void*)(&settingsData), sizeof(SettingsData), initSettings);
WiFiManager wifi = WiFiManager(&logger, &settingsData.network);
InfluxDBCollector telemetryCollector = InfluxDBCollector(
    &logger, &wifi, &settingsData.influxDB, &settingsData.network, collectData);

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
