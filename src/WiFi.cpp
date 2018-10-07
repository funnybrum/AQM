#include "AQMonitor.h"
#include "Secrets.h"

void ScanAndConnect() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    int networks = WiFi.scanNetworks();

    Serial.printf("Found %d networks\n", networks);

    String strongestSSID = String("");
    int strongestSignalStrength = -1000;

    for (int i = 0; i < networks; i++) {
        int signalStrength = WiFi.RSSI(i);
        String ssid = WiFi.SSID(i);
        if (strongestSignalStrength < signalStrength) {
            for (unsigned int j = 0; j < sizeof(WIFI_SSIDs)/sizeof(WIFI_SSIDs[0]); j++) {
                if (ssid.equals(WIFI_SSIDs[j])) {
                    strongestSignalStrength = signalStrength;
                    strongestSSID = WiFi.SSID(i);
                }
            }
        }
    }

    if (strongestSSID.compareTo("") != 0) {
        Serial.printf("Connectiong to %s (%ddBm)\n", strongestSSID.c_str(), strongestSignalStrength);

        const char* hostname;
        
        if (strlen(settings.get()->hostname) > 1) {
            hostname = settings.get()->hostname;    
        } else {
            hostname = HOSTNAME;
        }
        Serial.print("Hostname is ");
        Serial.println(hostname);

        WiFi.hostname(hostname);
        WiFi.begin(strongestSSID.c_str(), WIFI_PASSWORD.c_str());

        // Timeout after 30 seconds of connection attempts.
        int timeout = 60; // 60 * 0.5 = 30 seconds. 
        while (WiFi.status() != WL_CONNECTED and timeout > 0) {
            timeout--;
            delay(500);
            Serial.print(".");
        }

        Serial.println();

        if (timeout > 0) {
            Serial.print("Connected, ip address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.print("Failed to connect in 30 seconds. Please, check the provided password.");
        }
    } else {
        Serial.println("No known network found...");
    }

    WiFi.scanDelete();
}