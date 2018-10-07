#include "AQMonitor.h"

WebServer::WebServer(int port) {

    _server = new ESP8266WebServer(port);
    _server->on("/", std::bind(&WebServer::handle_root, this));
    _server->on("/get", std::bind(&WebServer::handle_get, this));
    _server->on("/reset", std::bind(&WebServer::handle_reset, this));
    _server->on("/hard-reset", std::bind(&WebServer::handle_hard_reset, this));
    _server->on("/blink", std::bind(&WebServer::handle_blink, this));
    _server->on("/temp-offset", std::bind(&WebServer::handle_temp_offset, this));
    _server->on("/humidity-offset", std::bind(&WebServer::handle_humidity_offset, this));
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
    _server->send(200, "text/plain", "Air Quality Monitor.");
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
    _server->send(200, "text/plain", "Calibration data erased.");
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

    _server->send(200, "text/plain", "Just blinked!");
}

int8_t WebServer::getBodyValue(uint8_t currentValue) {
    if (_server->method() == HTTP_POST) {
        if (_server->hasArg("plain") == true) { //Check if body received.
            //Convert body to uint_8
            int val = _server->arg("plain").toInt();
            if (-125 <= val && val <= 125) {
                char resp[20];
                sprintf(resp, "Offset set to %d", val);
                _server->send(200, "text/plain", resp);
                return val;
            }
        }

        _server->send(400, "text/plain", "Offset not in range [-125:125]");
        return -127;
    } else if (_server->method() == HTTP_GET) {
        char resp[16];
        sprintf(resp, "Offset is %d", currentValue);
        _server->send(200, "text/plain", resp);
        return -127;
    }
    _server->send(400);
    return -127;
}

void WebServer::handle_humidity_offset() {
    systemCheck.registerWebCall();
    int8_t val = getBodyValue(settings.get()->humidityOffset);
    if (-125 <= val && val <= 125) {
        settings.get()->humidityOffset = val;
        settings.save();
    }
}

void WebServer::handle_temp_offset() {
    systemCheck.registerWebCall();
    int8_t val = getBodyValue(settings.get()->temperatureOffset);
    if (-125 <= val && val <= 125) {
        settings.get()->temperatureOffset = val;
        settings.save();
    }
}


void WebServer::handle_hostname() {
    systemCheck.registerWebCall();
    if (_server->method() == HTTP_POST) {
        String new_hostname = _server->arg("plain");
        if (new_hostname.length() > 1) {
            bool valid = true;
            for (unsigned int i = 0; i < new_hostname.length(); i++) {
                char ch = new_hostname.charAt(i);
                if (!isalnum(ch) && ch != '-') {
                    valid = false;
                }
            }

            if (valid) {
                char* hostname = settings.get()->hostname;
                unsigned int max_size = sizeof(settings.get()->hostname);

                if (new_hostname.length() < max_size - 1) {
                    memset(hostname, 0, max_size);
                    strcpy(hostname, new_hostname.c_str());
                    settings.save();
                    _server->send(200);
                    return;
                }                
            }
        }

        _server->send(400, "text/plain", "Invalid hostname.");
    } else if (_server->method() == HTTP_GET) {
        char resp[128];
        sprintf(resp, "Hostname is '%s'", settings.get()->hostname);
        _server->send(200, "text/plain", resp);
    }

    _server->send(400);
}

WebServer webServer = WebServer(HTTP_PORT);
