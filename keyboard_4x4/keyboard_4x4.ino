#include <SPI.h>
#include "mcp_can.h"


/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif

const int SPI_CS_PIN = 10;
const unsigned char id = 0x03; // ID This devise !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

int PinOut[4] {5, 4, 3, 2}; // пины выходы
 
int PinIn[4] {9, 8, 7, 6}; // пины входа
//int val = 0;
const char value[4][4]
 
 
{ {'D', '#', '0', '*'},
  {'C', '9', '8', '7'},
  {'B', '6', '5', '4'},
  {'A', '3', '2', '1'}
};
// двойной массив, обозначающий кнопку
 
char key_val ;
boolean key_pres = 0;
boolean key_get = 1;
//int b = 0; // переменная, куда кладется число из массива(номер кнопки)
 
void setup()
{
  pinMode (2, OUTPUT); // инициализируем порты на выход (подают нули на столбцы)
  pinMode (3, OUTPUT);
  pinMode (4, OUTPUT);
  pinMode (5, OUTPUT);
 
  pinMode (6, INPUT); // инициализируем порты на вход с подтяжкой к плюсу (принимают нули на строках)
  digitalWrite(6, HIGH);
  pinMode (7, INPUT);
  digitalWrite(7, HIGH);
  pinMode (8, INPUT);
  digitalWrite(8, HIGH);
  pinMode (9, INPUT);
  digitalWrite(9, HIGH);
 
  Serial.begin(9600); // открываем Serial порт

while (CAN_OK != CAN.begin(CAN_20KBPS)) {            // init can bus : baudrate = 500k
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println(" Init CAN BUS Shield again");
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");

  
}
 
void matrix () // создаем функцию для чтения кнопок
{
  for (int i = 1; i <= 4; i++) // цикл, передающий 0 по всем столбцам
  {
    digitalWrite(PinOut[i - 1], LOW); // если i меньше 4 , то отправляем 0 на ножку
    for (int j = 1; j <= 4; j++) // цикл, принимающих 0 по строкам
    {
      if (digitalRead(PinIn[j - 1]) == LOW) // если один из указанных портов входа равен 0, то..
      {
 //       Serial.println( value[i - 1][j - 1]); // то b равно значению из двойного массива

        key_val = (value[i - 1][j - 1]) ;
        key_pres = 1;
      }
    }
    digitalWrite(PinOut[i - 1], HIGH); // подаём обратно высокий уровень
  }
}
 

unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
// unsigned char stmp1[8] = {'b', 'k', 's', 0, 0, 0, 0, 0};
// byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
// byte data1[1] = {77};



void loop()
{
  unsigned char len = 0; // recive length 
  unsigned char buf[8]; // resive buf
  matrix(); // используем функцию опроса матричной клавиатуры
  if (key_pres & key_get) 
   {
    Serial.println (key_val);
    key_get = 0;
    stmp[0] = key_val ;
    CAN.sendMsgBuf(id, 0, 1, stmp);
   }
  if (!key_pres) 
   {
    key_get = 1;
   }
  key_pres = 0;
  stmp[1] = key_val ;

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
  }



//  delay (100);
}
