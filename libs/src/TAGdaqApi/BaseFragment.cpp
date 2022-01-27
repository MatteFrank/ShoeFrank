
/*!
 \file BaseFragment.cpp
 \brief  Implementation of BaseFragment
 */

#include "DAQMarkers.hh"
#include "BaseFragment.hh"
#include "InfoEvent.hh"
#include "TrgEvent.hh"
#include "fADCEvent.hh"
#include "TDCEvent.hh"
#include "EmptyEvent.hh"
#include "DECardEvent.hh"
#include "DEMSDEvent.hh"
#include "WDEvent.hh"
#include <stdio.h>

/*!
 \class BaseFragment
 \brief Base class for all fragments..
 */

std::string FRAGnames[] =
  {"Event Header data",
   "Trigger fragment ",
   "FADC    fragment ",
   "TDC     fragment ",
   "Empty   fragment ",
   "VTX     fragment ",
   "WD      fragment ",
   "MSD     fragment "};  ///< fragment name

unsigned int FRAGkeys[] =
  {EventHeaderID, dataV2495, dataV1720, dataV1190,
   dataEmpty, dataVTX, dataWD, dataMSD}; ///< fragment keys


std::map<unsigned int, std::string> BaseFragment::fragnames;

//------------------------------------------+-----------------------------------
//! Destructor.
BaseFragment::~BaseFragment(){}

//------------------------------------------+-----------------------------------
//! read data
//!
//! \param[in] p1 daq file pointer
void BaseFragment::readData(unsigned int **p1){
  unsigned int *p = *p1;
  channelID= *p;
  ++p;
  *p1 = p;
}

//------------------------------------------+-----------------------------------
//! Print data
void BaseFragment::printData() const {
  printf ("Generic Base Fragment: \n");
  printf ("Channel ID (hex): %x\n",  channelID);
}

//------------------------------------------+-----------------------------------
//! create a concrete class based on channelID pointed ; it reads it; returns NULL if error
//!
//! \param[in] p daq file pointer
BaseFragment* BaseFragment::create(unsigned int **p){

  BaseFragment* p_bf = NULL;
  u_int chID = (*(*p)) & 0xffffff00;
  
  if( chID==EventHeaderID ){ // get event info
    p_bf = new InfoEvent;
  } else if( chID==dataV2495 ){ // get trigger info
    p_bf = new TrgEvent;
  } else if( chID==dataV1720 ){ // get Flash ADC info
    p_bf = new fADCEvent;
  }  else if( chID==dataV1190 ){ //get TDC info
    p_bf = new TDCEvent;
  } else if( chID==dataEmpty){ // get empty info
    p_bf = new EmptyEvent;
  } else if( chID==dataVTX ){ // get DECardEvent info
    p_bf = new DECardEvent;
  } else if( chID==dataWD ){ // get WD info
    p_bf = new WDEvent;
  } else if( chID==dataMSD ){ // get MSD info
    p_bf = new DEMSDEvent;
  }
  u_int chID2= (*(*p));
  unsigned int *p2=*p;
  //  printf("\n Processing for %x  \n",chID2);
  if( p_bf!=NULL ) p_bf->readData(p);
  //  printf(" Read %d, next word: %x\n",((*p)-(p2)), *p2);
  return p_bf;
}

//------------------------------------------+-----------------------------------
//! Get fragment name from key
//!
//! \param[in] key a given key
std::string BaseFragment::fragmentName(unsigned int key){
  if( fragnames.size()==0 ){
    for(unsigned int i=0; i<FRAGTYPES; i++){
      fragnames[FRAGkeys[i]] = FRAGnames[i];
    }
    fragnames[0] = "Unknown";
  }
  std::map<unsigned int, std::string>::iterator iter =
    fragnames.find( (key&0xffffff00) );
  if( iter==fragnames.end() ){
    return fragnames[0];
  } else {
    return iter->second;
  }
}
