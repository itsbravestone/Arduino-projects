int DEBUG = 0;      // DEBUG counter; if set to 1, will write values back via serial

#include <Wire.h>
#include <SPI.h>
#include <SFE_CC3000.h>         //Wifi
#include <SFE_CC3000_Client.h>  //Wifi
#include "DHT.h"                //Humidity
#include <Adafruit_BMP085.h>    //Barometer
#include "Adafruit_SI1145.h"    //UV sensor
