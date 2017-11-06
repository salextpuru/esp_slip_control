#define MAIN_PAGE	"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n\
<html>\
<head></head>\
<body>\
\
<form method='GET'>\
<h1>HTTP server on ESP8266 by Alex Shamin</h1>\
\
</div>\
<h2>Temperature: %s. Humidity: %s.</h2>\
\
</div>\
<h1>Relay control</h1>\
\
<h2>Relay 1: %s</h2><input type='submit' name='r1' value='%s'/><br><br>\
<font size=\"5\">Period of Relay1 (mm:ss): &nbsp;&nbsp;&nbsp;\
</font><input type='number' name='p1m' value='%u' min=\"0\" max=\"59\" /></td>:\
<td><input type='number' name='p1s' value='%u' min=\"0\" max=\"59\" /><br>\
<font size=\"5\">Temperature range of Relay1 in C.\
From: &nbsp;&nbsp;&nbsp;<input type='number' name='t1min' value='%i' min=\"-50\" max=\"50\" />&nbsp;&nbsp;&nbsp;\
To:&nbsp;&nbsp;&nbsp;<input type='number' name='t1max' value='%i' min=\"-50\" max=\"50\" />\
</font><br>\
<font size=\"5\">\
Humidity of Relay1 in %:\
From:&nbsp;&nbsp;&nbsp;<input type='number' name='h1min' value='%u' min=\"0\" max=\"100\" />&nbsp;&nbsp;&nbsp;\
To:&nbsp;&nbsp;&nbsp;<input type='number' name='h1max' value='%u' min=\"0\" max=\"100\" />\
</font>\
\
<h2>Relay 2: %s</h2><input type='submit' name='r2' value='%s'/><br><br>\
<font size=\"5\">Period of Relay2 (mm:ss): &nbsp;&nbsp;&nbsp;\
</font><input type='number' name='p2m' value='%u' min=\"0\" max=\"59\" /></td>:\
<td><input type='number' name='p2s' value='%u' min=\"0\" max=\"59\" /><br>\
<font size=\"5\">Temperature range of Relay2 in C.\
From: &nbsp;&nbsp;&nbsp;<input type='number' name='t2min' value='%i' min=\"-50\" max=\"50\" />&nbsp;&nbsp;&nbsp;\
To:&nbsp;&nbsp;&nbsp;<input type='number' name='t2max' value='%i' min=\"-50\" max=\"50\" />\
</font><br>\
<font size=\"5\">\
Humidity of Relay2 in %:\
From:&nbsp;&nbsp;&nbsp;<input type='number' name='h2min' value='%u' min=\"0\" max=\"100\" />&nbsp;&nbsp;&nbsp;\
To:&nbsp;&nbsp;&nbsp;<input type='number' name='h2max' value='%u' min=\"0\" max=\"100\" />\
</font>\
\
<h2>Relay 3: %s</h2><input type='submit' name='r3' value='%s'/><br><br>\
<font size=\"5\">Period of Relay3 (mm:ss): &nbsp;&nbsp;&nbsp;\
</font><input type='number' name='p3m' value='%u' min=\"0\" max=\"59\" /></td>:\
<td><input type='number' name='p3s' value='%u' min=\"0\" max=\"59\" /><br>\
<font size=\"5\">Temperature range of Relay3 in C.\
From: &nbsp;&nbsp;&nbsp;<input type='number' name='t3min' value='%i' min=\"-50\" max=\"50\" />&nbsp;&nbsp;&nbsp;\
To:&nbsp;&nbsp;&nbsp;<input type='number' name='t3max' value='%i' min=\"-50\" max=\"50\" />\
</font><br>\
<font size=\"5\">\
Humidity of Relay3 in %:\
From:&nbsp;&nbsp;&nbsp;<input type='number' name='h3min' value='%u' min=\"0\" max=\"100\" />&nbsp;&nbsp;&nbsp;\
To:&nbsp;&nbsp;&nbsp;<input type='number' name='h3max' value='%u' min=\"0\" max=\"100\" />\
</font>\
\
<br><br><td><input type='submit' name='save' value='Save'/></td>\
\
</body>\
</html>"
