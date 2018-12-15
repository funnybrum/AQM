#include "AQMonitor.h"
#include "Secrets.h"

void WiFiManager::begin() {
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    state = DISCONNECTED;
    lastStateSetAt = millis();
}

void WiFiManager::loop() {
    switch(state) {
        case CONNECTED:
            // if (millis() - lastStateSetAt > 10000) {
            //     disconnect();
            // }
            break;
        case CONNECTING:
            if (WiFi.status() == WL_CONNECTED) {
                logger.log("Connected in %.1f seconds!", (millis() - lastStateSetAt)/1000.0f);
                state = CONNECTED;
                lastStateSetAt = millis();
            } else if (millis() - lastStateSetAt > 30000) {
                logger.log("Failed to connect in 30 seconds. Switching to access point.");

                // For debug purposes - switch to access mode.
                WiFi.softAPConfig(
                    IPAddress(192, 168, 0, 1),
                    IPAddress(192, 168, 0, 1),
                    IPAddress(255, 255, 255, 0)); 
                WiFi.softAP(settings.get()->network.hostname);
                state = CONNECTED;
            }
            break;
        case DISCONNECTED:
            // if (millis() - lastStateSetAt > 10000) {
            //     connect();
            // }
            break;
    }
}

void WiFiManager::connect() {
    if (state != DISCONNECTED) {
        return;
    }
    WiFi.mode(WIFI_STA);
    _connect();
}

void WiFiManager::disconnect() {
    if (state == DISCONNECTED) {
        return;
    }
    WiFi.mode(WIFI_OFF);
    state = DISCONNECTED;
    lastStateSetAt = millis();
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED && state == CONNECTED;
}

void WiFiManager::_connect() {
    WiFi.disconnect();

    int networks = WiFi.scanNetworks();
    String strongestSSID = String("");
    int strongestSignalStrength = -1000;

    for (int i = 0; i < networks; i++) {
        int signalStrength = WiFi.RSSI(i);
        String ssid = WiFi.SSID(i);
        for (unsigned int j = 0; j < sizeof(WIFI_SSIDs)/sizeof(WIFI_SSIDs[0]); j++) {
            if (ssid.equals(WIFI_SSIDs[j])) {
                if (strongestSignalStrength < signalStrength) {
                    logger.log("Found %s (%ddBm)", ssid.c_str(), signalStrength);
                    strongestSignalStrength = signalStrength;
                    strongestSSID = WiFi.SSID(i);
                }
            }
        }
    }

    logger.log("Hostname is %s", settings.get()->network.hostname);

    if (strongestSSID.compareTo("") != 0) {
        logger.log("Connecting to %s", strongestSSID.c_str());
        WiFi.hostname(settings.get()->network.hostname);
        WiFi.begin(strongestSSID.c_str(), WIFI_PASSWORD.c_str());

        state = CONNECTING;
        lastStateSetAt = millis();
    } else {
        logger.log("No known network found. Switching to access point.");

        // For debug purposes - switch to access mode.
        WiFi.softAPConfig(
            IPAddress(192, 168, 0, 1),
            IPAddress(192, 168, 0, 1),
            IPAddress(255, 255, 255, 0)); 
        WiFi.softAP(settings.get()->network.hostname);
    }

    WiFi.scanDelete();

    Serial.printf("Free heap is %d\n", ESP.getFreeHeap());
}

WiFiManager wifi = WiFiManager();
