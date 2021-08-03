int DEBUG = 0;      // DEBUG counter; if set to 1, will write values back via serial

#include <Wire.h>
#include <SPI.h>
#include <SFE_CC3000.h>         //Wifi
#include <SFE_CC3000_Client.h>  //Wifi
#include "DHT.h"                //Humidity
#include <Adafruit_BMP085.h>    //Barometer
#include "Adafruit_SI1145.h"    //UV sensor
#include "RTClib.h" 
#include <io.h> 

// Pins
#define CC3000_INT      2   // Needs to be an interrupt pin (D2/D3)
#define CC3000_EN       7   // Can be any digital pin
#define CC3000_CS       10  // Preferred is pin 10 on Uno
#define DHTPIN          3   // DHT 22  (AM2302)
                            // The UV sensor and Barometer are on i2C pins

// Constants
#define DHTTYPE DHT22                                  // DHT 22 (AM2302)
char SERVER[] = "rtupdate.wunderground.com";           // Realtime update server - RapidFire
//char SERVER [] = "weatherstation.wunderground.com";  //standard server
char WEBPAGE [] = "GET /weatherstation/updateweatherstation.php?";
char ID [] = "xxxx";
char PASSWORD [] = "xxxx";

// Global Variables
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);
SFE_CC3000_Client client = SFE_CC3000_Client(wifi);
//RTC_DS1307 rtc;                       // Hardware RTC time -- If used
DHT dht(DHTPIN, DHTTYPE);               // DHT 22  (AM2302)
Adafruit_BMP085 bmp;                    // BMP Pressure Sensor
Adafruit_SI1145 uv = Adafruit_SI1145(); // UV Sensor
unsigned int connections = 0;           // number of connections
unsigned int timeout = 30000;           // Milliseconds -- 1000 = 1 Second

//Setup runs once at power on or reset
void setup(void){
  //Turn everything on
  Serial.begin(38400);
  Wire.begin();    //For I2C
  //rtc.begin();     //Hardware rtc
  bmp.begin();     //Pressure sensor
  dht.begin();     //Humidity Sensor
  
  // Turn the Wifi on
  Serial.print(F("\nInitializing..."));
  if ( wifi.init() ) {
    Serial.println("WiFi is go");
  } else {
    Serial.println("Something went wrong during WiFi setup");
    } 
  // Connect to WiFi network stored in non-volatile memory
  Serial.println("Connecting to network stored in profile...");
  if ( !wifi.fastConnect(timeout) ) {
    Serial.println("Error: Could not connect to network");
    }

}//End of Setup

