#ifndef DRIVE_H
#define DRIVE_H

#include "Arduino.h"

class Drive {

private:
    const int RMF = 9;
    const int RMB = 12;
    const int LMF = 11;
    const int LMB = 10;
    
    int R_FWD;
    int L_FWD;

    int R_BKD;
    int L_BKD;

    float RMF_STATE;
    float RMB_STATE;
    float LMF_STATE;
    float LMB_STATE;

    

public:
    Drive();

    const int STD_POWER = 0.75;
    const int STD_TURN = 0.5;

    void setPower(float R_FWD, float L_FWD, float R_BKD, float L_BKD);
    void forward(float power);
    void backward(float power);
    
    void leftInPlace(float power);
    void rightInPlace(float power);
    void leftArch(float power, float dist);
    void rightArch(float power, float dist);

    void stop();


};

#endif




