int speakerPin = 4;
unsigned long int reg;


void setup(){
  pinMode(speakerPin, OUTPUT);
  reg = 0x55aa55aaL; //The seed for the bitstream. It can be anything except 0.
}



void loop(){ // This is where the white-noise generator does its stuff.
   generateNoise();
}




void generateNoise(){
   unsigned long int newr;
   unsigned char lobit;
   unsigned char b31, b29, b25, b24;
   b31 = (reg & (1L << 31)) >> 31;
   b29 = (reg & (1L << 29)) >> 29;
   b25 = (reg & (1L << 25)) >> 25;
   b24 = (reg & (1L << 24)) >> 24;
   lobit = b31 ^ b29 ^ b25 ^ b24;
   newr = (reg << 1) | lobit;
   reg = newr;
   digitalWrite (speakerPin, reg & 1);
   delayMicroseconds (50);
} // Changing this value changes the frequency.
