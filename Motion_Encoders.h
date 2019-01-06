/**
 * Motion_Encoders.h
 *
 * @author: Will Patton http://willpatton.com 
 *
 */

#ifndef __MOTION_ENCODERS_H
#define __MOTION_ENCODERS_H

#include "Arduino.h"

class CEncoder {

  public:
    //prototypes
    CEncoder(uint8_t _swPin, uint8_t _enPinClk, uint8_t _enPinData);
    uint16_t get_instance();      //gets the unique identifier of the encoder object
    void loop_controls();         //caller
    void switches();              //read push button switch
    uint8_t get_swPos();          //get switch position. 0 = low pressed, 1 = not pressed
    void isrEncoderA();           //ISR -A
    void isrEncoderB();           //ISR -B
    int16_t get_enPos();          //get encoder position
    uint8_t get_enDir();          //encoder direction
    void set_enPos(int16_t val);  //set encoder position. useful for resetting position
    uint8_t get_focus();          //which control has the current focus

  private:
    uint16_t  instance;       //a random instance number to identify this object 
    uint8_t   _debug = false; //echo info to serial console

    //pins
    uint8_t   _swPin;        //switch
    uint8_t   _enPinClk;     //A-channel of encoder.  Sometimes known as "clk"
    uint8_t   _enPinData;    //B-channel "     "         "         "     "data"

    //switch
    uint8_t	sw_state; 	    //HIGH, LOW. logic state  
    uint8_t	sw_pos = 0;	    //indicates switch position 0,1,2,3...

    //encoder
    uint8_t	en_clk; 	      //clk is unknown at setup. HIGH, LOW
    uint8_t	en_data; 	      //data is unknown at setup. HIGH, LOW
    int16_t	en_pos = 0;     // +/- 32768
    int8_t  en_dir = 0;     //-1 ccw, 0 none, 1 cw
    int8_t en_rate = 1;     //a multiplier: 1x normal/fine (1:1), 10x coarse (10:1)  0 not allowed
    volatile uint8_t enIsrFlag = 0;  //1 ISR in progress, 0 isr cleared, free
    
    //screen
    #define SW_MIN 0 
    #define SW_MAX 5   

    //focus
    #define NONE 0
    #define SWITCH 1
    #define ENCODER 2
    int focus = NONE;

    //timers
    uint32_t  timer;			     //TBD
    uint32_t  timer_hold;		   //millisecond timer to measure duration of the switch press
    uint32_t 	timestamp_irq;	 //microsecond timer to measure durecation between IRQs
};


//extern CEncoder Encoder;

#endif