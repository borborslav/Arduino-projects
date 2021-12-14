/*
 * For led lamps Z-Light
 */

#include <IRremote.h>

#define PERIOD_1 60000    // період переключення світильникі
#define max_leds 5  // 1 to 16

unsigned long timer_1;
long randNumber;
int RECV_PIN = 2;
int rec, timer, number = 0 ;
byte flag = 0;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  randomSeed(analogRead(7));
  for (int i=3; i<=19; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite (i,HIGH);
  }
  digitalWrite (3,LOW);    
  for (int i=3; i<=max_leds+2; i++)
  {
    delay (500);
    digitalWrite (i,HIGH);
    digitalWrite (i+1,LOW);
  }
  digitalWrite (max_leds+3,HIGH);
  timer_1 = millis();
  digitalWrite (3,LOW); // 
}

void loop() {
  if (irrecv.decode(&results)) 
  {
    rec = results.value;
    Serial.println(rec);
    timer = 50;
    if (rec == -23971) number = number*10+1;
    if (rec == 25245) number = number*10+2;
    if (rec == -7651) number = number*10+3;
    if (rec == 8925) number = number*10+4;
    if (rec == 765) number = number*10+5;
    if (rec == -15811) number = number*10+6;
    if (rec == -8161) number = number*10+7;
    if (rec == -22441) number = number*10+8;
    if (rec == -28561) number = number*10+9;
    if (rec == -26521) number = number*10; // 0
    if (rec == 26775) number = 0; // *
    if (rec == -20401) flag = 1;  // #
    if (number >= 1000) number = 0 ;
    
    Serial.println(number);
    irrecv.resume(); // Receive the next value
  }
  delay (50);
  if (timer >= 0) timer--;
  if (timer = 0) 
  {
    number = 0;
    flag = 1;
  }
  if (flag)
  {
    for (int i=3; i < max_leds+3; i++)
    digitalWrite(i, HIGH);
    if (number <= max_leds+3) digitalWrite(number+3, LOW); //19 A7 in random 
    timer_1 = millis();
    flag = 0;
    number = 0;
  }
  if (millis() - timer_1 >= PERIOD_1) 
  {   
    timer_1 = millis();                   
    for (int i=3; i < max_leds+3; i++)  
    digitalWrite(i, HIGH);
    digitalWrite(random(3,max_leds+3), LOW); // [3,19)
  }
  
}
