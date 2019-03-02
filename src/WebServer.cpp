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
              bme280.getTemperature(),
              bme280.getHumidity(),
              bme280.getPressure(),
              sgp30.getTVOC(),
              sgp30.geteCO2());

    server->send(200, "application/json", buffer);
}

void WebServer::handle_settings() {
    systemCheck->registerWebCall();

    bool save = false;

    wifi.parse_config_params(this, save);
    telemetryCollector.parse_config_params(this, save);

    process_setting("temp_offset", settingsData.aqSensor.temperatureOffset, save);
    process_setting("humidity_offset", settingsData.aqSensor.humidityOffset, save);
    process_setting("calibration_period", settingsData.aqSensor.calibrationPeriod, save);
    process_setting("good_aq_res", settingsData.aqSensor.goodAQResistance, save);
    process_setting("bad_aq_res", settingsData.aqSensor.badAQResistance, save);
    
    process_setting("blink_interval", settingsData.led.blinkInterval, save);

    if (save) {
        settings.save();
    }

    char network_settings[strlen_P(NETWORK_CONFIG_PAGE) + 32];
    wifi.get_config_page(network_settings);

    char influxdb_settings[strlen_P(INFLUXDB_CONFIG_PAGE) + 96];
    telemetryCollector.get_config_page(influxdb_settings);

    sprintf_P(
        buffer,
        CONFIG_PAGE,
        network_settings,
        influxdb_settings,
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
