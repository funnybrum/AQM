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
 "pressure":%.0f,
 "tvoc":%.0f,
 "e_co2":%.0f,
 "abs_humidity": %.2f
})=====";