// Include all of the other libraries needed
#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <Time.h>

// RadioHead Configuration Tx = 12 & Rx = 11
#define ADDR_ALL 0
RH_ASK driver;
RHDatagram manager(driver, ADDR_ALL);

// Global Variables
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];



void setup() {
  Serial.begin(9600);
  // Initialze RadioHead
  if (!manager.init()){ Serial.println("RHDatagram Failed"); } else { Serial.println("RHDatagram Initialized"); }

  // Initialize I/O Pins
  Serial.println("Starting Program");
}

void loop(){
  getTime();
  printTime();
  delay(1);
}

void getTime(){
  if (manager.available()){
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint32_t t;
    manager.recvfrom(buf, &len, &from);
    if ((buf[0] == 1) && (buf[1] == 'T') && (buf[6] == 4)){
      t = (((uint32_t)buf[2] << 24) 
        + ((uint32_t)buf[3] << 16) 
        + ((uint32_t)buf[4] << 8) 
        + ((uint32_t)buf[5]));
      setTime(t);
    }
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

