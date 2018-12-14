#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "AQMonitor.h"
#include "WebPages.h"

class WebServer {
    public:
        WebServer(int port);
        void begin();
        void loop();
    private:
        ESP8266WebServer *_server;
        ESP8266HTTPUpdateServer *_httpUpdater;

        void handle_root();
        void handle_get();
        void handle_settings();
        void handle_reboot();
        void handle_reset();
        void handle_blink();
        void handle_logs();

        void process_setting(const char* name, char* destination, uint8_t max_size, bool& success);
        void process_setting(const char* name, int16_t& destination, bool& success);
        void process_setting(const char* name, uint16_t& destination, bool& success);
        void process_setting(const char* name, bool& destination, bool& success);
};

extern WebServer webServer;

#endif
