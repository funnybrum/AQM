#pragma once

#include "SoftwareSerial.h"

/*
 * MH-Z19 CO2 sensor interface. Works both with MH-Z19 and MH-Z19B.
 */
class MHZ19 {
    public:
        virtual void begin();
        virtual void loop();

        uint16_t getCO2();

    private:
        void processPacket();

        uint16_t co2 = 0;
        uint8_t data[9];
        uint8_t pos = 0;
        SoftwareSerial* serial = NULL;
        uint32_t lastRead;
};

extern MHZ19 CO2Sensor;
