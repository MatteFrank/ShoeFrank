#include <stdlib.h>
#include <stdio.h>
#include "DECardEvent.hh"
#include <iostream>

const u_int DECardEvent::m_vtxHeader = 0xfafafafa;
const u_int DECardEvent::m_vtxTail   = 0xabcdabcd;


DECardEvent::DECardEvent()
: RemoteEvent(),
  detectorHeader(0),
  boardHeader(0),
  hardwareEventNumber(0),
  triggerCounter(0),
  BCOofTrigger(0),
  clockCounter(0)
{
}


DECardEvent::DECardEvent(const DECardEvent& right)
: RemoteEvent(right),
 detectorHeader(right.detectorHeader),
 boardHeader(right.boardHeader),
 hardwareEventNumber(right.hardwareEventNumber),
 triggerCounter(right.triggerCounter),
 BCOofTrigger(right.BCOofTrigger),
 clockCounter(right.clockCounter)
{
}


const DECardEvent& DECardEvent::operator=(const DECardEvent& right)
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

DECardEvent::~DECardEvent()
{
}

void DECardEvent::readData(unsigned int **p1)
{
   evtSize   = 0;
   u_int * p = *p1;
   u_int * p_max = *p1 + 2047;
   
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
      // skip 4 words ???
      p += 4;
      
      // check header
      if (*p != m_vtxHeader)
         printf("Error in the event reader %x instead of %x\n", *p, m_vtxHeader);
      
      detectorHeader = *p;
      evtSize++;
      values.push_back(*p);
      
      // 1st board header
      boardHeader = *(++p);
      evtSize++;
      values.push_back(*p);
      
      // event number
      eventNumber = *(++p);
      evtSize++;
      values.push_back(*p);
      
      // trigger number
      triggerCounter = *(++p);
      evtSize++;
      values.push_back(*p);
      
      // timestamp
      BCOofTrigger = *(++p);
      evtSize++;
      values.push_back(*p);
   
      do {
         p++;
         evtSize++;
         if(*p != m_vtxTail)values.push_back(*p);
         
      } while (*p != m_vtxTail && p != p_max);
   }
   *p1 = (++p);
   //   printf ("Evento : %d, p1 = %x \n",  eventNumber,*p1);

}

void DECardEvent::printData () const
{
   printf ("DECardEvent DATA: \n");
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


bool DECardEvent::check() const
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
	
