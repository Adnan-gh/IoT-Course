#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <ESP8266mDNS.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "rgb_lcd.h"
#include <RFID.h>
#include <SPI.h>

/*********************************** Hardware defines ***********************************************

 ***************************************************************************************************/
#define LEDPIN 15
#define AnalogPIN A0
#define SS_PIN 0                     //Slave select pin for SPI
#define RST_PIN 2                    //Reset pin for RFID module
/******************************* Function prototypes ************************************************

 ***************************************************************************************************/
void MQTTconnect(void);
void servepage(char a[][20], float b[], char c[][20]);

void task1 (void);
void task2 (void);
void task3 (void);
void task4 (void);

/*********************************** Network defines ************************************************

 ***************************************************************************************************/
#define ssid      "adnan"    // WiFi Name
#define password  "35521081"  // WiFi password
#define domain    "covid_monitoring"    // domain name for webpage 

/*********************************** Adafruit IO defines*********************************************

 ***************************************************************************************************/

#define ADASERVER     "io.adafruit.com"     
#define ADAPORT       8883                               // 8883 used for secure connection
#define ADAUSERNAME   "Adnan35"                          // Adafruit username
#define ADAKEY        "aio_ofLl87Mes0u6zfjC2FExuruth36q" // Adafruit key

/******************************** Global instances / variables***************************************

 ***************************************************************************************************/
#define TIMEOUTVAL 250
#define SOCKET 8888
#define BUFFERLEN 255
#define Dev_ID "MAIN GATE"
#define RFID_tag "158173114195"

#define TASK1L 100
#define TASK2L 500
#define TASK3L 1000
#define TASK4L 2000

WiFiClientSecure user;                          // WiFiClientSecure used for making MQTT much secure
WiFiClient client;                               
WiFiServer SERVER(80);                          // Here 80 specifies port number, typically used by HTTP
WiFiUDP UDP;
rgb_lcd lcd;
RFID myrfid(SS_PIN, RST_PIN);                   // constructing class for RFID tyoe 

Adafruit_MQTT_Client MQTT(&user, ADASERVER, ADAPORT, ADAUSERNAME, ADAKEY);

int red = 0, green = 128, blue = 128;           // variables for RGB backlight colour
int current = 0, last = 0;                      // variables for RFID card detection
float temp, min_temp, max_temp;

String RFID_comb;
char RFID_scan[20];
char array1[15][20];                                              //for storing RFID tag values
float array2[15];                                                 //for storing temperature
char array3[15][20];                                              //for storing Device ID
char inputbuf[BUFFERLEN] = {};                                    //for HTTP
char inBuffer[BUFFERLEN], outBuffer[BUFFERLEN];                   //for UDP
unsigned long current_time, task1LC, task2LC, task3LC, task4LC;   //for task scheduling

static const char *fingerprint PROGMEM = "59 3C 48 0A B1 8B 39 4E 0D 58 50 47 9A 13 55 60 CC A0 1D AF";

/******************************** Feeds *************************************************************

 ***************************************************************************************************/
//Feeds we publish to

//feed for Temperature value
Adafruit_MQTT_Publish TEM = Adafruit_MQTT_Publish(&MQTT, ADAUSERNAME "/feeds/TEMP");

//feed for UID (RFID tag) value
Adafruit_MQTT_Publish RFID = Adafruit_MQTT_Publish(&MQTT, ADAUSERNAME "/feeds/RFID_UID");

//feed for Device ID value
Adafruit_MQTT_Publish DevID = Adafruit_MQTT_Publish(&MQTT, ADAUSERNAME "/feeds/location");

// Setup a feed called MinTemp to subscibe to minimum temperature
Adafruit_MQTT_Subscribe MinTemp = Adafruit_MQTT_Subscribe(&MQTT, ADAUSERNAME "/feeds/Temp_lower");

// Setup a feed called MaxTemp to subscibe to maximum temperature
Adafruit_MQTT_Subscribe MaxTemp = Adafruit_MQTT_Subscribe(&MQTT, ADAUSERNAME "/feeds/Temp_upper");

/********************************** Main**************************************************************
 ***************************************************************************************************/
