/**
 * Motion_Encoders.h
 * @author:   Will Patton 
 * @url:      https://github.com/willpatton/Motion_Encoders
 * @license:  MIT License
 */

#ifndef __MOTION_ENCODERS_H
#define __MOTION_ENCODERS_H

//UNCOMMENT - to use the state algorithm. Else, use the "signal only" algorithm
#define __MOTION_ENCODERS_STATE_H 

#include "Arduino.h"

#define SW_MIN 0        //sw limit min
#define SW_MAX 5        //sw limit max
#define NONE      0     //focus
#define SWITCH    1     //focus
#define ENCODER   2     //focus
#define SW_RESET  3     //focus - the "reset" has a temporary focus

class CEncoder {

  public:
    //prototypes
    CEncoder(uint8_t _swPin, uint8_t _enPinClk, uint8_t _enPinData);
    uint16_t get_instance();      //gets the unique identifier of the encoder object
    void loop_controls();         //caller
    void switches();              //read push button switch
    uint8_t get_swState();        //get switch state HIGH, LOW
    uint8_t get_swPos();          //get switch position 0,1,2,3...
    void set_swPos(int16_t val);  //set switch position 0,1,2,3...
    void isrEncoderA();           //ISR -A
    void isrEncoderB();           //ISR -B
    int16_t get_enPos();          //get encoder position
    uint8_t get_enDir();          //encoder direction
    void set_enPos(int16_t val);  //set encoder position
    uint8_t get_focus();          //which control has the current focus

  private:
    uint16_t  instance;      //a random instance number to identify this object 
    uint8_t   _debug = true; //echo info to serial console

    //pins
    uint8_t   _swPin;        //switch
    uint8_t   _enPinClk;     //A-channel of encoder.  Sometimes known as "clk"
    uint8_t   _enPinData;    //B-channel "     "         "         "     "data"

    //switch
    uint8_t	sw_pos = 0;     //indicates switch position 0,1,2,3...
    uint8_t sw_state;       //HIGH, LOW. logic state  

    //encoder
    uint8_t state;          //holds the combined logic states of the encoder's pin
    uint8_t	en_clk; 	      //clk is unknown at setup. HIGH, LOW
    uint8_t	en_data; 	      //data is unknown at setup. HIGH, LOW
    int16_t	en_pos = 0;     // +/- 32768
    int8_t  en_dir = 0;     //-1 ccw, 0 none, 1 cw
    int8_t  en_rate = 1;    //an adder to increase rate of change: 1x normal/fine (1:1), 10 coarse (10:1)  0 not allowed
    volatile uint8_t enIsrFlag = 0;  //1 ISR in progress, 0 isr cleared, free
    
    //focus
    uint8_t focus = NONE;   //none = 0, 1 = switch, 2 = encoder...

    //timers
    //uint32_t  timer;			   //TBD
    uint32_t  timer_sw_hold;	 //millisecond timer to measure duration of the switch press
    uint32_t 	timestamp_irq;	 //microsecond timer to measure durecation between IRQs


    //state encoder algo
    #ifdef __MOTION_ENCODERS_STATE_H
    #include "Motion_Encoders_state.h"
    #endif
 
};


//extern CEncoder Encoder;

#endif