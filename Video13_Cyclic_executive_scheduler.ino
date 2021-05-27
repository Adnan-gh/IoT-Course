#include <rgb_lcd.h>  // include lcd library to display text
#define button 15
#define TASK1L 1  // 5ms 
#define TASK2L 70  // 350 ms 
#define TASK3L 400 // 2000ms

int timespressed = 0;  // to store number of button count
const int red =0,green =0, blue =255; // variables used to tune the color of led display.
rgb_lcd lcd; // instance for rgb lcd

int checkbutton (void);
void task1 (void);
void task2 (void);
void task3 (void);


void setup() // The setup function is called once a startup
{


 lcd.begin(16, 2);  // initialize the lcd
 lcd.setRGB(red,green,blue);  // set lcd display
 pinMode(button,OUTPUT);
}

void loop() // the loop function is called repeatly
{

static int task1count = 0, task2count = 0, task3count = 0;
task1count++;
task2count++;
task3count++;

if (task1count == TASK1L ) // to check task 1 
{
  task1();
  task1count = 0;
}
if (task2count == TASK2L ) 
{
  task2();
  task2count = 0;
}

if (task3count == TASK3L ) // to check task 1 
{
  task3();
  task3count = 0;
}
delay(5);
}


int checkbutton (void)   // funtion to check button state
{
  int current;    
  static int last = 0; 
  pinMode(button, INPUT);  
  current = digitalRead(button);   
  if (current && !last)  
  {
    delay (5);  
  current = digitalRead(button); 
    if (current)   
    {
      pinMode(button,OUTPUT);  
      last = current; 
       return 1;      
    }
    else {      
      pinMode(button,OUTPUT); 
      last = current;   
    return 0; }  
  } 
  else {     
      pinMode(button,OUTPUT);
      last = current;  
      return 0;  
    }
  }

void task1 (void)  // function to count the increment of function
{
  if (checkbutton())
  {
    timespressed ++;
    
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
