/*!
 \file DEITREvent.cpp
 \brief  Implementation of DEITREvent
 */

#include <stdlib.h>
#include <stdio.h>
#include "DEITREvent.hh"
#include <iostream>

/*!
 \class DEITREvent
 \brief DE ITR card event
 */

const u_int DEITREvent::m_itrHeader = 0xfafafafa;
const u_int DEITREvent::m_itrTail   = 0xabcdabcd;
const u_int DEITREvent::m_maxSize   = 2047;


//------------------------------------------+-----------------------------------
//! Constructor.
DEITREvent::DEITREvent()
: RemoteEvent(),
  detectorHeader(0),
  boardHeader(0),
  hardwareEventNumber(0),
  triggerCounter(0),
  BCOofTrigger(0),
  clockCounter(0)
{
}

//------------------------------------------+-----------------------------------
//! Copy contructor.
//!
//! \param[in] right event to copy
DEITREvent::DEITREvent(const DEITREvent& right)
: RemoteEvent(right),
 detectorHeader(right.detectorHeader),
 boardHeader(right.boardHeader),
 hardwareEventNumber(right.hardwareEventNumber),
 triggerCounter(right.triggerCounter),
 BCOofTrigger(right.BCOofTrigger),
 clockCounter(right.clockCounter)
{
}

//------------------------------------------+-----------------------------------
//! operator=
//!
//! \param[in] right event to equal
const DEITREvent& DEITREvent::operator=(const DEITREvent& right)
{
   RemoteEvent::operator=(right);
   detectorHeader       = right.detectorHeader;
   boardHeader          = right.boardHeader;
   hardwareEventNumber  = right.hardwareEventNumber;
   triggerCounter       = right.triggerCounter;
   BCOofTrigger         = right.BCOofTrigger;
   clockCounter         = right.clockCounter;
   
   return *this;
}

//------------------------------------------+-----------------------------------
//! Destructor.
DEITREvent::~DEITREvent()
{
}

//------------------------------------------+-----------------------------------
//! read data
//!
//! \param[in] p1 daq file pointer
void DEITREvent::readData(unsigned int **p1)
{
   evtSize   = 0;
   u_int * p = *p1;
   u_int * p_max = *p1 + m_maxSize;
   
   channelID= *p;
   ++p;
   time_sec= *p;
   ++p;
   time_usec= *p;
   ++p;
   eventNumber= *p;
   ++p;
   ++p;
   evtSize= *p;
   ++p;
   
   if(evtSize != 0) {
      // check header
      if (*p != m_itrHeader)
         printf("Error in the event reader %x instead of %x\n", *p, m_itrHeader);
      
      detectorHeader = *p;
      values.push_back(*p);
      
      // 1st board header
      boardHeader = *(++p);
      values.push_back(*p);
      
      // trigger number
      triggerCounter = *(++p);
      values.push_back(*p);
      
      // timestamp
      BCOofTrigger = *(++p);
      values.push_back(*p);
   
      do {
         p++;
         if(*p != m_itrTail)values.push_back(*p);
         
      } while (*p != m_itrTail && p != p_max);
   }
   *p1 = (++p);
}

//------------------------------------------+-----------------------------------
//! Print data
void DEITREvent::printData () const
{
   printf ("DEITREvent DATA: \n");
   printf ("Channel ID (hex): %x\n",  channelID);
   printf ("Number of Event: %d\n",  eventNumber);
   printf ("Data size: %d\n",  evtSize);
   printf ("Detector Header: %x\n",  detectorHeader);
   printf ("Board Header: %x\n",  boardHeader);
   printf ("Trigger Counter (hw): %d\n",  triggerCounter);
   printf ("BCO counter: %d\n",  BCOofTrigger);
   printf ("BX counter: %d\n",  clockCounter);
   printf("\n");
}

//------------------------------------------+-----------------------------------
//! Check event/trigger number
bool DEITREvent::check() const
{
   if( evtSize!=0 ){
      if( eventNumber==triggerCounter)
         return true;
      else
         return false;
   }
   else
      return true;
}
	
