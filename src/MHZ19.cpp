#include "AQMonitor.h"
#include "SoftwareSerial.h"

void MHZ19::begin() {
    serial = new SoftwareSerial(D5, D6);
    serial->begin(9600);
    lastRead = millis();
}
 
void MHZ19::loop() {
    if (millis() - lastRead > 3 * 1000) {
        lastRead = millis();
        if (!initialized) {
            // Disable ABC (automatic baseline calibration) logic.
            uint8_t cmd[] = {0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00};
            sendCommand(cmd);
            initialized = true;
        } else {
            // Read the CO2 data.
            uint8_t cmd[] = {0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00};
            sendCommand(cmd);
        }
    }

    while (serial->available() > 0) {
        data[pos] = serial->read();
        pos++;

        if (pos == 1 && data[0] != 0xFF) {
            pos = 0;
        }

        if (pos == 2 && data[1] != 0x86) {
            pos = 0;
        }

        if (pos == 9) {
            processPacket();
            pos = 0;
        }
    }
}

uint16_t MHZ19::getCO2() {
    return co2;
}

void MHZ19::sendCommand(uint8_t cmd[]) {
    // Checksum. Calculated as in the specs:
    // (NOT(Byte1+Byte2+Byte3+Byte4+Byte5+Byte6+Byte7))+1
    uint8_t cs = 0;

    // Command start byte
    serial->write(0xFF);
    
    for (int i = 0; i < 7; i++) {
        serial->write(cmd[i]);
        cs += cmd[i];
    }

    cs = 0xFF - cs;
    cs += 0x01;

    // Command checksum byte
    serial->write(cs);
}

void MHZ19::processPacket() {
    uint8_t checksum = 0;
    for (int i = 1; i < 8; i++) {
        checksum += data[i];
    }
    checksum = 0xFF - checksum + 0x01;

    if (checksum != data[8]) {
        logger.log("Invalid MH-Z19 checksum!");
        return;
    }

    co2 = makeWord(data[2], data[3]);
}

MHZ19 CO2Sensor = MHZ19();
