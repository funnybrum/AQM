#ifndef _AQ_SENSORS_H_
#define _AQ_SENSORS_H_

#include "AQMonitor.h"
#include <Wire.h>
#include <BME280_t.h>

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
        BME280<> _bme280sensor;  
};

extern AQSensors aqSensors;

#endif
