#include "Photo.h"
#include "Arduino.h"



Photo::Photo() {
	pinMode(PHOTO_SENS, INPUT);
	pinMode(LED_RED, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
}

int Photo::getR(unsigned long crntMillis, byte state){
  if (state == 0) {
  	digitalWrite(LED_BLUE, LOW);
  	digitalWrite(LED_RED, HIGH);
    //Serial.println("asdfjklasdf;j");
    timeRedOn = crntMillis;
    return -1;
  } else if (crntMillis - timeRedOn > GET_DELAY) {
    return analogRead(PHOTO_SENS);
  } else {
    return -1;
  }
	//delay(GET_DELAY);
 
	/*int val = 0;
	for (int i=0; i <= SAMPLES; i++) {
	 val += analogRead(PHOTO_SENS);
 }
 */
  //ledOff();  
  //return analogRead(PHOTO_SENS);
}


int Photo::getB(unsigned long crntMillis, byte state){
  if (state == 0) {
  	digitalWrite(LED_BLUE, HIGH);
  	digitalWrite(LED_RED, LOW);
    timeBlueOn = crntMillis;
    return -1;
  } else if (crntMillis - timeBlueOn > GET_DELAY) {
    return analogRead(PHOTO_SENS);
  } else {
    return -1;
  }
	//delay(GET_DELAY);

 /* int val = 0;
  for (int i=0; i <= SAMPLES; i++) {
     val += analogRead(PHOTO_SENS);
  }*/
  //ledOff();  
  //return analogRead(PHOTO_SENS);


}

int Photo::getAMB(){
	ledOff();
	delay(GET_DELAY);
	return analogRead(PHOTO_SENS);
}



void Photo::ledOff() {
	analogWrite(LED_BLUE, LOW);
  analogWrite(LED_RED, LOW);
}



RB Photo::getRB(RB rb){


  static byte state = 0;

	//Serial.println(state);
 
  String toPrint; 
  float avg_intensity;
  static float R = 0;
  static float B = 0;
  float AMB = 0;

  switch (state) {
    case 0:
      R = float(getR(millis(), state%2));
      state = 1;
      break;
    
    case 1:
      R = float(getR(millis(), state%2));
      if (R != -1) state = 2;
      break;

    case 2:
      B = float(getB(millis(), state%2));
      state = 3;
      break;

    case 3:
      B = float(getB(millis(), state%2));
      if (B != -1) state = 4;
      break;

    case 4:
      avg_intensity = (R+B)/2;
      
      toPrint = "R:  " + String(R);
      //Serial.println(toPrint);
      
      toPrint = "B:  " + String(B);
      //Serial.println(toPrint);

      Serial.println();
      Serial.println();

      
      //toPrint = "AMB:  " + String(AMB);
      //Serial.println(toPrint);
      
      
      char color;
      
      if (avg_intensity < 300) {
          color = 'k';
      } else if (B > 700 ) {
          if (R > 450) {
              color = 'y';
          } else {
              color = 'b';
          }
      } else {
          color = 'r';
      }
      
      //Serial.println(color);
      
      //Serial.println();
      //Serial.println("END getColor:");
      //Serial.println();

      ledOff();
      state = 0;
      
      

      rb.r = R;
      rb.b = B;
      rb.valid = true;



      R = 0;
      B = 0;
      
      return rb; 
    default:
      return rb;
    break;
  }

  return rb;

}



bool Photo::isBlack(RB rb) {
  if ((rb.r + rb.b)/2 < INTENSITY_THRESH) {
    return true;
  } else {
    return false;
  }
}

//let's not use use, only need to differentiate between YB and YR
char Photo::getColor(RB rb) {
    // only called if we know it isn't black
    if (rb.r / rb.b < 0.2) {
        return 'b';
    } else if ((rb.r + rb.b)/2 < 300)
        return 'r';
    else {
        return 'y';
    }
}

char Photo::getYB(RB rb) {
    if (rb.r / rb.b < 0.2) {
        return 'b';  
    } else {
      return 'y';
    }
}


char Photo::getYR(RB rb) {
    if ((rb.r + rb.b) / 2 < 300) {
        return 'r';  
    } else {
      return 'y';
    }
}

