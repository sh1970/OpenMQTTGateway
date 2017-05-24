/*  
  OpenMQTTGateway Addon  - ESP8266 or Arduino program for home automation 

   Act as a wifi or ethernet gateway between your 433mhz/infrared IR signal  and a MQTT broker 
   Send and receiving command by MQTT
 
  DHT reading Addon

  Copyright: (c)1technophile

  Contributors:
  - prahjister
  - 1technophile
  
Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifdef ZsensorDHT
#include <DHT.h>
#include <DHT_U.h>
#define dht_always true // if false when the current value for temp or hum is the same as previous one don't send it by MQTT
#define TimeBetweenReading 30000
DHT dht(D3,DHT22); //on nodeMCU this is D3 GPIO0

/*----------------------------USER PARAMETERS-----------------------------*/
/*-------------DEFINE YOUR MQTT PARAMETERS BELOW----------------*/
#define HUM1   "home/433toMQTT/dht1/hum"
#define TEMP1  "home/433toMQTT/dht1/temp"

//Time used to wait for an interval before resending temp and hum
unsigned long timedht = 0;

void MeasureTempAndHum(){
  if (millis() > (timedht + TimeBetweenReading)) {//retriving value of temperature and humidity of the box from DHT every xUL
    timedht = millis();
    static float persistedh;
    static float persistedt;
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature(); 
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      trc(F("Failed to read from DHT sensor!"));
    }else{
      if(h != persistedh || dht_always){
        char hum[6];
        dtostrf(h,4,2,hum);
        trc(F("Sending Hum to MQTT"));
        trc(String(hum));
        client.publish(HUM1,hum);
       }else{
        trc(F("Same hum don't send it"));
       }
      if(t != persistedt || dht_always){
        char temp[6];
        dtostrf(t,4,2,temp);
        trc(F("Sending Temp to MQTT"));
        trc(String(temp));
        client.publish(TEMP1,temp);
      }else{
        trc(F("Same temp don't send it"));
      }
    }
    persistedh = h;
    persistedt = t;
  }
}
#endif
