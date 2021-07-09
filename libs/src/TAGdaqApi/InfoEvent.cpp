#include <stdlib.h>
#include <stdio.h>
#include "InfoEvent.hh"


InfoEvent::~InfoEvent(){
}


void InfoEvent::readData(unsigned int ** p1){

  unsigned int * p = *p1;
  channelID = *p;
  
  ++p;
  ++p;
  eventNumber = *p;
  ++p;
  ++p;
  ++p;
  time_sec = 0;
  //++p;
  time_nsec= 0;
  // ++p;
  // ++p;
  // ++p;
  runType= *p;
  // ++p;
  runNumber= *p;
  // ++p;
  // ++p;
  // eventNumber= *p;
  // ++p;
  // // eventNumber= *p;  this  is also an event number!!
  // ++p;
  // ++p;
  // ++p;
  ++p;
  *p1 = p;
}


void InfoEvent::printData() const {

  printf ("Event building time in seconds: %d\n",  time_sec);
  printf ("Event building time in nanoseconds: %d\n",  time_nsec);
  printf ("Run Type (hex): %x\n",  runType);
  printf ("Run Number: %d\n",  runNumber);
  printf ("Event number: %d\n",  eventNumber);
  printf ("\n");

}
