//  * +12         GNG
//  sign_open   sign_close
//  button_open button_close
//  led_open    led_close
//  motor1      motor2

#include <SPI.h>
#include "mcp_can.h"
#include <avr/wdt.h>
#include <SmartDelay.h>
// #include <EEPROM.h>

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

#define sign_close 5
#define sign_open 4
#define button_close 6
#define button_open 7
#define motor1 2
#define motor2 3
#define led_open 9
#define led_close 8
#define closeOK 9
#define openOK 10
const int delay_const = 500; // час відкривання і закримання замка
const byte chk = 3; // кількість разів перевірки натискання кнопки
const int SPI_CS_PIN = 9;
const unsigned char id = 0x02; // ID This devise !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin
byte priority = 255;
byte st[] = {0,0,0,0};
byte door = 0; // стан замка 
byte door_status = 0; //0-невідомо 1-закрито 2-закрито з помилкою 3-відкрито 4-відкрито з помилклю 5-знято блокування ще не відкрито 6-заблоковано
//                    7-заблоковано з помилкю 8-заблоковано у відкритому стані
//
// bool stop_motor = 0;

unsigned char len = 0;
unsigned char buf[8]; // recive buf
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // send buf


SmartDelay delay10(10000UL); // в микросекундах
SmartDelay delay_openclose(500000UL); // в микросекундах максимальний час відкривання або закривання

void close_door()
{
  bool stop_motor = 0;
  if (door != 1 || door != 8)
  {
  digitalWrite (motor1, LOW);
  digitalWrite (motor2, HIGH);
  while (delay_openclose.Now()== 0 && (stop_motor = 0));
  {
    if (digitalRead (closeOK) == LOW) {
      stop_motor = 1 ;
      door = 1; // двері закрилися 
    }
  }
//  delay (delay_const);
  digitalWrite (motor1, LOW);
  digitalWrite (motor2, LOW);
  digitalWrite (led_open, LOW);
  if (door = 1){
    digitalWrite (led_close, HIGH);
    stmp[0] = 0x03; // sent to 0x03
    stmp[1] = 0x01; // send data 0x01
    len = 2;
    door_status = 1;
    CAN.sendMsgBuf(id, 0, len, stmp); // двері закрито все ок
  }

  if (door = 0){
    stmp[0] = 0x03;
    stmp[1] = 0x02;
    len = 2;
    door_status = 2;
    CAN.sendMsgBuf(id, 0, len, stmp); // була спроба закрити двері, але замок не спрацював
  }
  
//  door = 1;
  
  }  
}

void open_door()
{
  bool stop_motor = 0;
  if (door != 2)
  {
  digitalWrite (motor1, HIGH);
  digitalWrite (motor2, LOW);
  while (delay_openclose.Now()== 0 && (stop_motor = 0));
  {
    if (digitalRead (openOK) == LOW) {
      stop_motor = 1 ;
      door = 2; // двері відкрилися
    }
  }
//  delay (delay_const);
  digitalWrite (motor1, LOW);
  digitalWrite (motor2, LOW);  
  digitalWrite (led_close, LOW);
 if (door = 2){
    digitalWrite (led_open, HIGH);
    stmp[0] = 0x03; // sent to 0x03
    stmp[1] = 0x03; // send data 0x01
    len = 2;
    door_status = 3;
    CAN.sendMsgBuf(id, 0, len, stmp); // двері відкрито все ок
  }

  if (door = 0){
    stmp[0] = 0x03;
    stmp[1] = 0x04;
    len = 2;
    door_status = 4;
    CAN.sendMsgBuf(id, 0, len, stmp); // була спроба відкрити двері, але замок не спрацював

  }

  }
}


void setup() {
  wdt_enable(WDTO_8S); // Сторожовий таймер на 8 секунд
  SERIAL.begin(9600);
  // put your setup code here, to run once:
  pinMode (sign_close, INPUT);
  pinMode (sign_open, INPUT);
  pinMode (button_open, INPUT);
  pinMode (button_close, INPUT);
  pinMode (led_close, OUTPUT);
  pinMode (led_open, OUTPUT);
  digitalWrite (sign_close, HIGH);
  digitalWrite (sign_open, HIGH);
  digitalWrite (button_open, HIGH);
  digitalWrite (button_close, HIGH);
  pinMode (motor1, OUTPUT);
  pinMode (motor2, OUTPUT);
  digitalWrite (motor1, LOW);
  digitalWrite (motor2, LOW);
  while (CAN_OK != CAN.begin(CAN_20KBPS)) {            // init can bus : baudrate = 500k
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println(" Init CAN BUS Shield again");
        delay(100);
    }

  
}

void loop() {
  if (delay10.Now())
  {
  // put your main code here, to run repeatedly:
if (digitalRead (sign_close) == LOW) st[0] = st[0]+1; else st[0] = 0;
if (digitalRead (sign_open) == LOW) st[1] = st[1]+1; else st[1] = 0;
if (digitalRead (button_close) == LOW) st[2] = st[2]+1; else st[2] = 0;
if (digitalRead (button_open) == LOW) st[3] = st[3]+1; else st[3] = 0;
for (int i=0; i<4; i++)
if (st[i] > 254) st[i] = chk + 1; // без коментарів 2,00 ночі
  } // end delay10
if (st[0] > chk)
{
  close_door ();
  priority = 0;
}
if (st[1] > chk && st[0] == 0)
{
  open_door ();
  priority = 255;
}
if (st[2] > chk && st[1] == 0)
{
  close_door ();
}
if (st[3] > chk && st[2] == 0 && st[0] == 0 && priority == 255)
{
  open_door ();
}

// -------------------------- resive from CAN ----------------------------

  if (CAN_MSGAVAIL == CAN.checkReceive()) {         // check if data coming
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
        if (buf[0] = id) switch (buf[1])
        {
          case 0x01 : if (st[2] == 0 && st[0] == 0 && priority == 255)
            {
              open_door ();
            }
          break;
          case 0x02 : if (st[1] == 0)
            {
               close_door ();
            }
          break;
          case 0x03 : if (st[0] == 0)
            {
                open_door ();
                priority = 255;
            }
          break;
          case 0x04 : if (st[0] == 0)
            {
                 close_door ();
                 priority = 0;
            }
          break;
          case 0x05 : 
            {
                 
            }
          break;
          case 0x06 : // power error
            {
                 digitalWrite (led_close, LOW);
                 digitalWrite (led_open, LOW);
            }
          break;
          
        }
        
        
  }







wdt_reset();
}
