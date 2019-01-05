/**
Motion_Encoders.h

@author: Will Patton http://willpatton.com 

ROTORARY ENCODERS

  See description and source examples from:
  https://playground.arduino.cc/Main/RotaryEncoders
   
  APPLICATION 
     This is for a physical user interface control with knob/detent 
     and push button switch such as a volume, tuning, or 
     brightness (response >10ms).

     This code may not be suitable for a high RPM motor encoder (response <10ms).
     
     QUADUATURE - There are 2 signals and therefore 4 possible 
     states (edges) that occur (i.e. "quad").

     METHOD 
     INTERUPT THEN SAMPLE - 
     Detect interupt on edge, then sample the I/O signal. 

     SIGNALS (EDGE DETETION)
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
    Example for a 24 point detent rotoray encoder.

    A <--> A  
    "INTERVAL" from DETENT to DETENT
    Twisting the knob speed  
      Slow    500ms
      Medium  100ms
      Fast    50ms 

    Here are typical timing measurements observed between 
    channels.  The detent forces the contacts to advance.

    A <--> B  
    LEAD/LAG DIFFERENCE (within one detent)
    Twisting the knob speed  
      Slow    10ms
      Medium   5ms
      Fast     3ms 
      
  DEBOUNCE
   * There is significant bounce on rotorary encorder's 
   * pins (likely due to the mechanical swingback of detent's armature).  

   * A significant bounce was typically observed at 2-3 milliseconds and 
   * sometimes up to 5ms on the "break". A bounce of 1ms was 
   * sometimes observed on a slow "make".  Since there are 2 switches 
   * in a quaduature encoder firing at offset times, it is doublely
   * complicated to confidently detect edges and sample the 
   * the polarity of the 2 switched signals.
   * 
   * A rotorary encoder without detents may have less bound (or no 
   * bounce) beause the dentent's armature doesn't exist thus reducing 
   * kickback. 
   * 
   * Signals may require hardware or software debounce to improve 
   * stablility of readings.
   * 
   * Hardware: Properly debounced encoder signals can be 
   * sample as fast as allowed by the processor. For IoT processors, 
   * 10 to 25 microseconds is a typical estimate per I/O sample. 
   * This allows an ISR to sample and exit within 100us.
   * 
   * Software: No hardware debounce means consequences for the 
   * ISR which may take tens-of-milliseconds to complete in order
   * to avoid milliseconds of noise.  This may be okay in
   * an application like rotating a knob where milliseconds
   * of debounce is required. The software must logically debounce 
   * the noise by using techniques like timing delays, multiple
   * samples, inference (best guess from last state) or by  
   * pattern, or in combination. 
 
  FYI 
   //PSNS states
    //(Previous State Next State - code as the input):
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