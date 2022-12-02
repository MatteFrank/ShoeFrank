#include <stdlib.h>
#include <stdio.h>
#include "ArduinoEvent.hh"
#include <iostream>

ArduinoEvent::~ArduinoEvent(){}


void ArduinoEvent::readData(unsigned int **p1){

  RemoteEvent::readData(p1);

  if( evtSize!=0){
    for (int i = 0; i < 16; i++){
      mux0[i] = values[i+2];
      mux1[i] = values[i+19];
      mux2[i] = values[i+36];
      mux3[i] = values[i+53];
      mux4[i] = values[i+70];
      mux5[i] = values[i+88];
      mux6[i] = values[i+105];
      mux7[i] = values[i+122];
      mux8[i] = values[i+139];
      mux9[i] = values[i+156];

    }
    
    
  }
  
}


void ArduinoEvent::printData () const{

  printf ("ArduinoEvent DATA: \n");
  printf ("Channel ID (hex): %x\n",  channelID);
  printf ("Time in seconds: %d\n",  time_sec);
  printf ("Time in microseconds: %d\n",  time_usec);
  printf ("Number of Event: %d\n",  eventNumber);
  printf ("Data size: %d\n",  evtSize);
  if(evtSize!=0){
    printf ("Number mux 0 board 0: %d\n", values[1]);
    printf ("Number mux 1 board 0: %d\n", values[18]);
    printf ("Number mux 2 board 0: %d\n", values[35]);
    printf ("Number mux 3 board 0: %d\n", values[52]);
    printf ("Number mux 4 board 0: %d\n", values[69]);
    printf ("Number mux 0 board 1: %d\n", values[87]);
    printf ("Number mux 1 board 1: %d\n", values[104]);
    printf ("Number mux 2 board 1: %d\n", values[121]);
    printf ("Number mux 3 board 1: %d\n", values[138]);
    printf ("Number mux 4 board 1: %d\n", values[155]);
  }
  printf("\n");
}


bool ArduinoEvent::check() const {
  if( evtSize!=0 ){
    bool isOK = true;
    isOK *= (values[1] == 0);
    isOK *= (values[18] == 1);
    isOK *= (values[35] == 2);
    isOK *= (values[52] == 3);
    isOK *= (values[69] == 4);
    isOK *= (values[87] == 0);
    isOK *= (values[104] == 1);
    isOK *= (values[121] == 2);
    isOK *= (values[138] == 3);
    isOK *= (values[155] == 4);
    return isOK;
  }
  else
    return true;
}  