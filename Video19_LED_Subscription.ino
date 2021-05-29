#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"  // add adafruit library 
#include "Adafruit_MQTT_Client.h"  // add mQTT library
#define LEDPIN      15

#define ssid          "adnan"     // change according to your wifi
#define password      "35521081"    // change according to your wifi
#define NOPUBLISH      // comment this out once publishing at less than 10 second intervals

#define ADASERVER     "io.adafruit.com"      // server webite, don't change
#define ADAPORT       1883                   // don't change 
#define ADAUSERNAME   "Adnan2023"               
#define ADAKEY        "aio_XTPb49huP5qWtUe0bONRf4xvtxCf" 

void MQTTconnect ( void );

WiFiClient client;    // create a class instance for the MQTT server


// create an instance of the Adafruit MQTT class. This requires the client, server, portm username and
// the Adafruit key
Adafruit_MQTT_Client MQTT(&client, ADASERVER, ADAPORT, ADAUSERNAME, ADAKEY);

// subscription code 
Adafruit_MQTT_Subscribe LED = Adafruit_MQTT_Subscribe(&MQTT, ADAUSERNAME "/feeds/LED");

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
  }

  #ifdef NOPUBLISH
  if ( !MQTT.ping() ) 
  {
    MQTT.disconnect();
  }
  #endif
}
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
