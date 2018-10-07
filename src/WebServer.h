#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "AQMonitor.h"

class WebServer {
    public:
        WebServer(int port);
        void begin();
        void loop();
    private:
      ESP8266WebServer *_server;
      ESP8266HTTPUpdateServer *_httpUpdater;

      int8_t getBodyValue(uint8_t currentValue);
      
      void handle_root();
      void handle_reset();
      void handle_get();
      void handle_hard_reset();
      void handle_blink();
      void handle_temp_offset();
      void handle_humidity_offset();
      void handle_hostname();
};

extern WebServer webServer;

#endif
