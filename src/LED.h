#ifndef _LED_H_
#define _LED_H_

#include "AQMonitor.h"

class LED {
    public:
        void begin();
        void loop();
        void set(uint8_t red, uint8_t green, uint8_t blue);
        void blink(float iaq);
    private:
        unsigned long _lastBlink = 0;
        unsigned long _lastUpdate = 0;
        uint8_t _red = 0, _targetRed = 0;
        uint8_t _green = 0, _targetGreen = 0;
        uint8_t _blue = 0, _targetBlue = 0;
};

extern LED led;

#endif
