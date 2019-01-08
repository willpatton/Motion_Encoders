/**
 * Motion_Encoders.h
 * @author:   Will Patton 
 * @url:      http://willpatton.com 
 * @license:  MIT License
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
    sw_pos    = SW_MIN;
    en_clk    = digitalRead(_enPinClk);   //read initial state
    en_data   = digitalRead(_enPinData);  //read initial state
    en_pos    = 0;
    en_dir    = 0;
    en_rate   = 1;    //1 normal (1:1 multiplier)
    focus     = NONE;

    //timers
    timestamp_irq = micros();
    //timer = millis();
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
 * read switch via polling.
 */
void CEncoder::switches(){

    //press 
    if(!digitalRead(_swPin) && sw_state == HIGH){
      sw_state = LOW;
      focus = SWITCH;
      if(_debug){Serial.print("Switch("); Serial.print(_swPin);Serial.println(") Press");}
      sw_pos++;
      if(sw_pos < SW_MIN){
        sw_pos = SW_MAX;
      }
      if(sw_pos > SW_MAX){
        sw_pos = SW_MIN;
      }
      timer_sw_hold = millis();  //begin timer upon each "press"
    }

    //release 
    if(digitalRead(_swPin) && sw_state == LOW){
      sw_state = HIGH;
      focus = SWITCH;
      if(_debug){Serial.print("Switch("); Serial.print(_swPin);Serial.println(") Release");}
    }
    
    //reset
    if((millis() - timer_sw_hold > 3000)){ 
      sw_state = HIGH;
      focus = SW_RESET;
      //sw_pos = SW_MIN;
      //en_pos = 0;  
      //en_dir = 0;
      if(_debug){Serial.print("Switch("); Serial.print(_swPin);Serial.println(") RESET");}
    }
    
}//end buttons


/**
 * ISR CH-A 
 * assumes "interrupt" on rising edge only
 * CH-A accelarates the variable rate rotation
 */
void CEncoder::isrEncoderA(){

  //if already set, do nothing, probably noise.  If state, then continue... noise handled okay.
  #ifndef __MOTION_ENCODERS_STATE_H
  if(enIsrFlag){
    return;
  }
  #endif


  //Set
  focus = ENCODER;    //this control is now in focus


  //"signal only" algorithm
  #ifndef __MOTION_ENCODERS_STATE_H
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
  #endif


  //"state" algo
  #ifdef __MOTION_ENCODERS_STATE_H
    //read - input pins
    unsigned char pinstate = (digitalRead(_enPinClk) << 1) | digitalRead(_enPinData);
    //state - lookup table
    state = ttable[state & 0xf][pinstate];

    //direction
    int8_t _dir = state & 0x30;
    if (_dir == DIR_NONE) {
      en_dir = 0; //no change, uncertain direction
      return; // avoid going any further
    }
    if (_dir == DIR_CW) {
      en_dir = 1; //Clockwise
    }
    if (_dir == DIR_CCW) {
      en_dir = -1;//Counter clockwise
    }
  #endif

  //velocity      
  //Measure interval between 2 consecutive encoder interrupts in microseconds
  //Has it been NN microseconds or less since the last interrupt?
  uint32_t interval = micros() - timestamp_irq;
  if((interval < 12000)){
    //twisting rapidly
    en_rate = 50;   //10x (10:1) multiplier coarse adjustment
  } else if(en_dir && (interval < 20000)){
    //medium
    en_rate = 10;   //10x (10:1) multiplier coarse adjustment
  } else {
    //slow (1:1)
    en_rate = 1;    //1x  (1:1) multiplier normal/fine adjustment
  }
  timestamp_irq = micros();  //reset timestamp each time through


  //position
  if(en_dir == 0) {
    //no change
  }
  if(en_dir == 1) {
    en_pos = en_pos + en_rate;
    //if(_debug){Serial.print("ClockWise:");}
  }
  if(en_dir == -1) {
    en_pos = en_pos - en_rate;
    //if(_debug){Serial.print("CounterClockWise:");}
  }

  
  if(_debug){
    Serial.print("isrEncoderA("); Serial.print(_enPinClk); 
    Serial.print(") pos: ");Serial.print(en_pos); 
    Serial.print(") inteval usec: ");Serial.print(interval); 
    Serial.println();
  }
}

/**
 * ISR CH-B 
 * assumes "interrupt" on rising edge only
 */
#ifndef __MOTION_ENCODERS_STATE_H
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
#endif

//GETTERS
uint16_t CEncoder::get_instance(){
  return instance;
}

uint8_t CEncoder::get_swPos(){
  return sw_pos;
}
uint8_t CEncoder::get_swState(){
  return sw_state;
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
void CEncoder::set_swPos(int16_t val){
  sw_pos = val;
}
