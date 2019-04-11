#include "AQMonitor.h"

AQSensors::AQSensors() {
    _lastStateUpdate = 0;
}

void AQSensors::begin() {
    Wire.begin(I2C_SDA, I2C_SCL);

    // Init the sensor
    _iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);

    logger.log("BSEC library version %d.%d.%d.%d",
               _iaqSensor.version.major,
               _iaqSensor.version.minor,
               _iaqSensor.version.major_bugfix,
               _iaqSensor.version.minor_bugfix);

    // As per the documentation - 'The default configuration (after calling bsec_init), to which
    // BSEC will be configured, is "generic_18v_300s_4d"'. However this firmware is for PCB that
    // works on 3.3V, so - the default config is overridden the generic_33v_300s_4d or the
    // generic_33v_300s_28d.
    const uint8_t generic_33v_300s_4d[BSEC_MAX_PROPERTY_BLOB_SIZE] = {
        3,7,4,1,61,0,0,0,0,0,0,0,174,1,0,0,48,0,1,0,0,192,168,71,64,49,119,76,0,0,225,68,137,65,
        0,63,205,204,204,62,0,0,64,63,205,204,204,62,0,0,0,0,0,80,5,95,0,0,0,0,0,0,0,0,28,0,2,0,
        0,244,1,225,0,25,0,0,128,64,0,0,32,65,144,1,0,0,112,65,0,0,0,63,16,0,3,0,10,215,163,60,
        10,215,35,59,10,215,35,59,9,0,5,0,0,0,0,0,1,88,0,9,0,229,208,34,62,0,0,0,0,0,0,0,0,218,
        27,156,62,225,11,67,64,0,0,160,64,0,0,0,0,0,0,0,0,94,75,72,189,93,254,159,64,66,62,160,
        191,0,0,0,0,0,0,0,0,33,31,180,190,138,176,97,64,65,241,99,190,0,0,0,0,0,0,0,0,167,121,71,
        61,165,189,41,192,184,30,189,64,12,0,10,0,0,0,0,0,0,0,0,0,229,0,254,0,2,1,5,48,117,100,0,
        44,1,112,23,151,7,132,3,197,0,92,4,144,1,64,1,64,1,144,1,48,117,48,117,48,117,48,117,100,
        0,100,0,100,0,48,117,48,117,48,117,100,0,100,0,48,117,48,117,100,0,100,0,100,0,100,0,48,
        117,48,117,48,117,100,0,100,0,100,0,48,117,48,117,100,0,100,0,44,1,44,1,44,1,44,1,44,1,44,
        1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,
        8,7,8,7,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,
        23,112,23,112,23,255,255,255,255,255,255,255,255,220,5,220,5,220,5,255,255,255,255,255,
        255,220,5,220,5,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,48,117,0,0,0,0,111,72,0,0};

    const uint8_t generic_33v_300s_28d[BSEC_MAX_PROPERTY_BLOB_SIZE] =  {
        3,7,4,1,61,0,0,0,0,0,0,0,174,1,0,0,48,0,1,0,0,168,19,73,64,49,119,76,0,0,225,68,137,65,0,
        63,205,204,204,62,0,0,64,63,205,204,204,62,0,0,0,0,0,80,5,95,0,0,0,0,0,0,0,0,28,0,2,0,0,
        244,1,225,0,25,0,0,128,64,0,0,32,65,144,1,0,0,112,65,0,0,0,63,16,0,3,0,10,215,163,60,10,
        215,35,59,10,215,35,59,9,0,5,0,0,0,0,0,1,88,0,9,0,229,208,34,62,0,0,0,0,0,0,0,0,218,27,
        156,62,225,11,67,64,0,0,160,64,0,0,0,0,0,0,0,0,94,75,72,189,93,254,159,64,66,62,160,191,0,
        0,0,0,0,0,0,0,33,31,180,190,138,176,97,64,65,241,99,190,0,0,0,0,0,0,0,0,167,121,71,61,165,
        189,41,192,184,30,189,64,12,0,10,0,0,0,0,0,0,0,0,0,229,0,254,0,2,1,5,48,117,100,0,44,1,112,
        23,151,7,132,3,197,0,92,4,144,1,64,1,64,1,144,1,48,117,48,117,48,117,48,117,100,0,100,0,
        100,0,48,117,48,117,48,117,100,0,100,0,48,117,48,117,100,0,100,0,100,0,100,0,48,117,48,117,
        48,117,100,0,100,0,100,0,48,117,48,117,100,0,100,0,44,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,
        44,1,44,1,44,1,44,1,44,1,44,1,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,112,
        23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,
        23,255,255,255,255,255,255,255,255,220,5,220,5,220,5,255,255,255,255,255,255,220,5,220,5,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,48,117,0,0,0,0,76,47,0,0};

    if (settingsData.aqSensor.calibrationPeriod != 28) {
        _iaqSensor.setConfig(generic_33v_300s_4d);
    } else {
        _iaqSensor.setConfig(generic_33v_300s_28d);
    }

    checkIaqSensorStatus();
    loadState();

    bsec_virtual_sensor_t sensorList[8] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ,             /*!< Unscaled indoor-air-quality estimate */ 
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    _iaqSensor.updateSubscription(sensorList, 8, BSEC_SAMPLE_RATE_LP);
    checkIaqSensorStatus();
}

