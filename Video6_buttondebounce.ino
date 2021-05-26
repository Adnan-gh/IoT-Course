#define button 15
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
if (checkbutton() ==1)
{
  Serial.println("Button is pressed");
}
}

int checkbutton(void)
{
int current =0;
static int last = 0;
pinMode(button,INPUT);
current = digitalRead(button);
if ( current && !last)
{
  delay(5);
  if (digitalRead(button))
  {
     pinMode(button,OUTPUT);
     last = current;
      return 1;
  }
  else
  {
      pinMode(button,OUTPUT);
      last = current;
      return 0;
  }
}
else
{
  pinMode(button,OUTPUT);
  last = current;
  return 0;
}
}
