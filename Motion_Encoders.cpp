/**
 * Motion_Encoders.h
 *
 * @author: Will Patton http://willpatton.com 
 *
 */

//#define __MOTION_ENCODERS_H
#include "Motion_Encoders.h"

//constructor
CEncoder::CEncoder(uint8_t pinSW, uint8_t pinEnClk, uint8_t pinEnData){

    //instance
    randomSeed(analogRead(0));    //create random seed value
    instance = random(100,1000);

    //pin
    pinMode(pinSW, INPUT);
    _swPin = pinSW;

    pinMode(pinEnClk, INPUT);
    _enPinClk = pinEnClk;

    pinMode(pinEnData, INPUT);
    _enPinData= pinEnData;

    //init
    sw_state  = digitalRead(_swPin);     //read initial state
    sw_pos  = 0;
    en_clk    = digitalRead(_enPinClk);   //read initial state
    en_data   = digitalRead(_enPinData);  //read initial state
    en_pos = 0;
    en_dir = 0;
    en_rate = 1;    //1 normal (1:1 multiplier)
    focus = NONE;

    //timers
    timestamp_irq = micros();
    timer = millis();
}


/**
 loop controls
*/
void CEncoder::loop_controls(){

  //READ 
  switches();   //a caller to read the pushbutton switch by polling.  

}


/**
 * SWITCHES
 * read switch via polling. Not millisecond time sensitive.
 */
void CEncoder::switches(){

    //press 
    if(!digitalRead(_swPin) && sw_state == HIGH){
      sw_state = LOW;
      focus = SWITCH;
      if(_debug){Serial.print("Press Switch "); Serial.println(_swPin);}
      sw_pos++;
      if(sw_pos < SW_MIN){
        sw_pos = SW_MAX;
      }
      if(sw_pos > SW_MAX){
        sw_pos = SW_MIN;
      }
      timer_hold = millis();  //begin timer upon each "press"
    }

    //release 
    if(digitalRead(_swPin) && sw_state == LOW){
      sw_state = HIGH;
      focus = SWITCH;
      if(_debug){Serial.print("Release Switch  "); Serial.println(_swPin);}
    }
    
    //reset - TIMER HOLD TODO
    if(!digitalRead(_swPin) && sw_state == LOW && (millis()-timer_hold > 3000)){ //&& !digitalRead(A4)
      sw_state = HIGH;
      focus = SWITCH;
      sw_pos = SW_MIN;  
      en_pos = 127;  //TEMP PATCH
      en_dir = 0;
      if(_debug){Serial.println("Reset Switch");}
    }
    
}//end buttons


/**
 * ISR CH-A 
 * assumes "interrupt" on rising edge only
 * CH-A accelarates the variable rate rotation
 */
void CEncoder::isrEncoderA(){

  //if already set, do nothing, possibly noise
  if(enIsrFlag){
    return;
  }

  //Detect turning rate in microseconds
  //Has it been 20ms or less since the last CH-A IRQ?
  if(micros() - timestamp_irq < 20000) {
    //yes, then the encoder is twisting rapidly
    en_rate = 10;   //10x (10:1) multiplier coarse adjustment
  } else {
    en_rate = 1;    //1x  (1:1) multiplier normal/fine adjustment
  }
  timestamp_irq = micros(); //reset timestamp this event

  //Set
  focus = ENCODER;    //this control is now in focus
  enIsrFlag = true;   //set flag
  en_clk = true;      //if here due to rising edge, then this signal must be high

  //Read
  en_data = digitalRead(_enPinData); //data pin

  //Calc
  if(!en_data){
    //clk is leading data, rotation is CW
    en_dir = 1;
    en_pos = en_pos + en_rate;
  }
  if(en_data){
    //clk is same as data, rotation is CCW
    en_dir = -1;
    en_pos = en_pos - en_rate;
  }
  if(_debug){Serial.print("isrEncoderA("); Serial.print(_enPinClk); Serial.print(") pos: ");Serial.println(en_pos);}
}

/**
 * ISR CH-B 
 * assumes "interrupt" on rising edge only
 */
void CEncoder::isrEncoderB(){

 //if already clear, do nothing
  if(!enIsrFlag){
    return;
  }

  focus = ENCODER;     //this control is now in focus
  enIsrFlag = false;   //clear flag
  en_data = true;      //if here due to rising edge, then this signal must be high
  
  en_clk = digitalRead(_enPinClk);  //clk pin
  if(en_clk){
    //clk is leading data, rotation is CW
    en_dir = 1;
    en_pos++;
  }
  if(!en_clk){
    //clk is same as data, rotation is CCW
    en_dir = -1;
    en_pos--;
  }

  if(_debug){Serial.print("isrEncoderB("); Serial.print(_enPinData); Serial.print(") pos: ");Serial.println(en_pos);}
}


//GETTERS
uint16_t CEncoder::get_instance(){
  return instance;
}

uint8_t CEncoder::get_swPos(){
  return sw_pos;
}

int16_t CEncoder::get_enPos(){
  return en_pos;
}

uint8_t CEncoder::get_enDir(){
  return en_dir;
}

uint8_t CEncoder::get_focus(){
  return focus;
}

//SETTERS
void CEncoder::set_enPos(int16_t val){
  en_pos = val;
}
