#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

//LED strips
#define NUMPIXELS 60 // Number of LEDs in strip
#define NUMSTRIPS 2 //Total Number of strips

//hall effect sensor
#define SENSORPIN 10
#define NUMMAGNETS 1

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BGR);

#define RPM 250 //This is just to set up a default microPerSecond value. Will be removed eventually
#define IMAGEWIDTH 13
#define IMAGEHEIGHT 8

#define OFFSET 17
#define RESOLUTION 5 //degrees per column in image array

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
  {1, 1, 1, 1, 1, 1, 1, 1},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 2, 2, 1, 1},
  {1, 1, 1, 1, 2, 2, 1, 1},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0}
};

int pixelsPerStrip = NUMPIXELS / NUMSTRIPS;
int stripOffset = 360 / NUMSTRIPS;

unsigned long rps = RPM / 60; //This is just to set a default value until we get our first sensor read
unsigned long microPerRotation = 1000000 / rps; //microseconds per rotation, this value is just to get started.
unsigned int loopInterval = microPerRotation / 360; //Time to travel 1 degree in microseconds

unsigned long startTime = 0;

unsigned long sensorTime = 0;
boolean pendingHigh = false;

void setup() {

  //Serial.begin(9600);
 
  strip.begin(); // Initialize pins for output
  strip.clear();
  strip.show();  // Turn all LEDs off ASAP

  sensorTime = micros();
}

void loop() {

  updateMPR();
  
  int curInterval = currentInterval();
  //Serial.println(curInterval);

  for(int i = 0; i < NUMSTRIPS; i++) {
    setStrip(i, curInterval);
  }

  strip.show();
}

void updateMPR() {
  //Serial.println(digitalRead(SENSORPIN));
  if(!digitalRead(SENSORPIN)) {
    pendingHigh = true;
    
  } else if(pendingHigh) {
    unsigned long now = micros();
    
    microPerRotation = (now - sensorTime) * NUMMAGNETS;
    loopInterval = microPerRotation / 360;

    sensorTime = now;

    pendingHigh = false;
  }
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


