// Include all of the other libraries needed
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <Wire.h>
#include <Time.h>

// No Specific Device Settings
#define THINKING 13
#define MAX_ARRAY_LEN = 20

// RadioHead Configuration Tx = 12 & Rx = 11
#define ADDR_ALL 255
#define ADDR_SVR 2
#define ADDR_ME 2
#define RF_TIMEOUT 400
//  Tx PIN = 12
//  Rx PIN = 11
RH_ASK driver;
RHReliableDatagram manager(driver, ADDR_ME);

// i2c Configuration
#define I2C_SVR 0
#define I2C_SLV 4

// Global Variables
uint8_t message[20];
uint8_t msgResp = 255;
uint8_t msgPlace = 0;
boolean msgReady = false;
uint8_t msgRespRF = 0;
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void setup() {
  pinMode (THINKING, OUTPUT);
  digitalWrite(THINKING, true);
  Serial.begin(9600);
  //Serial.print(F("Free RAM = ")); Serial.println(freeRam()); 
  // Initialize i2c
  Wire.begin(I2C_SLV);          // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  // Initialze RadioHead
  //manager.setTimeout(400);
  //manager.setRetries(10);
  if (!manager.init()){
    Serial.println(F("RHDatagram Failed")); 
  } else {
    Serial.println(F("RHDatagram Initialized")); 
  }
  // Initialize I/O Pins
  Serial.println(F("Starting Program"));
  digitalWrite(THINKING, false);
}

void loop() {
  if((msgReady == true)){
    Serial.println(F("--------------------------------------------------------"));
    Serial.print(F("Message = "));
    Serial.println((char*)message);
    Serial.print(F("Destination = "));
    Serial.println(message[1]);
    Serial.print(F("Command = "));
    Serial.println((char)message[2]);
    switch (message[1]){
      case 0:
        Serial.println(F("Bad Message - Dropping"));
        break;
      case ADDR_ME:
        Serial.println(F("Message for ME!"));
        if((message[2] == 'D') and (message[7] == 4)){
          uint32_t x;
          x = (((uint32_t)message[3] << 24)
            + ((uint32_t)message[4] << 16)
            + ((uint32_t)message[5] << 8)
            + ((uint32_t)message[6]));
          Serial.print(F("Message D = "));
          Serial.println(x);
          msgResp = 240;
        }
        break;
      case ADDR_ALL:
        Serial.println(F("Message for ALL!"));
        if((message[2] == 'T') and (message[7] == 4)){
          Serial.print(F("Setting Time = "));
          uint32_t x;
          x = (((uint32_t)message[3] << 24)
            + ((uint32_t)message[4] << 16)
            + ((uint32_t)message[5] << 8)
            + ((uint32_t)message[6]));
          Serial.println(x);
          setTime(x);
          printTime();
        }
        sendRF();
        msgResp = 240;
        break;
      default: 
        sendRF();
        break;
    }
    msgReady = false;
    for(uint8_t i=0; i>=20; i++){ message[i] = 0; }
  }
  delay(1);
}

void sendRF(){
  digitalWrite(THINKING, true);
  Serial.print(F("sendRF Device = "));
  Serial.println(message[1]);
  if (manager.sendtoWait(message, sizeof(message), message[1])){
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (message[1] != 255){
      uint32_t milStart = millis();
      //Serial.println(milStart);
      if (manager.recvfromAckTimeout(buf, &len, RF_TIMEOUT, &from)) {
        Serial.print(F("Time to Respond = "));
        Serial.println(millis() - milStart);
        msgResp = buf[1];
        Serial.print(F("RF Response = "));
        Serial.println(msgResp);
      } else {
        msgResp = 241;
        Serial.println(F("No RF Response"));
      }
    } else {
      Serial.println(F("Not waiting for RF Response"));
      msgResp = 240;
    }
  } else {
    Serial.println(F("No RF ACK"));
    msgResp = 242;
  }
  Serial.println(F("End - sendRF")); 
  digitalWrite(THINKING, false);
}

void receiveEvent(int howMany){
  digitalWrite(THINKING, true);
  //Serial.println(F("Start - receiveEvent()"));
  if(howMany == 1){
    int x = Wire.read();    // receive byte as an integer
    if (x == 1){
      msgPlace = 0;
      message[0] = 1;
    } else if(x == 4){
      msgPlace ++;
      message[msgPlace] = x;
      msgReady = true;
    } else {
      msgPlace ++;
      message[msgPlace] = x;
    }
  } else {
    Wire.begin();
  }
  //Serial.println(F("End - receiveEvent()"));
  digitalWrite(THINKING, false);
}

void requestEvent(){
  digitalWrite(THINKING, true);
  Wire.write(msgResp);
  msgResp = 255;
  msgPlace = 0;
  digitalWrite(THINKING, false);
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
