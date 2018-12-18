#pragma once

#include "Logger.h"

class SystemCheck {
    public:
        SystemCheck(Logger* logger) {
            this->logger = logger;
        }

        void begin() {
            lastWebCall = millis();
            lastWiFiConnectedState = millis();
        }

        void loop() {
            if (!enabled) {
                return;
            }

            if (WiFi.status() == WL_CONNECTED) {
                lastWiFiConnectedState = millis();
            }

            if (hasTimeoutOccur(lastWebCall, 600)) {
                logger->log("Reseting based on the lastWebCall timestamp!");
                ESP.reset();
            }

            if (hasTimeoutOccur(lastWiFiConnectedState, 120)) {
                logger->log("Reseting based on the lastWiFiConnectedState timestamp!");
                ESP.reset();
            }
        }

        void registerWebCall() {
            lastWebCall = millis();
        }

        void start() {
            enabled = true;
        }

        void stop() {
            enabled = false;
        }

    private:
        bool hasTimeoutOccur(unsigned long timer, unsigned  int timeoutSeconds) {
            unsigned long elapsedMillis = millis() - timer;
            return elapsedMillis >= timeoutSeconds * 1000;
        }

        unsigned long lastWebCall;
        unsigned long lastWiFiConnectedState;
        bool enabled = true;

        Logger* logger = NULL;
};
