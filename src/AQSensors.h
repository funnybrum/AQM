#ifndef _AQ_SENSORS_H_
#define _AQ_SENSORS_H_

#include "AQMonitor.h"
#include <Wire.h>
#include "DFRobot_BME680_I2C.h"

class AQSensors {
    public:
        AQSensors();
        void begin();
        void loop();
        float getTemp();
        float getHumidity();
        float getVOC();
        float getCO2e();
        float getPressure();
    private:
        float _temp = 0.0f;
        float _humidity = 0.0f;
        float _voc = 0.0f;
        float _co2e = 0.0f;
        float _pressure = 0.0f;
        unsigned long _lastRefresh;
        uint8_t _calibrated = 0;
        float _seaLevel;
        DFRobot_BME680_I2C _bme = DFRobot_BME680_I2C(0x77);
};

extern AQSensors aqSensors;

#endif
