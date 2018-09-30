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
};

#define DATA_SIZE sizeof(SettingsData)
// Temporary replacement for the calculated checksum to allow lossless migration.
#define TEMP_CHECKSUM_VAL BSEC_MAX_STATE_BLOB_SIZE + 1

class Settings {
    public:
        void begin();
        void loop();
        void save();
        void erase();
        SettingsData* get();
        bool isDataValid();
    private:
        SettingsData data;
};

extern Settings settings;

#endif
