/**********************************************************************

CurrentMonitor.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "DCCpp_Uno.h"
#include "CurrentMonitor.h"
#include "Comm.h"

///////////////////////////////////////////////////////////////////////////////

CurrentMonitor::CurrentMonitor(int pin, char *msg){
    this->pin=pin;
    this->msg=msg;
    current=0;
  } // CurrentMonitor::CurrentMonitor
  
boolean CurrentMonitor::checkTime(){
  if(millis()-sampleTime<CURRENT_SAMPLE_TIME)            // no need to check current yet
    return(false);
  sampleTime=millis();                                   // note millis() uses TIMER-0.  For UNO, we change the scale on Timer-0.  For MEGA we do not.  This means CURENT_SAMPLE_TIME is different for UNO then MEGA
  return(true);  
} // CurrentMonitor::checkTime
  
void CurrentMonitor::check(){
  current=analogRead(pin)*CURRENT_SAMPLE_SMOOTHING+current*(1.0-CURRENT_SAMPLE_SMOOTHING);        // compute new exponentially-smoothed current
  if(current>CURRENT_SAMPLE_MAX && digitalRead(SIGNAL_ENABLE_PIN_PROG)==HIGH){                    // current overload and Prog Signal is on (or could have checked Main Signal, since both are always on or off together)
#ifdef OVERCURRENT_REVERSE
    if(!lastSampleOvercurrent){
      reverserPinState=!reverserPinState;
      digitalWrite(OVERCURRENT_REVERSER_PIN, reverserPinState);
    }
    else {
#endif
      digitalWrite(SIGNAL_ENABLE_PIN_PROG,LOW);                                                     // disable both Motor Shield Channels
      digitalWrite(SIGNAL_ENABLE_PIN_MAIN,LOW);                                                     // regardless of which caused current overload
      INTERFACE.print(msg);                                                                         // print corresponding error message
#ifdef OVERCURRENT_REVERSE
    }
    lastSampleOvercurrent = true;
  }
  else if(lastSampleOvercurrent) {
    lastSampleOvercurrent = false;
  }
#endif
} // CurrentMonitor::check  

long int CurrentMonitor::sampleTime=0;
#ifdef OVERCURRENT_REVERSE
  int  CurrentMonitor::reverserPinState = LOW;
#endif
bool  CurrentMonitor::lastSampleOvercurrent = false;
