// Include all of the other libraries needed
#include <RHDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <Time.h>
#include <Wire.h>
#include "DHT.h"
#include <Adafruit_NeoPixel.h>
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
RHDatagram manager(driver, ADDR_ME);

// Sesnor confiugration
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define SENS_TEMP_PIN 1
#define SENS_MOIST_PIN 0


// LED Configuration
#define LED_PIN 6
#define LED_COUNT 60
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
  //  Preset several most common used solid colors
uint32_t off = strip.Color(0,0,0);          //Preset color variable = OFF
uint32_t r = strip.Color(255,0,0);        //Preset color variable = RED
uint32_t g = strip.Color(0,255,0);        //Preset color variable = GREEN
uint32_t b = strip.Color(0,0,255);        //Preset color variable = BLUE
uint32_t w = strip.Color(50,50,50);    //Preset color variable = WHITE
uint32_t o = strip.Color(255,75,0);        //Preset color variable = ORANGE
uint32_t p = strip.Color(255,0,255);        //Preset color variable = ORANGE


// LED Backpack Setup
Adafruit_7segment matrix = Adafruit_7segment();

// Global Variables
uint8_t message[20];  //array for holdin
uint8_t brightness = 15;
uint16_t count = 0;
uint8_t nextSend = 0;
uint16_t sendDelay = 0;

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
  dht.begin();
  strip.begin();
  strip.setBrightness(255);
  strip.show();
  matrix.begin(0x70);
  matrix.setBrightness(brightness);
  Serial.println(F("Waiting for Time Value"));
  displayTime();
  do {
    digitalWrite(THINKING, true);
    Serial.print(F("."));
    recRF();
    delay(100);
  } while(timeStatus() == timeNotSet);
  digitalWrite(THINKING, true);
  randomSeed(now());
  sendDelay = random(1000,15000);
  Serial.print(F("First Delay = "));
  Serial.println(sendDelay);
  Serial.println(F("Starting Program"));
  digitalWrite(THINKING, false);
}

void loop(){
  displayTime();
  stripTime();
  printTime();
  do {
    Serial.print(F("."));
    stripTime();
    recRF();
    delay(100);
    count += 100;
  } while  (count <= sendDelay);
  Serial.println();
  count = 0;
  //delay(sendDelay);
  sendRF(objTemp());
  delay(100);
  sendRF(objHumid());
  delay(100);
  sendRF(objMoist());
  delay(100);
  sendRF(objDry());
  uint16_t setDelay = 0;
  setDelay = 60-second();
  setDelay = setDelay*1000;
  Serial.print(F("Delay = "));
  Serial.println(setDelay);
    do {
    Serial.print(F("."));
    stripTime();
    recRF();
    delay(100);
    count += 100;
  } while  (count <= setDelay);
  Serial.println();
  count = 0;
  //delay(setDelay);
  //delay(1000);
}

void sendRF(uint8_t value){
  digitalWrite(THINKING, true);
  Serial.println(F("sendRF Device = "));
  //Serial.println(message[1]);
  //manager.sendto(message, sizeof(message), message[1]);
  digitalWrite(THINKING, false);

}

void recRF(){
  if (manager.available()) {
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    digitalWrite(THINKING, true);
    Serial.println();
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfrom(buf, &len, &from)){
      //Serial.println((char*)buf);
      //Serial.println(buf[3]);
      //  Message = Set the time
      if((buf[0] == 1) and (buf[2] == 'T') and (buf[7] == 4)){
        Serial.print(F("Setting Time = "));
        uint32_t x;
        x = (((uint32_t)buf[3] << 24)
          + ((uint32_t)buf[4] << 16)
          + ((uint32_t)buf[5] << 8)
          + ((uint32_t)buf[6]));
        Serial.print(x);
        Serial.print(F(" - "));
        setTime(x);
        printTime();
      //  Message = LED setting for individual pixel
      } else if((buf[0] == 1) and (buf[2] == 'L') and (buf[3] == 'I') and (buf[8] == 4)){
        strip.setPixelColor(buf[4],buf[5],buf[6],buf[7]);
        strip.show();
        message[1] = 240;
      //  Message = LED for entire string
      } else if((buf[0] == 1) and (buf[2] == 'L') and (buf[3] == 'S') and (buf[7] == 4)){
        for(uint8_t i=0; i <LED_COUNT; i++){
          strip.setPixelColor(i, buf[4],buf[5],buf[6]);
        }
        strip.show();
        message[1] = 240;
      //  Message = LED set a program - Not really in use yet
      } else if((buf[0] == 1) and (buf[2] == 'L') and (buf[3] == 'P') and (buf[5] == 4)){
        Serial.println("Preset LED Program");
        message[1] = 240;
      } else {
      //  Catch all for bad messages
        message[1] = 255;
      }
      message[0] = 1;
      message[2] = 4;
      //manager.sendto(message, sizeof(message), from);
    }
    for(uint8_t i=0; i>=20; i++){
      message[i] = 0;
    }
  }
  digitalWrite(THINKING, false);
}

uint8_t objDry(){
  return 0;
}
float objTemp(){
  float t = dht.readTemperature(true);
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return 255;
  } else {
    return t;
  }
//  return analogRead(SENS_TEMP_PIN) * 0.48828125;
}
uint8_t objHumid(){
  float h = dht.readHumidity();
  if (isnan(h)){
    Serial.println("Failed to read from DHT sensor!");
    return 255;
  } else {
    return h;
  }
}
uint16_t objMoist(){
  return analogRead(SENS_MOIST_PIN);
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

void stripTime(){
  strip.clear();
  int hourval = 60/12*hourFormat12();
  int preHour = hourval-2;
  int onHour = hourval-1;
  int postHour;
  if (hourval == 60){ postHour = 0;} else { postHour = hourval;}
  int minOut = minute()-1;
  strip.setPixelColor(second()-1,w);
  if (minOut==preHour){
    strip.setPixelColor(preHour,p);
    strip.setPixelColor(onHour,b);
    strip.setPixelColor(postHour,b*.1);
  } else if (minOut==onHour){
    strip.setPixelColor(preHour,b*.1);
    strip.setPixelColor(onHour,p);
    strip.setPixelColor(postHour,b*.1);
  } else if (minOut==postHour){
    strip.setPixelColor(preHour,b*.1);
    strip.setPixelColor(onHour,b);
    strip.setPixelColor(postHour,p);
  } else {
    strip.setPixelColor(preHour,b*.1);
    strip.setPixelColor(onHour,b);
    strip.setPixelColor(postHour,b*.1);
    strip.setPixelColor(minOut,r);
  }
  strip.show();
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
