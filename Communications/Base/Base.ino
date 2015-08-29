// Include all of the other libraries needed
#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <Time.h>
#include <Wire.h>

// RadioHead Configuration Tx = 12 & Rx = 11
#define ADDR_ALL 0
#define ADDR_SVR 1
#define ADDR_XXX 2
RH_ASK driver;
RHDatagram manager(driver, ADDR_SVR);

// i2c Configuration
#define I2C_SVR 0
#define I2C_SLV 4

// Global Variables
char message[] = "";
int msgResp;
int msgPlace = 0;
boolean limiter = 0;
boolean msgEnd = false;
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint32_t sensors[100];


void setup() {
  Serial.begin(9600);
  //Serial.print("Free RAM = "); 
  Serial.println(freeRam()); 
  // Initialize i2c
  Wire.begin(I2C_SLV);          // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  // Initialze RadioHead
  if (!manager.init()){
    Serial.println("RHDatagram Failed"); 
  } else {
    Serial.println("RHDatagram Initialized"); 
  }
  Serial.println("Populating sensor array");
  for(int i=0; i <= 100; i++){
    sensors[i] = 255;
  }
  Serial.print("Free RAM = "); 
  Serial.println(freeRam()); 
  
  // Initialize I/O Pins
  pinMode (13, OUTPUT);
  
  Serial.println("Starting Program");
}

void loop() {
  // Debug messages
  //Serial.println(freeRam());
  
  if((message[0] == 1) && (message[msgPlace] == 4)){
    messageHandler();
    for(int i=0; i <= msgPlace; i++){
      message[i] = 0;
    }
  } else {
    recTrans();
  }
  delay(1);
}

void messageHandler(){
  Serial.print("New Message - ");
  switch (message[1]){
    case 'T':
      Serial.print("Time Update = ");
      if(msgPlace == 12){
        syncTime();
        sendTime();
        msgResp = 6;
      } else { 
        Serial.println("ERROR BAD String...  Dropping Message");
        msgResp = 7;
      }
      break;
    case 'S':
      Serial.print("Request Sensor Data - ");
      if(msgPlace == 4){
        int sensor = (((message[2]-'0') * 10) + (message[3]-'0'));
        msgResp = (int)(sensors[sensor] & 0XFF);
        Serial.print("Sensor ");
        Serial.print(sensor);
        Serial.print(" = ");
        Serial.println(msgResp);
        sensors[sensor] = 255;
      } else {
        Serial.println("ERROR BAD String...  Dropping Message");
        msgResp = 7;
      }
      break;
    default:
      Serial.println("ERROR Missing Header");
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
      char xx = (char) x;
      msgPlace ++;
      message[msgPlace] = xx;
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
      Serial.print("Sensor ");
      Serial.print(y);
      Serial.print(" Recieve = ");
      Serial.println(buf[2]);
    } else {
      Serial.println("Sensor - Bad message");
    }
  }

}

void sendTime(){
  uint32_t t = now();  //read the time
  buf[0] = 1;  // add the Start of Transmission ascii
  buf[1] = 'T';  // T for Temp
  buf[2] = (int)((t >> 24) & 0xFF) ;  // This section converts
  buf[3] = (int)((t >> 16) & 0xFF) ;  // the long interger (epoch)
  buf[4] = (int)((t >> 8) & 0XFF);    // into a 4 byte array that is put
  buf[5] = (int)((t & 0XFF));         // back together on other side
  buf[6] = 4;  // End of transmission ascii
  manager.sendto(buf, sizeof(buf), 0);  //broadcast the message
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

void syncTime(){
  //Serial.println("Starting");
  time_t pctime = 0;
  //Serial.println("Starting...");
  for(int i=2; i <= msgPlace-1; i++){
    //Serial.println(i);
    if(message[i] >= '0' && message[i] <= '9'){
      //Serial.println(message[i]);
      pctime = (10 * pctime) + (message[i] - '0') ; // convert digits to a number    
      //Serial.println(pctime);
    }
  }
  setTime(pctime);
  printTime();
}

int freeRam(){
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
