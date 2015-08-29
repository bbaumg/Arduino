#include <Adafruit_NeoPixel.h>

#define PIN 6
#define LED_COUNT 60

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

int photoOverride = 3;    // Used to override the photo cell sensor
int selector = A1;        // Pot to select the program to run.
int setting = 0;          // default value defined for setting the program to run.
int lightLevel = 0;       // default value defined for setting the light levels.
int brightness = 0;       // default value defined for setting the brightness level.
int hour = 0;
int minute = 0;
int second = 0;
byte hourval, minuteval, secondval; // holds the time
byte pixelColorRed, pixelColorGreen, pixelColorBlue; // holds color values


// Pot defined variables
int lightVal() { int x = 0; x = map(analogRead(A0),0,1023,0,255); return x;}  // Value of the photo cell sensor
int delayVal() { int x = 0; x = map(analogRead(A2),0,1023,10,500); return x;}  // Value of the delay pot
int brightVal() { int x = 0; x = map(analogRead(A3),0,1023,0,255); return x;}  // Value of the brightness pot

//  Preset several most common used solid colors
uint32_t off = strip.Color(0,0,0);          //Preset color variable = OFF
uint32_t r = strip.Color(255,0,0);        //Preset color variable = RED
uint32_t g = strip.Color(0,255,0);        //Preset color variable = GREEN
uint32_t b = strip.Color(0,0,255);        //Preset color variable = BLUE
uint32_t w = strip.Color(50,50,50);    //Preset color variable = WHITE
uint32_t o = strip.Color(255,75,0);        //Preset color variable = ORANGE
uint32_t p = strip.Color(255,0,255);        //Preset color variable = ORANGE

void setup() {
  strip.begin();
  pinMode(photoOverride, INPUT);
  strip.show(); // Initialize all pixels to 'off'
  hour = 8;
  minute = 21;
  second = 0;
}

void loop() {
  // First chedk the override switch for the photo cell is off, then read the photo cell level.
  if (digitalRead(photoOverride) == HIGH){ lightLevel = 0; }  else { lightLevel = lightVal(); }
  // Check if the brightness is turned all the way down.  If so use the light sensor to determin the brightness.
  if (brightVal() == 0){ strip.setBrightness(lightVal()); } else { strip.setBrightness(brightVal()); }
  keepTime();
  showTime();
  delay(1000);
}

void keepTime() {
  if (hour > 12){ hour = 1; }
  if (minute > 59 ){ minute = 0; hour++; }
  if (second > 59 ){ second = 0; minute++; } else { second++; }
}

void getTime(){
}

void showTime(){
  strip.clear();

  for(int seconds = 0; seconds <= second; seconds++){
    strip.setPixelColor(seconds,w);
  }
  
  int hourval = 60/12*hour;
  int preHour = hourval-2;
  int onHour = hourval-1;
  int postHour = hourval;
  int minOut = minute-1;
  
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

void showTime_v2(){
  char* colon = ":"; // static characters save a bit
  char* slash = "/"; // of memory

  // get time
  secondval = second;  // get seconds
  minuteval = minute;  // get minutes
  hourval = hour;  	// get hours
  if(hourval > 11) hourval -= 12; // This clock is 12 hour, if 13-23, convert to 0-11

  hourval = (hourval*60 + minuteval) / 12;  //each red dot represent 24 minutes.

  // arc mode
  for(uint8_t i=0; i<strip.numPixels(); i++) {

	if (i <= secondval) {
  	pixelColorBlue = 255;
	}
	else {
  	pixelColorBlue = 0;
	}

	if (i <= minuteval) {
  	pixelColorGreen = 255;  
	}
	else {
  	pixelColorGreen = 0;
	}

	if (i <= hourval) {
  	pixelColorRed = 255;   
	}
	else {
  	pixelColorRed = 0;
	}

	strip.setPixelColor(i, strip.Color(pixelColorRed, pixelColorGreen, pixelColorBlue));
  }

  /*
  // for serial debugging
   Serial.print(hourval, DEC);
   Serial.print(':');
   Serial.print(minuteval, DEC);
   Serial.print(':');
   Serial.println(secondval, DEC);
   */

  //display
  strip.show();
 
  // wait
}

