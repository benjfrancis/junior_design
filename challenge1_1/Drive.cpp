#include "Drive.h"
#include "Arduino.h"

Drive::Drive() {
    pinMode(RMF, OUTPUT);
    pinMode(RMB, OUTPUT);
    pinMode(LMF, OUTPUT);
    pinMode(LMB, OUTPUT);

    RMF_STATE = 0;
    RMB_STATE = 0;
    LMF_STATE = 0;
    LMB_STATE = 0;

    stop();
}

void Drive::forward(float power) {
    if (RMF_STATE == power * R_FWD && RMB_STATE == 0 && 
        LMF_STATE == power * L_FWD && LMB_STATE == 0) {
          return;
    }
  
    stop();

    

    if (power < 0 || power > 1) {
        Serial.println("ERROR: forward: power must be between 0 and 1");
        return;
    }

    analogWrite(RMF, R_FWD * power);                       
    analogWrite(LMF, L_FWD * power);  
    analogWrite(RMB, 0);                       
    analogWrite(LMB, 0);

    RMF_STATE = power * R_FWD ;
    RMB_STATE = 0;
    LMF_STATE = power * L_FWD;
    LMB_STATE = 0;
}

void Drive::backward(float power) {
    if (RMB_STATE == power * R_BKD && RMF_STATE == 0 && 
        LMB_STATE == power * L_BKD && LMF_STATE == 0) {
          return;
    }
    
    stop();

    if (power < 0 || power > 1) {
        Serial.println("ERROR: backward: power must be between 0 and 1");
        return;
    }

    analogWrite(RMB, R_BKD * power);                       
    analogWrite(LMB, L_BKD * power);  
    analogWrite(RMF, 0);                       
    analogWrite(LMF, 0);
    
    RMB_STATE = power * R_BKD ;
    RMF_STATE = 0;
    LMB_STATE = power * L_BKD;
    LMF_STATE = 0;

}


void Drive::leftInPlace(float power) {
    if (RMF_STATE == power * R_FWD && RMB_STATE == 0 && 
        LMB_STATE == power * L_FWD && LMF_STATE == 0) {
          return;
    }
  
    stop();

    if (power < 0 || power > 1) {
        Serial.println("ERROR: leftInPlace: power must be between 0 and 1");
        return;
    }

    analogWrite(RMF, R_FWD * power);                       
    analogWrite(LMB, L_FWD * power);

    RMF_STATE = power * R_FWD ;
    RMB_STATE = 0;
    LMF_STATE = 0;
    LMB_STATE = power * L_FWD;
}

void Drive::rightInPlace(float power) {
    if (RMB_STATE == power * R_FWD && RMF_STATE == 0 && 
        LMF_STATE == power * L_FWD && LMB_STATE == 0) {
          return;
    }
    
    stop();

    if (power < 0 || power > 1) {
        Serial.println("ERROR: rightInPlace: power must be between 0 and 1");
        return;
    }

    analogWrite(RMB, R_FWD * power);                       
    analogWrite(LMF, L_FWD * power);

    RMF_STATE = 0;
    RMB_STATE = power * R_FWD;
    LMF_STATE = power * L_FWD;
    LMB_STATE = 0;
}

void Drive::leftArch(float power, float dist) {
    if (RMF_STATE == power * R_FWD && RMB_STATE == 0 && 
        LMF_STATE == power * L_FWD * (1-dist) && LMB_STATE == 0) {
          return;
    }
  
    stop();

    if (power < 0 || power > 1) {
        Serial.println("ERROR: leftArch: power must be between 0 and 1");
        return;
    } 

    if (dist < 0 || dist > 1) {
        Serial.println("ERROR: leftArch: dist must be between 0 and 1");
        return;
    } 

    analogWrite(RMF, R_FWD * power);                       
    analogWrite(LMF, L_FWD * power * (1-dist));
    analogWrite(RMB, 0);                       
    analogWrite(LMB, 0);

    RMF_STATE = power * R_FWD ;
    RMB_STATE = 0;
    LMF_STATE = power * L_FWD * (1-dist);
    LMB_STATE = 0;
}

void Drive::rightArch(float power, float dist) {
    if (RMF_STATE == power * R_FWD * (1-dist) && RMB_STATE == 0 && 
        LMF_STATE == power * L_FWD && LMB_STATE == 0) {
          return;
    }

    stop();

    if (power < 0 || power > 1) {
        Serial.println("ERROR: rightArch: power must be between 0 and 1");
        return;
    } 

    if (dist < 0 || dist > 1) {
        Serial.println("ERROR: rightArch: dist must be between 0 and 1");
        return;
    } 

    analogWrite(LMF, L_FWD * power);
    analogWrite(RMF, R_FWD * power * (1-dist)); 
    analogWrite(RMB, 0);                       
    analogWrite(LMB, 0);   

    RMF_STATE = power * R_FWD * (1-dist);
    RMB_STATE = 0;
    LMF_STATE = power * L_FWD;
    LMB_STATE = 0;

}


void Drive::stop() {
    if (RMF_STATE == 0 && RMB_STATE == 0 && 
        LMF_STATE == 0 && LMB_STATE == 0) {
          return;
    }
  
    analogWrite(RMF, 0);                       
    analogWrite(LMF, 0); 
    analogWrite(RMB, 0);                       
    analogWrite(LMB, 0); 

    RMF_STATE = 0;
    RMB_STATE = 0;
    LMF_STATE = 0;
    LMB_STATE = 0;

}

