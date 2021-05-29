#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <SimpleDHT.h>      // add temp and humidity 

#define LEDPIN      15
#define analogpin   A0
#define DHTPIN 16
float temperature = 0;
float humidity = 0;

#define ssid          "adnan"    
#define password      "35521081"    

#define NOPUBLISH      // comment this out once publishing at less than 10 second intervals

#define ADASERVER     "io.adafruit.com"     // do not change this
#define ADAPORT       1883                  // do not change this 
#define ADAUSERNAME   "Adnan2023"               // ADD YOUR username here between the qoutation marks
#define ADAKEY        "aio_vFoK44RLyGNzJW670NXPzmr3EMhw" // ADD YOUR Adafruit key here betwwen marks
void MQTTconnect ( void );
int gettemphumid ( float *temperature, float *humidity );
WiFiClient client;    // create a class instance for the MQTT server
SimpleDHT22 dht22;

// create an instance of the Adafruit MQTT class. This requires the client, server, portm username and
// the Adafruit key
Adafruit_MQTT_Client MQTT(&client, ADASERVER, ADAPORT, ADAUSERNAME, ADAKEY);


//Feeds we publish to

// Setup a feed called LED to subscibe to HIGH/LOW changes
Adafruit_MQTT_Subscribe LED = Adafruit_MQTT_Subscribe(&MQTT, ADAUSERNAME "/feeds/led");

// Setup a feed Called POT to show Potentiometer values
Adafruit_MQTT_Publish LDR = Adafruit_MQTT_Publish(&MQTT, ADAUSERNAME "/feeds/LDR");

// Setup a feed Called TEMPERATURE to show temp values
Adafruit_MQTT_Publish TEMPERATURE = Adafruit_MQTT_Publish(&MQTT, ADAUSERNAME "/feeds/temperature");

// Setup a feed Called HUMIDITY to show humi values
Adafruit_MQTT_Publish HUMIDITY = Adafruit_MQTT_Publish(&MQTT, ADAUSERNAME "/feeds/humidity");

/********************************** Main*************************************************************
 *  
 ***************************************************************************************************/
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
     
    }
  }

  // Add publishing code here
  LDR.publish(analogRead(analogpin));    /// store the result from ldr/potentiometer in temm

  if ( LDR.publish(analogRead(analogpin)))
  {
    Serial.print("Successfully Transmitted Potentiometer Values");}
    else 
    {
       Serial.print("Failed to Transimit Potentiometer Values");
    }

  gettemphumid(&temperature, &humidity);
  TEMPERATURE.publish(temperature);  
  HUMIDITY.publish(humidity); 
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

int gettemphumid(float *temperature, float *humidity)
{
  if (dht22.read2(DHTPIN, temperature, humidity, NULL) != SimpleDHTErrSuccess)
  {
    Serial.print("Failed to read DHt Sensor");
    delay(5000);
    *temperature = -999;
    *humidity = -999;
    return 0; // fall
  }
  Serial.print("Humidity: ");
  Serial.print(*humidity);
  Serial.println("RH(%)");
  Serial.print("Temperature: ");
  Serial.print(*temperature);
  Serial.println("*c");
  return 1; //success
}
