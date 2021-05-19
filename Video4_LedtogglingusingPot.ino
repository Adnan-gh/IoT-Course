#define LED 15
#define pot A0
void setup() {
  // put your setup code here, to run once:
pinMode(LED,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
int var;
var = analogRead(pot);
digitalWrite(LED,HIGH);
delay(var);
digitalWrite(LED,LOW);
delay(var);
}
