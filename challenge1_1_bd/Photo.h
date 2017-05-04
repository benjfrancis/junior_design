#ifndef PHOTO_H
#define PHOTO_H

#include "Arduino.h"

struct RB {
  int r;
  int b;
  bool valid;
};

class Photo {

private:
    const int LED_RED = 24;
    const int LED_BLUE = 26;
    const int PHOTO_SENS = A0;
    
    const int BLUE_BRIGHT = 255;
    const int RED_BRIGHT = 255;
    const int INTENSITY_THRESH = 150;
    const float DELTA_THRESH = 0.15;
    const int GET_DELAY = 50;
    //const int GET_DELAY = 250;
    const int SAMPLES = 5;

    unsigned long timeBlueOn;
    unsigned long timeRedOn;
    
    int getR(unsigned long, byte);
    int getB(unsigned long, byte);
    //int getRB();
    int getAMB();

    
    float delta(float A, float B);

    

public:
    Photo();

    RB getRB(RB);
    bool isBlack(RB);
    char getColor(RB);
    char getYB(RB);
    char getYR(RB);
    void ledOff();
    




};

/*
    The getColor function by itself could be sufficent if we can get it operate
    fast enough.  The way we are detecting color now depends on the output of the
    sensor for all three LED combinations.  It is possible that we could have it 
    following a specific color and waiting for the intensity to change by a certain 
    amount, but that goes against our goal of not hard coding numbers into the color
    detection.  I think that the get color function should be able to operate fast enough.  
*/



#endif
