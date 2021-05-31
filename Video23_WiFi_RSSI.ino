#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char ssid[] = "adnan";  // name of wifi
const char password[] = "35521081"; // password of wifi
void setup() {
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
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print("WiFi Signal Strength : ");
Serial.println(WiFi.RSSI());
delay(5000);
}
