#include "AQMonitor.h"

char buffer[4096];

WebServer::WebServer(NetworkSettings* networkSettings, Logger* logger, SystemCheck* systemCheck)
    :WebServerBase(networkSettings, logger, systemCheck) {
}

void WebServer::registerHandlers() {
    server->on("/", std::bind(&WebServer::handle_root, this));
    server->on("/get", std::bind(&WebServer::handle_get, this));
    server->on("/settings", std::bind(&WebServer::handle_settings, this));
    server->on("/reset", std::bind(&WebServer::handle_reset, this));
    server->on("/blink", std::bind(&WebServer::handle_blink, this));
}

void WebServer::handle_root() {
    systemCheck->registerWebCall();
    server->sendHeader("Location","/settings");
    server->send(303);
}

void WebServer::handle_get() {
    systemCheck->registerWebCall();

    sprintf_P(buffer,
              GET_JSON,
              aqSensors.getTemp(),
              aqSensors.getHumidity(),
              aqSensors.getPressure(),
              aqSensors.getGasResistance(),
              aqSensors.getAccuracy(),
              aqSensors.getIAQ(),
              aqSensors.getStaticIAQ(),
              aqSensors.getCalculatedIAQ());
    server->send(200, "application/json", buffer);
}

void WebServer::handle_settings() {
    systemCheck->registerWebCall();

    bool save = false;

    process_setting("hostname", settingsData.network.hostname, sizeof(settingsData.network.hostname), save);
    process_setting("ssid", settingsData.network.ssid, sizeof(settingsData.network.ssid), save);
    process_setting("password", settingsData.network.password, sizeof(settingsData.network.password), save);

    process_setting("ifx_enabled", settingsData.influxDB.enable, save);
    process_setting("ifx_address", settingsData.influxDB.address, sizeof(settingsData.influxDB.address), save);
    process_setting("ifx_db", settingsData.influxDB.database, sizeof(settingsData.influxDB.database), save);
    process_setting("ifx_collect", settingsData.influxDB.collectInterval, save);
    process_setting("ifx_push", settingsData.influxDB.pushInterval, save);

    process_setting("temp_offset", settingsData.aqSensor.temperatureOffset, save);
    process_setting("humidity_offset", settingsData.aqSensor.humidityOffset, save);
    process_setting("calibration_period", settingsData.aqSensor.calibrationPeriod, save);
    process_setting("good_aq_res", settingsData.aqSensor.goodAQResistance, save);
    process_setting("bad_aq_res", settingsData.aqSensor.badAQResistance, save);
    
    process_setting("blink_interval", settingsData.led.blinkInterval, save);

    if (save) {
        settings.save();
    }

    sprintf_P(
        buffer,
        CONFIG_PAGE,
        settingsData.network.hostname,
        settingsData.network.ssid,
        (settingsData.influxDB.enable)?"selected":"",
        (!settingsData.influxDB.enable)?"selected":"",
        settingsData.influxDB.address,
        settingsData.influxDB.database,
        settingsData.influxDB.collectInterval,
        settingsData.influxDB.pushInterval,
        settingsData.aqSensor.temperatureOffset, 
        settingsData.aqSensor.humidityOffset,
        (settingsData.aqSensor.calibrationPeriod != 28)?"selected":"",
        (settingsData.aqSensor.calibrationPeriod == 28)?"selected":"",
        settingsData.aqSensor.goodAQResistance,
        settingsData.aqSensor.badAQResistance,
        settingsData.led.blinkInterval);
    server->send(200, "text/html", buffer);
}

void WebServer::handle_reset() {
    systemCheck->registerWebCall();
    settings.erase();
    server->send(200, "text/plain", "Calibration data erased.");
}

void WebServer::handle_blink() {
    systemCheck->registerWebCall();

    if (server->hasArg("iaq")) {
        float iaq = server->arg("iaq").toFloat();
        led.blink(iaq);
        server->send(200, "text/plain", "Just blinked!");
    } else {
        server->send(400, "text/plain", "Missing iaq argument!");
    }
}
