#ifndef _AQMONITOR_H_
#define _AQMONITOR_H_

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

#include "user_interface.h"

#include "WiFi.h"
#include "AQSensors.h"
#include "WebServer.h"

#define HTTP_PORT 80
#define HOSTNAME "br-aq-monitor"

#define I2C_SCL D1
#define I2C_SDA D2

#endif
