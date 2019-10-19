#define PROGMEM ICACHE_RODATA_ATTR

const char CONFIG_PAGE[] PROGMEM = R"=====(
<HTML>
 <HEAD>
   <TITLE>Air quality monitor</TITLE>
 </HEAD>
 <BODY>
  <form action="/settings" method="get">
   %s<br><br>
   %s<br><br>
   <fieldset style='display: inline-block; width: 300px'>
    <legend>AQ Sensor settings</legend>
    Temperature offset:<br>
    <input type="text" name="temp_offset" value="%d"><br>
    <small><em>in 0.1 degrees, from -125 to 125</em></small><br>
    <br>
    Humidity offset:<br>
    <input type="text" name="humidity_offset" value="%d"><br>
    <small><em>in 0.1 percents, from -125 to 125</em></small><br>
    <br>
    Calibration period:<br>
    <select name="calibration_period">
     <option value="4" %s>4 days</option>
     <option value="28" %s>28 days</option>
    </select><br>
    <small><em>Period over which the sensor will recalibrate its IAQ values (requires restart)</em></small><br>
   </fieldset>
   <br><br>
   <fieldset style='display: inline-block; width: 300px'>
    <legend>LED settings</legend>
    Blinik interval:<br>
    <input type="text" name="blink_interval" value="%d"><br>
    <small><em>in seconds, 0 to disable blinking, max value - 3600</em></small><br>
   </fieldset>
   <br><br>
   <input type="submit" value="Save" style='width: 150px;'>
   &nbsp;&nbsp;&nbsp;
   <a href="/reboot">
    <button type="button" style='width: 150px;'>Restart</button>
   </a>
  </form>
 </BODY>
</HTML>
)=====";

const char GET_JSON[] PROGMEM = R"=====({
 "temp":%.2f,
 "humidity":%.2f,
 "pressure":%.2f,
 "gas_resistance":%.0f,
 "iaq_accuracy":%d,
 "iaq":%.2f,
 "static_iaq":%.2f,
 "free_heap": %d
})=====";