# AQM
## Summary

An Air Quality Monitor firmware for ESP8266 board and BME680 sensor.

Several sensors were considered and tested before stopping on BME680. Such are the MH-Z19, MICS-VZ-89TE, BME280. The BME680 provide single index of the IAQ that seems to be more than sufficient. As optional improvment I'm considering to add also support for dust particle sensor (probably PMS5003).

There is a [PCB project](https://easyeda.com/funnybrum/IAQ_Board) available.

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

Calibration is performed automatically in the background if the sensor is exposed to clean and polluted air for approximately 30 minutes each. Keep the sensor running for a day or two and it should pass the calibration process. Have in mind that the sensor is reporting AQ values in yor environment, so forced calibration is not preffered.

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

The project uses a common set of tools that are availabe in another repo - https://github.com/funnybrum/esp8266-base. Clone the esp8266-base repo in the lib folder:

```
cd lib
git clone git@github.com:funnybrum/esp8266-base.git
```

After that try building the project. You should hit linking failure stating that the firmware can't fit in the ESP8266 memory. Check the details in the ./lib/README.md to find out how to address that.

## Initial sensor configuration

Power on the sensor. It will run in AP mode with SSID br-aq-monitor. Connect to the WiFi network and open http://192.168.0.1/ . Configure the details there and click the save button. Once the WiFi SSID/password is properly configured - click the restart button and the sensor will connect to the WiFi network. Then try to ping it based on the hostname that was configured (or open your router and see the IP address that was given to the sensor).

# Notes

## BME680 precision

The sensor temperature reading is usually offsetted by 2-2.5 degrees C. If you can calibrate against a precise source - use that, but if you can't - it would be safe to set the temperature offset to -22 (-2.2C) on the settings page.

The humidity is quite accurate once the temperature offset have been specified. A test with Magnesium Chloride and Sodium Chloride were used and confirmed that sensor deviation was:
* ~0.5% at 33.2% RH
* ~5% at 75.4% RH

Without calibrating the temperature reading - the humidity would not be correct. The sensor is reporting relative humidity to the temperature. And as expected - wrong temperature, wrong humidity values.

## BME680 temperature/humidity effect on AQ values

Both temperature and humitdity seems to have an impact on the AQ values.

A simple test for the temperature - in winter (should be cold outside) take out the sensor and observe the AQ values. Expectation would be to get low AQ values, but instead a spike is observed. The gas sensor resistance drops as result of the cooling, but the BSEC library seems to be ignoring the temperature readings for the AQ value calculation.

A test for the humidity can be performed humidifier. Once a humidifier is started the AQ spikes quite fast. Observing the humidity reading and the gas sensor reading - the correlation is pretty obvious. Once the humidity spikes the gas resistance drops. And again - the BSEC library seems to be ignoring the humidity reading for the AQ value calculation.

So the sensor works fine only if the temperature and the humidity are kept constant. A drop in the temperature is rarely observed unless you are not keeping the window open for quite some time in the winter and is kind of OK. But a spike in the humidity is observed under regular day to day operations - take the cooking for example. This should definitely be considered as factor for the reported AQ.

Two ways for solving that:
* (preferred one) Wait for Bosch to fix this.
* Create a custom library for calculating the AQ values.

## Bug in BSEC 1.4.7.1
The library seems to be introducing regression in the AQ value calculation process. Once the sensor calibrates itself and the information is saved to the EEPROM it should not be restarted. If restarted - the AQ values will start jumping between 0 and 500. In such case - reset the sensor calibration data (http://sensor/reset) and restart the sensor (http://sensor/reboot). Or switch to the [code using the BSEC 1.4.6.0](https://github.com/funnybrum/AQM/tree/downgrade_bsec_to_1_4_6_0).

The issue have been [reported to Bosch](https://github.com/BoschSensortec/BSEC-Arduino-library/issues/38), but there doesn't seems to be an activity on their side.
