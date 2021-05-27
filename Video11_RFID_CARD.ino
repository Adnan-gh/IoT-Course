#include <RFID.h>  // include RFID library 
#include <SPI.h>   // SPI that is required for communicating with RFID 
#define SS_PIN 0   // slave pin
#define RST_PIN 2 
int cardpresent = 0, lastcardpresent = 0; 
RFID myrfid(SS_PIN,RST_PIN);   

void setup() // The setup function is called once a startup
{
  Serial.begin(115200);             

  SPI.begin();  // initialize the SPI
  myrfid.init(); 
}

void loop() 
{
  int i;  
  cardpresent = myrfid.isCard(); // check if card is available 
  if (cardpresent && !lastcardpresent)   // to check new card
  {
    if (myrfid.readCardSerial())  
    for (i=0; i<4; i++) 
    {
      Serial.printf("%3d ",myrfid.serNum[i]);
      }
      Serial.printf("\n");
      if (myrfid.serNum[0] == 230 && myrfid.serNum[1] == 4 && myrfid.serNum[2] == 160 && myrfid.serNum[3] == 76 )  // here add the serial of the card that you want to give access
      {
        Serial.println(" Access Granted");
      }
      else
      {
        Serial.println(" Access Denied ");
      }
    }
    lastcardpresent = cardpresent;
    myrfid.halt();

}
