//Include required libraries
#include "WiFi.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <HTTPClient.h>
#include "time.h"
#include "googleID.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -8*3600;
const int   daylightOffset_sec = 3600;

int count = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm; 
  // reset settings - wipe stored credentials for testing 
  // these are stored by the esp library
  // wm.resetSettings();  // -- uncomment for test only
  bool res;
  res = wm.autoConnect("ESP32-AP","physicspnt"); // password protected ap only used if ssid not found
  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected to WiFi router...");
  }  
  delay(1000);
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
void loop() {
   if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    static char timeStringBuff[50]; //50 chars should be enough --**edit** made static to avoid putting array on stack
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    {  // replace spaces with hyphens
      int i;
      for(i = 0; i <= strlen(timeStringBuff); i++) {
  		  if(timeStringBuff[i] == ' ')  
        {
          timeStringBuff[i] = '-';
        }
      }
    }
    Serial.print("Time:");
    Serial.println(timeStringBuff);
    static char google_send[165]; //115+5+30+8+6+1=165
    sprintf(google_send,"%sdate=%s&sensor=%i" , google_script_url, timeStringBuff, count);
    Serial.print("POST data to spreadsheet:");
    Serial.println(google_send);
    HTTPClient http;
    http.begin(google_send);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: ");
        Serial.println(payload);    
    }
    //---------------------------------------------------------------------
    http.end();
  }
  count++;
  delay(10000);
} 