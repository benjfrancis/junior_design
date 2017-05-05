#include "Comms.h"
#include "Drive.h"
#include "Photo.h"

// ### PINS ###

// LEDS
const int BLUE_LED = 53;
const int YELLOW_LED = 52;
const int RED_LED = 51;
const int BOMB_LED = 50;


// COLLISION

// these pins cannot change, they are interrupt pins
// pins 2 and 3 are also interrupt pins but are slightly different
const int FRONT_BUMP = 18;
const int LEFT_BUMP = 19;
const int RIGHT_BUMP = 2; 
const int BACK_BUMP = 3;


// HALL
const byte HALL_SENS = 20;
volatile byte hallState = LOW;
bool hallArmState = false;
bool mineFound = false;

// OTHER

const int SWITCH = 46;
const int SWITCH_BOT = 48;




// ### GLOBALS ###

// COLLISION
int front = 0;
int left = 0;
int right = 0;
int bumperState = 0;
int val = 0;
unsigned long interval = 300000;
int endISR = 0;
bool collisionArmState = false;
//int last_interrupt_time1;
//int last_interrupt_time2;
//int last_interrupt_time3;
//int last_interrupt_time4;

// LED
unsigned long lastSet = millis();
unsigned long lastBombSet = millis();



// ### FUNCTIONS ###

// LED
void displayState(byte state);
void ledFlash();

// HALL
void hallSetup();
void hallTrigger();
void hallArm(bool);

// COLLISION
void collisionSetup();
void front_ISR();
void back_ISR();
void left_ISR();
void right_ISR();
void collisionArm(bool);


// ### CLASSES ###

// also performs setup for all pins etc... within constructor
Comms c;
Photo p;
Drive d;




void setup() {
  Serial.begin(9600);

  pinMode(SWITCH, INPUT);
  pinMode(SWITCH_BOT, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BOMB_LED, OUTPUT);

  collisionSetup();
  hallSetup();
  //hallArm(true);
  //collisionArm(true);

  TCCR3A = _BV(COM3A0) | _BV(COM3B0) | _BV(WGM30) | _BV(WGM31);
  // sets COM Output Mode to FastPWM with toggle of OC3A on compare match with OCR3A
  // also sets WGM to mode 15: FastPWM with top set by OCR3A

  TCCR3B = _BV(WGM32) | _BV(WGM33) |  _BV(CS31);
  // sets WGM as stated above; sets clock scaling to "divide by 8"

  OCR3A = 53;
  // above sets the counter value at which register resets to 0x0000;
  // generate 18.523 kHz when OCR3A=53 on Mega pin 5



}


/*
 * STATES: 
 * 0 = start
 * 1 = detect yellow
 * 2 = finding the far wall / turn around
 * 4 = find line @ opposite side
 * 4 = turn and follow line (with mine)
 * 5 = found far wall, stop turn around and transmit 300ms to ccp/bot2
 * 5 = waiting for 500ms from bot2
 * 6 = follow red again back to yellow
 * 7 = drive to corner
 * 8 = blink ending LEDs
 * 10 = stop
 */
 
// BOT 1 IS RED LINE
// BOT 2 IS BLUE LINE
int BOT = 1;
 
