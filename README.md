Motion Encoders
===========
A class to sample quadrature encoded signals to determine position and direction.

## Example
Here's an example to instantiate the class in a sample program:

	#include "Motion_Encoders.h"

	#define pinEnClk0   A2  // "A" pins being used as signal inputs
	#define pinEnData0  A3
	#define pinEnClk1   A4
	#define pinEnData1  A5

	//instantiate encoder objects
	CEncoder DinL(11, pinEnClk0, pinEnData0);
	CEncoder DinR(12, pinEnClk1, pinEnData1);

	/**
	 * setup
	 */
	void setup() { 
	  //interrupts
	  attachInterrupt(pinEnClk0, isrEncoder0A, RISING);   // encoder 0 clock 
	  attachInterrupt(pinEnData0, isrEncoder0B, RISING);  // encoder 0 data  
	  attachInterrupt(pinEnClk1, isrEncoder1A, RISING);   // encoder 1 clock  
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

		

## Supported platforms
This code known to compile using the Arduino IDE on a SAMD21 processor. 

## For more information

More information at http://github.com/willpatton.
