#include <rgb_lcd.h>  // include lcd library to display text
#define button 15
#define TASK1L 5
#define TASK2L 350
#define TASK3L 2000
#define TASK4L 5000


int timespressed = 0;
static int total =0;
const int red =0,green =0, blue =255; // variables used to tune the color of led display.
rgb_lcd lcd; // instance for rgb lcd

int checkbutton (void);
void task1 (void);
void task2 (void);
void task3 (void);
void task4 (void);


void setup() // The setup function is called once a startup
{


 lcd.begin(16, 2);  // initialize the lcd
 lcd.setRGB(red,green,blue);  // set lcd display
 pinMode(button,OUTPUT);
}

void loop() // the loop function is called repeatly
{
unsigned long current_millis = 0;
static unsigned long task1LC = 0,  task2LC =0, task3LC =0, task4LC =0;
current_millis = millis();

if ((current_millis-task1LC)>= TASK1L) // to check task 1 
{
  task1();
  task1LC = current_millis;
}
if ((current_millis-task2LC)>= TASK2L)   // to check task 2
{
  task2();
  task2LC = current_millis;
}

if ((current_millis-task3LC)>= TASK3L)   // to check task 3
{
  task3();
  task3LC = current_millis;
}
if ((current_millis-task4LC)>= TASK4L)   // to check task 3
{
  task4();
  task4LC = current_millis;
  total = 0;
}
}


int checkbutton (void)   // funtion to check button state
{
  int current;    // initialize current variable to check the current state of button
  static int last = 0; // initialize last variable to check the current state of bu
  pinMode(button, INPUT);  // button will take input
  current = digitalRead(button);    // read the current state of button 
  if (current && !last)   // if current state and complement of last state are true
  {
    delay (5);   // wait 5 milli second 
  current = digitalRead(button);  // rread the state of button state again
    if (current)  // if button new state is high or greater than 0 
    {
      pinMode(button,OUTPUT);  // set button as output
      last = current; // last button state is equal to current state
      return 1;       // return 1 for high state of button
    }
    else {      // if current state and complemet of last state are not true 
      pinMode(button,OUTPUT); // set button as output
      last = current;   // current state eqquals to last
    return 0; }  // return 0 because both states are diff
  } 
  else {     // if current state and complemet of last state are not true 
      pinMode(button,OUTPUT);  // set button as output
      last = current;  // current state eqquals to last
      return 0;  // current state eqquals to last
    }
  }

void task1 (void)  // function to count the increment of function
{
  if (checkbutton())
  {
    timespressed ++;
    total++;
    }
}

void task2 (void) // to blink on and off led
{
  static int currentstate = 0;
  currentstate = digitalRead(button);
  if (currentstate)
  {
     digitalWrite(button,LOW);
    }
    else 
    {
      digitalWrite(button,HIGH);
    } 
}
void task3(void)  // update the value of timespressed on lcd 
{
  lcd.setCursor(0,0); 
  lcd.print(timespressed);
  }

  void task4(void)  // update the value of timespressed on lcd 
{ 
  float  average;
  average = total/5.0;
  lcd.setCursor(0,1); 
  lcd.print("Averge : ");
  lcd.print(average);
  }
