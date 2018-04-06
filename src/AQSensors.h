#ifndef _AQ_SENSORS_H_
#define _AQ_SENSORS_H_

#include "AQMonitor.h"

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
        bool _isI2CInitialized;
};

extern AQSensors aqSensors;

#endif
