#include "AQMonitor.h"

AQSensors::AQSensors() {
    _lastRefresh = 0;
    _lastStateUpdate = 0;
}

void AQSensors::begin() {
    EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1); // 1st address for the length
    Wire.begin(I2C_SDA, I2C_SCL);

    // Init the sensor
    _iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);

    _output = "\nBSEC library version " +
        String(_iaqSensor.version.major) +
        "." +
        String(_iaqSensor.version.minor) +
        "." + String(_iaqSensor.version.major_bugfix) +
        "." + String(_iaqSensor.version.minor_bugfix);
    Serial.println(_output);

    // As per the documentation - 'The default configuration (after calling bsec_init), to which
    // BSEC will be configured, is "generic_18v_300s_4d"'. However this firmware is for PCB that
    // works on 3.3V, so - the default config is overridden with the generic_33v_300s_4d one.
    const uint8_t generic_33v_300s_4d[BSEC_MAX_PROPERTY_BLOB_SIZE] = {
        0,6,4,1,61,0,0,0,0,0,0,0,24,1,0,0,40,0,1,0,137,65,0,63,0,0,64,63,205,204,76,62,0,0,225,68,
        0,192,168,71,0,0,0,0,0,80,10,90,0,0,0,0,0,0,0,0,21,0,2,0,0,244,1,225,0,25,10,144,1,0,0,112,
        65,0,0,0,63,16,0,3,0,10,215,163,60,10,215,35,59,10,215,35,59,9,0,5,0,0,0,0,0,1,51,0,9,0,
        10,215,163,59,205,204,204,61,225,122,148,62,41,92,15,61,0,0,0,63,0,0,0,63,154,153,89,63,
        154,153,25,62,1,1,0,0,128,63,6,236,81,184,61,51,51,131,64,12,0,10,0,0,0,0,0,0,0,0,0,131,0,
        254,0,2,1,5,48,117,100,0,44,1,151,7,132,3,197,0,144,1,64,1,64,1,48,117,48,117,48,117,48,
        117,100,0,100,0,100,0,48,117,48,117,48,117,100,0,100,0,48,117,100,0,100,0,100,0,100,0,48,
        117,48,117,48,117,100,0,100,0,100,0,48,117,48,117,100,0,44,1,44,1,44,1,44,1,44,1,44,1,44,1,
        44,1,44,1,44,1,44,1,44,1,44,1,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,44,1,0,0,0,0,98,149,0,0
    };
    _iaqSensor.setConfig(generic_33v_300s_4d);

    // Sensor shows the temperature of the chip itself and this is usually above ambient. An offset
    // can be configured to compensate for this. The offset depends on the PCB layout.
    _iaqSensor.setTemperatureOffset(3.5f);

    checkIaqSensorStatus();
    loadState();

    bsec_virtual_sensor_t sensorList[7] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ_ESTIMATE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY
    };

    _iaqSensor.updateSubscription(sensorList, 7, BSEC_SAMPLE_RATE_LP);
    checkIaqSensorStatus();
}

void AQSensors::loop() {
    bool newDataAvailable = _iaqSensor.run();
    if (millis() - _lastRefresh > 1000 * 30) {
        if (!newDataAvailable) {
            // Sensor is running. Get its status and try next time.
            checkIaqSensorStatus();
        } else {
            if (_lastRefresh == 0) {
                // Print the header
                _output = "sec,\tr_temp,\tpressure,\tr_hum,\tgas (Ohm),\tIAQ,\t\tacc,\ttemp,\thumidity";
                Serial.println(_output);
            }

            _lastRefresh = millis();

            _temp = _iaqSensor.temperature;
            _pressure = _iaqSensor.pressure;
            _humidity = _iaqSensor.humidity;
            _iaq = _iaqSensor.iaqEstimate;
            _iaq_accuracy = _iaqSensor.iaqAccuracy;
            _gas_resistance = _iaqSensor.gasResistance;

            _output = String(millis()/1000);
            _output += ",\t" + String(_iaqSensor.rawTemperature);
            _output += ",\t" + String(_iaqSensor.pressure);
            _output += ",\t" + String(_iaqSensor.rawHumidity);
            _output += ",\t" + String(_iaqSensor.gasResistance);
            _output += ",\t" + String(_iaqSensor.iaqEstimate);
            _output += ",\t\t" + String(_iaqSensor.iaqAccuracy);
            _output += ",\t" + String(_iaqSensor.temperature);
            _output += ",\t" + String(_iaqSensor.humidity);

            Serial.println(_output);

            updateState();
        }
    }
}

float AQSensors::getIAQAccuracy() {
    return _iaq_accuracy;
}

float AQSensors::getIAQ() {
    return _iaq;
}

float AQSensors::getHumidity() {
    return _humidity;
}

float AQSensors::getTemp() {
    return _temp;
}

float AQSensors::getPressure() {
    return _pressure;
}

float AQSensors::getGasResistance() {
    return _gas_resistance;
}

// Helper function definitions
void AQSensors::checkIaqSensorStatus(void)
{
    if (_iaqSensor.status != BSEC_OK) {
        if (_iaqSensor.status < BSEC_OK) {
            _output = "BSEC error code : " + String(_iaqSensor.status);
            Serial.println(_output);
            for (;;) {
                Serial.println(_output);
                delay(1000);
            }
        } else {
            _output = "BSEC warning code : " + String(_iaqSensor.status);
            Serial.println(_output);
        }
    }

    if (_iaqSensor.bme680Status != BME680_OK) {
        if (_iaqSensor.bme680Status < BME680_OK) {
            _output = "BME680 error code : " + String(_iaqSensor.bme680Status);
            Serial.println(_output);
            for (;;) {
                Serial.println(_output);
                delay(1000);
            }
        } else {
            _output = "BME680 warning code : " + String(_iaqSensor.bme680Status);
            Serial.println(_output);
        }
    }
}

void AQSensors::loadState(void)
{
    if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE) {
        // Existing state in EEPROM
        Serial.println("Reading state from EEPROM");
        uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
        for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
            bsecState[i] = EEPROM.read(i + 1);
        }

        _iaqSensor.setState(bsecState);
        checkIaqSensorStatus();
    } else {
        this->eraseEEPROM();
    }
}

void AQSensors::eraseEEPROM() {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
        EEPROM.write(i, 0);

    EEPROM.commit();
}

void AQSensors::updateState(void)
{
    bool update = false;
    if (_lastStateUpdate == 0) {
        if (_iaqSensor.iaqAccuracy >= 3) {
            update = true;
        }
    } else {
        unsigned long timeSinceLastStateUpdate = millis() - _lastStateUpdate;
        if (timeSinceLastStateUpdate > BME680_SAVE_STATE_PERIOD) {
            update = true;
        }
    }

    if (update) {
        _lastStateUpdate = millis();
        uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
        _iaqSensor.getState(bsecState);
        checkIaqSensorStatus();

        Serial.println("Writing state to EEPROM");

        for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE ; i++) {
            EEPROM.write(i + 1, bsecState[i]);
        }

        EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
        EEPROM.commit();
    }
}

AQSensors aqSensors = AQSensors();
