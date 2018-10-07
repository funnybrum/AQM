#include "AQMonitor.h"

void SystemCheck::begin() {
    lastWebCall = millis();
    lastWiFiConnectedState = millis();
}

void SystemCheck::loop() {
    if (WiFi.status() == WL_CONNECTED) {
        lastWiFiConnectedState = millis();
    }

    if (hasTimeoutOccur(lastWebCall, 600)) {
        Serial.println("Reseting based on the lastWebCall timestamp!");
        ESP.reset();
    }

    if (hasTimeoutOccur(lastWiFiConnectedState, 120)) {
        Serial.println("Reseting based on the lastWiFiConnectedState timestamp!");
        ESP.reset();
    }
}

void SystemCheck::registerWebCall() {
    lastWebCall = millis();
}

bool SystemCheck::hasTimeoutOccur(unsigned long timer, unsigned int timeoutSeconds) {
    unsigned long elapsedMillis = millis() - timer;
    return elapsedMillis >= timeoutSeconds * 1000;
}

SystemCheck systemCheck = SystemCheck();