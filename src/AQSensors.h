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
        float getPressure();
        float getGasResistance();
        uint8_t getAccuracy();
        float getIAQ();
        float getStaticIAQ();

    private:
        float _temp = 0.0f;
        float _humidity = 0.0f;
        float _pressure = 0.0f;
        float _gas_resistance = 0.0f;

        uint8_t _accuracy = 0;
        float _iaq = 0.0f;
        float _static_iaq = 0.0f;

        unsigned long _lastStateUpdate;

        Bsec _iaqSensor = Bsec();

        // Helper function definitions
        void checkIaqSensorStatus(void);
        void loadState(void);
        void updateState(void);
};

extern AQSensors aqSensors;

#endif
