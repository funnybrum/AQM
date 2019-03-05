#pragma once

#include "esp8266-base.h"

struct SettingsData {
    NetworkSettings network;
    struct AQSensor {
        // uint8_t sensorCalibration[BSEC_MAX_STATE_BLOB_SIZE];
        // Actual correction = value / 10. I.e. value of -23 is -2.3C. Same apply for humidity.
        int16_t temperatureOffset;
        uint16_t eco2_base;
        uint16_t tvoc_base;
    } aqSensor;

    struct LED {
        uint16_t blinkInterval;
    } led;

    struct InfluxDBCollectorSettings influxDB;
};
