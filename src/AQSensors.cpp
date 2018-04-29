#include "AQMonitor.h"

AQSensors::AQSensors() {
    this->_lastRefresh = 0;
}

void AQSensors::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);

    uint8_t rslt = 1;
    Serial.println();
    while(rslt != 0) {
        rslt = _bme.begin();
        if(rslt != 0) {
            Serial.println("bme begin failure");
            delay(2000);
        }
    }
    Serial.println("bme begin successful");
    _bme.supportIAQ();
    _bme.startConvert();
}

void AQSensors::loop() {
    if (millis() - _lastRefresh > 1000 * 10) {
        _lastRefresh = millis();

        if(_calibrated == 0) {
            if(_bme.iaqUpdate() == 0) {
                _seaLevel = _bme.readSeaLevel(627.0);
                _calibrated = 1;
            }
        }

        if(_calibrated) {
            if (_bme.iaqUpdate() == 0) {
                _temp = _bme.readTemperature();
                _pressure = _bme.readPressure();
                _humidity = _bme.readHumidity();

                Serial.print("timestamp(sec): ");
                Serial.println();
                Serial.println(millis()/1000);
                Serial.print("temperature(C): ");
                Serial.println(_temp, 2);
                Serial.print("pressure(Pa): ");
                Serial.println(_pressure);
                Serial.print("humidity(%rh): ");
                Serial.println(_humidity, 2);
                Serial.print("calibrated altitude(m): ");
                Serial.println(_bme.readCalibratedAltitude(_seaLevel));
                Serial.print("IAQ Accuracy: ");
                Serial.println(_bme.readIAQAccuracy());
                if(_bme.isIAQReady() == 0) {
                    _voc = _bme.readIAQ();
                    Serial.print("IAQ: ");
                    Serial.print(_voc);
                    if(_voc < 50)
                        Serial.println(" good");
                    else if(_voc < 100)
                        Serial.println(" average");
                    else if(_voc < 150)
                        Serial.println(" little bad");
                    else if(_voc < 200)
                        Serial.println(" bad");
                    else if(_voc < 300)
                        Serial.println(" worse");
                    else
                        Serial.println(" very bad");
                } else {
                    Serial.print("IAQ not ready, please wait about ");
                    Serial.print((int)(305000-millis())/1000);
                    Serial.println(" seconds");
                }                
            }
        }

        // if (_lastRefresh == 0) {


        // _temp = bme680.readTemperature();
        // _humidity = bme680.readHumidity();
        // _pressure = bme680.readPressure();
        // _voc = bme680.readGasResistance();
        // _co2e = 0.0f;
    }
}

float AQSensors::getCO2e() {
    return this->_co2e;
}

float AQSensors::getVOC() {
    return this->_voc;
}

float AQSensors::getHumidity() {
    return this->_humidity;
}

float AQSensors::getTemp() {
    return this->_temp;
}

float AQSensors::getPressure() {
    return this->_pressure;
}

AQSensors aqSensors = AQSensors();
