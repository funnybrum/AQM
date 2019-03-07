#pragma once

#include "esp8266-base.h"
#include "Adafruit_BME280.h"

class BME280 {
    public:
        BME280(int poolingInterval = 1000, int16_t* temp_offset = NULL) {
            this->bme280 = new Adafruit_BME280();
            this->_tempOffset = temp_offset;
            this->_poolingInterval = poolingInterval;
        }

        void begin() {
            if (!bme280->begin()) {
               logger.log("Could not find the BME280 sensor!");
            }

            bme280->setSampling(
                Adafruit_BME280::MODE_NORMAL,
                Adafruit_BME280::SAMPLING_X4,
                Adafruit_BME280::SAMPLING_X4,
                Adafruit_BME280::SAMPLING_X1,
                Adafruit_BME280::FILTER_OFF,
                Adafruit_BME280::STANDBY_MS_1000);

            // Schedule the next data collect to be after 0.5 second before.
            _nextDataRead = millis() + 500;
        }

        void loop() {
            if (_nextDataRead < millis()) {
                bool reset = false;

                float t = bme280->readTemperature();
                float h = bme280->readHumidity();

                reset = (_temperature != 0.0f) && (abs(t - _temperature) > 5 || abs(h - _humidity) > 5);

                if (reset) {
                    logger.log("BME280 reset -> t %0.2f to %0.2f, h %0.2f to 0.2f",
                                _temperature,
                                t,
                                _humidity,
                                h);
                    _temperature = 0.0f;
                    _humidity = 0.0f;
                    bme280->init();
                    delay(1000);
                } else {
                    _temperature = bme280->readTemperature();
                    _humidity = bme280->readHumidity();
                    _pressure = bme280->readPressure();
                }

                if (reset) {
                    logger.log("BME280 reset -> t %0.2f, h %0.2f", _temperature, _humidity);
                }

                _nextDataRead += _poolingInterval;
            }
        }

        float getTemperature() {
            float result = _temperature;
            if (_tempOffset != NULL) {
                result += *_tempOffset / 10;
            }
            return result;
        }

        float getHumidity() {
            if (_tempOffset == NULL) {
                return _humidity;
            }

            // Binary search for the corrected himidity
            float correctedTemp = getTemperature();
            float l = 0.0f;
            float r = 100.0f;
            float ah = getAbsoluteHimidity();

            // 10 steps guarantee 0.1 precision, but let's go a bit further.
            for (int i = 0; i < 20; i++) {
                float m = l + (r-l) / 2;
                float ah_at_m = rhToAh(m, correctedTemp);
                if ( ah_at_m < ah) {
                    l = m;
                } else if (ah_at_m > ah) {
                    r = m;
                } else {
                    break;
                }
            }

            return l + (r-l) / 2;
        }

        float getPressure() {
            return _pressure;
        }

        float getAbsoluteHimidity() {
            return rhToAh(_humidity, _temperature);
        }

    private:
        float rhToAh(float rh, float temp) {
            // https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
            double p_sat = 6.112 * pow(EULER, (17.67 * temp) / (temp + 243.5));
            return (p_sat * rh * 2.167428434) / (273.15 + temp);
        }

        unsigned long _nextDataRead;
        unsigned long _poolingInterval;
        Adafruit_BME280* bme280;

        float _temperature = 0.0f;
        float _humidity = 0.0f;
        float _pressure = 0.0f;

        int16_t* _tempOffset;
};