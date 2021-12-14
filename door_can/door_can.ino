
#include <SPI.h>
#include "mcp_can.h"
#include <avr/wdt.h>
#include <SmartDelay.h>
#include <EEPROM.h>

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
MCP_CAN CAN(SPI_CS_PIN);    
unsigned char len = 0;
unsigned char buf[8]; // recive buf
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // send buf


SmartDelay delay10(10000UL); // в микросекундах
SmartDelay delay_openclose(500000UL); // в микросекундах максимальний час відкривання або закривання

byte door_status;


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
