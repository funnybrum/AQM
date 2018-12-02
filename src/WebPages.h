#define PROGMEM ICACHE_RODATA_ATTR

const char CONFIG_PAGE[] PROGMEM = R"=====(
<HTML>
	<HEAD>
      <TITLE>Air quality monitor</TITLE>
	</HEAD>
  <BODY>
    <form action="/settings" method="get">
      Hostname:<br>
      <input type="text" name="hostname" value="%s"><br>
      <small><em>from 4 to 63 characters lenght, can contain chars, digits and '-'</em></small><br>
      <br>
      Temperature offset:<br>
      <input type="text" name="temp_offset" value="%d"><br>
      <small><em>in 0.1 degrees, from -125 to 125</em></small><br>
      <br>
      Humidity offset:<br>
      <input type="text" name="humidity_offset" value="%d"><br>
      <small><em>in 0.1 percents, from -125 to 125</em></small><br>
      <br>
      Blinik interval:<br>
      <input type="text" name="blink_interval" value="%d"><br>
      <small><em>in seconds, 0 to disable blinking, max value - 3600</em></small><br>
      <br>
      Gas sensor resistance at AQ 250:<br>
      <input type="text" name="bad_aq_res" value="%d"><br>
      <small><em>in kohm</em></small><br>
      <br>
      Gas sensor resistance at AQ 25:<br>
      <input type="text" name="good_aq_res" value="%d"><br>
      <small><em>in kohm</em></small><br>
      <br>
      <input type="submit" value="Save">
    </form> 	
  </BODY>
</HTML>
)=====";

const char GET_JSON[] PROGMEM = R"=====({
  "temp":%.2f,
  "humidity":%.2f,
  "pressure":%.2f,
  "iaq":%.2f,
  "iaq_accuracy":%.2f,
  "gas_resistance":%.0f,
  "calculated_iaq":%.2f
})=====";