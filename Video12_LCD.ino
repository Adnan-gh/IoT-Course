#include <rgb_lcd.h>  // include lcd library
const int red = 255, green = 255, blue = 255;
#include <RFID.h>  // include RFID library 
#include <SPI.h>   // SPI that is required for communicating with RFID 
#define SS_PIN 0   // slave pin
#define RST_PIN 2 
int cardpresent = 0, lastcardpresent = 0; 
RFID myrfid(SS_PIN,RST_PIN);   
rgb_lcd lcd;
void setup() // The setup function is called once a startup
{           
  lcd.begin(16,2);
  lcd.setRGB(red,green,blue);
  SPI.begin();  // initialize the SPI
  myrfid.init(); 
}

void loop() 
{
  int i;  
  static int counter = 0;
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
        lcd.setCursor(0,0);
        lcd.print(" Access Granted");
        lcd.setRGB(0,255,0);  // set green color 
        delay(5000);
        lcd.setRGB(red,green,blue);
        
      }
      else
      {
        lcd.setCursor(0,0);
        lcd.print(" Access Denied ");
        counter ++;
        lcd.setRGB(255,0,0);  // set red color
        lcd.setCursor(0,1);
        lcd.print("Denied = ");
        lcd.print(counter);
        delay(5000);
        lcd.setRGB(red,green,blue);
        
      }
    }
    lastcardpresent = cardpresent;
    myrfid.halt();

}
