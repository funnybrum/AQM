#include "AQMonitor.h"

void Settings::begin() {
    EEPROM.begin(DATA_SIZE);

    for (unsigned int i = 0; i < DATA_SIZE; i++) {
        *(((uint8_t*)&this->data) + i) = EEPROM.read(i);
    }

    EEPROM.end();

    if (data.checksum != TEMP_CHECKSUM_VAL) {
        this->erase();
    }
}

void Settings::loop() {
}

void Settings::erase() {
    Serial.print("Erasing EEPROM...");
    memset(&this->data, 0, DATA_SIZE);
    this->save();
}

void Settings::save() {
    Serial.println("Writing state to EEPROM");

    EEPROM.begin(DATA_SIZE);
    this->data.checksum = TEMP_CHECKSUM_VAL;
    for (unsigned int i = 0; i < DATA_SIZE; i++) {
        EEPROM.write(i, *(((uint8_t*)&this->data) + i));
    }
    EEPROM.end();
}

SettingsData* Settings::get() {
    return &this->data;
}

bool Settings::isDataValid() {
    return data.checksum == TEMP_CHECKSUM_VAL; 
}

Settings settings = Settings();
