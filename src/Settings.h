#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "AQMonitor.h"

struct SettingsData {
    uint8_t checksum;
    uint8_t sensorCalibration[BSEC_MAX_STATE_BLOB_SIZE];
    // Actual correction = value / 10. I.e. value of -23 is -2.3C. Same apply for humidity.
    int8_t temperatureOffset;
    int8_t humidityOffset;
    char hostname[64];
    uint16_t blinkInterval;
    int16_t badAQResistance;
    int16_t goodAQResistance;

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
