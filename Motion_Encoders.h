/**
Motion_Encoders.h

ROTARY ENCODERS
   
  APPLICATION 
    This code is for an encoder/switch typically found
    as volume, tuning, speed, or brightness control.
    The encoder has a physical knob with detents and push button switch. 

    The advantage of this code is that it can be 
    re-used for each encoder as an instantiated object and as a library.
     
    QUADRATURE - There are 2 signals and therefore 4 possible 
     states (edges) that occur (i.e. "quad").

     METHOD 
     INTERRUPT THEN SAMPLE - 
     Detect interrupt on edge, then sample the I/O signal. 

     SIGNALS (EDGE DETECTION)
     PERSPECTIVE from "A" RISING edge

     A leads B - CW
     A  ____|----|____|----|____|
     B     ____|----|____|----|
            ^ ^  ^ ^
            | |  | |
            | |  | SAMPLE (*) 
            | |  IRQ A FALLING edge samples B as HIGH
            | SAMPLE (*)  
            IRQ A RISING edge samples A as LOW
 
     A lags B - CCW
     A    ____|----|____|----|____|
     B  ____|----|____|----|
              ^ ^  ^ ^
              | |  | |
              | |  | SAMPLE (*) 
              | |  IRQ A FALLING edge samples B as LOW
              | SAMPLE (*) 
              IRQ A RISING edge samples B as HIGH

                (*) 
                If A AND B are both the same level, then CCW.
                If A XOR B are both different levels, the CW
     
  TIMING
    Here are timing measurements using an oscilloscope 
    for a 24 point rotary encoder (with detents).

    A <--> A  
    "INTERVAL" from CH A to CH A.
    Twisting speed  Interval
      Slow          200ms
      Medium        100ms
      Fast          50ms
      Rapid         10ms      Super-quick twist
      Flick         1.50ms    Fast as humanly possible

    A <--> B  
    "INTERVAL" from CH A to CH B.
    Twisting speed  Interval
      Slow          10ms
      Medium        5ms
      Fast          3ms 
      Rapid         1.0ms to 2.0ms   
      Flick         500us      

      
  DEBOUNCE
   There is typically significant bounce on rotary encoder's 
   pins on the "break" and "make".  

   A significant bounce was typically observed at 2-3 milliseconds and 
   sometimes up to 5ms on the "break". A bounce of 1ms was 
   sometimes observed on a slow "make".  Since there are 2 signals 
   in a quadrature encoder firing at offset times, it is doubly
   complicated to confidently detect edges and sample the 
   the polarity of the 2 switched signals due to bounce.
   
   A rotary encoder without detents may have less bound because 
   the detent's armature doesn't exist thus reducing kickback. 
   
   The 2 encoder signals will likely require hardware or software 
   debounce to improve the stability of readings.
   
   Hardware: Typically, an RC circuit is applied to the encoder's 
   signal lines in order to reduce the bounce.  
   Often times, this is effective but not always a perfect fix.  
   Additionally, a Schottky input may further help reject 
   unintended noise from triggering logic levels.
   
   Software: To debounce the encoder's signals in software, 
   techniques like timing delays, multiple samples, 
   edge detection, inference (best guess from the last state), 
   pattern/state, or in combination may be used.  For IoT processors, 
   10 to 25 microseconds is a typical estimate per I/O sample. 
   This allows an ISR to sample and exit quickly 
   (typically within 100us).
 

  REFERENCES
  See description and source examples from:
  https://playground.arduino.cc/Main/RotaryEncoders

  //PSNS states (Previous State Next State - code as the input):
  //https://www.best-microcontroller-projects.com/rotary-encoder.html#Taming_Noisy_Rotary_Encoders

*/

#ifndef __MOTION_ENCODERS_H
#define __MOTION_ENCODERS_H


#include "Arduino.h"

class CEncoder {

public:
  //prototypes
  CEncoder(uint8_t _swPin, uint8_t _enPinClk, uint8_t _enPinData);
  uint16_t get_instance();
  void loop_controls();         //caller
  void switches();              //read
  uint8_t get_swPos();          //get switch position. 0 = low pressed, 1 = not pressed
  //void encoders();
  void isrEncoderA();           //A-channel of encoder "clk"
  void isrEncoderB();           //B-channel of encoder "data"
  int16_t get_enPos();          //get encoder position
  uint8_t get_enDir();          //encoder direction
  void set_enPos(int16_t val);  //set encoder position. useful for resetting position
  uint8_t get_focus();          //which control has the current focus

private:
  uint16_t  instance;     //a random instance number for this object
  uint8_t   _debug = false;

  //pins
  uint8_t   _swPin;
  uint8_t   _enPinClk;
  uint8_t   _enPinData;

  //switch
  uint8_t	sw_state; 	    //state is unpressed setup.  HIGH, LOW
  uint8_t	sw_pos = 0;	    //

  //encoder
  uint8_t	en_clk; 	      //clk is unknown at setup. HIGH, LOW
  uint8_t	en_data; 	      //data is unknown at setup. HIGH, LOW
  int16_t	en_pos = 0;     // +/- 32768
  int8_t  en_dir = 0;     //-1 ccw, 0 none, 1 cw
  volatile uint8_t enIsrFlag = 0;  //1 ISR in progress, 0 isr cleared, free
  
  //screen
  #define SW_MIN 0 
  #define SW_MAX 5   
  //uint8_t   screen = SCR_MIN;

  //focus
  #define NONE 0
  #define SWITCH 1
  #define ENCODER 2
  int focus = NONE;

  //timers
  uint32_t  timer;			     //TBD
  uint32_t  timer_hold;		  //millis timer
  uint32_t 	timestamp0_irq;	//micros timer
};


//extern CEncoder Encoder;

#endif