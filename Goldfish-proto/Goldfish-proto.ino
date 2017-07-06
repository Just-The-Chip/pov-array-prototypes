#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 60 // Number of LEDs in strip

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BGR);

#define RPM 250 //eventually we won't need this.
#define IMAGEWIDTH 8
#define IMAGEHEIGHT 13

#define OFFSET 0
#define RESOLUTION 3 //degrees per column in image array

uint32_t colors[3] = {
  0x000000, //black
  0x441100, //orange
  0x000044  //blue
};

//orange = 0xff5000; //Starting color
//blue = 0x0000ff;
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

unsigned long rps = RPM / 60;
unsigned long microPerRotation = 1000000 / rps; //microseconds per rotation, this will eventually be determined by a hall effect sensor or something
unsigned int loopInterval = microPerRotation / 360; //Time to travel 1 degree in microseconds

//TODO: loopInterval should represent 1 degree for precision and then we can do some math to divide the current interval by 3 ro something

unsigned long startTime = 0;

void setup() {

  //Serial.begin(9600);
 
  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.show();  // Turn all LEDs off ASAP
}

//int currentInterval = 0;
void loop() {

  int curInterval = currentInterval() / RESOLUTION;
  //Serial.println(curInterval);

  if(curInterval < IMAGEWIDTH && curInterval >= 0) {

    for(int i = 0; i < IMAGEHEIGHT; i++) {

      int pixelVal = image[curInterval][i];
      
      uint32_t pixelColor = colors[pixelVal];
      
      strip.setPixelColor(i + OFFSET, pixelColor);
    }
    
  } else {
    strip.clear();
  }

  strip.show();
}

//TODO: deal with rollover
int currentInterval() {
  unsigned long now = micros();
  long timeDiff = now - startTime;

  if(timeDiff > microPerRotation) {
    startTime = now;
    timeDiff = 0;
  }

  return (int) (timeDiff / loopInterval);
}


