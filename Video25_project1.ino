// Defining Libraries
#include <ESP8266WiFi.h> // for wifi
#include <WiFiUdp.h> // udp packets
#include <NTPClient.h>  // NTP for time 
#include "SimpleDHT.h" // temperature sesor
#include "rgb_lcd.h"   // lcd 
#include <RFID.h>   // rfid
#include <SPI.h>  // slave 

// Hardware defines
#define DHTPIN 16
#define LEDPIN 15
#define SS_PIN 0
#define RST_PIN 2
#define AnalogPin A0
#define socket 8888

// Defining variables
#define TASKL 10


// Variable initialization
const char ssid[] = "adnan";  // name of wifi
const char password[] = "35521081"; // password of wifi
int cardpresent=0, lastcardpresent=0;
int hr_enter=0, min_enter=0, sec_enter=0, hr_exit=0, min_exit=0, sec_exit=0;
boolean state_of_pet = 1; // pet inside = 1 and pet outside = 0
float temperature=0, average_light=0;
int pet_tag[] = {158, 173, 114, 195};

// Defining class instances
rgb_lcd lcd;
SimpleDHT22 dht22;
RFID myrfid(SS_PIN, RST_PIN);
WiFiUDP UDP;
NTPClient ntpTIME(UDP, "pool.ntp.org", 0);
// Defining function prototypes
void checkRFID(void);
void getVariables(float *a, float *b);
void displayTimes(void);

void setup()                  // the setup function is called once on startup
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

  lcd.begin(16, 2);
  lcd.setRGB(0, 255, 0); // initially green as pet is inside

  SPI.begin();
  myrfid.init();

  pinMode(LEDPIN, OUTPUT);

}

void loop()
{
  static int taskcount = 0;

  checkRFID();
 
  taskcount++;

  if (taskcount == TASKL)
  {
    taskcount = 0;
    displayTimes();
  }
  delay(100);
}

// do not change this function
void printtime ( void )
{
  ntpTIME.update();

  Serial.print(ntpTIME.getHours());
  Serial.print(":");
  Serial.print(ntpTIME.getMinutes());
  Serial.print(":");
  Serial.println(ntpTIME.getSeconds());
}

void checkRFID(void)
{
  cardpresent = myrfid.isCard();
  if (cardpresent && !lastcardpresent)
  {
    if (myrfid.readCardSerial())
    {
      if (myrfid.serNum[0] == pet_tag[0] && myrfid.serNum[1] == pet_tag[1] && myrfid.serNum[2] == pet_tag[2] && myrfid.serNum[3] == pet_tag[3])
      {
        if (state_of_pet == 0) //Pet is outside
        {
          state_of_pet = 1; //Pet returns

          //LED blink for 5 sec
          digitalWrite(LEDPIN, HIGH);
          delay (5000);
          digitalWrite(LEDPIN, LOW);

          lcd.setRGB(0, 255, 0); //LCD color changed to GREEN

          //Measurement of returning time
          ntpTIME.update();
          hr_enter = ntpTIME.getHours();
          min_enter = ntpTIME.getMinutes();
          sec_enter = ntpTIME.getSeconds();
        }
        else //Pet is inside
        {
          getVariables(&temperature, &average_light);
          if (temperature > 15 && average_light > 512) //warm weather AND day time
          {
            state_of_pet = 0; //Pet leaves

            //LED blink for 5 sec
            digitalWrite(LEDPIN, HIGH);
            delay (5000);
            digitalWrite(LEDPIN, LOW);

            lcd.setRGB(255, 0, 0); //LCD color changed to RED

            //Measurement of leaving time
            ntpTIME.update();
            hr_exit = ntpTIME.getHours();
            min_exit = ntpTIME.getMinutes();
            sec_exit = ntpTIME.getSeconds();
          }
          else //cold weather OR night time
          {
            lcd.setRGB(255, 255, 0); //LCD color changed to YELLOW for 1 sec
            delay(1000);
            lcd.setRGB(0, 255, 0);
          }
        }
      }
    }
  }
  lastcardpresent = cardpresent;
  myrfid.halt();
}

void getVariables(float *a, float *b)
{
  float x;
  while (dht22.read2(DHTPIN, a, &x, NULL) != SimpleDHTErrSuccess)
  {
    Serial.print(".");
  }
  Serial.println("Temperature measured successfully");

  int i = 1;
  float inst_light;

  for (i = 1; i <= 8; i++)
  {
    inst_light = inst_light + analogRead(AnalogPin);
    delay(5);
  }
  *b = inst_light / 8;
}

void displayTimes(void)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Left: ");
  lcd.print(hr_exit);
  lcd.print(":");
  lcd.print(min_exit);
  lcd.print(":");
  lcd.print(sec_exit);
  lcd.setCursor(0, 1);
  lcd.print("Return: ");
  lcd.print(hr_enter);
  lcd.print(":");
  lcd.print(min_enter);
  lcd.print(":");
  lcd.print(sec_enter);
}
