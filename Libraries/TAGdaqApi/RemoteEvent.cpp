/*!
 \file RemoteEvent.cpp
 \brief  Implementation of RemoteEvent
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "RemoteEvent.hh"

/*!
 \class RemoteEvent
 \brief Remote event
 */

//------------------------------------------+-----------------------------------
//! Constructor.
RemoteEvent::RemoteEvent()
: BaseFragment(),
  time_sec(0),
  time_usec(0),
  eventNumber(0),
  evtSize(0)
{
}

//------------------------------------------+-----------------------------------
//! Copy contructor.
//!
//! \param[in] right event to copy
RemoteEvent::RemoteEvent(const RemoteEvent& right)
: BaseFragment(right),
  time_sec(right.time_sec),
  time_usec(right.time_usec),
  eventNumber(right.eventNumber),
  evtSize(right.evtSize)
{
   values.reserve(right.values.size());
   values = right.values;
}

//------------------------------------------+-----------------------------------
//! operator=
//!
//! \param[in] right event to equal
const RemoteEvent& RemoteEvent::operator=(const RemoteEvent& right)
{
   BaseFragment::operator=(right);

   time_sec    = right.time_sec;
   time_usec   = right.time_usec;
   eventNumber = right.eventNumber;
   evtSize     = right.evtSize;
   
   return *this;
}

//------------------------------------------+-----------------------------------
//! Destructor.
RemoteEvent::~RemoteEvent(){
}

//------------------------------------------+-----------------------------------
//! read data
//!
//! \param[in] p1 daq file pointer
void RemoteEvent::readData(unsigned int **p1){

  unsigned int * p = *p1;
  channelID= *p;
  ++p;
  time_sec= *p;
  ++p;
  time_usec= *p;
  ++p;
  eventNumber= *p;
  ++p;
  //  remoteInfo= *p;
  ++p;
  evtSize= *p;
  ++p;
  if( evtSize!=0){
    for (unsigned int i=0; i<evtSize-1; i++){
      values.push_back(*p);
      ++p;
    }
    //int WORD= *p;
    //std::cout<<"\n The previous word is:"<< std::hex << *(p-1) << std::endl;
    //std::cout<<"\n The current word is:"<< std::hex << WORD << std::endl;
  } else {
    //std::cout<<"Size of RemoteEvent " << std::hex << channelID << " is zero!"<<std::endl;
  }
  *p1 = p;
}

//------------------------------------------+-----------------------------------
//! Print data
void RemoteEvent::printData () const {
  printf ("Remote DATA: \n");
  printf ("Channel ID (hex): %x\n",  channelID);
  printf ("Time in seconds: %d\n",  time_sec);
  printf ("Time in microseconds: %d\n",  time_usec);
  printf ("Number of Event: %d\n",  eventNumber);
  printf ("Data size: %d\n",  evtSize);
  printf("\n");
}
