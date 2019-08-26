Motion Encoders
===========
This project is a library/class to sample quadrature encoded signals to determine the position, direction, focus, and spin velocity of encoders. This code's technique uses the edge detection of an interrupt, a state machine, and microsecond timer. The advantage of this code is that it provides precise and stable control over the encoder with sub-1-millisecond quickness. This code is organized as a Class so every encoder is its software object without duplicating code (DNR). This library is compatible with the Arduino IDE.

## Application
This code is for an encoder with pushbutton switch typically found as volume, tuning, brightness, or motor control. The encoder portion may also have detents and rotated by a physical knob of countless varieties. 
The position, direction, and focus (switch versus encoder) of the encoder's properties can be read by the calling application using a public method (function call). The velocity of the encoder's rotation is also measured via a microsecond timer so that the software can vary the rate-of-change.  
All of the above is intended to create the feel of an analog potentiometer using digital techniques of an encoder/switch.

    QUADRATURE - There are 2 signals (Channel A, Channel B)
    and therefore 4 possible states (edges) that 
    occur, hence "quad".

    METHOD 
    Detect interrupt on edge, then sample an encoder's signals, 
    then set variables to indicate direction and position. 
    For precise control, a PSNS state machine supplements the signals that were sampled to more accurately sense the encoder's movement.   
    Optionally, a microsecond timer is used to measure the rotational speed.  
    
    SIGNAL SAMPLING 
    The 2 encoder signals are assigned to interrupts 
    (or might be polled).  For interrupts, it may be necessary to interrupt on every half-cycle (half-step) or full-cycle (full-steps) of the sampled signals.  In my finding, full-steps provided the best combination of precision and quickness. 

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
     
    For IoT processors, 10 to 25 microseconds is a typical estimate per I/O sample using a library call.  It is desirable for the interrupt service routine (ISR) to complete within 100 microseconds. 
     
    Note: The encoder's signals will typically need to be debounced using an RC circuit to dampen noise. See DEBOUNCE below.
    
    STATE
    A PSNS (Previous-State, Next-State) state machine is a table of desired states. As the encoder turns and the 
    CH-A CH-B signals are sampled, the values read are compared with the previous state (PS) and next state (NS) in a 
    lookup table. As desired states are found, the direction becomes known for the desired sample. 
    Subsequently, the position can be incremented or decremented. Uncertain/unknown states are discarded. 

    SCALE
    Each state typically corresponds to 1 detent. 
    A 24 point encoder with 24 detents would have 24 positions in one rotation. If adding "1" to every position change,
    it would take 4 rotations to increase the position indicator from 1 to 96. If adding "10" to every position change, it would take 1/2 turn or 10 detents to reach 96.
    If an application requires 1024 positions (10-bit resolution),
    then it becomes necessary to vary the rate of change
    based on the speed of the encoder's rotation to
    traverse the entire scale with minimal effort.  This 
    variable rate of change allows for coarse/fine adjustment.
    It can be accomplished by timing the interval between the encoder's interrupts.  

      TIMING
    Timing the interval between interrupts can indicate the twisting speed of the encoder (velocity).  This can be used to vary the rate-of-change to create a coarse/fine feel like that of a potentiometer. 

    Here are rotational timing measurements 
    using an oscilloscope for a 24 point 
    rotary encoder with a 25mm (1") diameter knob
    as turned by a real human.

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
    If the interval between CH-A interrupts is around 
    20 ms, then the twisting speed is considered "fast" 
    so the desired adjustment is "coarse" 10x (10:1) instead of 
    "normal/fine" 1x (1:1) default. This technique would be considered a "variable rate" adjustment. 

  DEBOUNCE
   There is typically bounce (switching noise) on a rotary 
   encoder's pins on the "break" (and somewhat on the "make") 
   when the encoder's signals are viewed on an oscilloscope. 
   Unfortunately, the bounce happens within the window the signals need to be sampled thus making encoders tricky to use for precise and stable adjustments.  

   A significant bounce on the "break" was typically observed at 2-3 milliseconds (and sometimes up to 5ms). 
   A bounce of 1ms was sometimes observed on a slow "make".  
   Since there are 2 signals in a quadrature encoder firing at offset times, it is doubly complicated to confidently detect levels/edges due to bounce. 
   
   The 2 encoder signals will likely require hardware or software 
   debounce to improve the stability of readings (or both).
   
   Hardware: Typically, an RC circuit is applied to the 
   encoder's signal lines to reduce the bounce
   (10K ohm, 0.01uf). Often, this is effective but not always a perfect fix.  Additionally, a Schottky input may further reject unintended noise from triggering logic levels. 
   
   Software: To further debounce the encoder's signals, 
   software techniques like timing delays, multiple samples, 
   edge detection, inference (best guess from the last state), 
   pattern/state (PSNS), or in combination may be used.  

## Example
An example program is included in the examples folder. This code is organized as a Class so each encoder can be its software object.

## Build
This code is organized as an installable Arduino library and is listed as "Motion Encoders".  Copy this library into the Arduino libraries folder and #include the Motion_Encoders.h file into your program. 

## Supported Platforms
This code applies to newer style "3.3-volt" boards with the SAMD21 processor (such as the Adafruit Feather or Arduino MKR). That's because these boards/processors have enough interrupt pins to directly handle 3 encoders at once (with pushbutton switches, 9 signals total). 

## References
A variety of implementations and understandings of encoders from the Arduino community.
See description and source examples from:
https://playground.arduino.cc/Main/RotaryEncoders

PSNS states (Previous State, Next State) write-up:
https://www.best-microcontroller-projects.com/rotary-encoder.html#Taming_Noisy_Rotary_Encoders

State machine lookup table with half/full steps sampling - Ben Buxton (2011).
http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html

## Contact
https://github.com/willpatton/Motion_Encoders

## License
Copyright (c) 2018 Will Patton. MIT License.