void loop() {

if (digitalRead(SWITCH_BOT)) BOT = 1;
else BOT = 2;

  
if (BOT == 1) {

  
  static byte state = 0;


  
  static RB rb;

  if (millis() > lastBombSet + 10) {
    lastBombSet = millis();
    if (hallState == 1) {
        digitalWrite(BOMB_LED, HIGH);
    } else {
        digitalWrite(BOMB_LED, LOW);
    }
  }

  if (digitalRead(SWITCH) == LOW) {
      state = 0;
      d.stop();
      p.ledOff();
      hallArm(false);
      hallArmState = false;
      hallState = 0;
      
      collisionArm(false);
      collisionArmState = false;
      bumperState = 0;
  }

  switch (state) {
    case 0:
      displayState(state);
      Serial.println("State 0");
      //delay(5000);
      if (digitalRead(SWITCH) == HIGH) {
          delay(2000);
          state = 1;
      }
      break;
    case 1: 
      Serial.println("State 1");
      displayState(state);
      //////////////////turn yellow LED on
      d.rightInPlace(0.5);
      delay(220);
      d.stop();
      state = 2;
        break; 
    case 2: 
        Serial.println("State 2");
        
        displayState(state);
        collisionArm(true);
        d.forward(0.75);
        if (bumperState != 0) {
          bumperState = 0;
          collisionArm(false);
          d.backward(0.75);
          delay(2700);
          d.rightInPlace(0.5);
          delay(450);
          bumperState = 0;
          state = 3;
        }
        break;
    case 3:
      Serial.println("State 3");
      displayState(state);
      collisionArm(true);
      
      
      rb = p.getRB(rb);

      if (!mineFound) {
          hallArm(true);  
      }
      
      if (bumperState != 0) {
          collisionArm(false);
          bumperState = 0;
          p.ledOff();
          
          state = 4; //
      } else if (!mineFound && hallState != 0) { 
          hallState = 0;
          hallArm(false);
          digitalWrite(BOMB_LED, HIGH);
          
          //////////////// flash blue LED
          d.stop();
          c.transmitDelay(200);
          d.rightInPlace(0.5);
          delay(750);
          d.stop();
          delay(1000);
          while (c.receive() != 2) {
            //Serial.println("Waiting for 200ms message...");
          }
          mineFound = true;
          d.leftInPlace(0.5);
          delay(750);
          d.stop();
          delay(500);
          rb.valid = false;
          while(!rb.valid) {
            rb = p.getRB(rb);
          }
      } else if (p.isBlack(rb)) {
          d.leftArch(0.6, 0.75);
          //Serial.println("RIGHT");
          rb.valid = false;
      } else if (!p.isBlack(rb)) {
          d.rightArch(0.6, 0.75);
          //Serial.println("LEFT");
          rb.valid = false;
      } 
      


      

      
      break;


      
        
        break;

    case 4:
        Serial.println("State 4");
        displayState(state);
        bumperState = 0;
        d.stop();
        d.backward(0.75);
        delay(250);
        d.leftInPlace(0.4);
        delay(1000); // SHOULD BE 180 TURN
        d.leftInPlace(0.4);
        rb.valid = false;
        while(!rb.valid) {
          rb = p.getRB(rb); 
        }
        while(p.isBlack(rb)) {
          rb = p.getRB(rb);
        }
        d.stop();
        delay(500);
        c.transmitDelay(320);
        state = 5; // 6;
        
        break;

    case 5:
        Serial.println("State 5");
        displayState(state);
        while (c.receive() != 4) {
          //Serial.println("Waiting for 500ms from bot 2");
        }
        rb.valid = false;
        state = 6; // 7;
        break;

    case 6:
        Serial.println("State 6");
        // CHANGE THIS TO DETECT YELLOW AT THE END OF THE RED LINE
        bumperState = 0;
        collisionArm(true);
        if (bumperState != 0) {
            d.stop();
            state = 99;
        } else if (rb.valid == false) {
            rb = p.getRB(rb);
        } else if (p.isBlack(rb)) {
            d.rightArch(0.7, 0.75);
            rb.valid = false;
        } else if (p.getYR(rb) == 'r'){
            d.leftArch(0.7, 0.75);
            rb.valid = false;
        } else {
            d.stop();
            rb.valid = false;
            state = 7;
        }
        
        break;

    case 7: 
        Serial.println("State 7");
        d.leftInPlace(0.5);
        delay(600);
        d.forward(0.75);
        delay(400);
        d.stop();
        state = 9;
        
        break;

    case 9:
        Serial.println("State 8");
        ledFlash();
        state = 99;
        break;

    case 10:
        break;



      
    case 99: 
      Serial.println("State STOP");
      displayState(7);
      d.stop();
      break; 


    case 100:
      while(c.receive() != 4) {
        //Serial.println("Waiting for 200ms Message...");
      }
        Serial.println("RECIEVED");
        delay(5000);

    
    default: 
      
      break;
  }



  
}












if (BOT == 2) { // hi ben! you might see this soon but its celia, its 5/4/17 and we are testing :)

  static byte state = 0;


  
  static RB rb;

  if (millis() > lastBombSet + 10) {
    lastBombSet = millis();
    if (hallState == 1) {
        digitalWrite(BOMB_LED, HIGH);
    } else {
        digitalWrite(BOMB_LED, LOW);
    }
  }

  if (digitalRead(SWITCH) == LOW) {
      state = 0;
      d.stop();
      p.ledOff();
      hallArm(false);
      hallArmState = false;
      hallState = 0;
      
      collisionArm(false);
      collisionArmState = false;
      bumperState = 0;
  }

  switch (state) {
    case 0:
      displayState(state);
      Serial.println("State 0");
      //delay(5000);
      if (digitalRead(SWITCH) == HIGH) {
          delay(2000);
          state = 1;
      }
      break;
    case 1: 
      Serial.println("State 1");
      displayState(state);
      //////////////////turn yellow LED on
      d.leftInPlace(0.5);
      delay(220);
      d.stop();
      state = 2;
        break; 
    case 2: 
        Serial.println("State 2");
        
        displayState(state);
        collisionArm(true);
        d.forward(0.75);
        if (bumperState != 0) {
          bumperState = 0;
          collisionArm(false);
          d.backward(0.75);
          delay(2200);
          d.leftInPlace(0.5);
          delay(450);
          
          state = 3;
        }
        break;
    case 3:
      //Serial.println("State 3");
      displayState(state);
      collisionArm(true);
      bumperState = 0;
      
      rb = p.getRB(rb);

      if (!mineFound) {
          hallArm(true);  
      }
      
      if (bumperState != 0) {
          collisionArm(false);
          bumperState = 0;
          p.ledOff();
          
          state = 4; //
      } else if (!mineFound && hallState != 0) { 
          hallState = 0;
          hallArm(false);
          digitalWrite(BOMB_LED, HIGH);
          
          //////////////// flash blue LED
          d.stop();
          c.transmitDelay(400);
          d.rightInPlace(0.5);
          delay(750);
          d.stop();
          delay(1000);
          while (c.receive() != 4) {
            //Serial.println("Waiting for 200ms message...");
          }
          mineFound = true;
          d.leftInPlace(0.5);
          delay(750);
          d.stop();
          delay(500);
          rb.valid = false;
          while(!rb.valid) {
            rb = p.getRB(rb);
          }
      } else if (p.isBlack(rb)) {
          d.leftArch(0.6, 0.75);
          //Serial.println("RIGHT");
          rb.valid = false;
      } else if (!p.isBlack(rb)) {
          d.rightArch(0.6, 0.75);
          //Serial.println("LEFT");
          rb.valid = false;
      } 
      


      

      
      break;


      
        
        break;

    case 4:
        Serial.println("State 4");
        displayState(state);
        bumperState = 0;
        d.stop();
        d.backward(0.75);
        delay(250);
        d.rightInPlace(0.4);
        delay(1000); // SHOULD BE 180 TURN
        d.rightInPlace(0.4);
        rb.valid = false;
        while(!rb.valid) {
          rb = p.getRB(rb); 
        }
        while(p.isBlack(rb)) {
          rb = p.getRB(rb);
        }
        d.stop();
        delay(500);
        c.transmitDelay(320);
        state = 5; // 6;
        
        break;

    case 5:
        Serial.println("State 5");
        displayState(state);
        while (c.receive() != 4) {
          //Serial.println("Waiting for 500ms from bot 2");
        }
        rb.valid = false;
        state = 6; // 7;
        break;

    case 6:
        Serial.println("State 6");
        // CHANGE THIS TO DETECT YELLOW AT THE END OF THE RED LINE
        bumperState = 0;
        collisionArm(true);
        if (bumperState != 0) {
            d.stop();
            state = 99;
        } else if (rb.valid == false) {
            rb = p.getRB(rb);
        } else if (p.isBlack(rb)) {
            d.leftArch(0.8, 0.75);
            rb.valid = false;
        } else if (p.getYB(rb) == 'b'){
            d.rightArch(0.8, 0.75);
            rb.valid = false;
        } else {
            d.stop();
            rb.valid = false;
            state = 7;
        }
        
        break;

    case 7: 
        Serial.println("State 7");
        d.rightInPlace(0.5);
        delay(600);
        d.forward(0.75);
        delay(400);
        d.stop();
        state = 9;
        
        break;

    case 9:
        Serial.println("State 8");
        ledFlash();
        state = 99;
        break;

    case 10:
        break;



      
    case 99: 
      Serial.println("State STOP");
      displayState(7);
      d.stop();
      break; 


    case 100:
      while(c.receive() != 4) {
        //Serial.println("Waiting for 200ms Message...");
      }
        Serial.println("RECIEVED");
        delay(5000);

    case 101:
      Serial.println("200");
      c.transmitDelay(200);
      delay(1000);
      
      break;

    
    default: 
      
      break;
  }



  
}
  
}













