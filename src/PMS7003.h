#pragma once

/*
 * PMS7003 dust sensor interface. The PMS5003 is identical and this would work
 * for it too. However the correction applied is for hte PMS7003 sensor and it
 * may not be a bit inaccurate for the PMS5003.
 */
class PMS7003 {
    public:
        virtual void begin();
        virtual void loop();

        uint16_t getPM025();
        uint16_t getPM100();

    private:
        void processPacket();
        uint16_t correct(uint16_t);

        uint16_t pm025 = 0.0f;
        uint16_t pm100 = 0.0f;
        uint8_t data[32];
        uint8_t pos = 0;
};

extern PMS7003 dustSensor;
