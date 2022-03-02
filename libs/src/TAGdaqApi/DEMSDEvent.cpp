/*!
 \file DEMSDEvent.cpp
 \brief  Implementation of DEMSDEvent
 */

#include <stdlib.h>
#include <stdio.h>
#include "DEMSDEvent.hh"
#include <iostream>

/*!
 \class DEMSDEvent
 \brief DE MSD card event
 */

//------------------------------------------+-----------------------------------
//! Destructor.
DEMSDEvent::~DEMSDEvent(){}


//------------------------------------------+-----------------------------------
//! read data
//!
//! \param[in] p1 daq file pointer
void DEMSDEvent::readData(unsigned int **p1){

   RemoteEvent::readData(p1);
   
   if( evtSize!=0){
      detectorHeader = values[0];
      boardHeader = values[1];
      hardwareEventNumber = values[2];
      triggerCounter = values[2];
      BCOofTrigger = values[3];
   }else
   {
      detectorHeader = 0x00000bad;
      boardHeader =    0x0000dead;
      hardwareEventNumber = -1;
      triggerCounter = -1;
      BCOofTrigger = -1;
   }
   fillStrip();
} 

//------------------------------------------+-----------------------------------
//! Print data
void DEMSDEvent::printData () const{

  printf ("DEMSDEvent DATA: \n");
  printf ("Channel ID (hex): %x\n",  channelID);
  printf ("Time in seconds: %d\n",  time_sec);
  printf ("Time in microseconds: %d\n",  time_usec);
  printf ("Number of Event: %d\n",  eventNumber);
  printf ("Data size: %d\n",  evtSize);
  printf ("Detector Header: %x\n",  detectorHeader);
  printf ("Board Header: %x\n",  boardHeader);
  printf ("HW Event Number: %d\n",  hardwareEventNumber);
  printf ("Trigger Counter (hw): %d\n",  triggerCounter);
  printf ("BCO counter: %d\n",  BCOofTrigger);
  printf("\n");
}

//------------------------------------------+-----------------------------------
//! Check event/trigger number
bool DEMSDEvent::check() const {
  if( evtSize!=0 ){
    if( eventNumber==triggerCounter)
      return true;
    else 
      return false;
  }
  else
    return true;
}

//------------------------------------------+-----------------------------------
//! Fill strip information
void DEMSDEvent::fillStrip(){

   if(evtSize!=0) {
      
      u_int data = 0;
      int wordOfData = 10;
      for (int k = 0; k < 128; ++k){//loop on strip read  by one ADC
         
         for (int ADC = 0; ADC < 5; ++ADC){
            data = values[wordOfData];
            wordOfData += 1;
            if (ADC < 2){
               Yplane[ADC*256+k] = ((data >> 16) & 0xFFFF) >> 2;
               Yplane[ADC*256+128+k] = (data & 0xFFFF) >> 2;
            }
            else if (ADC == 2){
               Yplane[ADC*256+k] = ((data >> 16) & 0xFFFF) >> 2;
               Xplane[(ADC-2)*256+k] = (data & 0xFFFF) >> 2;
            }
            else if(ADC == 3){
               Xplane[128+k] = ((data >> 16) & 0xFFFF) >> 2;
               Xplane[256+k] = (data & 0xFFFF) >> 2;
            }
            else if(ADC==4){
               Xplane[384+k] = ((data >> 16) & 0xFFFF) >> 2;
               Xplane[512+k] = (data & 0xFFFF) >> 2;
            }
            
         }
      }
   }
}
