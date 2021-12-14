/*
  0  UART
  1  UART
  2  CAN int
  
outputs
  3  LED washroom
  4  LED hallway
  5  LED ext
  6  FAN washroom
  7  nigth led
  8  ACC room

  10  SPI CAN CS
  11  SPI MOSI
  12  SPI MISO
  13  SPI CLC

inputs
 14  door_washroom
 15  door_in
 16  washroom
 17  hallway
 18
 19
 A6
 A7 random init

CAN ------------------------------------------------
CAN.sendMsgBuf(my_ID, 0, 8, stmp)
stmp[0] addres of recover
stmp[1] comand
  128 при зміні виходу по таймеру
  129 при зміні входу
stmp[2-7] data
*/

#include <SPI.h>
#include "mcp_can.h"
#define firstPin 3
#define maxPin 8 // 9-max
#define inputMin 14 //A0
#define inputMax 19 //A5
#define my_id 10 // my CAN id to send

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif
 
const int SPI_CS_PIN = 10; // CAN
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

unsigned char len = 0; // CAN recive
unsigned char buf[8]; // CAN recive
unsigned char stmp[8] = {1, 0, 0, 0, 0, 0, 0, 0}; // CAN send reciver, comand, data 4 byte, reserver 2 byte
unsigned long timer;
long timerOuts[maxPin] = {0};
long timerPeopleInWashroom = 0;
byte portbSave, portdSave;
bool outs_invert_info [maxPin] = {1,1,1,1,1,1,1};
bool PeopleInWashroom = 0;

bool inputs_previos [inputMax - inputMin];


void in0() // door_washroom
{
  if (inputs_previos [0] == 1) // відкриття дверей в туалет
  {
    ON (3, 10); // включити світло на 10 секунд
    ON (6, 3600); // включити вентилятор на 3600 секунд
  }
  if (inputs_previos [0] == 0) //закриття дверей в туалет
  {
    if (timerPeopleInWashroom > 60)
    ON (6, timerPeopleInWashroom * 2); // включити вентилятор на час який там хтось пробув * 2
    else
    ON (6, 60); // включити вентилятор на 60 секунд
    ON (3, 10); // включити світло на 10 секунд
    PeopleInWashroom = 0;
  }
}

void in1() //door in
{
  if (inputs_previos [1] == 1) // відкриття вхідних дверей
  {
    ON (4, 15); // включити світло в коридорі на 15 секунд
  }
}

void in2() // washroom PIR
{
  if (inputs_previos [0] == 0 ) // двері закриті
  {
    ON (3, 7200); // хтось у  теалеті, включити світло на 2 години
    ON (6, 180); // включити вентилятор на 3 хв
    PeopleInWashroom = 1;
  }
  else
    ON (3, 120);
}


void in3() // hallway
{
  ON (4, 120); // освітлення коридор
}

void in4()
{
  
}

void in5()
{
  
}

void in6()
{
  
}

void inputs()
{
  for (int i = inputMin; i <= inputMax; i++)
  {
    if (digitalRead (i) != inputs_previos[i - inputMin])
    {
      inputs_previos[i - inputMin] = !inputs_previos[i-inputMin];
      if ( 0 == i - inputMin) in0();
      if ( 1 == i - inputMin) in1();
      if ( 2 == i - inputMin) in2();
      if ( 3 == i - inputMin) in3();
      if ( 4 == i - inputMin) in4();
      if ( 5 == i - inputMin) in5();
      if ( 6 == i - inputMin) in6();
      stmp[0] = 1;
      stmp[1] = 129; // from inputs
      stmp[2] = i;
      CAN.sendMsgBuf(my_id, 0, 3, stmp);
    }
  }
}



