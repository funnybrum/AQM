#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "Logger.h"

enum WiFiState {
    CONNECTING,
    CONNECTED,
    DISCONNECTED
};

struct NetworkSettings {
    char hostname[64];
    char ssid[32];
    char password[32];
};

class WiFiManager {
    public:
        WiFiManager(Logger* logger, NetworkSettings* settings) {
            this->logger = logger;
            this->settings = settings;
        }

        void begin() {
            WiFi.persistent(false);
            WiFi.mode(WIFI_STA);
            state = DISCONNECTED;
            lastStateSetAt = millis();
        }

        void loop() {
            switch(state) {
                case CONNECTED:
                    // Do nothing.
                    break;
                case CONNECTING:
                    if (WiFi.status() == WL_CONNECTED) {
                        logger->log("Connected in %.1f seconds", (millis() - lastStateSetAt)/1000.0f);
                        logger->log("IP address is %s", WiFi.localIP().toString().c_str());
                        state = CONNECTED;
                        lastStateSetAt = millis();
                    } else if (millis() - lastStateSetAt > 30000) {
                        logger->log("Failed to connect in 30 seconds, switching to AP mode");

                        // For debug purposes - switch to access mode.
                        WiFi.softAPConfig(
                            IPAddress(192, 168, 0, 1),
                            IPAddress(192, 168, 0, 1),
                            IPAddress(255, 255, 255, 0)); 
                        WiFi.softAP(settings->hostname);
                        state = CONNECTED;
                        lastStateSetAt = millis();
                    }
                    break;
                case DISCONNECTED:
                    // Do nothing.
                    break;
            }
        }

        void connect() {
            if (state != DISCONNECTED) {
                return;
            }
            WiFi.mode(WIFI_STA);
            _connect();
        }

        void disconnect() {
            if (state == DISCONNECTED) {
                return;
            }
            WiFi.mode(WIFI_OFF);
            state = DISCONNECTED;
            lastStateSetAt = millis();
        }

        bool isConnected() {
            return WiFi.status() == WL_CONNECTED && state == CONNECTED;
        }
    private:
        void _connect() {
            WiFi.disconnect();

            int networks = WiFi.scanNetworks();
            String strongestSSID = String("");
            int strongestSignalStrength = -1000;

            for (int i = 0; i < networks; i++) {
                int signalStrength = WiFi.RSSI(i);
                String ssid = WiFi.SSID(i);
                if (ssid.equals(settings->ssid)) {
                    if (strongestSignalStrength < signalStrength) {
                        logger->log("Found %s (%ddBm)", ssid.c_str(), signalStrength);
                        strongestSignalStrength = signalStrength;
                        strongestSSID = WiFi.SSID(i);
                    }
                }
            }

            logger->log("Hostname is %s", settings->hostname);

            if (strongestSSID.compareTo("") != 0) {
                WiFi.hostname(settings->hostname);
                WiFi.begin(strongestSSID.c_str(), settings->password);

                lastStateSetAt = millis();
                state = CONNECTING;
            } else {
                logger->log("%s not found, switching to AP mode", settings->ssid);

                // For debug purposes - switch to access mode.
                WiFi.softAPConfig(
                    IPAddress(192, 168, 0, 1),
                    IPAddress(192, 168, 0, 1),
                    IPAddress(255, 255, 255, 0)); 
                WiFi.softAP(settings->hostname);

                lastStateSetAt = millis();
                state = CONNECTED;
            }

            WiFi.scanDelete();
        }

        WiFiState state;
        unsigned long lastStateSetAt;

        Logger* logger = NULL;
        NetworkSettings* settings = NULL;
};