void hallSetup() {
  pinMode(HALL_SENS, INPUT_PULLUP);
}

void hallTrigger() {
  hallState = !hallState;
}

void hallArm(bool arm) {
  if (arm && !hallArmState)  {
      attachInterrupt(digitalPinToInterrupt(HALL_SENS), hallTrigger, CHANGE);
      hallArmState = !hallArmState;
  } else if(!arm && hallArmState) {
      detachInterrupt(digitalPinToInterrupt(HALL_SENS));
      hallArmState = !hallArmState;
  }
}

void collisionSetup() {
  pinMode(FRONT_BUMP, INPUT);
  pinMode(LEFT_BUMP, INPUT);
  pinMode(RIGHT_BUMP, INPUT);
  pinMode(BACK_BUMP, INPUT);
}


void collisionArm(bool arm) {
  if (arm && !collisionArmState) {
    attachInterrupt(digitalPinToInterrupt(FRONT_BUMP), front_ISR, RISING);
    attachInterrupt(digitalPinToInterrupt(LEFT_BUMP), left_ISR, RISING);
    attachInterrupt(digitalPinToInterrupt(RIGHT_BUMP), right_ISR, RISING);
    attachInterrupt(digitalPinToInterrupt(BACK_BUMP), back_ISR, RISING);
    collisionArmState = !collisionArmState;
  } else if(!arm && collisionArmState){
    detachInterrupt(digitalPinToInterrupt(FRONT_BUMP));
    detachInterrupt(digitalPinToInterrupt(LEFT_BUMP));
    detachInterrupt(digitalPinToInterrupt(RIGHT_BUMP));
    detachInterrupt(digitalPinToInterrupt(BACK_BUMP));
    collisionArmState = !collisionArmState;
  }

}


