#include <Wire.h>
 
#define SLAVE_ADDRESS 0x04

void setup() {
 pinMode(13, OUTPUT);
 Serial.begin(9600);
 
 // initialize i2c as slave
 Wire.begin(SLAVE_ADDRESS);
 
 // define callbacks for i2c communication
 Wire.onReceive(receiveEvent);
 Wire.onRequest(requestEvent);
}

void loop() {
 delay(100);
}

void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}

void requestEvent()
{
  Wire.write("hello "); // respond with message of 6 bytes
                       // as expected by master
                       
  //Serial.println("hello ");
  
  
  //Wire.write("123456789 ");
  //x++;
}
