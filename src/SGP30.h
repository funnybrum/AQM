#pragma once

#include "esp8266-base.h"
#include "Settings.h"
#include "Adafruit_SGP30.h"

extern BME280 bme280;
extern SettingsData settingsData;

class SGP30 {
    public:
        SGP30(int poolingInterval = 1000) {
            this->_sgp30 = new Adafruit_SGP30();
            this->_poolingInterval = poolingInterval;
        }

        void begin() {
            if (!_sgp30->begin()) {
               logger.log("Could not find the SGP30 sensor!");
            }

            if (!_sgp30->IAQinit()) {
               logger.log("Failed on _sgp30->IAQinit()!");
            }

            // Schedule the next data collect to be after 1 second.
            _nextDataRead = millis() + 1000;

            // Schedule next baseline read after 12 hours.
            _nextBaselineRead = millis() + 3600L * 1000L * 12L;


            if (settingsData.aqSensor.eco2_base > 0 || settingsData.aqSensor.tvoc_base > 0) {
                if (!_sgp30->setIAQBaseline(settingsData.aqSensor.eco2_base, settingsData.aqSensor.tvoc_base)) {
                    logger.log("Failed on _sgp30->setIAQBaseline()!");
                } else {
                    // Baseline is restored, next baseline read should be in 1 hour.
                    _nextBaselineRead = millis() + 3600L * 1000L;
                    logger.log("SGP30 calibration data applied");
                }
            }            
        }

        void loop() {
            if (_nextDataRead < millis()) {
                if (abs(_bme280_humidity - bme280.getHumidity()) > 0.25) {
                    _bme280_humidity = bme280.getHumidity();
                    if (!_sgp30->setHumidity(bme280.getAbsoluteHimidity() * 1000)) {
                        logger.log("Failed on _sgp30->setHumidity()!");
                    }
                }

                if (!_sgp30->IAQmeasure()) {
                   logger.log("Failed on _sgp30->IAQmeasure()!");
                }

                _nextDataRead += _poolingInterval;
            }

            if (_nextBaselineRead < millis()) {
                if (_sgp30->getIAQBaseline(&settingsData.aqSensor.eco2_base,
                                           &settingsData.aqSensor.tvoc_base)) {
                    logger.log("eco2_base: %u (%u, %u), tvoc_base: %u (%u, %u)",
                               settingsData.aqSensor.eco2_base,
                               settingsData.aqSensor.eco2_base / 256,
                               settingsData.aqSensor.eco2_base % 256,
                               settingsData.aqSensor.tvoc_base,
                               settingsData.aqSensor.tvoc_base / 256,
                               settingsData.aqSensor.tvoc_base % 256);
                    _nextBaselineRead += 3600L * 1000L;
                } else {
                    // Next attempt to save settings will be after 10 seconds;
                    _nextBaselineRead += 10L * 1000L;
                    logger.log("Failed on _sgp30->getIAQBaseline()");
                }
            }
        }

        float getTVOC() {
            return _sgp30->TVOC;
        }

        float geteCO2() {
            return _sgp30->eCO2;
        }

    private:
        unsigned long _nextDataRead;
        unsigned long _nextBaselineRead;
        unsigned long _poolingInterval;
        Adafruit_SGP30* _sgp30;
        float _bme280_humidity = -1;
};