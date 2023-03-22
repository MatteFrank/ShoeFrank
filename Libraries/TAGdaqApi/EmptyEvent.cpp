/*!
 \file EmptyEvent.cpp
 \brief  Implementation of EmptyEvent
 */

#include <stdlib.h>
#include <stdio.h>
#include "EmptyEvent.hh"

/*!
 \class EmptyEvent
 \brief empty event
 */

//------------------------------------------+-----------------------------------
//! Destructor.
EmptyEvent::~EmptyEvent(){}

//------------------------------------------+-----------------------------------
//! read data
//!
//! \param[in] p1 daq file pointer
void EmptyEvent::readData(unsigned int ** p1){

  unsigned int * p = *p1;
  channelID= *p;
  ++p;
  time_sec= *p;
  ++p;
  time_usec= *p;
  ++p;
  lumiBlock= *p;
  ++p;
  eventNumber= *p;
  ++p;
  ++p;
  ++p;
  *p1 = p;
}

//------------------------------------------+-----------------------------------
//! Print data
void EmptyEvent::printData () const {

    printf ("EMPTY DATA ROB: \n");
    printf ("Channel ID (hex): %x\n",  channelID);
    printf ("Time in Seconds: %d\n",  time_sec);
    printf ("Time in MicroSeconds: %d\n",  time_usec);
    printf ("LumiBlock Number: %d\n",  lumiBlock);
    printf ("Event Number: %d\n",  eventNumber);
    printf ("\n");
}
