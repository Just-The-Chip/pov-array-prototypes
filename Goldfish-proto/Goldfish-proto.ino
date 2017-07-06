#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 60 // Number of LEDs in strip

// LED data and clock pins:
#define DATAPIN    11
#define CLOCKPIN   13

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BGR);

#define RPM 240
//#define INTERVALS 60
#define IMAGEWIDTH 8
#define IMAGEHEIGHT 13
//#define HEIGHT 8
#define OFFSET 0

uint32_t colors[3] = {
  0x000000, //black
  0x441100, //orange
  0x000044  //blue
};

//uint32_t orange = 0xff5000; //Starting color
//uint32_t blue = 0x0000ff;
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
unsigned long microPerRotation = 1000000 / rps; //microseconds per rotation
unsigned int loopInterval = microPerRotation / 120; //Time to travel 3 degrees in microseconds

//TODO: loopInterval should represent 1 degree for precision and then we can do some math to divide the current interval by 3 ro something

unsigned long startTime = 0;

void setup() {

  Serial.begin(9600);
  //Serial.print(loopInterval);
 
  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.show();  // Turn all LEDs off ASAP
}

//int currentInterval = 0;
void loop() {

  int curInterval = currentInterval();
  Serial.println(curInterval);

  if(curInterval <= IMAGEWIDTH && curInterval >= 0) {

    bool resetPixels = curInterval == IMAGEWIDTH;
    //int column[CIRCLEWIDTH] = image[curInterval];

    for(int i = 0; i < IMAGEHEIGHT; i++) {

      int pixelVal = resetPixels ? 0 : image[curInterval][i];
      
      uint32_t pixelColor = colors[pixelVal];
      //Serial.println(pixelColor);
      //Serial.println();
      strip.setPixelColor(i + OFFSET, pixelColor);
    }
  } else {
    strip.clear();
  }

  strip.show();

//  if(++currentInterval == INTERVALS) {
//    currentInterval = 0;
//  }
//  
//  delay(loopInterval);                        // Pause 20 milliseconds (~50 FPS)
}

//TODO: deal with rollover
int currentInterval() {
  unsigned long now = micros();
  long timeDiff = now - startTime;
  //Serial.println((long) 1 / rps);

  if(timeDiff > microPerRotation) {
    startTime = now;
    timeDiff = 0;
  }

  return (int) (timeDiff / loopInterval);
}


