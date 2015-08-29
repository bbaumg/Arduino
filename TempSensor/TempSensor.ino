// Include all of the other libraries needed
#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

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
//int sensor = 2;

void setup() {
  Serial.begin(9600);
  if (!manager.init()){
    Serial.println("RHDatagram Failed"); 
  } else {
    Serial.println("RHDatagram Initialized"); 
  }
}

void loop(){
  tempNow = analogRead(tempPin) * 0.48828125;
  Serial.println((int)tempNow);
  tempDiff = abs(tempLast - tempNow);
  if((tempDiff <= 5) or (tempLast == 255)){
    // Send a message to manager_server
    //uint8_t buf[4];
    buf[0] = 1;
    buf[1] = SENSOR;
    buf[2] = (int)tempNow;
    buf[3] = 4;
    manager.sendto(buf, sizeof(buf), ADDR_SVR);
    tempLast = tempNow;
    Serial.println("Temp Sent");
    delay(random(18000, 19000));
  } else {
    Serial.print("Reading Delta  = ");
    Serial.println((int)tempDiff);
    tempLast = tempNow;
    delay(3000);
  }
}
