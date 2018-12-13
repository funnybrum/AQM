#ifndef _WIFI_H_
#define _WIFI_H_

#include "AQMonitor.h"

enum WiFiState {
    CONNECTING,
    CONNECTED,
    DISCONNECTED
};

class WiFiManager {
    public:
        void begin();
        void loop();
        void connect();
        void disconnect();
        bool isConnected();
    private:
        void _connect();

        WiFiState state;
        unsigned long lastStateSetAt;
};

extern WiFiManager wifi;

#endif
