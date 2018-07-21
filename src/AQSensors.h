#ifndef _AQ_SENSORS_H_
#define _AQ_SENSORS_H_

#include "AQMonitor.h"
#include <Wire.h>
#include <EEPROM.h>
#include "bsec.h"

class AQSensors {
    public:
        AQSensors();
        void begin();
        void loop();
        float getTemp();
        float getHumidity();
        float getIAQ();
        float getIAQAccuracy();
        float getPressure();
    private:
        float _temp = 0.0f;
        float _humidity = 0.0f;
        float _iaq = 0.0f;
        float _iaq_accuracy = 0.0f;
        float _pressure = 0.0f;
        unsigned long _lastRefresh;
        unsigned long _lastStateUpdate;

        Bsec _iaqSensor = Bsec();
        String _output;

        // Helper function definitions
        void checkIaqSensorStatus(void);
        void loadState(void);
        void updateState(void);
};

extern AQSensors aqSensors;

#endif
