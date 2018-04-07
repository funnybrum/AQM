MH-Z19 - UART, PWM, VCC@5V, TTL@3.3V, PWM@?V
http://www.winsen-sensor.com/d/files/PDF/Infrared%20Gas%20Sensor/NDIR%20CO2%20SENSOR/MH-Z19%20CO2%20Ver1.0.pdf


MICS-VZ-89TE - PWM, I2C, TTL@3.3V, address = 0b11100000 / 0b11100001
https://www.sgxsensortech.com/content/uploads/2017/03/I2C-Datasheet-MiCS-VZ-89TE-rev-H-ed170214-Read-Only.pdf
https://www.sgxsensortech.com/content/uploads/2016/07/MiCS-VZ-89TE-V1.0.pdf


PMS5003 - UART, VCC@5V, TTL@3.3V
http://www.aqmd.gov/docs/default-source/aq-spec/resources-page/plantower-pms5003-manual_v2-3.pdf


BME280 - I2C, TTL@3.3V, address = 0b1110110 / 0b1110111


Wemos D1 mini pins (https://wiki.wemos.cc/products:d1:d1_mini):
I2C:
  * D1 (GPIO5) - SCL
  * D2 (GPIO4) - SDA

PWM in:
  * D0

 UART:
   * D5, D6, D7



Updates:
=========================

07 April 2018

Looking at the values from the MICS-VZ-89TE - I'm not very convinced for the reliability of the sensor. Considering BME680 as alternative.