void setup() {
  pinMode(LEDPIN, OUTPUT);
  
  Serial.begin(115200);
  
  Serial.print("Attempting to connect to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Successfully connected to ");
  Serial.println(ssid);

  if (MDNS.begin(domain))
  {
    Serial.print("Access your server at http://");
    Serial.print(domain);
    Serial.println(".local/?25?45? where 25 and 45 are min and max temperature values for defining user programmable range.");
  }
  SERVER.begin();
  Serial.println("Server is now running");

  if (UDP.begin(SOCKET))
  {
    Serial.println("UDP Socket opened successfully");
  }
  else
  {
    Serial.println("UDP Socket opening unsuccessful");
  }

  //configuring API to use thumbprint for SSL/TLS
  user.setFingerprint(fingerprint);   
  //configuring subscriptions for min/max temperature
  MQTT.subscribe(&MinTemp);
  MQTT.subscribe(&MaxTemp);
  //Connecting to MQTT
  MQTTconnect();

  SPI.begin();                                  // initializing SPI
  myrfid.init();                                // initializing RFID module
  
  lcd.begin(16, 2);                             // initializing LCD
  lcd.setRGB(red, green, blue);                 // setting background colour
  lcd.setCursor(0, 0);
  lcd.print("Please Scan RFID");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  for (int i = 0; i < 15; i++)  //Setting up arrays for recording data to be used for webpage
  {
    strcpy(array1[i], "-xxx-");
    array2[i] = 0;
    strcpy(array3[i], "-xxx-");
  }
  //default temperature values
  min_temp = 25;
  max_temp = 45; 
}

void loop()
{
  current_time = millis();
  if ((current_time - task1LC) >= TASK1L)
  {
    task1LC = current_time;
    task1();
  }
  if ((current_time - task2LC) >= TASK2L)
  {
    task2LC = current_time;
    task2();
  }
  if ((current_time - task3LC) >= TASK3L)
  {
    task3LC = current_time;
    task3();
  }
  if ((current_time - task4LC) >= TASK4L)
  {
    task4LC = current_time;
    task4();
  }
}

void task1 (void)
{
  current = myrfid.isCard();
  if (current && !last)   //If a card is newly present
  {
    if (myrfid.readCardSerial())         //If serial/UID can be read   
    {
      RFID_comb = String(myrfid.serNum[0]) + String(myrfid.serNum[1]) + String(myrfid.serNum[2]) + String(myrfid.serNum[3]);
      RFID_comb.toCharArray(RFID_scan,BUFFERLEN);
      
      //Comparing UID bytes with stored RFID tag values
      if (!strcmp(RFID_scan,RFID_tag))     
      {
        //Temperature measurement using potentiometer (min:25 and max:45)
        temp = 25 + analogRead(AnalogPIN) * 0.01953125; 
        
        //Displaying temperature on LCD for 2 seconds
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(temp);
        lcd.print(" *C");
        delay(2000);                     

        //Publishing UID, temperature and device ID values on MQTT     
        if (RFID.publish(RFID_scan))
        {
          Serial.println("RFID tag Published successfully");
          Serial.println(RFID_scan);
        }  
        if (TEM.publish(temp))           
        {
          Serial.println("Temperature Published successfully");
          Serial.println(temp);
        } 
        if (DevID.publish(Dev_ID))
        {
          Serial.println("Device ID Published successfully");
          Serial.println(Dev_ID);
        }
       
        //Making space for new UID, temperature and Device ID values
        for (int i = 13; i >= 0; i--)     
        {
          strcpy(array1[i+1], array1[i]);
          array2[i+1] = array2[i];
          strcpy(array3[i+1], array3[i]);
        }

        //Storing new temperature, RFID tag and Device ID values  
        strcpy(array1[0], RFID_scan);
        array2[0] = temp;
        strcpy(array3[0], Dev_ID);

        //Checking measured temperature with user programmable range
        if (temp >= min_temp && temp <= max_temp) 
        {
          lcd.setCursor(0, 0);
          lcd.print("                ");
          lcd.setCursor(0, 0);
          lcd.print("Door Opening");
          Serial.println("Door Opening");
          digitalWrite(LEDPIN, HIGH);
          delay (1000);
          digitalWrite(LEDPIN, LOW);
        }
        else
        {
          lcd.setCursor(0, 0);
          lcd.print("                ");
          lcd.setCursor(0, 0);
          lcd.print("Return home");
          Serial.println("Return Home");
          delay (500);
        }
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print("Please Scan RFID");
      }
    }
  }
  last = current;    //update the last card present
  myrfid.halt();                    //halt any processing of tag data
}

