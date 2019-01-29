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
        uint8_t cmd[9];
        cmd[0] = 0xFF;
        cmd[1] = 0x01;
        cmd[2] = 0x86;
        cmd[3] = 0x00;
        cmd[4] = 0x00;
        cmd[5] = 0x00;
        cmd[6] = 0x00;
        cmd[7] = 0x00;
        cmd[8] = 0x79;
        serial->write(cmd, sizeof(cmd));
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