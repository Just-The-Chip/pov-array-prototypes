#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

//LED strips
#define NUMPIXELS 60 // Number of LEDs in strip
#define NUMSTRIPS 2 //Total Number of strips

//hall effect sensor
#define SENSORPIN 10
#define NUMMAGNETS 2

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BGR);

#define RPM 250 //This is just to set up a default microPerSecond value. Will be removed eventually
#define IMAGEWIDTH 18
#define IMAGEHEIGHT 21

#define OFFSET 8
#define RESOLUTION 2 //degrees per column in image array

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

uint32_t colors[9] = {
  0x000000,   //off(actual black)
  0x120442,   //dark purple(fake black)
  0xFF0000,   //red
  0xffff00,   //yellow
  0xff5000,   //orange
  0xffffff    //white
};

int image[IMAGEWIDTH][IMAGEHEIGHT] = {
  {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
  {0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 1, 0, 0, 0},
  {0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 4, 1, 0, 0},
  {0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 4, 1, 0, 0},
  {1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 1, 4, 4, 4, 4, 4, 4, 4, 4, 1, 0},
  {1, 2, 3, 2, 2, 1, 0, 0, 0, 0, 1, 4, 4, 4, 1, 5, 4, 4, 4, 4, 1},
  {1, 2, 3, 3, 2, 1, 0, 0, 0, 1, 4, 4, 4, 4, 1, 1, 4, 4, 4, 4, 1},
  {0, 1, 1, 3, 1, 0, 0, 0, 0, 1, 4, 4, 4, 4, 1, 1, 4, 4, 4, 4, 1},
  {0, 0, 1, 4, 1, 0, 0, 0, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 0},
  {0, 0, 1, 4, 4, 1, 0, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 0, 0},
  {0, 0, 0, 1, 4, 4, 1, 1, 4, 4, 4, 1, 4, 4, 1, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 4, 4, 1, 4, 4, 4, 4, 4, 1, 3, 3, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 4, 1, 4, 4, 4, 1, 1, 3, 3, 3, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 1, 4, 4, 4, 4, 3, 3, 3, 1, 5, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 1, 4, 4, 4, 3, 3, 1, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 1, 1, 4, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 1, 5, 4, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int pixelsPerStrip = NUMPIXELS / NUMSTRIPS;
int stripOffset = 360 / NUMSTRIPS;

unsigned long rps = RPM / 60; //This is just to set a default value until we get our first sensor read
unsigned long microPerRotation = 1000000 / rps; //microseconds per rotation, this value is just to get started.
unsigned int loopInterval = microPerRotation / 360; //Time to travel 1 degree in microseconds

unsigned long startTime = 0;

unsigned long sensorTime = 0;
boolean pendingHigh = false;
int magnetsPassed = 0;

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
    
    //estimate start time based on how much time it would have took 
    //to travel to the point that it's at now.
    startTime = now - ((now - sensorTime) * magnetsPassed); 

    //we have reached the "bottom" (i.e. last magnet encountered)
    if(++magnetsPassed == NUMMAGNETS) {
      magnetsPassed = 0;
    }

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

  int interval = (int) (timeDiff / loopInterval);
  if(interval >= 360) {
    startTime = now;
    interval = 0;
  }

  return interval;
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


