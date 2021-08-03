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

//Loop runs continuously
void loop(void){  
  //Lets see what time the RTC is set at! -- If RTC is used
  //DateTime now = rtc.now();

  //Get sensor data
  float tempc = bmp.readTemperature(); //Can read temp from bmp or dht sensors
  float tempf =  (tempc * 9.0)/ 5.0 + 32.0; //was dht.readTemperature, need to convert native C to F
  float humidity = dht.readHumidity(); 
  float baromin = bmp.readPressure()* 0.0002953;// Calc for converting Pa to inHg (wunderground)
  float dewptf = (dewPoint(tempf, dht.readHumidity())); //Dew point calc(wunderground) //replaced dht.readtemp with converted temp
  float UVindex = uv.readUV();
  // the index is multiplied by 100 so to get the integer index, divide by 100!
        UVindex /= 100.0; 
                            
  if (DEBUG) {   
  // Debug, or you can sit up all night watching it.
  Serial.println("+++++++++++++++++++++++++");
  /*
  //If you are using Real Time Clock (RTC)
  Serial.println("RTC TIME ");
  Serial.print("&dateutc=");
  Serial.print(now.year());
  Serial.print("-");
  Serial.print(now.month());
  Serial.print("-");
  Serial.print(now.day());
  Serial.println("+");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
  */
  Serial.print("temp= ");
  Serial.print(tempf);
  Serial.println(" *F");
  Serial.print("baro= ");
  Serial.print(baromin);
  Serial.println(" inHg");
  Serial.print("dew point= ");
  Serial.println(dewptf);
  Serial.print("humidity= ");
  Serial.println(humidity);
  Serial.print("UV: ");  
  Serial.println(UVindex);
  }//End debug loop

  //Send data to Weather Underground
 if (client.connect(SERVER, 80)) { 
    if (DEBUG) {    
      Serial.println("Sending DATA ");
      }
    // Ship it!
    client.print(WEBPAGE); 
    client.print("ID=");
    client.print(ID);
    client.print("&PASSWORD=");
    client.print(PASSWORD);
    client.print("&dateutc=");
    client.print("now");    //can use instead of RTC if sending in real time
    /*
    //If you are using Real Time Clock (RTC)
    client.print(now.year());
    client.print("-");
    client.print(now.month());
    client.print("-");
    client.print(now.day());
    client.print("+");
    client.print(now.hour()+8);// YOU MUST Add 8 hours -for pacific time- to get back to UTC or Wunderground wont show RAPID FIRE
    client.print("%3A");
    client.print(now.minute());
    client.print("%3A");
    client.print(now.second());
    */
    client.print("&tempf=");
    client.print(tempf);
    client.print("&baromin=");
    client.print(baromin);
    client.print("&dewptf=");
    client.print(dewptf);
    client.print("&humidity=");
    client.print(humidity);
    client.print("&uv=");
    client.print(UVindex);
    //client.print("&action=updateraw");//Standard update
    client.print("&softwaretype=Arduino%20UNO%20version1&action=updateraw&realtime=1&rtfreq=2.5");//Rapid Fire
    client.print(" HTTP/1.0\r\n");
  	client.print("Accept: text/html\r\n");
  	client.print("Host: ");
  	client.print(SERVER);
  	client.print("\r\n\r\n");
    client.println();
    
    if (DEBUG) {   
      Serial.println("Upload complete");
      }
      
   }//End send loop 
    else {
      if (DEBUG) { Serial.println(F("Connection failed")); }
      return;
      }
    
    delay(2500); // --If plugged in send every 2.5 seconds  
    //lpDelay(1200); // Low Power Delay. Value of 4=1sec, 40=10sec, 1200=5min --If battery, send every 5 min.

}//End loop
