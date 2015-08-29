#include "LPD8806.h"
#include "SPI.h"

// Knight Rider KITT and KARR scan bar (light chaser).
// Uses a 1 metre Adafruit digital RGB LED strip with an LPD8806 chip.
// http://www.adafruit.com/products/306

/*****************************************************************************/

// Number of RGB LEDs in strand:
int nLEDs = 32;
int dataPin  = 2;
int clockPin = 3;

LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

void setup() {
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}

void scanBar(uint32_t (*getColor)(byte)) {
  int tail = 7;

  for(int i=0; i<(int)strip.numPixels()+tail; i++) {
    for(int j=0; j<=i; j++) {
      int diff = min(tail, abs(j-i));
      int brightness = (128 >> diff) - 1;
      strip.setPixelColor(min(strip.numPixels()-1, j), getColor(brightness));
    }
    
    strip.show();
    delay(20);
  }

  for(int i=strip.numPixels()-1; i>=-tail; i--) {
    for(int j=strip.numPixels()-1; j>=i; j--) {
      int diff = min(tail, abs(j-i));
      int brightness = (128 >> diff) - 1;
      strip.setPixelColor(max(0, j), getColor(brightness));
    }
    
    strip.show();
    delay(20);
  }
}

void loop() {
  for(int i = 0; i<3; i++) {
    scanBar(&kitt);
  }
  for(int i = 0; i<3; i++) {
    scanBar(&karr);
  }
}

uint32_t kitt(byte brightness) {
  return strip.Color(brightness, 0, 0);
}

uint32_t karr(byte brightness) {
  return strip.Color(brightness, brightness*0.4, 0);
}
