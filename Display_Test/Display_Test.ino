#include <Wire.h>

// No Specific Device Settings
#define THINKING 13

// 4 Digit 7 Segment LED Setup
#define SVN_BRIGHTNESS 0xE0  //Base value for brightness needs to be combined with 0-15 and bit OR (|)
#define SVN_OSCIL_OFF 0x20   //Turn the oscillator off
#define SVN_OSCIL_ON 0x21    //Turn the oscillator on
#define SVN_DISP_OFF 0x80    //Turn the display off
#define SVN_DISP_ON 0x81     //Turn the display on solid
#define SVN_DISP_2HZ 0x83    //Turn the display on and blink at 2Hz
#define SVN_DISP_1HZ 0x85    //Turn the display on and blink at 1Hz
#define SVN_DISP_HLFHZ 0x87  //Turn the display on and blink at 0.5Hz

#define SVN_ADDR 0x70

static const uint8_t numbertable[] = {
	0x3F, /* 0 */
	0x06, /* 1 */
	0x5B, /* 2 */
	0x4F, /* 3 */
	0x66, /* 4 */
	0x6D, /* 5 */
	0x7D, /* 6 */
	0x07, /* 7 */
	0x7F, /* 8 */
	0x6F, /* 9 */
};


void setup() {
  pinMode (THINKING, OUTPUT);
  digitalWrite(THINKING, true);
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(SVN_ADDR);
  Wire.write(SVN_OSCIL_ON);    //  start the oscillator
  Wire.endTransmission();
  Wire.beginTransmission(SVN_ADDR);
  Wire.write(SVN_DISP_ON);    //  display on, blink off
  Wire.endTransmission();
  Wire.beginTransmission(SVN_ADDR);
  Wire.write(SVN_BRIGHTNESS | 0);    //  brightness to maximum
  Wire.endTransmission();
  digitalWrite(THINKING, false);
}

void loop(){
  for (uint8_t b=0; b<16; b++){
    Serial.println(b);
    Wire.beginTransmission(SVN_ADDR);
    Wire.write(SVN_BRIGHTNESS | b);    //  brightness to maximum
    Wire.endTransmission();
    for (uint8_t x=0; x<9; x++){
      //Serial.println(numbertable[x]);
      Wire.beginTransmission(SVN_ADDR);
      Wire.write((uint8_t)0x00); // start at address $00
      Wire.write(numbertable[x] & 0xFF);  //1st digit
      Wire.write(0 >> 8);    //shift bits
      Wire.write(numbertable[x] & 0xFF);  //1st digit
      Wire.write(0 >> 8);    //shift bits
      Wire.write(2 & 0xFF);  //colin
      Wire.write(2 >> 8);    //shift bits
      Wire.write(numbertable[x] & 0xFF);  //1st digit
      Wire.write(x >> 8);    //shift bits
      Wire.write(numbertable[x] & 0xFF);  //1st digit
      Wire.write(x >> 8);    //shift bits
      Wire.endTransmission();  
      delay(500);
    }
  }
}