void task2 (void)
{
  Adafruit_MQTT_Subscribe *subscription;                    // creating subscriber object instance
  while ( subscription = MQTT.readSubscription(2000) )      // Read a subscription and wait for 2 seconds.
  {
    if (subscription == &MinTemp)
    {
      Serial.print("Min_Temp from MQTT: ");
      Serial.println((char*)MinTemp.lastread);
      min_temp = atof((char*)MinTemp.lastread);
    }
    if (subscription == &MaxTemp)
    {
      Serial.print("Max_Temp from MQTT: ");
      Serial.println((char*)MaxTemp.lastread);
      max_temp = atof((char*)MaxTemp.lastread);
    }
  }
}

void task3 (void)
{
  int timeout = 0;
  MDNS.update();
  client = SERVER.available();
  if (client)                         //If client is connected                                                                    //wait for client to connect
  {
    Serial.println("There is a client, waiting for data");
    while (!client.available())       //While there are no characters                                   //while there are no characters
    {
      delay(1);
      timeout++;
      if (timeout >= TIMEOUTVAL)
      {
        return;                       //Return if timeout is over i.e. 250ms
      }
    }

    //Reading characters from buffer to extract min/max temperature values
    Serial.println("Reading the request");
    client.readStringUntil('?');
    client.readStringUntil('?').toCharArray(&inputbuf[0], BUFFERLEN);
    min_temp = atof(inputbuf);
    client.readStringUntil('?').toCharArray(&inputbuf[0], BUFFERLEN);
    max_temp = atof(inputbuf);

    //checking temperature values
    if (min_temp >= 25 && min_temp <= 45 && max_temp >= 25 && max_temp <= 45) 
    {
      Serial.print("Min_Temp from HTTP: ");
      Serial.println(min_temp);
      Serial.print("Max_Temp from HTTP: ");
      Serial.println(max_temp);
    }
    else
    {
      Serial.println("Temperature values out of range");
      //Setting default values
      min_temp = 25;
      max_temp = 45;       
    }

    client.flush();
    servepage(array1, array2, array3);  //Publishing recorded data on webpage 
    client.stop();
  }
}

void task4 (void)
{
  int packetsize = 0;
  packetsize = UDP.parsePacket();
  if (packetsize != 0)                   //checking for UDP packet
  {
    UDP.read(inBuffer, BUFFERLEN);       //Reading contents of packet
    inBuffer[packetsize] = '\0';
    Serial.print("Recieved ");
    Serial.print(packetsize);
    Serial.println(" bytes");
    
    Serial.println("Contents:");        
    Serial.println(inBuffer);            //Print packet contents
    Serial.print("From IP: ");
    Serial.println(UDP.remoteIP());      //Print IP of remote connection 
    Serial.print("From port ");
    Serial.println(UDP.remotePort());    //Print port that sent the packet

    if (!strcmp(inBuffer, "IPOF:MAIN GATE")) //checking content of broadcast message
    {
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      strcpy(outBuffer, "DEVICE:MAIN GATE");   //replying with username for device discovery
      UDP.write(outBuffer);
      UDP.endPacket();
    }   
  }
}

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

void servepage(char a[][20], float b[], char c[][20])
{
  int j = 0;
  String reply;

  reply += "<!DOCTYPE HTML>";                                  // start of actual HTML file

  reply += "<html>";                                           // start of html
  reply += "<head>";                                           // the HTML header
  reply += "<title>Temperature monitoring system</title>";            // page title as shown in the tab
  reply += "<style>";                                          // styling of table for displaying data
  reply += "table, th, td {border: 1px solid black; border-collapse: collapse;}";
  reply += "th, td (padding: 15px;)";
  reply += "th (text-align: left;)";
  reply += "</style>";
  reply += "</head>";
  
  reply += "<body>";                                           // start of the body
  reply += "<h1>Access Data of Employees</h1>";                // heading text
  reply += "<table>";                                          // start of table
  
  //first row of table containing heading for each column
  reply += "<tr>";
  reply += "<th>S.No (Latest value at #1)</th>";
  reply += "<th>RFID tag</th>";
  reply += "<th>Measured Temperature</th>";
  reply += "<th>Device ID</th>";
  reply += "</tr>";
  
  //displaying recorded data from arrays onto webpage
  for (j = 0; j < 15; j++)
  {
    reply += "<tr>";
    reply += "<td>"; reply += j + 1; reply += "</td>";
    reply += "<td>"; reply += a[j];  reply += "</td>";
    reply += "<td>"; reply += b[j]; reply += "</td>";
    reply += "<td>"; reply += c[j]; reply += "</td>";
  }
  reply += "</table>";                                         // end of table

  reply += "</body>";                                          // end of body
  reply += "</html>";                                          // end of HTML

  client.printf("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s", reply.length(), reply.c_str());
}
