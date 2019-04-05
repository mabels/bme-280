/**********************************************
* Read temperature and pressure and humanity 
* from BME280
**********************************************/

#include <Wire.h>
#include <ESP8266WiFi.h>

#include "i2c/io.h"
#include "bme280/sensor.h"

I2C::Io i2cIo;
BME280::Sensor bme(i2cIo); // I2C

// replace with your channel’s thingspeak API key,
String apiKey = "YOUR-API-KEY";
const char* ssid = "YOUR-SSID";
const char* password = "YOUR-ROUTER-PASSWORD";
const char* server = "api.thingspeak.com";
// WiFiClient client;


/**************************  
 *   S E T U P
 **************************/
void setup() {
  Serial.begin(115200);
  Serial.println(F("BME280 test"));
  
  if (!bme.begin()) {  
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  // WiFi.begin(ssid, password);
  
  /*
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  // WiFi.begin(ssid, password);
  
  // while (WiFi.status() != WL_CONNECTED) 
  // {
  //    delay(500);
  //  Serial.print(".");
  // }
  Serial.println("");
  Serial.println("WiFi connected");  
  */
}

  /**************************  
 *  L O O P
 **************************/
void loop() {
    Serial.print("T=");
    Serial.print(bme.readTemperature());
    Serial.print(" *C");
    
    Serial.print(" P=");
    Serial.print(bme.readPressure());
    Serial.print(" Pa");

    Serial.print(" H=");
    Serial.print(bme.readHumid());
    Serial.print(" xx");

    Serial.print(" A= ");
    Serial.print(bme.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");

/*
    if (client.connect(server,80))  // "184.106.153.149" or api.thingspeak.com
    {
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(bme.readTemperature());
        postStr +="&field2=";
        postStr += String(bme.readPressure());
        postStr += "\r\n\r\n";
        postStr += String(bme.readHumid());
        postStr += "\r\n\r\n";
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);    
    }
    client.stop(); 
*/
    //every 20 sec   
    delay(2000);
}
