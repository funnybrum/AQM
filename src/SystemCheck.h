#ifndef _SYSTEM_CHECK_H_
#define _SYSTEM_CHECK_H_

#include "AQMonitor.h"

class SystemCheck {
    public:
        void begin();
        void loop();
        void registerWebCall();
        void start();
        void stop();
    private:
        unsigned long lastWebCall;
        unsigned long lastWiFiConnectedState;
        bool hasTimeoutOccur(unsigned long timer, unsigned  int timeoutSeconds);
        
        bool enabled = true;
};

extern SystemCheck systemCheck;

#endif
