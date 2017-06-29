#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 60 // Number of LEDs in strip

// LED data and clock pins:
#define DATAPIN    11
#define CLOCKPIN   13

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);

//Pattern Enumerations
#define AWESOME_PATTERN 1
#define BORING_PATTERN  2


#define RPM 380
#define INTERVALS 60
#define IMAGEWIDTH 8
#define IMAGEHEIGHT 13
//#define HEIGHT 8
#define OFFSET 15

uint32_t colors[3] = {
  0x000000, //black
  0xff5000, //orange
  0x0000ff  //blue
};

uint32_t orange = 0xff5000; //Starting color
uint32_t blue = 0x0000ff;
// red = 0xFF0000;
// green = 0x00ff00;
// blue = 0x0000ff;
// aqua = 0x00ffff;
// teal = 0x00ffbb;
// purple = 0x6600ff;
// violet = 0xff00ee;
// pink = 0xff0090;
// orange = 0xff5000;
// yellow = 0xffff00;
// gold = 0xffbf00;


int image[IMAGEWIDTH][IMAGEHEIGHT] = {
  {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
  {1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 2, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1},
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

  if(currentInterval <= IMAGEWIDTH) {

    bool resetPixels = currentInterval == IMAGEWIDTH;
    //int column[CIRCLEWIDTH] = image[currentInterval];

    for(int i = 0; i < IMAGEHEIGHT; i++) {

      int pixelVal = resetPixels ? 0 : image[currentInterval][i];
      
      uint32_t pixelColor = colors[pixelVal];
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


