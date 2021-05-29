#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define LEDPIN      15
#define ssid          "adnan"    
#define password      "35521081"    
#define NOPUBLISH      // comment this out once publishing at less than 10 second intervals

#define ADASERVER     "io.adafruit.com"     // do not change this
#define ADAPORT       1883                  // do not change this 
#define ADAUSERNAME   "Adnan2023"               // ADD YOUR username here between the qoutation marks
#define ADAKEY        "aio_tFLE71ZWG7fjyCaz7m7oVyzyz1V3" // ADD YOUR Adafruit key here betwwen marks
void MQTTconnect ( void );
WiFiClient client;    // create a class instance for the MQTT server
Adafruit_MQTT_Client MQTT(&client, ADASERVER, ADAPORT, ADAUSERNAME, ADAKEY);

// Setup a feed called LED to subscibe to HIGH/LOW changes
Adafruit_MQTT_Subscribe LED = Adafruit_MQTT_Subscribe(&MQTT, ADAUSERNAME "/feeds/led");
void setup() 
{
  
Serial.begin(115200);
Serial.print("Trying to connect to Wifi");
Serial.print(ssid);

WiFi.begin(ssid,password);

while(WiFi.status() != WL_CONNECTED)
{
  delay(500);
  Serial.print("\n");
  Serial.print("not Connected to Wifi Yet");
}
Serial.print("\n");
Serial.println("Successfully connected with Wifi");

Serial.print("Ip Address : ");
Serial.println(WiFi.localIP());
Serial.print("Mac Address of Board : ");
Serial.println(WiFi.macAddress());
Serial.print("Subnet Mask  : ");
Serial.println(WiFi.subnetMask());

Serial.print("Gateway IP  : ");
Serial.println(WiFi.gatewayIP());
  
  MQTT.subscribe(&LED);                         // subscribe to the LED feed

  pinMode(LEDPIN,OUTPUT);
}
/************************************** Loop ********************************************************
 *  
 ***************************************************************************************************/
void loop() 
{  
  MQTTconnect();

  // an example of subscription code
  Adafruit_MQTT_Subscribe *subscription;                    // create a subscriber object instance
  while ( subscription = MQTT.readSubscription(5000) )      // Read a subscription and wait for max of 5 seconds.
  {                                                         // will return 1 on a subscription being read.
    if (subscription == &LED)                               // if the subscription we have receieved matches the one we are after
    {
      Serial.print("Recieved from the LED subscription:");  // print the subscription out
      Serial.println((char *)LED.lastread);                 // we have to cast the array of 8 bit values back to chars to print
         if (strcmp("HIGH",(char *)LED.lastread) == 0)
         {
           digitalWrite(LEDPIN, HIGH);
         }
         if (strcmp("LOW",(char *)LED.lastread) == 0)
         {
          digitalWrite(LEDPIN, LOW);
         }

      // ADD code here to compare (char *)LED.lastread against "HIGH" or "LOW". Refer to previous labs 
      // for how to use STRCMP. You should use two ifs rather than an if then else structure.
      // we could also convert characters received into integer variables via the use of atoi for passing values from the 
      // the broker to the ESP8266
      
     
    }
  }

  #ifdef NOPUBLISH
  if ( !MQTT.ping() ) 
  {
    MQTT.disconnect();
  }
  #endif
}
/******************************* MQTT connect *******************************************************
 *  
 ***************************************************************************************************/
void MQTTconnect ( void ) 
{
  unsigned char tries = 0;

  // Stop if already connected.
  if ( MQTT.connected() ) 
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ( MQTT.connect() != 0 )                                        // while we are 
  {     
       Serial.println("Will try to connect again in five seconds");   // inform user
       MQTT.disconnect();                                             // disconnect
       delay(5000);                                                   // wait 5 seconds
       tries++;
       if (tries == 3) 
       {
          Serial.println("problem with communication, forcing WDT for reset");
          while (1)
          {
            ;   // forever do nothing
          }
       }
  }
  
  Serial.println("MQTT succesfully connected!");
}
