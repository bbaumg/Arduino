// Include all of the other libraries needed
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <Time.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

// No Specific Device Settings
#define THINKING 13

// RadioHead Configuration Tx = 12 & Rx = 11
#define ADDR_ALL 255
#define ADDR_SVR 2
#define ADDR_ME 4
//  Tx PIN = 12
//  Rx PIN = 11
RH_ASK driver;
RHReliableDatagram manager(driver, ADDR_ME);

// LED Backpack Setup
Adafruit_7segment matrix = Adafruit_7segment();

// Global Variables
uint8_t message[20];
uint8_t msgResp = 0;
uint8_t msgPlace = 0;
boolean msgSendRF = true;
uint8_t msgRespRF = 0;
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t brightness = 0;

void setup() {
  pinMode (THINKING, OUTPUT);
  digitalWrite(THINKING, true);
  Serial.begin(9600);
  //Serial.print(F("Free RAM = ")); Serial.println(freeRam());
  if (!manager.init()){
    Serial.println(F("RHDatagram Failed")); 
  } else {
    Serial.println(F("RHDatagram Initialized")); 
  }
  matrix.begin(0x70);
  matrix.setBrightness(brightness);
  Serial.println(F("Starting Program"));
  digitalWrite(THINKING, false);
}

void loop(){
  boolean drawDots = true;
  recRF();
  for(uint8_t i=0; i>=20; i++){
    message[i] = 0;
  }
  if (map(analogRead(A3),0,1023,0,15) != brightness){
    matrix.print(map(analogRead(A3),0,1023,0,15));
    matrix.writeDisplay();
    brightness = map(analogRead(A3),0,1023,0,15);
    delay(1000);
  }
  if(timeStatus() == timeNotSet) {
    matrix.printError();
    matrix.writeDisplay();
  } else {
    matrix.setBrightness(map(analogRead(A3),0,1023,0,15));
    matrix.print((hourFormat12()*100)+minute());
    if ((second() & 0x01) == 0) {
      matrix.drawColon(true);
    } else {
      matrix.drawColon(false);
    }
    matrix.writeDisplay();
  }
  delay(1);
}

void recRF(){
  if (manager.available()) {
    digitalWrite(THINKING, true);
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from)){
      Serial.println((char*)buf);
      Serial.println(buf[3]);
      //  Message = Set the time
      if((buf[0] == 1) and (buf[2] == 'T') and (buf[7] == 4)){
        Serial.println(F("Setting Time = "));
        uint32_t x;
        x = (((uint32_t)buf[3] << 24)
          + ((uint32_t)buf[4] << 16)
          + ((uint32_t)buf[5] << 8)
          + ((uint32_t)buf[6]));
        Serial.println(x);
        setTime(x);
        printTime();
      }
    } 
    digitalWrite(THINKING, false);
  }
}

void displayTime(){
  if(timeStatus() == timeNotSet) {
    matrix.printError();
    matrix.writeDisplay();
  } else {
    matrix.print((hourFormat12()*100)+minute());
    matrix.drawColon(true);
    matrix.writeDisplay();
  }
  
}

void printTime(){
  Serial.print(dayStr(weekday()));
  Serial.print(" ");
  Serial.print(monthStr(month()));
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.print(" ");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
  Serial.print("   Epoch(");
  Serial.print(now());
  Serial.println(")");
}


int freeRam(){
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
