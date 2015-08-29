// Include all of the other libraries needed
#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <Time.h>

// RadioHead Configuration Tx = 12 & Rx = 11
#define ADDR_ALL 0
#define ADDR_SVR 1
#define ADDR_XXX 2
RH_ASK driver;
RHDatagram manager(driver, ADDR_SVR);

// Time Configurations
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message

void setup() {
  Serial.begin(9600);
  // Initialze RadioHead
  if (!manager.init()){ Serial.println("RHDatagram Failed"); } else { Serial.println("RHDatagram Initialized"); }
  
  //Initialize time
  Serial.println("What is the time?");
  do { syncTime(); } while (timeStatus() == timeNotSet); 
}

void loop() {
  messageHandler();
  if (minute() == 00){ Serial.println("min");}
  //sendTime();
  //Serial.println("Loop End");
  delay(100);
}

void messageHandler(){
  while(Serial.available()){
    char c = Serial.read();
    //Serial.print(c);
    if (c == '*') { 
      //Serial.println(" - new command");
      char s = Serial.read();
      switch (s) {
        case 'T':
          //do {
            syncTime();
          //} while (timeStatus() == timeNotSet);
          break;
        default:
          //Do nothing
          Serial.println(" - no case options");
      }
      //Serial.println(" - end case");
    }
    delay(100);
    //Serial.println(" - end reading serial");
  }
  //Serial.println(" - end message handler");
}

void sendTime(){
  uint8_t dtnow[] = "1416663274";
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  manager.sendto(dtnow, sizeof(dtnow), 0);
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
  // if time sync available from serial port, update time and return true
  //Serial.println("syncTime");
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of a header and ten ascii digits
    //Serial.write(13);
    //char c = Serial.read();
    //Serial.print(c);  
    //if( c == TIME_HEADER ) { Serial.read(); }
    //Serial.print(c);
    char c;
    time_t pctime = 0;
    for(int i=0; i < TIME_MSG_LEN -1; i++){
      c = Serial.read();
      if( c >= '0' && c <= '9'){   
        pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
      }
    }   
    //Serial.println("Setting Time");
    setTime(pctime);   // Sync Arduino clock to the time received on the serial port
    printTime();
  }
}

time_t requestSync()
{
  Serial.print((char)TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
