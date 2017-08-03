#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
#include <avr/pgmspace.h>

#include "Count.h"

//LED strips
#define NUMPIXELS 60 // Number of LEDs in strip
#define NUMSTRIPS 2 //Total Number of strips

//hall effect sensor
#define SENSORPIN 10
#define NUMMAGNETS 1

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BGR);

#define RPM 250 //This is just to set up a default microPerSecond value. Will be removed eventually

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
    long timeDiff = now - sensorTime;
    
    //estimate start time based on how much time it would have took 
    //to travel to the point that it's at now.
    
    //startTime = now - (timeDiff * magnetsPassed); 
    if(magnetsPassed == 0) {
      startTime = now;
    }

    //we have reached the "bottom" (i.e. last magnet encountered)
    if(++magnetsPassed == NUMMAGNETS) {
      magnetsPassed = 0;
    }

    microPerRotation = timeDiff * NUMMAGNETS;
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

      uint8_t pixelVal = pgm_read_byte(&(image[stripInterval][i]));

      uint32_t pixelColor = colors[pixelVal];
      //uint32_t pixelColor = pgm_read_byte_far(&(colors[pixelVal]));
      
      strip.setPixelColor(i + pixelOffset + OFFSET, pixelColor);
    }
    
  } else {
    for(int i = 0; i < IMAGEHEIGHT; i++) {    
      strip.setPixelColor(i + pixelOffset + OFFSET, 0);
    }
  }
}
