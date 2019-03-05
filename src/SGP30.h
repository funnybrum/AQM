#pragma once

#include "esp8266-base.h"
#include "Settings.h"
#include "Adafruit_SGP30.h"

extern BME280 bme280;
extern SettingsData settingsData;

class SGP30 {
    public:
        SGP30(int poolingInterval = 1000) {
            this->sgp30 = new Adafruit_SGP30();
        }

        void begin() {
            if (!sgp30->begin()) {
               logger.log("Could not find the SGP30 sensor!");
            }

            if (!sgp30->IAQinit()) {
               logger.log("Failed on sgp30->IAQinit()!");
            }

            if (settingsData.aqSensor.eco2_base > 0 || settingsData.aqSensor.tvoc_base > 0) {
                if (!sgp30->setIAQBaseline(settingsData.aqSensor.eco2_base, settingsData.aqSensor.tvoc_base)) {
                    logger.log("Failed on sgp30->setIAQBaseline()!");
                } else {
                    logger.log("SGP30 calibration data applied");
                }
            }
            
            // Schedule the next data collect to be after 1 second.
            _lastUpdate = millis() - _poolingInterval + 1000;

            _lastSaveState = millis();
        }

        void loop() {
            if (millis() - _lastUpdate > _poolingInterval) {
                if (abs(_bme280_humidity - bme280.getHumidity()) > 0.25) {
                    _bme280_humidity = bme280.getAbsoluteHimidity();
                    if (!sgp30->setHumidity(_bme280_humidity)) {
                        logger.log("Failed on sgp30->setHumidity()!");
                    }
                }

                if (!sgp30->IAQmeasure()) {
                   logger.log("Failed on sgp30->IAQmeasure()!");
                }
                _lastUpdate = millis();
            }

            if (millis() - _lastSaveState > 4 * 3600 * 1000) {
                if (sgp30->getIAQBaseline(&settingsData.aqSensor.eco2_base, &settingsData.aqSensor.tvoc_base)) {
                    _lastSaveState = millis();
                    logger.log("eco2_base: %u (%u, %u), tvoc_base: %u (%u, %u)",
                               settingsData.aqSensor.eco2_base,
                               settingsData.aqSensor.eco2_base / 256,
                               settingsData.aqSensor.eco2_base % 256,
                               settingsData.aqSensor.tvoc_base,
                               settingsData.aqSensor.tvoc_base / 256,
                               settingsData.aqSensor.tvoc_base % 256);
                } else {
                    // Next attempt to save settings will be after 10 seconds;
                    _lastSaveState+=10 * 1000;

                    logger.log("Failed on sgp30->getIAQBaseline()");
                }
            }
        }

        float getTVOC() {
            return sgp30->TVOC;
        }

        float geteCO2() {
            return sgp30->eCO2;
        }

    private:
        unsigned long _lastUpdate;
        unsigned long _lastSaveState;
        unsigned long _poolingInterval;
        Adafruit_SGP30* sgp30;
        float _bme280_humidity = -1;
};