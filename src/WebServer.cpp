#include "AQMonitor.h"

char buffer[4096];

WebServer::WebServer(int port) {

    _server = new ESP8266WebServer(port);
    _server->on("/", std::bind(&WebServer::handle_root, this));
    _server->on("/get", std::bind(&WebServer::handle_get, this));
    _server->on("/settings", std::bind(&WebServer::handle_settings, this));
    _server->on("/reboot", std::bind(&WebServer::handle_reboot, this));
    _server->on("/reset", std::bind(&WebServer::handle_reset, this));
    _server->on("/blink", std::bind(&WebServer::handle_blink, this));
    _server->on("/logs", std::bind(&WebServer::handle_logs, this));

    _httpUpdater = new ESP8266HTTPUpdateServer(true);
    _httpUpdater->setup(_server);

    MDNS.begin(HOSTNAME);
    MDNS.addService("http", "tcp", 80);

}

void WebServer::begin() {
    _server->begin();
}

void WebServer::loop() {
    _server->handleClient();
}

void WebServer::handle_root() {
    systemCheck.registerWebCall();
    _server->sendHeader("Location","/settings");
    _server->send(303);
}

void WebServer::handle_get() {
    systemCheck.registerWebCall();

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
    _server->send(200, "application/json", buffer);
}

void WebServer::handle_settings() {
    systemCheck.registerWebCall();

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
    _server->send(200, "text/html", buffer);
}

void WebServer::handle_reboot() {
    systemCheck.registerWebCall();
    _server->send(200, "text/plain", "Restarting...");
    delay(1000);
    ESP.reset();
}

void WebServer::handle_reset() {
    systemCheck.registerWebCall();
    settings.erase();
    _server->send(200, "text/plain", "Calibration data erased.");
}

void WebServer::handle_blink() {
    systemCheck.registerWebCall();

    if (_server->hasArg("iaq")) {
        float iaq = _server->arg("iaq").toFloat();
        led.blink(iaq);
        _server->send(200, "text/plain", "Just blinked!");
    } else {
        _server->send(400, "text/plain", "Missing iaq argument!");
    }
}

void WebServer::handle_logs() {
    systemCheck.registerWebCall();
    _server->send(200, "text/html", logger.getLogs());
}

void WebServer::process_setting(const char* name, char* destination, uint8_t max_size, bool& success) {
    if (_server->hasArg(name)) {
        String new_value = _server->arg(name);
        if (new_value.length() > 2 && new_value.length()+1 < max_size) {
            strcpy(destination, new_value.c_str());
            success = true;
        }
    }
}

void WebServer::process_setting(const char* name, int16_t& destination, bool& success) {
    if (_server->hasArg(name)) {
        destination = _server->arg(name).toInt();
        success = true;
    }
}

void WebServer::process_setting(const char* name, uint16_t& destination, bool& success) {
    if (_server->hasArg(name)) {
        destination = _server->arg(name).toInt();
        success = true;
    }
}

void WebServer::process_setting(const char* name, bool& destination, bool& success) {
    if (_server->hasArg(name)) {
        String val = _server->arg(name);
        if (val.compareTo("true") == 0) {
            destination = true;
            success = true;
        } else if (val.compareTo("false") == 0) {
            destination = false;
            success = true;
        }
    }
}

WebServer webServer = WebServer(HTTP_PORT);
