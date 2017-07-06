#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 60 // Number of LEDs in strip
#define NUMSTRIPS 2 //Total Number of strips

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BGR);

#define RPM 250 //eventually we won't need this.
#define IMAGEWIDTH 8
#define IMAGEHEIGHT 13

#define OFFSET 0
#define RESOLUTION 3 //degrees per column in image array

// orange = 0xff5000;
// blue = 0x0000ff;
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

uint32_t colors[3] = {
  0x000000, //black
  0x441100, //orange
  0x000044  //blue
};

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

int pixelsPerStrip = NUMPIXELS / NUMSTRIPS;
int stripOffset = 360 / NUMSTRIPS;

unsigned long rps = RPM / 60;
unsigned long microPerRotation = 1000000 / rps; //microseconds per rotation, this will eventually be determined by a hall effect sensor or something
unsigned int loopInterval = microPerRotation / 360; //Time to travel 1 degree in microseconds

unsigned long startTime = 0;

void setup() {

  //Serial.begin(9600);
 
  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.show();  // Turn all LEDs off ASAP
}

void loop() {

  int curInterval = currentInterval();
  //Serial.println(curInterval);

  for(int i = 0; i < NUMSTRIPS; i++) {
    setStrip(i, curInterval);
  }

  strip.show();
}

//TODO: deal with rollover
int currentInterval() {
  unsigned long now = micros();
  long timeDiff = now - startTime;

  if(timeDiff >= microPerRotation) {
    startTime = now;
    timeDiff = 0;
  }

  return (int) (timeDiff / loopInterval);
}

void setStrip(int stripNum, int curInterval) {
  int pixelOffset = pixelsPerStrip * stripNum;
  int stripInterval = curInterval + (stripNum * stripOffset); //interval relative to what strip we are using

  if(stripInterval >= 360) {
    stripInterval -= 360;
  }

  stripInterval /= RESOLUTION;

  if(stripInterval < IMAGEWIDTH && stripInterval >= 0) {

    for(int i = 0; i < IMAGEHEIGHT; i++) {

      int pixelVal = image[stripInterval][i];
      
      uint32_t pixelColor = colors[pixelVal];
      
      strip.setPixelColor(i + pixelOffset + OFFSET, pixelColor);
    }
    
  } else {
    for(int i = 0; i < IMAGEHEIGHT; i++) {    
      strip.setPixelColor(i + pixelOffset + OFFSET, 0);
    }
  }
}


