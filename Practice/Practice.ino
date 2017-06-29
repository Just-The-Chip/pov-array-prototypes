// We started with Adafruit's strand test code and modified it so we could
// figure out what we were doing with the library.

// Simple strand test for Adafruit Dot Star RGB LED strip.
// This is a basic diagnostic tool, NOT a graphics demo...helps confirm
// correct wiring and tests each pixel's ability to display red, green
// and blue and to forward data down the line.  By limiting the number
// and color of LEDs, it's reasonably safe to power a couple meters off
// the Arduino's 5V pin.  DON'T try that with other code!

#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 22 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    11
#define CLOCKPIN   13
Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);

//Pattern Enumerations
#define AWESOME_PATTERN 1
#define BORING_PATTERN  2

void setup() {

  //Serial.begin(9600);
 
  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.show();  // Turn all LEDs off ASAP
}

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.
int currentPattern = AWESOME_PATTERN;

void loop() {

  switch(currentPattern) {
    case AWESOME_PATTERN : 
      awesomePatternLoop();
      break;
    case BORING_PATTERN :
    default :
      boringPatternLoop();
  }
  
  delay(60000);                        // Pause 20 milliseconds (~50 FPS)
}

uint32_t startColor = 0x880000;      // 'On' color (starts red)
uint32_t color = startColor;      // 'On' color (starts red)
boolean startEven = true;

void awesomePatternLoop() {
  color = startColor;

  //Serial.print(NUMPIXELS);

  for(int i = 0; i < NUMPIXELS; i++) {
    //uint32_t nextColor;
    if((startEven && i % 2 == 0) || (!startEven && i % 2 > 0)) {
      strip.setPixelColor(i, color);
      if((color >>= 8) == 0) {
        color = 0x880000;             //   Yes, reset to red
      }    
    } else {
      strip.setPixelColor(i, 0);
    }
  }
  strip.setPixelColor(NUMPIXELS, 0);

  if(!startEven) {
    if(startColor == 0x880000) {
      startColor = 0x000088;
    } else {
      startColor <<= 8;
    }
    
  }

  startEven = !startEven;

  strip.show();                     // Refresh strip
}

int      head  = 0, tail = -10; // Index of first 'on' and 'off' pixels

void boringPatternLoop() {
  strip.setPixelColor(head, color); // 'On' pixel at head
  strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
  strip.show();                     // Refresh strip

  if(++head >= NUMPIXELS) {         // Increment head index.  Off end of strip?
    head = 0;                       //  Yes, reset head index to start
    if((color >>= 8) == 0)          //  Next color (R->G->B) ... past blue now?
      color = 0x880000;             //   Yes, reset to red
  }
  if(++tail >= NUMPIXELS) tail = 0; // Increment, reset tail index
}

