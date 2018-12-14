#define PROGMEM ICACHE_RODATA_ATTR

const char CONFIG_PAGE[] PROGMEM = R"=====(
<HTML>
  <HEAD>
      <TITLE>Air quality monitor</TITLE>
  </HEAD>
  <BODY>
    <form action="/settings" method="get">
      <fieldset style='display: inline-block; width: 300px'>
        <legend>Network settings</legend>
        Hostname:<br>
        <input type="text" name="hostname" value="%s"><br>
        <small><em>from 4 to 63 characters lenght, can contain chars, digits and '-'</em></small><br>
        <br>
        SSID:<br>
        <input type="text" name="ssid" value="%s"><br>
        <small><em>WiFi network to connect to</em></small><br>
        <br>
        Password:<br>
        <input type="password" name="password"><br>
        <small><em>WiFi network password</em></small><br>
      </fieldset>
      <br><br>
      <fieldset style='display: inline-block; width: 300px'>
        <legend>InfluxDB settings</legend>
        InfluxDB integration:<br>
        <select name="ifx_enabled">
          <option value="true" %s>Enabled</option>
          <option value="false" %s>Disabled</option>
        </select><br>
        <br>
        Address:<br>
        <input type="text" name="ifx_address" value="%s"><br>
        <small><em>like 'http://192.168.0.1/8086'</em></small><br>
        <br>
        Database:<br>
        <input type="text" name="ifx_db" value="%s"><br>
        <small><em>WiFi network to connect to</em></small><br>
        <br>
        Collect interval:<br>
        <input type="text" name="ifx_collect" value="%d"><br>
        <small><em>in seconds, from 0 to 65535</em></small><br>
        <br>
        Push interval:<br>
        <input type="text" name="ifx_push" value="%d"><br>
        <small><em>in seconds, from 0 to 65535</em></small><br>
        <br>
      </fieldset>
      <br><br>
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
        <br>
        Gas sensor resistance at AQ 25:<br>
        <input type="text" name="good_aq_res" value="%d"><br>
        <small><em>in kohm</em></small><br>
        <br>
        Gas sensor resistance at AQ 250:<br>
        <input type="text" name="bad_aq_res" value="%d"><br>
        <small><em>in kohm</em></small><br>
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
      <a href="/reset">
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
  "calculated_iaq":%.2f
})=====";