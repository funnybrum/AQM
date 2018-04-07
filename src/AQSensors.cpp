#include "AQMonitor.h"

AQSensors::AQSensors() {
    this->_lastRefresh = 0;
}

void AQSensors::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    _temperatureSensor.begin();
    // _micsvz89te.begin();
}

void AQSensors::loop() {
    if (millis() - _lastRefresh > 1000 * 10) {
        _lastRefresh = millis();
        _temperatureSensor.refresh();
        _micsvz89te.readSensor();
        _micsvz89te.getVersion();

        Serial.println();
        Serial.println();
        Serial.println();

        Serial.print("Temperature: ");
        Serial.print(_temperatureSensor.temperature);
        Serial.println("C");

        Serial.print("Humidity:    ");
        Serial.print(_temperatureSensor.humidity);
        Serial.println("%");

        Serial.print("Pressure:    ");
        Serial.print(_temperatureSensor.pressure  / 100.0F);
        Serial.println("hPa");

        Serial.print("VOC:         ");
        Serial.println(_micsvz89te.getVOC());

        Serial.print("CO2e:        ");
        Serial.println(_micsvz89te.getCO2());

        Serial.print("Status:      ");
        Serial.println(_micsvz89te.getStatus());

        Serial.print("Revision:    ");
        Serial.println(_micsvz89te.getRev());

        Serial.print("Year:        ");
        Serial.println(_micsvz89te.getYear());

        Serial.print("Month:       ");
        Serial.println(_micsvz89te.getMonth());

        Serial.print("Day:         ");
        Serial.println(_micsvz89te.getDay());
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
