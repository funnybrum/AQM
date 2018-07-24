# AQM
## Summary

An Air Quality Monitor firmware for ESP8266 board and BME680 sensor.

Several sensors were considered and tested before stopping on BME680. Such are the MH-Z19, MICS-VZ-89TE, BME280. The BME680 provide single index of the IAQ that seems to be more than sufficient. As optional improvment I'm considering to add also support for dust particle sensor (probably PMS5003).


## Calibrating the sensor

### Temperature calibration

The sensor is quite accurate, but it measures the temperature of the sensor itself. If the sensor is close to heat emitting components - it will get heated and the temperature reading will have to be compoensated for that. The calibration is based on an offset value and it should be calculated with precise reference point.

Currently the temperature offset is set directly in the code as parameter of the setTemperatureOffset method.

### IAQ index calibration

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

Calibration is performed automatically in the background if the sensor is exposed to clean and polluted air for approximately 30 minutes each. What I do - use small plastic bag and fill it with dirty air (breath inside several times or whatever you came up with). Seal the bag with the sensor inside and keep it like this for 30 minutes. For the clean air - bring the sensor outside your home and keep it running there in a clear day for 30 minutes. Once the process is complete you'll get the accuracy output with value 3.

*Note* - I'm considering a precalibration logic and process that will eliminate the BSEC autocalibration feature. The calibraiton will most likely be done once and will allow consistent reading over time (without further automatic recalibration). The goal is to have IAQ that is absolute and a sensor that will produce exactly the same value for specific air quality.