#define LED 15
#define pot A0
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

pinMode(LED,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
int var;
int num;

if( Serial.available()>0)
{
  num = Serial.parseInt();
}

if (Serial.read () == '\n')
{
  Serial.print("Decimal Number : ");
  Serial.println(num,DEC);

  Serial.print("Binary Number : ");
  Serial.println(num,BIN);

  Serial.print("Hexadecimal Number : ");
  Serial.println(num,HEX);
}

var = analogRead(pot);
Serial.print("Value of Potentiometer : ");
Serial.println(var);
delay(1000);
if (num>var)
{
  digitalWrite(LED,HIGH);
}
else
{
  digitalWrite(LED,LOW);
}


}
