#ifndef _AQ_SENSORS_H_
#define _AQ_SENSORS_H_

#include "AQMonitor.h"
#include <Wire.h>
#include <SparkFunBME280.h>
#include <MICS-VZ-89TE.h>

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
        BME280 _temperatureSensor;
        MICS_VZ_89TE _micsvz89te;
};

extern AQSensors aqSensors;

#endif
