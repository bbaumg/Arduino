#include <Adafruit_NeoPixel.h>

#define PIN 6
#define LED_COUNT 30

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

// Pot defined variables
int lightVal() { int x = 0; x = map(analogRead(A0),0,1023,0,255); return x;}  // Value of the photo cell sensor
int delayVal() { int x = 0; x = map(analogRead(A2),0,1023,10,500); return x;}  // Value of the delay pot
int brightVal() { int x = 0; x = map(analogRead(A3),0,1023,0,255); return x;}  // Value of the brightness pot

//  Preset several most common used solid colors
uint32_t off = strip.Color(0,0,0);          //Preset color variable = OFF
uint32_t r = strip.Color(255,0,0);        //Preset color variable = RED
uint32_t g = strip.Color(0,255,0);        //Preset color variable = GREEN
uint32_t b = strip.Color(0,0,255);        //Preset color variable = BLUE
uint32_t w = strip.Color(255,255,255);    //Preset color variable = WHITE
uint32_t o = strip.Color(255,75,0);        //Preset color variable = ORANGE

//color combination strings
uint32_t rrrroffrrrroff[] = {r,r,r,r,off,g,g,g,g,off};
uint32_t rroffwwoffbboff[] = {r,r,off,w,w,off,b,b,off,off};
uint32_t testcolor[] = {o,o,o,o,o,off,off,off,off,off};

void setup() {
  strip.begin();
  pinMode(photoOverride, INPUT);
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // First chedk the override switch for the photo cell is off, then read the photo cell level.
  if (digitalRead(photoOverride) == HIGH){ lightLevel = 0; }  else { lightLevel = lightVal(); }
  // Check if the brightness is turned all the way down.  If so use the light sensor to determin the brightness.
  if (brightVal() == 0){ strip.setBrightness(lightVal()); } else { strip.setBrightness(brightVal()); }
  // Based on the value from the photocell/bypass switch decide to call the program selector
  if (lightLevel < 200 ) { programSelector(); } else { strip.clear(); strip.show(); }
  // Very short delay to prevent bugs.
  delay(1);
}

void programSelector(){
    setting = map(analogRead(selector), 10, 1000, 1,10);
    switch (setting) {
      case 1:
        running_v2(rrrroffrrrroff);
        break;
      case 2:
        running_v2(rroffwwoffbboff);
        break;
      case 3:
        twinkle_v1();
        break;
      case 4:
        twinkleFull_v1();
        break;
      case 5:
        delay(2000);
        flash_v1();
        break;
      case 6:
        randomWhite();
        break;
      case 7:
        fullRandomWhite();
        break;
      case 8:
        randomColors();
        break;
      case 9:
        fullRandomColors();
        break;
      case 10:
        strip.clear();
        strip.show();
        break;
      default:
        strip.clear();
        strip.show();
        break;
    }
}

void twinkle_v1(){
  uint32_t c;
  int p;
  c = strip.Color(255,random(150,255),random(50,150));
  p=random(10);
  for(int i=0; i<=LED_COUNT/10; i++){
    strip.setPixelColor(p+(i*10),c);
  }
  strip.show();
  delay(delayVal());
}

void twinkleFull_v1(){
  uint32_t c;
  int p;
  c = strip.Color(255,random(150,255),random(50,150));
  p=random(LED_COUNT);
  strip.setPixelColor(p,c);
  strip.show();
  delay(delayVal());
}

void running_v1(){
  uint32_t p;
  uint32_t s[] = {r,r,r,r,off,g,g,g,g,off};
  for(int p=0; p<10; p=p+1){
    for(int i=0; i<=LED_COUNT/10; i++){
      for(int ii=0; ii<10; ii++){
        strip.setPixelColor(p+(i*10)-ii, s[ii]);
      }
    }
    strip.show();
    delay(delayVal());
  }
}

void running_v2(uint32_t s[]){
  uint32_t p;
  for(int p=0; p<10; p=p+1){
    for(int i=0; i<=LED_COUNT/10; i++){
      for(int ii=0; ii<10; ii++){
        strip.setPixelColor(p+(i*10)-ii, s[ii]);
      }
    }
    strip.show();
    delay(delayVal());
  }
}

void surprise_v1(){
  uint32_t c[] = {o,o,o,o,off,off,off,off,off,off};
  if (random(100) != 1) {
    running_v2(c);
  } else {
    flash_v1();
  }
}

void flash_v1(){
  strip.setBrightness(255);
  for (int i=0; i<random(3,20); i++){
    all_set(w);
    delay(1);
    all_set(off);
    delay(1);
  }
}


void randomWhite(){
  int p,c;
  p=random(10);
  c=random(2);
  for(int i=0; i<=LED_COUNT/10; i++){
    switch (c) {
      case 0:
        strip.setPixelColor(p+(i*10),255,255,random(50,150));
        break;
      case 1:
        strip.setPixelColor(p+(i*10),off);
    }
  }
  strip.show();
  delay(delayVal());
}

void fullRandomWhite(){
  int p;
  p=random(LED_COUNT);
  switch (random(5)) {
    case 0:
        strip.setPixelColor(p,255,255,random(50,150));
      break;
    case 1:
        strip.setPixelColor(p,off);
  }
  strip.show();
  delay(delayVal());
}

void randomColors(){
  int p,c;
  p=random(10);
  c=random(5);
  for(int i=0; i<=LED_COUNT/10; i++){
    switch (c) {
      case 0:
        strip.setPixelColor(p+(i*10),r);
        break;
      case 1:
        strip.setPixelColor(p+(i*10),g);
        break;
      case 2:
        strip.setPixelColor(p+(i*10),b);
        break;
      case 3:
        strip.setPixelColor(p+(i*10),w);
        break;
      case 4:
        strip.setPixelColor(p+(i*10),off);  
    }
  }
  strip.show();
  delay(delayVal());
}


void fullRandomColors(){
  int p;
  p=random(LED_COUNT);
  switch (random(5)) {
    case 0:
      strip.setPixelColor(p,r);
      break;
    case 1:
      strip.setPixelColor(p,g);
      break;
    case 2:
      strip.setPixelColor(p,b);
      break;
    case 3:
      strip.setPixelColor(p,w);
      break;
    case 4:
      strip.setPixelColor(p,off);
      break;
  }
  strip.show();
  delay(delayVal());
}


//  Used to set all LEDs in the strip to one color or to off
void all_set(uint32_t c){
  for(int i=0; i<LED_COUNT; i++){
    strip.setPixelColor(i,c);
    strip.show();
  }
}
