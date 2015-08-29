// Include all of the other libraries needed
#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <Time.h>

// RadioHead Configuration Tx = 12 & Rx = 11
#define ADDR_SVR 1
#define ADDR_ME 2
#define SENSOR 2

RH_ASK driver;
RHDatagram manager(driver, ADDR_ME);
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

float tempNow;
float tempDiff = 255;
float tempLast = 255;
int tempPin = 0;
int count = 0;
int nextSend = 0;

void setup() {
  Serial.begin(9600);
  if (!manager.init()){
    Serial.println("RHDatagram Failed"); 
  } else {
    Serial.println("RHDatagram Initialized"); 
  }
  pinMode (13, OUTPUT);
  //Serial.println(nextSend);
}

void loop(){
  //Serial.println(count);
  if(count >= nextSend){
    sendTemp();
    count = 0;
    if(tempDiff > 5){
      nextSend = 1000;
    } else {
      nextSend = random(18000,19000);
    }
    //Serial.println(nextSend);
  } else {
    count++;
  }
  recTrans();
  delay(1);
}

void recTrans(){
  if (manager.available()){
    digitalWrite(13, true);
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint32_t x;
    manager.recvfrom(buf, &len, &from);
    if((buf[0] == 1) and (buf[1] == 'T') and (buf[6] == 4)){
      x = (((uint32_t)buf[2] << 24)
        + ((uint32_t)buf[3] << 16)
        + ((uint32_t)buf[4] << 8)
        + ((uint32_t)buf[5]));
      //Serial.println(x);
      setTime(x);
      printTime();
    }
  }
  digitalWrite(13, false);
}

void sendTemp(){
  tempNow = analogRead(tempPin) * 0.48828125;
  tempDiff = abs(tempLast - tempNow);
  if((tempDiff <= 5) or (tempLast == 255)){
    // Send a message to manager_server
    buf[0] = 1;
    buf[1] = SENSOR;
    buf[2] = (int)tempNow;
    buf[3] = 4;
    manager.sendto(buf, sizeof(buf), ADDR_SVR);
    tempLast = tempNow;
    Serial.print("Temp Sent = ");
    Serial.println((int)tempNow);
  } else {
    Serial.print("Reading Delta  = ");
    Serial.println((int)tempDiff);
    tempLast = tempNow;
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

