// Include all of the other libraries needed
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <Time.h>
#include <Adafruit_NeoPixel.h>

// No Specific Device Settings
#define THINKING 13

// RadioHead Configuration Tx = 12 & Rx = 11
#define ADDR_ALL 255
#define ADDR_SVR 1
#define ADDR_ME 2
//  Tx PIN = 12
//  Rx PIN = 11
RH_ASK driver;
RHReliableDatagram manager(driver, ADDR_ME);
uint8_t data[] = "";
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

// Sesnor confiugration
#define SENS_DRY 1
#define SENS_DRY_PIN 8
int varDryNow = 255;
int varDryLast = 255;
int varDryDiff = 255;
#define SENS_TEMP 2
#define SENS_TEMP_PIN 0
int varTempNow = 255;
int varTempLast = 255;
int varTempDiff = 255;
#define SENS_HUMID 3
#define SENS_HUMID_PIN 1
int varHumidNow = 255;
int varHumidLast = 255;
int varHumidDiff = 255;
#define SENS_MOIST 4
#define SENS_MOIST_PIN 2
int varMoistNow = 255;
int varMoistLast = 255;
int varMoistDiff = 255;
int varSendDelay = 0;
int varSendNext = 0;

// LED Configuration
#define LED_PIN 6
#define LED_COUNT 29
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// base color codes
uint32_t off = strip.Color(0,0,0);        //Preset color variable = OFF
uint32_t r = strip.Color(255,0,0);        //Preset color variable = RED
uint32_t g = strip.Color(0,255,0);        //Preset color variable = GREEN
uint32_t b = strip.Color(0,0,255);        //Preset color variable = BLUE
uint32_t w = strip.Color(255,255,255);    //Preset color variable = WHITE
uint32_t o = strip.Color(255,75,0);       //Preset color variable = ORANGE

void setup() {
  pinMode (THINKING, OUTPUT);
  digitalWrite(THINKING, true);
  Serial.begin(9600);
  //Serial.print("Free RAM = "); 
  //Serial.println(freeRam()); 
  if (!manager.init()){
    Serial.println("RHDatagram Failed"); 
  } else {
    Serial.println("RHDatagram Initialized"); 
  }
  //Serial.println(nextSend);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.print("Free RAM = "); 
  Serial.println(freeRam()); 
  Serial.println("Fully Initialized"); 
  digitalWrite(THINKING, false);
}

void loop(){
  //Serial.println(count);
  //if(varSendDelay >= varSendNext){
    //readSensors();
    //strip.setPixelColor(25,r);
    //strip.show();
    //varSendDelay = 0;
    //varSendNext = random(18000,19000);
    //Serial.println(nextSend);
  //} else {
    //varSendDelay++;
  //}
  recTrans();
  delay(1);
}

void recTrans(){
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      if((buf[0] == 1) and (buf[2] == 'L') and (buf[3] == 4)){
        Serial.println("True");
  
        // Send a reply back to the originator client
        data[0] = 1;
        data[1] = objTemp();
        data[2] = 4;
        Serial.println(data[1]);
        manager.sendtoWait(data, sizeof(data), from);
      }
    }
  }
}

uint8_t objTemp(){
  return analogRead(SENS_TEMP_PIN) * 0.48828125;
}

void recTrans_v1(){
  uint8_t data[] = "";
  if (manager.available()){
    digitalWrite(13, true);
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from)){
      Serial.print("Buffer = ");
      Serial.println((char*)buf);
      Serial.print("Temp = ");
      uint8_t varTemp = analogRead(SENS_TEMP_PIN) * 0.48828125;
      Serial.println(varTemp);
      data[0] = 1;
      data[1] = varTemp;
      data[2] = 4;
      Serial.println((char*)data);
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed");
    }
    
    uint32_t x;    
    if((buf[0] == 1) and (buf[1] == 'T') and (buf[6] == 4)){
      x = (((uint32_t)buf[2] << 24)
        + ((uint32_t)buf[3] << 16)
        + ((uint32_t)buf[4] << 8)
        + ((uint32_t)buf[5]));
      //Serial.println(x);
      setTime(x);
      printTime();
    } else if((buf[0] == 1) and (buf[1] == 'L')){
      Serial.println("LED Message");
    }
  }
  digitalWrite(13, false);
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

//  Used to set all LEDs in the strip to one color or to off
void all_set(uint32_t c){
  for(int i=0; i<LED_COUNT; i++){
    strip.setPixelColor(i,c);
    strip.show();
  }
}

int freeRam(){
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
