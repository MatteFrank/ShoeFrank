#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "RemoteEvent.hh"

RemoteEvent::~RemoteEvent(){
}

void RemoteEvent::readData(unsigned int **p1){

  uint32_t * p = *p1;
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
      //      uint32_t WORD= *p;
      //      std::cout<<"\n The previous word is:"<< std::hex << *(p-1) << std::endl;
      //      std::cout<<"\n The current word is:"<< std::hex << WORD << std::endl;
      values.push_back(*p);
      ++p;
    }
  } else {
     if (debugLevel)
        std::cout<<"Size of RemoteEvent " << std::hex << channelID << " is zero!"<<std::endl;
    ++p; //to check
  }
  *p1 = p;
}


void RemoteEvent::printData () const {
  printf ("Remote DATA: \n");
  printf ("Channel ID (hex): %x\n",  channelID);
  printf ("Time in seconds: %d\n",  time_sec);
  printf ("Time in microseconds: %d\n",  time_usec);
  printf ("Number of Event: %d\n",  eventNumber);
  printf ("Data size: %d\n",  evtSize);
  printf("\n");
}
