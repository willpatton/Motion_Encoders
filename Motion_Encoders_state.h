
/**
 * Motion_Encoders.h
 * @author:   Will Patton 
 * @url:      https://github.com/willpatton/Motion_Encoders
 * @license:  MIT License
 *
 *
 * Rotary encoder state machine
 *
 *
 * Source Credits:
 *
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 * This is a modified version of Ben Buxton's rotary encoder
 * state machine organized for the Arduino IDE.
 * http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
 * 
 */

#ifdef __MOTION_ENCODERS_STATE_H 

	//direction of state table
    #define DIR_NONE 0x0
    #define DIR_CW   0x10
    #define DIR_CCW  0x20

    /*
    * The below state table has, for each state (row), the new state
    * to set based on the next encoder output. From left to right in,
    * the table, the encoder outputs are 00, 01, 10, 11, and the value
    * in that position is the new state to set.
    */

	//UNCOMMENT - to emit codes twice per step (FINE).
	//#define HALF_STEP

	//HALF-STEP - Uses the half-step state table (emits a code at 00 and 11)
	#ifdef HALF_STEP
	  #define R_START 		0x0
	  #define R_CCW_BEGIN 	0x1
	  #define R_CW_BEGIN 	0x2
	  #define R_START_M 	0x3
	  #define R_CW_BEGIN_M 	0x4
	  #define R_CCW_BEGIN_M 0x5
	  const unsigned char ttable[6][4] = {
	    {R_START_M,           R_CW_BEGIN,     R_CCW_BEGIN,  R_START},           // R_START (00)
	    {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},           // R_CCW_BEGIN
	    {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},           // R_CW_BEGIN
	    {R_START_M,           R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},           // R_START_M (11)  
	    {R_START_M,           R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},  // R_CW_BEGIN_M 
	    {R_START_M,           R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW}, // R_CCW_BEGIN_M
	  };
	#endif

	#ifndef HALF_STEP
	//FULL-STEP - Uses the full-step state table below (emits a code at 00 only) 
	  #define FULL_STEP
	  #define R_START 		0x0
	  #define R_CW_FINAL 	0x1
	  #define R_CW_BEGIN 	0x2
	  #define R_CW_NEXT 	0x3
	  #define R_CCW_BEGIN 	0x4
	  #define R_CCW_FINAL 	0x5
	  #define R_CCW_NEXT 	0x6
	  const unsigned char ttable[7][4] = { 
	    {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},            // R_START
	    {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},   // R_CW_FINAL 
	    {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},            // R_CW_BEGIN 
	    {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},            // R_CW_NEXT 
	    {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},            // R_CCW_BEGIN 
	    {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},  // R_CCW_FINAL 
	    {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},            // R_CCW_NEXT
	  };
	#endif


#endif