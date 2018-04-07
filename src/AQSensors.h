#ifndef _AQ_SENSORS_H_
#define _AQ_SENSORS_H_

#include "AQMonitor.h"
#include <Wire.h>
#include <BME280_t.h>
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
    private:
        unsigned long _lastRefresh;
        BME280<> _temperatureSensor;
        MICS_VZ_89TE _micsvz89te;
};

extern AQSensors aqSensors;

#endif
