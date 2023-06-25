# ESP32 Google Sheets Update using Arduino IDE

This is a modified example of using an ESP32 to update a google sheet with simulated sensor data and a time stamp.
It is derived from the IoTDesignPro article 
[ESP32 Data Logging to Google Sheets with Google Scripts ](https://iotdesignpro.com/articles/esp32-data-logging-to-google-sheets-with-google-scripts).
That article gives a mostly good description of steps needed to set up a spreadsheet and associated script to be used with this code.

## Deviations from Original Example

* Hard coded **SSID and passphrase of WiFi router** were removed and [WiFiManager](https://github.com/tzapu/WiFiManager) was used instead. A full description
  of what [WiFiManager](https://github.com/tzapu/WiFiManager) does is given at the link. It essentially allows reconfiguration of the SSID at run time. 
* Two new files were added to the project.
    * googleID.ino contains the *definition* of the variable **const char \*google_script_url** which is initialized with the URL string of the google 
	  script upto and including the "?". The original example required repeated concatenation of the first part of that string with the script ID  at run time. 
	  This string initialization must be edited with the correct script ID before compile time. Note that the Arduino IDE will include the contents of 
	  googleID.ino after the contents of esp-google-sheets.ino
	* googleID.h contains the *declaration*  of the variable **extern const char \*google_script_url;**. An **\#include "googleID.h"** directive is 
	  added to **esp32-google-sheets.ino**.
* A number of String object operations were removed and replaced with operations on c-strings. This was motivated to eliminate heap fragmentation. Also,
  the library functions strftime() and http.begin() require c-strings. It made little sense to me to take the c-string from strftime() and convert it to
  a String object for manipulation and concatenation only to convert the result back to a c-string for http.begin().
* One place I left a String object in place was the response string coming from the google site. This makes sense as it is of unknown length at compile time. 
  The concatenation for a single Serial.print() statement was removed and replaced with two Serial.print() statements.
  
## Correction to Script

As stated in the first section the iotdesignpro article gives mostly good information. One error to the script needed to be corrected. 

Original Script contained:

~~~~
...
var sensor = Number(e.parameter.sensor);
var date = Number(e.parameter.date);
...
~~~~

To work with the timestamp sent this needed to be changed to:

~~~~
...
var sensor = Number(e.parameter.sensor);
var date = String(e.parameter.date);
...
~~~~

Note that the original script worked fine with the manual test of the script but needed to be fixed for the actual timestamp to work.
