#include "AQMonitor.h"

WebServer::WebServer(int port) {

    _server = new ESP8266WebServer(port);
    _server->on("/", std::bind(&WebServer::handle_root, this));
    _server->on("/get", std::bind(&WebServer::handle_get, this));
    _server->on("/reset", std::bind(&WebServer::handle_reset, this));

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
    _server->send(
        200,
        "text/plain",
        "Air Quality Monitor.\nExposed endpoints are:\n *...");
}

void WebServer::handle_reset() {
    _server->send(200);
    delay(1000);
    ESP.reset();
}

void WebServer::handle_get() {
    char resp[128];

    sprintf(resp,
            "{\"temp\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f,\"voc\":%.2f,\"co2e\":%.2f}",
            aqSensors.getTemp(),
            aqSensors.getHumidity(),
            aqSensors.getPressure(),
            aqSensors.getVOC(),
            aqSensors.getCO2e());

    Serial.println(resp);

    _server->send(200, "application/json", resp);
}

WebServer webServer = WebServer(HTTP_PORT);
