#include <stdlib.h>
#include <stdio.h>
#include "TrgEvent.hh"

TrgEvent::~TrgEvent(){	
}


void TrgEvent::readData( unsigned int **p1 ){

  unsigned int * p = *p1;

  channelID= *p;
  ++p;
  time_sec= *p;
  ++p;
  time_usec= *p;
  ++p;
  eventNumber= *p;
  ++p;
  liveTime= *p;
  ++p;
  clockCounter= *p;
  ++p;
  timeSinceLastTrigger= *p;
  ++p;
  triggerCounter= *p;
  ++p;
  BCOofTrigger= *p;
  ++p;
  spillNrAndTrgFineDelay= *p;
  ++p;
  PMTsAndBusy= *p;
  ++p;
  ++p;
  *p1 = p;
}


void TrgEvent::printData() const {

  printf ("Trigger DATA: \n");
  printf ("Channel ID (hex): %x\n",  channelID);
  printf ("Time in Seconds: %d\n",  time_sec);
  printf ("Time in MicroSeconds: %d\n",  time_usec);
  printf ("Number of Event: %d\n",  eventNumber);
  printf ("Live Time (clock periods): %d\n",  liveTime);
  printf ("Clock Counter: %d\n",  clockCounter);
  printf ("Time since last Trigger: %d\n",  timeSinceLastTrigger);
  printf ("Trigger Counter (hw): %d\n",  triggerCounter);
  printf ("BCO of Trigger: %d\n",  BCOofTrigger);
  printf ("Trigger Delay: %d\n",  spillNrAndTrgFineDelay);
  printf ("PMTs & Busy: %d\n",  PMTsAndBusy);
  printf ("\n");
}
