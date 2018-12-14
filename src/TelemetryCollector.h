#ifndef _TELEMETRY_COLLECTOR_H_
#define _TELEMETRY_COLLECTOR_H_

#include "AQMonitor.h"
#include <ESP8266HTTPClient.h>

#define TELEMETRY_BUFFER_SIZE 32768

class TelemetryCollector {
    public:
        void begin();
        void loop();
        unsigned long getTimestamp();
        void stop();
        void start();
    private:
        void syncTime(const char* dateTime);
        void collect();
        void append(const char* metric, float value, uint8_t precision);
        void ping();
        bool push();

        char telemetryData[TELEMETRY_BUFFER_SIZE];
        unsigned int telemetryDataSize = 0;

        unsigned long lastDataCollect;
        unsigned long lastDataPush;

        unsigned long remoteTimestamp;
        unsigned long remoteTimestampMillis;

        bool enabled = false;

        HTTPClient* http = NULL;
};

extern TelemetryCollector telemetryCollector;

#endif
