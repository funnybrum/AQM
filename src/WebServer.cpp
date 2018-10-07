#include "AQMonitor.h"

WebServer::WebServer(int port) {

    _server = new ESP8266WebServer(port);
    _server->on("/", std::bind(&WebServer::handle_root, this));
    _server->on("/get", std::bind(&WebServer::handle_get, this));
    _server->on("/reset", std::bind(&WebServer::handle_reset, this));
    _server->on("/hard-reset", std::bind(&WebServer::handle_hard_reset, this));
    _server->on("/blink", std::bind(&WebServer::handle_blink, this));
    _server->on("/temp-offset", std::bind(&WebServer::handle_temp_offset, this));
    _server->on("/hummidity-offset", std::bind(&WebServer::handle_humidity_offset, this));
    _server->on("/hostname", std::bind(&WebServer::handle_hostname, this));

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
    _server->send(
        200,
        "text/plain",
        "Air Quality Monitor.\nExposed endpoints are:\n *...");
}

void WebServer::handle_reset() {
    systemCheck.registerWebCall();
    _server->send(200);
    delay(1000);
    ESP.reset();
}

void WebServer::handle_hard_reset() {
    systemCheck.registerWebCall();
    settings.erase();
    _server->send(
        200,
        "text/plain",
        "Calibration data erased.");
}

void WebServer::handle_get() {
    systemCheck.registerWebCall();
    char resp[128];

    sprintf(resp,
            "{\"temp\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f,\"iaq\":%.2f,\"iaq_accuracy\":%.2f,\"gas_resistance\":%.0f}",
            aqSensors.getTemp(),
            aqSensors.getHumidity(),
            aqSensors.getPressure(),
            aqSensors.getIAQ(),
            aqSensors.getIAQAccuracy(),
            aqSensors.getGasResistance());

    _server->send(200, "application/json", resp);
}

void WebServer::handle_blink() {
    systemCheck.registerWebCall();
    led.set(255, 0, 0);
    delay(500);
    led.set(0, 255, 0);
    delay(500);
    led.set(0, 0, 255);
    delay(500);
    led.set(0, 0, 0);

    _server->send(
        200,
        "text/plain",
        "Just blinked!");
}

int8_t WebServer::getBodyValue() {
    if (_server->hasArg("plain") == true) { //Check if body received.
        //Convert body to uint_8
        String body = _server->arg("plain");
        long val = body.toInt();
        if (-125 <= val && val <= 125) {
            String result = "Offset set to " + val;
            _server->send(200,
                        "text/plain",
                        result);
            return val;
        }
    }

    _server->send(
        400,
        "text/plain",
        "Offset not in range [-125:125]");
    return -127;
}

void WebServer::handle_humidity_offset() {
    systemCheck.registerWebCall();
    int8_t val = getBodyValue();
    if (-125 <= val && val <= 125) {
        settings.get()->humidityOffset = val;
        settings.save();
    }
}

void WebServer::handle_temp_offset() {
    systemCheck.registerWebCall();
    int8_t val = getBodyValue();
    if (-125 <= val && val <= 125) {
        settings.get()->temperatureOffset = val;
        settings.save();
    }
}


void WebServer::handle_hostname() {
    systemCheck.registerWebCall();
    String hostname = _server->arg("plain");
    if (hostname.length() > 1) {

        bool valid = true;

        for (unsigned int i = 0; i < hostname.length(); i++) {
            char ch = hostname.charAt(i);
            if (isalnum(ch) == false && ch != '-') {
                valid = false;
            }
        }

        if (valid) {
            memset(settings.get()->hostname,
                0,
                sizeof(settings.get()->hostname));
            memcpy(settings.get()->hostname,
                hostname.begin(),
                min((unsigned int) 64, hostname.length()));
            settings.save();
            _server->send(200);
            return;
        }
    }

    _server->send(
        400,
        "text/plain",
        "Invalid host name, use [a:z], [A:Z], [0:9] and '-'.");
}

WebServer webServer = WebServer(HTTP_PORT);
