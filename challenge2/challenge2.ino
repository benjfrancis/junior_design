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

//String BOT_NAME = "DD";
String BOT_NAME = "PL";


void setup() {
  Serial.begin(9600);

  pinMode(SWITCH, INPUT);
  pinMode(SWITCH_BOT, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BOMB_LED, OUTPUT);


  // PINHEAD LARRY
  if (BOT_NAME = "PL")
      d.setPower(255, 235, 255, 225);

  // DIRTY DAN
  else
      d.setPower(255, 255, 240, 255);

  
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

while (digitalRead(SWITCH) == LOW) {}
delay(2000);
  
if (BOT == 1) {

  Serial.println("BOT1");

  while(c.receive() != 5) {
    
  }

  
  
  d.forward(0.5);
  delay(1950);
  d.stop();
  delay(300);
  
  d.rightInPlace(0.4);
  delay(1700);
  d.stop();
  delay(300);
  
  d.backward(0.5);
  delay(490);
  
  
  
  d.leftInPlace(0.5);
  delay(660);
  d.stop();
  delay(300);

  
  d.rightInPlace(0.5);
  delay(660);
  d.stop();
  delay(300);
 
  
  d.rightInPlace(0.5);
  delay(660);
  d.stop();
  delay(300);

  
  d.rightInPlace(0.5);
  delay(640);
  d.stop();
  delay(300);
  d.backward(0.5);
  delay(2210);
  

  d.stop();

  delay(1000);
  Serial.println("asdf");
  c.transmitDelay(500);

  
  delay(10000000);
  
  
  
  
}












if (BOT == 2) { 

  Serial.println("BOT2");

  c.transmitDelay(500);
  delay(1000);

  while(c.receive() != 5) { 
    
  }

  d.forward(0.5);
  delay(1950);
  d.stop();
  delay(300);
  
  d.rightInPlace(0.4);
  delay(1700);
  d.stop();
  delay(300);
  
  d.backward(0.5);
  delay(490);
  
  
  
  d.leftInPlace(0.5);
  delay(660);
  d.stop();
  delay(300);

  
  d.rightInPlace(0.5);
  delay(660);
  d.stop();
  delay(300);
 
  
  d.rightInPlace(0.5);
  delay(660);
  d.stop();
  delay(300);

  
  d.rightInPlace(0.5);
  delay(640);
  d.stop();
  delay(300);
  d.backward(0.5);
  delay(2210);
  

  d.stop();
  delay(10000000);
  
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

