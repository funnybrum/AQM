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
            if (isConnected()) {
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
                WiFi.softAP(settings.get()->hostname);
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
    Serial.println("Connecting...");
    WiFi.mode(WIFI_STA);
    _connect();
}

void WiFiManager::disconnect() {
    if (state == DISCONNECTED) {
        return;
    }
    Serial.println("Disconnecting...");
    WiFi.mode(WIFI_OFF);
    state = DISCONNECTED;
    lastStateSetAt = millis();
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::_connect() {
    WiFi.disconnect();

    int networks = WiFi.scanNetworks();
    logger.log("Found %d networks", networks);

    String strongestSSID = String("");
    int strongestSignalStrength = -1000;

    for (int i = 0; i < networks; i++) {
        int signalStrength = WiFi.RSSI(i);
        String ssid = WiFi.SSID(i);
        logger.log("Found %s (%ddBm)", ssid.c_str(), signalStrength);
        if (strongestSignalStrength < signalStrength) {
            for (unsigned int j = 0; j < sizeof(WIFI_SSIDs)/sizeof(WIFI_SSIDs[0]); j++) {
                if (ssid.equals(WIFI_SSIDs[j])) {
                    strongestSignalStrength = signalStrength;
                    strongestSSID = WiFi.SSID(i);
                }
            }
        }
    }

    const char* hostname;    
    if (strlen(settings.get()->hostname) > 1) {
        hostname = settings.get()->hostname;    
    } else {
        hostname = HOSTNAME;
    }

    logger.log("Hostname is %s", hostname);

    if (strongestSSID.compareTo("") != 0) {
        logger.log("Connecting to %s (%ddBm)", strongestSSID.c_str(), strongestSignalStrength);

        WiFi.hostname(hostname);
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
        WiFi.softAP(hostname);
    }

    WiFi.scanDelete();
}

WiFiManager wifi = WiFiManager();