void cron_1s ()
{
  for (int i=firstPin; i<= maxPin; i++)
   {
     if (timerOuts[i] > 0) timerOuts[i]--;
     if (timerOuts[i] = 0)
     {
       timerOuts[i] = -1;
       if (i<=7) PORTD ^= (1 << i);
       else 
         if (i <= 13) PORTB ^= (1 << i-8);
       stmp[0] = 1;
       stmp[1] = 128; // from timer
       stmp[2] = i;
       CAN.sendMsgBuf(my_id, 0, 3, stmp);
     } 
   }
   if (PeopleInWashroom) timerPeopleInWashroom++;
     else if (timerPeopleInWashroom > 0 ) timerPeopleInWashroom--;
 }

void ON (byte pin, long times)
{
  if (outs_invert_info[pin] == 0 )
    digitalWrite(pin, HIGH);
  if (outs_invert_info[pin] == 1 )
    digitalWrite (pin, LOW);
  timerOuts[pin] = times-1;
  Serial.print ("Pin #");
  Serial.print (pin);
  Serial.print (" is ON on seconds =");
  Serial.println (times);
}

void OFF (byte pin, long times)
{
  if (outs_invert_info[pin] == 0 )
    digitalWrite(pin, LOW);
  if (outs_invert_info[pin] == 1 )
    digitalWrite (pin, HIGH);
  timerOuts[pin] = times-1;
  Serial.print ("Pin #");
  Serial.print (pin);
  Serial.print (" is OFF on seconds =");
  Serial.println (times);
}

void invert (byte pin, long times)
{
   if (pin <= 7) PORTD ^= (1 << pin);
   else 
     if (pin <= 13) PORTB ^= (1 << pin-8);
   timerOuts[pin] = times-1;
}

void OFFAll ()
{
  portbSave = PORTB;
  portdSave = PORTD;
  for (int i = firstPin; i <= maxPin; i++)
  {
    OFF(i, -1);
  }
  
}



void ONAll ()
{
  PORTB = portbSave;
  PORTD = portdSave;
  
  
}


void setup() {

  for (int i=firstPin; i<=maxPin; i++)
  {
    pinMode (i, OUTPUT);
    digitalWrite (i, HIGH);
  };
  for (int i = 14; i <= 19; i++)
  {
    pinMode (i, INPUT);
    digitalWrite (i, HIGH);
  }
  delay (50);


  for (int i = inputMin; i <= inputMax; i++)
    inputs_previos[i - inputMin] = digitalRead (i);
     
  SERIAL.begin(9600);

  while (CAN_OK != CAN.begin(CAN_20KBPS)) {            // init can bus : baudrate = 10k
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println(" Init CAN BUS Shield again");
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");




  timer = millis();

}

void loop() {
  if (millis()- timer >= 1000)
  {
    timer = millis();
    cron_1s();
  }
  if (CAN_MSGAVAIL == CAN.checkReceive()) 
  {         // check if data coming
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned long canId = CAN.getCanId();

        SERIAL.println("-----------------------------");
        SERIAL.print("Get data from ID: 0x");
        SERIAL.println(canId, HEX);

        for (int i = 0; i < len; i++) { // print the data
            SERIAL.print(buf[i], HEX);
            SERIAL.print("\t");
        }
        SERIAL.println();
        if (buf[0] == my_id)
        {
          stmp[0] = 1;
          stmp[1] = 130; // comand from CAN
          stmp[2] = buf[1];
          stmp[3] = buf[2];
          stmp[4] = buf[3];
          CAN.sendMsgBuf(my_id, 0, 5, stmp);
          if (buf[1] == 1) // ON
            ON (buf[2],buf[3]+buf[4]*256);
          if (buf[1] == 2) // OFF
            OFF (buf[2],buf[3]+buf[4]*256);
          if (buf[1] == 3) // invert
            invert (buf[2],buf[3]+buf[4]*256);
          if (buf[1] == 4 && buf[2] == 0 && buf[3] == 0) // OFF ALL
            OFFAll();
          if (buf[1] == 5 && buf[2] == 0 && buf[3] == 0) // ON ALL
            ONAll();
        }
   }
  
  

}
