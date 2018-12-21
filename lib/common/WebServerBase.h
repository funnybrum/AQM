#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

#include "Logger.h"
#include "SystemCheck.h"
#include "WiFi.h"

class WebServerBase {
    public:
        WebServerBase(NetworkSettings* networkSettings, Logger* logger, SystemCheck* systemCheck=NULL) {
            this->logger = logger;
            this->systemCheck = systemCheck;
            this->networkSettings = networkSettings;

        };
 
        void begin() {
            server = new ESP8266WebServer(80);
            server->on("/reboot", std::bind(&WebServerBase::handle_reboot, this));
            server->on("/logs", std::bind(&WebServerBase::handle_logs, this));
            registerHandlers();

            httpUpdater = new ESP8266HTTPUpdateServer(true);
            httpUpdater->setup(server);

            MDNS.begin(networkSettings->hostname);
            MDNS.addService("http", "tcp", 80);

            server->begin();
        }

        void loop() {
            server->handleClient();
        }

        virtual void registerHandlers() = 0;

    protected:
        Logger* logger = NULL;
        SystemCheck* systemCheck = NULL;
        ESP8266WebServer *server;

        void process_setting(const char* name, char* destination, uint8_t max_size, bool& success) {
            if (server->hasArg(name)) {
                String new_value = server->arg(name);
                if (new_value.length() > 2 && new_value.length()+1 < max_size) {
                    strcpy(destination, new_value.c_str());
                    success = true;
                }
            }
        }

        void process_setting(const char* name, int16_t& destination, bool& success) {
            if (server->hasArg(name)) {
                destination = server->arg(name).toInt();
                success = true;
            }
        }

        void process_setting(const char* name, uint16_t& destination, bool& success) {
            if (server->hasArg(name)) {
                destination = server->arg(name).toInt();
                success = true;
            }
        }

        void process_setting(const char* name, bool& destination, bool& success) {
            if (server->hasArg(name)) {
                String val = server->arg(name);
                if (val.compareTo("true") == 0) {
                    destination = true;
                    success = true;
                } else if (val.compareTo("false") == 0) {
                    destination = false;
                    success = true;
                }
            }
        }

    private:
        ESP8266HTTPUpdateServer *httpUpdater;
        NetworkSettings* networkSettings = NULL;

        void handle_reboot() {
            server->send(200, "text/plain", "Restarting...");
            delay(1000);
            ESP.reset();
        }

        void handle_logs() {
            if (systemCheck != NULL) {
                systemCheck->registerWebCall();
            }
            server->send(200, "text/plain", logger->getLogs());
        }
};
