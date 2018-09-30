#ifndef _LED_H_
#define _LED_H_

#include "AQMonitor.h"

class LED {
    public:
        void begin();
        void loop();
        void set(uint8_t red, uint8_t green, uint8_t blue);
};

extern LED led;

#endif