void AQSensors::loop() {
    // Sensor shows the temperature of the chip itself and this is usually above ambient. An offset
    // can be configured to compensate for this. The offset depends on the PCB layout.
    _iaqSensor.setTemperatureOffset(settingsData.aqSensor.temperatureOffset * -0.1f);

    if (_iaqSensor.run()) {
        _temp = _iaqSensor.temperature;
        _humidity = _iaqSensor.humidity + (settingsData.aqSensor.humidityOffset * 0.1f);
        _pressure = _iaqSensor.pressure;
        _gas_resistance = _iaqSensor.gasResistance;

        _accuracy = _iaqSensor.iaqAccuracy;
        _iaq = _iaqSensor.iaqEstimate;
        _static_iaq = _iaqSensor.staticIaq;


        // Some calculation with fixed values for good and bad air quality sensor resistance.
        // Bad sensor resistance should be the value for AQ=250, good for AQ=25. Calculations
        // are using linear interpolation to get the AQ value.
        float dRper1AQ =
            1000 * (
                settingsData.aqSensor.goodAQResistance -
                settingsData.aqSensor.badAQResistance
            ) / 225.0f;
        if (_gas_resistance - 1000 * settingsData.aqSensor.badAQResistance == 0) {
            _calculated_iaq = 250.0f;
        } else {
            _calculated_iaq =
                250.0f - (
                    _gas_resistance - 1000 *
                    settingsData.aqSensor.badAQResistance
                ) / dRper1AQ;
        }
        _calculated_iaq = min(_calculated_iaq, 500.0f);
        _calculated_iaq = max(_calculated_iaq, 0.0f);

        updateState();
    } else {
        checkIaqSensorStatus();
    }
}

uint8_t AQSensors::getAccuracy() {
    return _accuracy;
}

float AQSensors::getIAQ() {
    return _iaq;
}

float AQSensors::getStaticIAQ() {
    return _static_iaq;
}

float AQSensors::getHumidity() {
    return _humidity;
}

float AQSensors::getTemp() {
    return _temp;
}

float AQSensors::getCalculatedIAQ() {
    return _calculated_iaq;
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
            logger.log("BSEC error code : %d", _iaqSensor.status);
            for (;;) {
                delay(1000);
            }
        } else {
            logger.log("BSEC warning code : %d", _iaqSensor.status);
        }
    }

    if (_iaqSensor.bme680Status != BME680_OK) {
        if (_iaqSensor.bme680Status < BME680_OK) {
            logger.log("BME680 error code : %d", _iaqSensor.bme680Status);
            for (;;) {
                delay(1000);
            }
        } else {
            logger.log("BME680 warning code : %d",_iaqSensor.bme680Status);
        }
    }
}

void AQSensors::loadState(void)
{
    uint8_t *state = settingsData.aqSensor.sensorCalibration;
    for (int i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
        // Check if we have non-zero calibration byte. If we do - the calibraiton
        // data is valid and can be used.
        if (state[i] != 0) {
            logger.log("Saved state not applied!");

            _iaqSensor.setState(settingsData.aqSensor.sensorCalibration);
            checkIaqSensorStatus();
            logger.log("Settings applied.");
            break;
        }
    }
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
        _iaqSensor.getState(settingsData.aqSensor.sensorCalibration);
        checkIaqSensorStatus();
        settings.save();
    }
}

AQSensors aqSensors = AQSensors();
