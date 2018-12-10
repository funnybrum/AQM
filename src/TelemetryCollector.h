#ifndef _TELEMETRY_COLLECTOR_H_
#define _TELEMETRY_COLLECTOR_H_

#include "AQMonitor.h"
#include <ESP8266HTTPClient.h>

class TelemetryCollector {
    public:
        void begin();
        void loop();
        unsigned long getTimestamp();
    private:
        void syncTime(const char* dateTime);
        void collect();
        void append(const char* metric, float value);
        bool push();

        String data = "";

        unsigned long lastDataCollect;
        unsigned long lastDataPush;

        unsigned long remoteTimestamp;
        unsigned long remoteTimestampMillis;

        HTTPClient* http = NULL;
};

extern TelemetryCollector telemetryCollector;

#endif