void displayState(byte state) {
  //digitalWrite(RED_LED, HIGH);

  if (millis() < lastSet + 10) return;
  else lastSet = millis();

  if (state == 0) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
  }

  if (state == 1) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
  }

  if (state == 2) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);
  }

  if (state == 3) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);
  }

  if (state == 4) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);
  }

  if (state == 5) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);
  }

  if (state == 6) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);
  }

  if (state == 7) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(BLUE_LED, HIGH);
  }






  /*
  if (state % 1 == 1) {
      digitalWrite(RED_LED, HIGH);
  }

  if ((state >> 1) % 1 == 1) {
      digitalWrite(YELLOW_LED, HIGH);
  }

  if ((state >> 2) % 1 == 1) {
      digitalWrite(BLUE_LED, HIGH);
  }
  */
}

void ledFlash() {
  for(int i=0; i < 10; i++) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(BLUE_LED, HIGH);
    delay(250);
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    delay(250);
  }
}



void front_ISR(){
  //Serial.println("FRONT");
  interrupts();
  front++;
  val = 0;
  
  static unsigned long last_interrupt_time1 = 0;
  unsigned long interrupt_time1 = millis();
  
  // If interrupts come faster than 100ms, assume it's a bounce and ignore
  if (interrupt_time1 - last_interrupt_time1 > 100){
    //Serial.println("ENOUGH TIME HAS PASSED");
    if (right == 1) {
      bumperState = 6;
      Serial.println(bumperState);
      front = 0;
      right = 0;
      endISR = 1;
      return;
    }
    if (left == 1) {
      bumperState = 5;
      Serial.println(bumperState);
      front = 0;
      left = 0;
      endISR = 1;
      return;
    }
    while(val < interval) {
      val++;
      //Serial.println(val);
    }
    //Serial.println("OUT OF WHILE");
    if (endISR == 1){
      //Serial.println("endISR is one");
      endISR = 0;
      return;
    } else {
      front = 0;
      bumperState = 2;
      Serial.println(bumperState);
    }
  }
  last_interrupt_time1 = interrupt_time1;

}












void left_ISR(){
  //Serial.println("LEFT");
  interrupts();
  right++;
  val = 0;
  
  static unsigned long last_interrupt_time2 = 0;
  unsigned long interrupt_time2 = millis();

  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time2 - last_interrupt_time2 > 100) {
    if (front == 1) {
      bumperState = 6;
      Serial.println(bumperState);
      front = 0;
      right = 0;
      endISR = 1;
      return;
    }    
    while(val < interval) {
      val++;
    }
    if (endISR == 1){
      endISR = 0;
      return;
    } else {
      right = 0;
      bumperState = 3;
      Serial.println(bumperState); 
    }
  }
  last_interrupt_time2 = interrupt_time2;
}

void right_ISR(){
    //Serial.println("RIGHT");
  interrupts();
  left++;
  val = 0;
  
  static unsigned long last_interrupt_time3 = 0;
  unsigned long interrupt_time3 = millis();


  
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time3 - last_interrupt_time3 > 100) {
    if (front == 1) {
      bumperState = 5;
      Serial.println(bumperState);
      front = 0;
      left = 0;
      endISR = 1;
      return;
    }    
    while(val < interval) {
      val++;
    }
    if (endISR == 1){
      endISR = 0;
      return;
    } else {
      left = 0;
      bumperState = 1;
      Serial.println(bumperState); 
    }
  }
  last_interrupt_time3 = interrupt_time3;
}

void back_ISR(){
    //Serial.println("BACK");
  static unsigned long last_interrupt_time4 = 0;
 unsigned long interrupt_time4 = millis();
 
 // If interrupts come faster than 200ms, assume it's a bounce and ignore
 if (interrupt_time4 - last_interrupt_time4 > 100)
 {
   bumperState = 4;
   Serial.println(bumperState);
 }
 last_interrupt_time4 = interrupt_time4;
}

