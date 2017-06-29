// Simple strand test for Adafruit Dot Star RGB LED strip.
// This is a basic diagnostic tool, NOT a graphics demo...helps confirm
// correct wiring and tests each pixel's ability to display red, green
// and blue and to forward data down the line.  By limiting the number
// and color of LEDs, it's reasonably safe to power a couple meters off
// the Arduino's 5V pin.  DON'T try that with other code!

#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 60 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    11
#define CLOCKPIN   13

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
  
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


#define RPM 380
#define INTERVALS 60
#define CIRCLEWIDTH 8
#define OFFSET 15

uint32_t orange = 0x31004c;
//uint32_t color = 0x000000;

int image[CIRCLEWIDTH][CIRCLEWIDTH] = {
  {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
  {1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
  {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0}
};

int rotationInterval = 60000 / RPM;
int loopInterval = rotationInterval / INTERVALS;

void setup() {

  //Serial.begin(9600);
  //Serial.print(loopInterval);
 
  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.show();  // Turn all LEDs off ASAP
}

int currentInterval = 0;
void loop() {

  if(currentInterval <= CIRCLEWIDTH) {

    bool resetPixels = currentInterval == CIRCLEWIDTH;
    //int column[CIRCLEWIDTH] = image[currentInterval];

    for(int i = 0; i < CIRCLEWIDTH; i++) {

      int pixelVal = resetPixels ? 0 : image[currentInterval][i];
      
      uint32_t pixelColor = pixelVal == 1 ? orange : 0;
      //Serial.println(pixelColor);
      //Serial.println();
      strip.setPixelColor(i + OFFSET, pixelColor);
    }

    strip.show();
  }

  if(++currentInterval == INTERVALS) {
    currentInterval = 0;
  }
  
  delay(loopInterval);                        // Pause 20 milliseconds (~50 FPS)
}


