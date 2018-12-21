# AQM
## Summary

An Air Quality Monitor firmware for ESP8266 board and BME680 sensor.

Several sensors were considered and tested before stopping on BME680. Such are the MH-Z19, MICS-VZ-89TE, BME280. The BME680 provide single index of the IAQ that seems to be more than sufficient. As optional improvment I'm considering to add also support for dust particle sensor (probably PMS5003).


## Calibrating the sensor

### Temperature calibration

The sensor is quite accurate, but it measures the temperature of the sensor itself. If the sensor is close to heat emitting components - it will get heated and the temperature reading will have to be compoensated for that. The calibration is based on an offset value and it should be calculated with precise reference point.

Currently the temperature offset is set directly in the code as parameter of the setTemperatureOffset method.

### IAQ calibration

The current implementation relies on the BSEC library from Bosch. The library provide build in algorithms that can calculate the air quality and provide a IAQ reference value between 0 and 500. The IAQ index has the following semantic:
* 0 .. 50 Good
* 51 .. 100 Average
* 101 .. 150 Little bad
* 151 .. 200 Bad
* 201 .. 300 Worse
* 301 .. 500 Very bad

The IAQ is not an absolute value, but is relative to the condition that the sensor is in. A quote from BME680 documentation that is providing a bit more details on that:

```
The BSEC software auto-calibrates the low and high concentrations applied during testing to IAQ of 25 and 250, respectivel.
```

When assembled and started the sensor have to be calibrated. There is an accuracy output provided on the REST interface and the serial output that can be used as indicator about that. The accuracy values as specified by Bosch have the following semantic:
* 0 - The sensor is not yet stablized or in a run-in status
* 1 - Calibration required
* 2 - Calibration on-going
* 3 - Calibration is done, now IAQ estimate achieves best perfomance

Calibration is performed automatically in the background if the sensor is exposed to clean and polluted air for approximately 30 minutes each. Keep the sensor running for a day or two and it should pass the calibration process.

*Note* - A precalibration logic is added. It calculates the IAQ based on fixed gas sensor resistance for the AQ of 25 and 250. Not as usuful as expected initially. Moreover - Bosch has added staticIAQ output to the BSEC library that seems to do exactly what I tried to do, but a lot better.

## Over-the-air programming

The firmware supports OTA update. Below curl command executed from the project root will perform the update:
> curl -F "image=@.pioenvs/d1_mini/firmware.bin" br-aq-monitor1/update

## InfluxDB integration

The sensor can be configured for regular pushes to an InfluxDB (must be a DB that is not password protected). If the InfluxDB integration is enabled the sensor will turn off the WiFi and keep running by collecting data in the background. Once the telemtry data buffers are full or the time for pushing the data has come - the sensor will turn on the WiFi, push the data and turn it back off.

The WiFi will stay on until the first data push cycle has been completed. In this time window the sensor can be reconfigured (open the http://sensor_address/ to get to the settings page). 

## REST interface

The firmware provides a REST API to ease the usage. There are several endpoints that should be mentioned:

* http://{address}/get - get the current sensor readings as JSON document.
* http://{address}/logs - get the logs (for debugging purposes).
* http://{address}/settings - a form that is used to configure the sensor. Details like WiFi SSID/password, InfluxDB integration and offsets can be configured here.

## Building the project

The project uses a common set of tools that are availabe in another repo - https://github.com/funnybrum/esp8266-base. Clone the esp8266-base repo in the lib/conmmon folder:

```
cd lib
git clone git@github.com:funnybrum/esp8266-base.git common
```

After that try building the project. You should hit linking failure stating that the firmware can't fit in the ESP8266 memory. Check the details in the ./lib/README.md to find out how to address that.

## Initial sensor configuration

Power on the sensor. It will run in AP mode with SSID br-aq-monitor. Connect to the WiFi network and open http://192.168.0.1/ . Configure the details there and click the save button. Once the WiFi SSID/password is properly configured - click the restart button and the sensor will connect to the WiFi network. Then try to ping it based on the hostname that was configured (or open your router and see the IP address that was given to the sensor).