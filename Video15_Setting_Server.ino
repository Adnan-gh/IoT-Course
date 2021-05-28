#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h> // to convert ip into name
#include <ESP8266WebServerSecure.h>  // to make server secure
const char ssid[]= "adnan";  // Access point name
const char password[]="35521081"; //Access point Password
BearSSL::ESP8266WebServerSecure SERVER (443);
const char domainname[] = "asquare";  // to store domain name // change it 
static const char cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC8jCCAlugAwIBAgIUYOIOrtubcFsmeN3TrSfpvaJoMHAwDQYJKoZIhvcNAQEL
BQAwfjELMAkGA1UEBhMCVUsxFjAUBgNVBAgMDVdFU1QgTUlETEFORFMxEzARBgNV
BAcMCkJJUk1JTkdIQU0xGjAYBgNVBAoMEUFzdG9uIFVuaXZlcnNpdHkgMQ0wCwYD
VQQLDARFRVBFMRcwFQYDVQQDDA51c2VybmFtZS5sb2NhbDAeFw0yMDAyMTUxOTQ5
MzZaFw00NzA3MDMxOTQ5MzZaMH4xCzAJBgNVBAYTAlVLMRYwFAYDVQQIDA1XRVNU
IE1JRExBTkRTMRMwEQYDVQQHDApCSVJNSU5HSEFNMRowGAYDVQQKDBFBc3RvbiBV
bml2ZXJzaXR5IDENMAsGA1UECwwERUVQRTEXMBUGA1UEAwwOdXNlcm5hbWUubG9j
YWwwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAKZPMudaME7JDhWSUhH+ZfOQ
dRZ6Pa4I1Xt0RJCb9F9PUX4vU7cyVWufoYpBlyOxzPyn1kMAzBmEQ0vAwXG0m7PU
urft8ZB/ZZallIfscgEI8HDcOVdwt/uEbfsONp7R4BAGKGhijx+niMgWLL3aoIF1
Tj7AqzjU85e/v2kPrdR5AgMBAAGjbTBrMB0GA1UdDgQWBBTAz79RjP3Mbs4Y32Ga
9p44Vycp+zAfBgNVHSMEGDAWgBTAz79RjP3Mbs4Y32Ga9p44Vycp+zAPBgNVHRMB
Af8EBTADAQH/MBgGA1UdEQQRMA+CDXVzZXJuYW1lLmxvY2EwDQYJKoZIhvcNAQEL
BQADgYEABmkelyDzfZRcqVGM8edRJM5dAQvBlHnjnsMPhvhgkG6MUisBa+vtVhKC
cOE4gAFvv8/hCaWIQwtFdWNfyle5meRX/Tj4L9N1aHASyPfTBcqKTGpjq6GiqtmL
rTINar7hi15E+adMVRRg/1VNfnBXKxh6IYch5hgeHj5TRgGwgAs=
-----END CERTIFICATE-----
)EOF";

static const char key[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----
MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAKZPMudaME7JDhWS
UhH+ZfOQdRZ6Pa4I1Xt0RJCb9F9PUX4vU7cyVWufoYpBlyOxzPyn1kMAzBmEQ0vA
wXG0m7PUurft8ZB/ZZallIfscgEI8HDcOVdwt/uEbfsONp7R4BAGKGhijx+niMgW
LL3aoIF1Tj7AqzjU85e/v2kPrdR5AgMBAAECgYBgNFpW+JYPTUDne6AcJpSlY8BH
w2jgvt13r9dl68FeTQzwOMJtrCE7w7j3uF+M13KkCRbp5ZErhZZEQPnmI7sZSkal
3TIWmLQq9g9mIscdIjc6rsfWJ7DAdpVgdsClS3sHPxv3D3RTy6Z40vzPgb2977/y
26pUHHJVEG27dqthbQJBANJPu7fm8EOUcvg1/wwnp3p+dCHEmZ7snQlUerMGbw3+
fSJTqFhrwEXYhfzj2VQUqZyaDBM+tZ9vUQU+BreM+fcCQQDKcFUVeUh6PzJJ1uSJ
F3gabOc1IoIPyotQ9RWAU70vaj/aAuIXJd6PsUt+0DucYfcg1G5YteUqJGl/5Mpv
CokPAkADNLHw2LVa4l1qSTBtGAGmjVzp0txgnsy6Aq6oIfX5aaKwrkPHrUTOC8Hn
G/YJIROAzpxWgsMz/fdnNA3YKG77AkEAiA7NsJwOMVNuKiCLAvTKHQCauKSTw6c+
0U+XfuNJIKgJeC495I7oMa1Yb0fm+KkDHoaID4lZF2TXn0SXJeBv0wJBAKhRejgb
R/KkoohDnWQCNkI9TOMPtTohGjoqMakVcaFHB+SOYhKi7iI0+coxNv/1dMydaE5X
xkjlush/Q6zlliA=
-----END PRIVATE KEY-----
)EOF";

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);  // begin serial communincation 
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

if (MDNS.begin(domainname))  // if MDND begins 
 {
  Serial.print("\n");
  Serial.print("Acess your server at https://");
  Serial.print(domainname);
  Serial.print(".local");
 }
 
 SERVER.getServer().setRSACert(new BearSSL::X509List(cert),new BearSSL::PrivateKey(key));
 SERVER.on("/", respond);
 SERVER.begin();
 Serial.print("\n");
 Serial.println("Server is now running");
}
void loop() {
  // put your main code here, to run repeatedly:

}

void respond (void)  // 
{
  
}
