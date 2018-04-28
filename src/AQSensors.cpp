#include "AQMonitor.h"

AQSensors::AQSensors() {
    this->_lastRefresh = 0;
}

void AQSensors::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);

    _temperatureSensor.setI2CAddress(0x76);
    _temperatureSensor.begin();
    _temperatureSensor.setMode(MODE_SLEEP);
    // _micsvz89te.begin();
}

void AQSensors::loop() {
    if (millis() - _lastRefresh > 1000 * 30) {
        // if (_lastRefresh == 0) {
        //     Serial.print("status");
        //     Serial.print("\t");
        //     Serial.print("ts");
        //     Serial.print("\t");
        //     Serial.print("temp");
        //     Serial.print("\t");
        //     Serial.print("humidity");
        //     Serial.print("\t");
        //     Serial.print("pressure");
        //     Serial.print("\t");
        //     Serial.print("voc");
        //     Serial.print("\t");
        //     Serial.print("co2e");
        //     Serial.print("\t");
        //     Serial.println();
        // }
        _lastRefresh = millis();

        _temperatureSensor.setMode(MODE_FORCED);
        while (_temperatureSensor.isMeasuring() == false) {
            delay(1);
        } //Wait for sensor to start measurment

        while (_temperatureSensor.isMeasuring() == true) {
            delay(1);
        } //Hang out while sensor completes the reading    

        _temp = _temperatureSensor.readTempC();
        _humidity = _temperatureSensor.readFloatHumidity();
        _pressure = _temperatureSensor.readFloatPressure();

        for (int i = 0; i < 3; i++) {
            if (_micsvz89te.readSensor()) {
                _voc = _micsvz89te.getVOC();
                _co2e = _micsvz89te.getCO2();
                break;
            } else {
                // Serial.println("Failed to read the VOC sensor");
                delay(100);
            }    
        }

        // Serial.print(millis() / 30000L);
        // Serial.print("\t");

        // Serial.print(_temperatureSensor.readTempC(), 2);
        // Serial.print("\t");

        // Serial.print(_temperatureSensor.readFloatHumidity(), 1);
        // Serial.print("\t");

        // Serial.print(_temperatureSensor.readFloatPressure(), 1);
        // Serial.print("\t");

        // Serial.print(_micsvz89te.getVOC());
        // Serial.print("\t");

        // Serial.print(_micsvz89te.getCO2());
        // Serial.print("\t");

        // Serial.println();
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
