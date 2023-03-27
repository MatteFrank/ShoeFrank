/*!
 \file TDCEvent.cpp
 \brief  Implementation of TDCEvent
 */

#include <stdlib.h>
#include <stdio.h>
#include "TDCEvent.hh"

/*!
 \class TDCEvent
 \brief TDC event
 */

//------------------------------------------+-----------------------------------
//! Destructor.
TDCEvent::~TDCEvent(){
}

//------------------------------------------+-----------------------------------
//! read data
//!
//! \param[in] p1 daq file pointer
void TDCEvent::readData( unsigned int **p1 ){

  unsigned int * p = *p1;
  unsigned int buffer1;

  channelID = *p;
  ++p;
  tdc_sec = *p;
  ++p;
  tdc_usec = *p;
  ++p;
  eventNumber = *p;
  ++p;
  globalHeader = *p;
  buffer1 = ( p[1] >>27 ) & 0x1f;
  while ( buffer1 != 0x10 ){
    if ( buffer1 == 0x0 ){
      ++p;
      ( measurement ).push_back( *p ); //& 0x7FFFF to select real measurement
    }
    else if ( buffer1 == 0x1 ){
      ++p;
      ( tdcHeader ).push_back( *p );
    }
    else if ( buffer1 == 0x3 ){
      ++p;
      ( tdcTrailer ).push_back( *p );
    }
    else if ( buffer1 == 0x11 ){
      ++p;
      extendedTimeTag = *p;
    }
    else {
      printf("EventReader::getTDCData --> unexpected sequence in lookAtTDC evaluation \n");
      exit(0);
    }
    buffer1 = ( p[1] >>27 ) & 0x1f;
  }
  ++p;
  globalTrailer = *p;
  ++p;
  ++p;
  *p1 = p;
}

//------------------------------------------+-----------------------------------
//! Print data
void TDCEvent::printData() const {

  printf ("TDC DATA: \n");
  printf ("Channel ID (hex): %x\n",  channelID);
  printf ("Time in seconds: %d\n",  tdc_sec);
  printf ("Time in microseconds: %d\n",  tdc_usec);
  printf ("Number of Event: %d\n",  eventNumber);
  printf ("Global Header: %x\n",  globalHeader);
  for ( unsigned int i = 0; i < tdcHeader.size(); ++i ){
    printf ("TDC Header %i: %x\n", i, tdcHeader.at( i ) );
  }
  for ( unsigned int i = 0; i < measurement.size(); ++i ){
    printf ("Measurement number %i: %x\n", i, measurement.at( i ) );
  }
  for ( unsigned int i = 0; i < tdcTrailer.size(); ++i ){
    printf ("TDC Trailer %i: %x\n", i, tdcTrailer.at( i ) );
  }
  printf ("TDC Extended Time Tag (800 ns unit): %x\n", extendedTimeTag);
  printf ("Global Trailer: %x\n",  globalTrailer);
  printf("\n");
}
