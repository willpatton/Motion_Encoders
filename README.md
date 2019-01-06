Motion Encoders
===========
This project is a library/class to sample quadrature encoded signals to determine position, direction, focus, and spin velocity of an encoder. This technique uses the edge detection of an interrupt,  a state machine, and microsecond timer. The advantage of this code is that it can an instantiate an object for numerous encoders in an application without duplicating code (DNR). This library is compatible with the Arduino IDE.

## About Rotary Encoders

ROTARY ENCODERS
   
  APPLICATION 
    This code is for an encoder/switch typically found
    as volume, tuning, speed, or brightness control.
    The encoder has a physical knob with detents and 
    push button switch. The position, direction, and
    focus (switch or encoder) of the encoder is 
    read by the calling application using a 
    public method (function call). 
     
    QUADRATURE - There are 2 signals and therefore 4 possible 
    states (edges) that occur (i.e. "quad").  

    METHOD 
	Detect interrupt on edge, then sample an encoder's signal once, 
	then set flag(s) to indicate state.  
	Optionally, use a microsecond timer to measure the rotational speed.  
	Also, the signals are also debounced using a 
	typical RC circuit in order to minimize spurious interrupts. 

	SIGNALS 
     CH-A leads CH-B = Clockwise CW
     A  ____|----|____|----|____|
     B     ____|----|____|----|
            ^ ^  ^ ^
            | |  | |
            | |  | SAMPLE (*) 
            | |  IRQ A FALLING edge samples B as HIGH
            | SAMPLE (*)  
            IRQ A RISING edge samples A as LOW
 
     CH-A lags CH-B = Counter Clockwise CCW
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
	Here are rotational timing measurements 
	using an oscilloscope for a 24 point 
	rotary encoder with a 1" diameter knob
	turned by a real human.

    A <--> A  
    "INTERVAL" from CH A to CH A.
    Twisting speed  Interval
		Slow          200ms
		Medium        100ms
		Fast          50ms
		Rapid         10ms      (Super-quick twist)
		Flick         1.50ms    (Fast as humanly possible)

    A <--> B  
    "INTERVAL" from CH A to CH B.
    	Twisting speed  Interval
      	Slow          10ms
      	Medium        5ms
      	Fast          3ms 
      	Rapid         1.0ms to 2.0ms   
      	Flick         500us      

	The timing interval A <--> A is measured with a 
	microsecond timer (in software, by the ISR).  
	If the interval between CH-A interrupt is around 
	20 ms, then the twisting speed is considered "fast" 
	so the desired adjustment is "coarse" 10x (10:1) instead of 
	"normal/fine" 1x (1:1) default.  This would be a considered
	"variable rate" adjustment. 

  DEBOUNCE
   There is typically bounce (switching noise) on a rotary 
   encoder's pins on the "break" (and somewhat on the "make") 
   when the encoder's signals are viewed on an oscilloscope. 
   Unfortunately, the bounce happens within the window the
   signals need to be sampled thus making encoders tricky to 
   use for precise and stable adjustments.  

   A significant bounce on the "break" was typically 
   observed at 2-3 milliseconds and sometimes up to 5ms. 
   A bounce of 1ms was sometimes observed on a slow "make".  
   Since there are 2 signals in a quadrature encoder 
   firing at offset times, it is doubly complicated to 
   confidently detect edges and sample the 
   the polarity of the 2 switched signals due to bounce.
   
   A rotary encoder without detents may have less bound because 
   the detent's armature doesn't exist thus reducing kickback. 
   
   The 2 encoder signals will likely require hardware or software 
   debounce to improve the stability of readings (or both).
   
   Hardware: Typically, an RC circuit is applied to the encoder's 
   signal lines in order to reduce the bounce.  
   Often times, this is effective but not always a perfect fix.  
   Additionally, a Schottky input may further help reject 
   unintended noise from triggering logic levels.  Software 
   debounce is also needed to ensure stability. 
   
   Software: To further debounce the encoder's signals in software, 
   techniques like timing delays, multiple samples, 
   edge detection, inference (best guess from the last state), 
   pattern/state, or in combination may be used.  For IoT processors, 
   10 to 25 microseconds is a typical estimate per I/O sample. 
   This allows an ISR to sample and exit quickly 
   (typically within 100us).
 
## Example
An example program is included in the examples folder.

## Supported Platforms
This code is known to compile using the Arduino IDE on a SAMD21 processor. 

## For more information

REFERENCES
A variety of implementations and understandings of encoders.
See description and source examples from:
https://playground.arduino.cc/Main/RotaryEncoders

PSNS states (Previous State, Next State):
https://www.best-microcontroller-projects.com/rotary-encoder.html#Taming_Noisy_Rotary_Encoders
