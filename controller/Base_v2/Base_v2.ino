// Include all of the other libraries needed
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
//#include <Time.h>
#include <Wire.h>


// RadioHead Configuration Tx = 12 & Rx = 11
#define ADDR_ALL 255
#define ADDR_SVR 1
#define ADDR_ME 1
//  Tx PIN = 12
//  Rx PIN = 11
RH_ASK driver;
RHReliableDatagram manager(driver, ADDR_ME);

// i2c Configuration
#define I2C_SVR 0
#define I2C_SLV 4

// Global Variables
uint8_t message[RH_ASK_MAX_MESSAGE_LEN];
uint8_t msgResp;
uint8_t msgPlace = 0;
boolean limiter = 0;
boolean msgEnd = false;
uint32_t sensors[100];
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void setup() {
  Serial.begin(9600);
  Serial.print(F("Free RAM = ")); Serial.println(freeRam()); 
  // Initialize i2c
  Wire.begin(I2C_SLV);          // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  // Initialze RadioHead
  if (!manager.init()){
    Serial.println(F("RHDatagram Failed")); 
  } else {
    Serial.println(F("RHDatagram Initialized")); 
  }
  Serial.println("Populating sensor array");
  for(int i=0; i <= 100; i++){
    sensors[i] = 255;
  }
  Serial.print(F("Free RAM = ")); Serial.println(freeRam());  
  for(int i=0; i <= 50; i++){
    message[i] = 0;
  }
  Serial.print(F("Free RAM = ")); Serial.println(freeRam()); 
  
  // Initialize I/O Pins
  pinMode (13, OUTPUT);
  
  Serial.println(F("Starting Program"));
}

void loop() {
  // Debug messages
  if((message[0] == 1) && (message[msgPlace] == 4)){
    Serial.print(F("Free RAM = ")); Serial.println(freeRam());
    Serial.println((char*)message);
    sendRF();
    Serial.print(F("Free RAM = ")); Serial.println(freeRam());
    for(int i=0; i <= 50; i++){
      message[i] = 0;
    }
    Serial.print(F("Free RAM = ")); Serial.println(freeRam());
  } else {
    //recTrans();
  }
  delay(1);
}

void sendRF(){
  Serial.println("Sending to ask_reliable_datagram_server");
    
  // Send a message to manager_server
  if (manager.sendtoWait(message, sizeof(message), 2))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      Serial.println(buf[1], HEX);
    }
    else
    {
      Serial.println("No reply, is ask_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
}

void sendRF_V1(){
  Serial.println(F("sendRF Called"));
  uint8_t varTo = message[1];
  Serial.println(varTo);
  Serial.println((char*)message);
  if(manager.sendtoWait(message, sizeof(message), varTo)){
    uint8_t len = sizeof(buf);
    uint8_t from;
    Serial.println(F("Sending RF Message"));
    if (manager.recvfromAckTimeout(buf, &len, 3000, &from)){
      Serial.print(F("got reply from : 0x"));
      Serial.print(from, HEX);
      Serial.print(F(": "));
      Serial.println((char*)buf);
    } else {
      Serial.println(F("No reply, is ask_reliable_datagram_server running?"));
    }
  } else {
    Serial.println(F("sendtoWait failed"));
  }
}

void receiveEvent(int howMany){
  digitalWrite(13, true);
  if(howMany == 1){
    int x = Wire.read();    // receive byte as an integer
    if (x == 1){
      msgPlace = 0;
      message[0] = 1;
    } else {
      msgPlace ++;
      message[msgPlace] = (char) x;
    }
  } else {
    Wire.begin();
  }
  digitalWrite(13, false);
}

void requestEvent(){
  Wire.write(msgResp);
  msgResp = 0;
}

void recTrans(){
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  if (manager.available()){
    uint8_t len = sizeof(buf);
    uint8_t from;
    manager.recvfrom(buf, &len, &from);
    
    // make sure it is a valid message
    if((buf[0] == 1) and (buf[3] == 4)){
      // take the message and pack it into 3 bytes of data as a single 32bit.
      uint32_t x;
      x = (((uint32_t)from << 16)
        + ((uint32_t)buf[1] << 8)
        + ((uint32_t)buf[2]));
      //sensors[1] = x;
      
      // Define the aray ID = # of sensor & # of measurement
      uint8_t y;
      y = (from * 10) + (buf[1]);
      sensors[y] = x;
      Serial.print(F("Sensor "));
      Serial.print(y);
      Serial.print(F(" Recieve = "));
      Serial.println(buf[2]);
    } else {
      Serial.println(F("Sensor - Bad message"));
    }
  }

}

int freeRam(){
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
