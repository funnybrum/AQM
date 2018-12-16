#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Logger.h"

// struct SettingsData {
//     uint8_t checksum;
//     struct Network {
//         char hostname[64];
//         char ssid[32];
//         char password[32];
//     } network;
//     struct AQSensor {
//         uint8_t sensorCalibration[BSEC_MAX_STATE_BLOB_SIZE];
//         // Actual correction = value / 10. I.e. value of -23 is -2.3C. Same apply for humidity.
//         int16_t temperatureOffset;
//         int16_t humidityOffset;
//         int16_t badAQResistance;
//         int16_t goodAQResistance;
//         uint16_t calibrationPeriod;
//     } aqSensor;

//     struct LED {
//         uint16_t blinkInterval;
//     } led;

//     struct InfluxDB {
//         bool enable;
//         char address[64];
//         char database[16];
//         uint16_t pushInterval;
//         uint16_t collectInterval;
//     } influxDB;
// };

extern Logger logger;

class Settings {
    public:
        Settings(Logger logger,
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
                *(((uint8_t*)&this->settingsData) + i) = EEPROM.read(i+1);
            }
            EEPROM.end();

            if (checksum == settingsSize % 256) {
                logger.log("Settings loaded successfully.");
            } else {
                logger.log("Invalid settings checksum.");
                erase();
            }
        }

        void loop() {
        }

        void save();

        void erase() {
            logger.log("Erasing EEPROM.");
            memset(this->settingsData, 0, settingsSize);
            initFunction();
            this->writeToEEPROM();
        }

    private:
        void writeToEEPROM() {
            EEPROM.begin(settingsSize+1);
            EEPROM.write(0, settingsSize % 256);
            for (unsigned int i = 0; i < settingsSize; i++) {
                EEPROM.write(i+1, *(((uint8_t*)&this->settingsData) + i));
            }
            EEPROM.end();
        }

        Logger logger;
        void* settingsData;
        int16_t settingsSize;
        void (*initFunction)(void);
};

extern Settings settings;

#endif
