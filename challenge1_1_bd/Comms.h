#ifndef COMMS_H
#define COMMS_H

#include "Arduino.h"


class Comms{

private:
    // Setup constants and variables
    const byte RX_PIN = A7;
    const byte TX_PIN = 22;
    const unsigned THRESH_200 = 2300;
    const unsigned THRESH_300 = 3300;
    const unsigned THRESH_400 = 4300;
    const unsigned THRESH_500 = 5300;
    
    
    
    // Reserve pins 13, 5, and 2 for carrier signal generation

    int RxIn;
    unsigned long RxCount = 0;
    unsigned long crntMillis;
    unsigned long crntMicros;
    unsigned long RxMillis = 0;
    unsigned long RxMicros = 0;
    unsigned long TxMillis = 0;

    int RxMessage();

public:
    Comms();
    void transmit(int);
    void transmitDelay(int);
    void transmitTest();
    int receive();


  
};









#endif
