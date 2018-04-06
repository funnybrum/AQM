#include "AQMonitor.h"

AQSensors::AQSensors() {
    this->_isI2CInitialized = false;
}

void AQSensors::begin() {
}

void AQSensors::loop() {
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
