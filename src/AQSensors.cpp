#include "AQMonitor.h"

AQSensors::AQSensors() {
    this->_lastRefresh = 0;
}

void AQSensors::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    _bme280sensor.begin();
}

void AQSensors::loop() {
    if (millis() - _lastRefresh > 1000 * 5) {
        _lastRefresh = millis();
        _bme280sensor.refresh();

        Serial.print("Temperature: ");
        Serial.print(_bme280sensor.temperature);
        Serial.println("C");

        Serial.print("Humidity:    ");
        Serial.print(_bme280sensor.humidity);
        Serial.println("%");

        Serial.print("Pressure:    ");
        Serial.print(_bme280sensor.pressure  / 100.0F);
        Serial.println("hPa");
    }
}

float AQSensors::getCO2e() {
    return 1.0f;
}

float AQSensors::getVOC() {
    return 1.0f;
}

float AQSensors::getHumidity() {
    return 1.0f;
}

float AQSensors::getTemp() {
    return 1.0f;
}

AQSensors aqSensors = AQSensors();
