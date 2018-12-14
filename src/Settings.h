#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "AQMonitor.h"

struct SettingsData {
    uint8_t checksum;
    struct Network {
        char hostname[64];
        char ssid[32];
        char password[32];
    } network;
    struct AQSensor {
        uint8_t sensorCalibration[BSEC_MAX_STATE_BLOB_SIZE];
        // Actual correction = value / 10. I.e. value of -23 is -2.3C. Same apply for humidity.
        int16_t temperatureOffset;
        int16_t humidityOffset;
        int16_t badAQResistance;
        int16_t goodAQResistance;
        uint16_t calibrationPeriod;
    } aqSensor;

    struct LED {
        uint16_t blinkInterval;
    } led;

    struct InfluxDB {
        bool enable;
        char address[64];
        char database[16];
        uint16_t pushInterval;
        uint16_t collectInterval;
    } influxDB;
};

#define DATA_SIZE sizeof(SettingsData)
// Temporary replacement for the calculated checksum to allow lossless migration.
#define TEMP_CHECKSUM_VAL BSEC_MAX_STATE_BLOB_SIZE

class Settings {
    public:
        void begin();
        void loop();
        void save();
        void erase();
        /**
         * Get the stored settings. If there was no saved settings the result
         * will be initialized with zeroes.
         */
        SettingsData* get();
    private:
        SettingsData data;
        void writeToEEPROM();
};

extern Settings settings;

#endif
