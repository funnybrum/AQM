#pragma once

#include "esp8266-base.h"
#include "Adafruit_BME280.h"

class BME280 {
    public:
        BME280(int poolingInterval = 1000) {
            this->bme280 = new Adafruit_BME280();
        }

        void begin() {
            bool status = bme280->begin();
            if (!status) {
               logger.log("Could not find the BME280 sensor!");
            }

            _lastUpdate = millis() - _poolingInterval;
        }

        void loop() {
            if (millis() - _lastUpdate > _poolingInterval) {
                _temperature = bme280->readTemperature();
                _humidity = bme280->readHumidity();
                _pressure = bme280->readPressure();
                _lastUpdate = millis();
            }
        }

        float getTemperature() {
            return _temperature;
        }

        float getHumidity() {
            return _humidity;
        }

        float getPressure() {
            return _pressure;
        }

    private:
        unsigned long _lastUpdate;
        unsigned long _poolingInterval;
        Adafruit_BME280* bme280;

        float _temperature = 0.0f;
        float _humidity = 0.0f;
        float _pressure = 0.0f;
};