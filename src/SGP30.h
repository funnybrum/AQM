#pragma once

#include "esp8266-base.h"
#include "Adafruit_SGP30.h"

extern BME280 bme280;

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

            _lastUpdate = millis() - _poolingInterval;
        }

        void loop() {
            if (millis() - _lastUpdate > _poolingInterval) {
                if (abs(_bme280_humidity - bme280.getHumidity()) > 1) {
                    _bme280_humidity = bme280.getHumidity();
                    if (!sgp30->setHumidity(_bme280_humidity)) {
                        logger.log("Failed on sgp30->setHumidity()!");
                    }
                }

                if (!sgp30->IAQmeasure()) {
                   logger.log("Failed on sgp30->IAQmeasure()!");
                }
                _lastUpdate = millis();
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
        unsigned long _poolingInterval;
        Adafruit_SGP30* sgp30;
        float _bme280_humidity = -1;
};