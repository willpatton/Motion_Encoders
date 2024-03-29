/**
 * encoder_basic.ino
 * 
 * @author: Will Patton 
 * @url:      https://github.com/willpatton/Motion_Encoders
 * @license:  MIT License
 *
 * Example
 * This project demos the library using an Arduino IDE sketch with serial console output.
 *
 */

#include "Motion_Encoders.h"

//PINOUTS
#define pinEnClk0   A2
#define pinEnData0  A3
#define pinEnClk1   A4
#define pinEnData1  A5
#define pinSw0	    11  
#define pinSw1      12

//OBJECTS
CEncoder DinL(pinSw0, pinEnClk0, pinEnData0);
CEncoder DinR(pinSw1, pinEnClk1, pinEnData1);

/**
 * setup
 */
void setup() { 
  //interrupts
  attachInterrupt(pinEnClk0,  isrEncoder0A, RISING);  // encoder 0 clock 
  attachInterrupt(pinEnData0, isrEncoder0B, RISING);  // encoder 0 data  
  attachInterrupt(pinEnClk1,  isrEncoder1A, RISING);  // encoder 1 clock  
  attachInterrupt(pinEnData1, isrEncoder1B, RISING);  // encoder 1 data 
}

/**
 * loop
 */
void loop() {
  DinL.loop_controls(); //read switches by polling
  DinR.loop_controls();

  int swPosL = DinL.get_swPos();
  int enPosL = DinL.get_enPos();
  int focusL = DinL.get_focus();
  String strL = String("SwL: " + String(swPosL) + ", En: " + String(enPosL) + " Focus: " + String(focusL));

  int swPosR = DinR.get_swPos();
  int enPosR = DinR.get_enPos();
  int focusR = DinR.get_focus();
  String strR = String("SwR: " + String(swPosR) + ", En: " + String(enPosR) + " Focus: " + String(focusR));

  //TODO: write strL and strL strings to console
  Serial.print (strL);Serial.print (" ");Serial.println(strR);

}

/**
 * ISR's
 */
void isrEncoder0A(){
  DinL.isrEncoderA();        //call the fast, lean  ISR
}
void isrEncoder0B(){
  DinL.isrEncoderB();        
}
void isrEncoder1A(){
  DinR.isrEncoderA();        
}
void isrEncoder1B(){
  DinR.isrEncoderB();        
}
