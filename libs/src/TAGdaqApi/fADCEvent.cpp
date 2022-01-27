/*!
 \file fADCEvent.cpp
 \brief  Implementation of fADCEvent
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "fADCEvent.hh"

/*!
 \class fADCEvent
 \brief Flash ADC event
 */

//------------------------------------------+-----------------------------------
//! Destructor.
fADCEvent::~fADCEvent(){}

//------------------------------------------+-----------------------------------
//! read data  v1742 when only 1 GROUP ON
//!
//! \param[in] p1 daq file pointer
void  fADCEvent::readData(unsigned int **p1){

  unsigned int * p = *p1;

  channelID = *p;
  ++p;
  fadc_sec = *p;
  ++p;
  fadc_usec = *p;
  ++p;
  eventNumber = *p;
  ++p;
  data = *p;
  eventSize = data & 0xFFFFFFF;
  ++p;
  data = *p;
  nchans = getnchans(data);
  // MSBTimeTag = (data >> 8) & 0xFFFF;
  ++p;
  //eventFADCounter = *p & 0xFFFFFF;
  eventFADCounter = *p & 0x3FFFFF;
  ++p;
  triggerTimeTag = *p;
  // extendedTriggerTimeTag = (((uint64_t)MSBTimeTag << 32) | triggerTimeTag) & 0xFFFFFFFFFFFF;
  ++p;

  if( nchans > 0 ){
    //reading the number of word per channel   
    for(int i=0; i<1024; i++){
      
      ++p; 
      data = *p;

      channel[0].push_back(data & 0xFFF);
      channel[1].push_back( (data>>12) & 0xFFF);
      ++p;
      channel[2].push_back( ((data>>24) & 0xFF) | ((*p & 0xF)<<8) );

      data = *p; 
      channel[3].push_back( (data>>4)  & 0xFFF );
      channel[4].push_back( (data>>16) & 0xFFF );

      ++p;
      channel[5].push_back( ((data>>28) & 0xF) | ((*p & 0xFF)<<4) );

      data = *p; 
      channel[6].push_back( (data>>8)  & 0xFFF );
      channel[7].push_back( (data>>20)  & 0xFFF );      
    }

    ++p;
    ++p;

  } else {
    std::cout<< "FADC error: 0 channels" << std::endl;
    std::cout<< "TIME OUT"<<std::endl;
    std::cout<< "sizeof(unsigned int)" << sizeof(unsigned int)<<std::endl;

    
    *p1 = p;
  }

  ++p;
  *p1 = p;
}

//------------------------------------------+-----------------------------------
//! print V
void fADCEvent::printV (){

  for ( u_int sample = 0; sample < channel[0].size(); sample++ ){
    std::cout << sample <<" - ";
    for ( u_int i = 0; i < nchans; i++ ){
      std::cout << channel[i][sample] << " ";
    }
    std::cout << std::endl;
  }
}

//------------------------------------------+-----------------------------------
//! get number of words for a  channel
//!
//! \param[in] nCHans channel number
int fADCEvent::getWordsChannel( int nCHans ) {
  int WordsChannel=(eventSize - 4)/nCHans;
  return WordsChannel;
}

//------------------------------------------+-----------------------------------
//! get number of channels per data
//!
//! \param[in] aData data word
int fADCEvent::getnchans ( int aData ){

  int mask = 1;
  int numChans = 0;
  bool status;
  for ( int i = 0; i < 4; i++ ){
    status = false;
    if( ( aData & mask ) == mask ){
      status = true;
      numChans++;
    }
    active.push_back(status);
    mask *= 2;
  }
  return numChans;
}

//------------------------------------------+-----------------------------------
//! Print data
void fADCEvent::printData () const {

  printf ("FLASH ADC DATA: \n");
  printf ("Channel ID (hex): %x\n",  channelID);
  printf ("Number of Event: %d\n",  eventNumber);
  printf ("Number of channels: %d\n",  nchans);
  printf ("Event Size: %d\n",  eventSize);
  printf ("Internal event counter: %d\n",  eventFADCounter);
  printf ("ETTT (unit of 4 ns):");
  printf ("%llu PRIx64\n", extendedTriggerTimeTag);
  printf("\n");
}
