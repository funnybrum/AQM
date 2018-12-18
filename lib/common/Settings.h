#pragma once

#include "Logger.h"
#include "Arduino.h"

extern Logger logger;

class Settings {
    public:
        Settings(Logger* logger,
                 void* settingsData,
                 int16_t settingsSize,
                 void (*initFunction)(void)) {
            this->logger = logger;
            this->settingsData = settingsData;
            this->settingsSize = settingsSize;
            this->initFunction = initFunction;
        }

        void begin() {
            EEPROM.begin(settingsSize+1);

            uint8_t checksum = EEPROM.read(0);
            for (unsigned int i = 0; i < settingsSize; i++) {
                *(((uint8_t*)this->settingsData) + i) = EEPROM.read(i+1);
            }
            EEPROM.end();

            if (checksum == settingsSize % 256) {
                logger->log("Settings loaded successfully.");
            } else {
                logger->log("Invalid settings checksum.");
                erase();
            }
        }

        void loop() {
        }

        void save() {
            logger->log("Writing state to EEPROM.");
            writeToEEPROM();
        }

        void erase() {
            logger->log("Erasing EEPROM.");
            memset(this->settingsData, 0, settingsSize);
            initFunction();
            writeToEEPROM();
        }

    private:
        void writeToEEPROM() {
            EEPROM.begin(settingsSize+1);
            EEPROM.write(0, settingsSize % 256);
            for (unsigned int i = 0; i < settingsSize; i++) {
                EEPROM.write(i+1, *(((uint8_t*)this->settingsData) + i));
            }
            EEPROM.end();
        }

        Logger* logger = NULL;
        void* settingsData = NULL;
        uint16_t settingsSize = 0;
        void (*initFunction)(void) = NULL;
};
