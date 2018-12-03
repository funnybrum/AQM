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
        float getCalculatedIAQ();
        float getPressure();
        float getGasResistance();
        uint8_t getIAQAccuracy();
        float getIAQ();

        uint8_t getStaticIaqAccuracy();
        float getStaticIaq();
        uint8_t getCo2Accuracy();
        float getCo2Equivalent();
        uint8_t getBreathVocAccuracy();
        float getBreathVocEquivalent();
        uint8_t getCompGasAccuracy();
        float getCompGasValue();
        uint8_t getGasPercentageAcccuracy();
        float getGasPercentage();
    private:
        float _temp = 0.0f;
        float _humidity = 0.0f;
        float _pressure = 0.0f;
        float _gas_resistance = 0.0f;
        float _calculated_iaq = 0.0f;

        float _iaq = 0.0f;
        uint8_t _iaq_accuracy = 0;

        // New outputs provided by BSEC version 1.4.7.1
        uint8_t _static_iaq_accuracy = 0;
        float _static_iaq = 0.0f;
        uint8_t _co2_accuracy = 0;
        float _co2_equivalent = 0.0f;
        uint8_t _breath_voc_accuracy = 0;
        float _breath_voc_equivalent = 0.0f;
        uint8_t _comp_gas_accuracy = 0;
        float _comp_gas_value = 0.0f;
        uint8_t _gas_percentage_acccuracy = 0;
        float _gas_percentage = 0.0f;

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
