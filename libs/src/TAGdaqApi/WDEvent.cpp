/*!
 \file WDEvent.cpp
 \brief  Implementation of WDEvent
 */

#include <stdlib.h>
#include <stdio.h>
#include "WDEvent.hh"
#include <iostream>

/*!
 \class WDEvent
 \brief Wave Dream event
 */

//------------------------------------------+-----------------------------------
//! Destructor.
WDEvent::~WDEvent(){}

//------------------------------------------+-----------------------------------
//! read data
//!
//! \param[in] p1 daq file pointer
void WDEvent::readData(unsigned int **p1){

  RemoteEvent::readData(p1);

  for(unsigned int ibin = 0; ibin<32; ibin++)
    TriggerGenerationBin[ibin] = 0; 

  if( evtSize!=0 ){
    detectorHeader = values[0];
    boardHeader = values[1];
    numberWords = evtSize;
    BCOofTrigger = values[3];
    hardwareEventNumber = values[5];
    numBoards=0;
    TWChans=0;
    CaloChans=0;
    TriggerPattern = 0;
    trigType=values[7]>>16;
    u_int idx=9;
    while(idx<evtSize){
      //      std::cout<<"idx = "<<idx<<std::endl;
      if( (values[idx]&0xFFFF)==0x2342 ){ // a WD board  'B#'
	numBoards++;
	u_int board= (values[idx]>>16)&0xFFFF;
	//	std::cout<<"Board id: "<<board<<std::endl;
	idx+=4;
	while(idx<evtSize && (values[idx]&0xFFFF)==0x3043  ){ // channel 'C0'
	  // a WD channel 'C0'
	  if( board==158 || board==159 || (board>=165 && board<=168)) TWChans++;
	  if( board<157 || board>=169 ) CaloChans++;
	  idx+=512+2;
	}
      } else if( (values[idx]&0xFFFF)==0x2354 ){ // a Trig board  'T#'
	idx++;
	int nbanks= values[idx++];
	while( idx<evtSize && nbanks>0){
	  if((values[idx]&0xFFFFFFFF)==0x49475254  ){ // banca TRGI'
	    TriggerPattern = ((uint64_t)values[idx+3]) | 
	      (((uint64_t)values[idx+4])<<32);
	    idx +=values[idx+1]+2;
	    nbanks--;
	  } else if ( (values[idx]&0xFFFFFFFF)==0x4e454754  ){ // banca TGEN
	    idx++;           // skip bank header
	    idx++;           // skip bank size
	    // LG	    
	    int pos = values[idx++];   // this should contain the words stored (32)
	    // pos e' lo stop ID di un vettore circolare lungo 32
	    for(unsigned int ibin = 0; ibin<32; ibin++)
	      TriggerGenerationBin[(ibin+32-pos) % 32] = values[idx++]; // bit 31:0
	    for(unsigned int ibin = 0; ibin<32; ibin++)
	      TriggerGenerationBin[(ibin+32-pos) % 32] = 
		(((uint64_t)values[idx++])<<32) | TriggerGenerationBin[(ibin+32-pos) % 32]; // bit 63:32	    
	    //	    idx += values[idx+1]+2;   // MV could be:  
	    nbanks--;
	  } else {
	    idx=evtSize;
	    nbanks--;
	  }
	}
      } else if( values[idx]==0xfafefafe || values[idx]==0xbacca000) {
	idx=evtSize;
      } else {
	std::cout<<" Wrong data:"<<idx<<"  "<<values[idx]<<std::endl;
	idx=evtSize;
      }
    }
  }
}

//------------------------------------------+-----------------------------------
//! Print data
void WDEvent::printData () const{

  printf ("WDEvent DATA: \n");
  printf ("Channel ID (hex): %x\n",  channelID);
  printf ("Time in seconds: %d\n",  time_sec);
  printf ("Time in microseconds: %d\n",  time_usec);
  printf ("Number of Event: %d\n",  eventNumber);
  printf ("Number of words: %d\n",  numberWords);
  printf ("Detector Header: %x\n",  detectorHeader);
  printf ("Board Header: %x\n",  boardHeader);
  printf ("HW Event Number: %x\n",  hardwareEventNumber);
  printf ("BCO Counter: %x\n",  BCOofTrigger);
  printf("\n");
}
