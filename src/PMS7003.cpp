#include "AQMonitor.h"

void PMS7003::begin() {
    Serial.begin(9600);
    while (! Serial) {
        delay(1);
    }
    logger.log("HW Serial started");
}

void PMS7003::loop() {
        while (Serial.available() > 0) {
        data[pos] = Serial.read();
        pos++;

        if (pos == 1 && data[0] != 0x42) {
            // Package from the sensor should start with 0x42, 0x4d. Wait till
            // we get 0x42 as first byte.
            pos--;
        }

        if (pos == 2 && data[1] != 0x4d) {
            // Wait till we get 0x42 as first byte and 0x4d as second byte.
            pos-=2;
        }

        if (pos == 32) {
            pos = 0;
            processPacket();

            // --- DEBUG LOG START ---
            // if (millis() - last_log > 5000) {
            //     String hex_dump = String();
            //     for (int i = 0; i < 24; i++) {
            //         hex_dump.concat("0x" + String(data[i],HEX) + " ");
            //     }
            //     logger.log(hex_dump);
            //     logger.log("V:%lu, C:%lu, P:%lu", voltage, current, power);
            //     last_log = millis();
            // }
            // --- DEBUG LOG END ---

        }
    }
}

void PMS7003::processPacket() {
    uint16_t checksum = 0;
    for (int i = 0; i < 30; i++) {
        checksum += data[i];
    }

    if (checksum != makeWord(data[30], data[31])) {
        pm025 = pm100 = -1;
        return;
    }

    pm025 = makeWord(data[6], data[7]);
    pm100 = makeWord(data[8], data[9]);
}

uint16_t PMS7003::correct(uint16_t value) {
    // Cheap sensors like the PMS7003 are not very accurate. Fortunately there
    // is quite good correlation between different speciments (as reported all
    // over the internet) and thanks to research results like the one published
    // on http://downloads.hindawi.com/journals/js/2018/5096540.pdf - we can
    // get some quite accurate PM readings. The following correction formula is
    // applied: corrected_pm_value = a*x + b. Where (based on the research):
    // 1) If RH is in [0, 80) -> a=0.29, b=4.39.
    // 2) If RH is in [80, 90) -> a=0.29, b is interpolated in the [4.39, -0.1]
    // range.
    // 3) If RH is in [90, 100] -> a=0.29, b=-0.1

    float humidity = aqSensors.getHumidity();
    float b = 4.39;
    if (humidity >= 80 && humidity <= 90) {
        b = map(humidity, 80, 90, 4.39, -0.1);
    } else if (humidity > 90) {
        b = -0.1;
    }
    return 0.29 * value + b;
}

uint16_t PMS7003::getPM025() {
    return correct(pm025);
}

uint16_t PMS7003::getPM100() {
    return correct(pm100);
}

PMS7003 dustSensor = PMS7003();
