#include "Comms.h"
#include "Arduino.h"



Comms::Comms() {
  // set up pins
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);

  pinMode(13, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(2, OUTPUT);
 
}

void Comms::transmit(int msgLength) {
  crntMillis = millis();
  if (crntMillis - TxMillis < msgLength) {
    digitalWrite(TX_PIN, HIGH);
  } else if (crntMillis - TxMillis < msgLength + 2000) {
    digitalWrite(TX_PIN, LOW);
  } else if (crntMillis - TxMillis < 2*msgLength + 2000) {
    digitalWrite(TX_PIN, HIGH);
  } else if (crntMillis - TxMillis < 2*msgLength + 4000) {
    digitalWrite(TX_PIN, LOW);
  } else {
    TxMillis = crntMillis;
  }
}

void Comms::transmitDelay(int msgLength) {
  digitalWrite(TX_PIN, HIGH);
  delay(msgLength);
  digitalWrite(TX_PIN, LOW);
  delay(2000);
  digitalWrite(TX_PIN, HIGH);
  delay(msgLength);
  digitalWrite(TX_PIN, LOW);
}




void Comms::transmitTest() {
  crntMillis = millis();
  if (crntMillis - TxMillis < 200) {
    digitalWrite(TX_PIN, HIGH);
  } else if (crntMillis - TxMillis < 2200) {
    digitalWrite(TX_PIN, LOW);
  } else if (crntMillis - TxMillis < 2400) {
    digitalWrite(TX_PIN, HIGH);
  } else if (crntMillis - TxMillis < 7400) {
    digitalWrite(TX_PIN, LOW);
  } else if (crntMillis - TxMillis < 7700) {
    digitalWrite(TX_PIN, HIGH);
  } else if (crntMillis - TxMillis < 9700) {
    digitalWrite(TX_PIN, LOW);
  } else if (crntMillis - TxMillis < 10000) {
    digitalWrite(TX_PIN, HIGH);
  } else if (crntMillis - TxMillis < 17000) {
    digitalWrite(TX_PIN, LOW);
  } else if (crntMillis - TxMillis < 17410) {
    digitalWrite(TX_PIN, HIGH);
  } else if (crntMillis - TxMillis < 19410) {
    digitalWrite(TX_PIN, LOW);
  } else if (crntMillis - TxMillis < 19820) {
    digitalWrite(TX_PIN, HIGH);
  } else if (crntMillis - TxMillis < 24820) {
    digitalWrite(TX_PIN, LOW);
  } else {
    TxMillis = crntMillis;
  }
}

// Call every 0.1 milliseconds
int Comms::receive() {
  crntMicros = micros();
  crntMillis = millis();
  int msgLength = 0;
  if ((crntMicros - RxMicros >= 100)){
    
    // Read the pin
    RxIn = digitalRead(RX_PIN);
  
    // Do if pin is high
    if (RxIn == HIGH) {
      // leaving the line below as a reminder, NEVER do this, it changes the RxCount 
      // because println takes time to execute
      //Serial.println("HIGH");
      // Reset time interval if this is first detection
      if (RxCount==0){
        RxMillis = crntMillis;
      }
  
      // Increment counter
      RxCount++;
  
    // Do if pin is low
    } else if (crntMillis - RxMillis > 500) {
      // Trigger the message reception; reset time interval and counter
      msgLength = RxMessage();
      RxMillis = crntMillis;
      RxCount = 0;
    }




  RxMicros = crntMicros;
  }
  return msgLength;
}

int Comms::RxMessage(){

  Serial.println(RxCount);
  
  if (RxCount > 1000 and RxCount <= THRESH_200) {
    Serial.println("200ms message");
    return 2;
  } else if (RxCount > THRESH_200 and RxCount <= THRESH_300) {
    Serial.println("300ms message");
    return 3;
  } else if (RxCount > THRESH_300 and RxCount <= THRESH_400) {
    Serial.println("400ms message");
    return 4;
  } else if (RxCount > THRESH_400 and RxCount <= THRESH_500) {
    Serial.println("500ms message");
    return 5;
  } else {
    //Serial.println(RxCount);
    return 0;
  }
}
