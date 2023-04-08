/*!
  \file TATWparMap.cxx
  \brief   Implementation of TATWparMap.
*/

#include <string.h>
#include <fstream>

#include "TSystem.h"
#include "TString.h"

#include "TAGrecoManager.hxx"
#include "TATWparMap.hxx"

//##############################################################################

/*!
  \class TATWparMap
  \brief Map parameters for TW
*/

ClassImp(TATWparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.
TATWparMap::TATWparMap()
{
  fcMap = new TATWchannelMap();

   // what that's good for ?
  vector<int> tdchaID;             tdchaID.clear();   
  vector<int> tdboaID;             tdboaID.clear();   
  vector<int> deID;                deID.clear();   

  fTDchaID =    tdchaID;   
  fTDboaID =    tdboaID;   
  fDetID    =   deID;   
  
}

//------------------------------------------+-----------------------------------
//! Destructor.
TATWparMap::~TATWparMap()
{
	if (!fcMap)
		delete fcMap;
}


//------------------------------------------+-----------------------------------
//! Read mapping data from file \a name .
Bool_t TATWparMap::FromFile(const TString& name) {

  Clear();
  
  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);
   Int_t verbose = 0;
   
  fcMap->LoadChannelMap(name_exp.Data());
 
  return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TATWparMap::Clear(Option_t*)
{
  TAGpara::Clear();
  fTDchaID.clear();
  fTDboaID.clear();
  fDetID.clear();
  return;
}

Bool_t TATWparMap::IsTWChannel(int iCha){

  if(std::find(fTDchaID.begin(),fTDchaID.end(),iCha) == fTDchaID.end()){
    return false;
  }
  return true;

    
}



Bool_t TATWparMap::IsTWBoard(int iBo){

  if(std::find(fTDboaID.begin(),fTDboaID.end(),iBo) == fTDboaID.end()){
    return false;
  }
  return true;

    
}


pair<int,int> TATWparMap::GetClockChannel(int ch_num, int bo_num){

  int bo_clk = bo_num;
  int ch_clk;
  
  if(ch_num <8){
    ch_clk = 16;
  }else if(ch_num>=8 && ch_num<16){
    ch_clk = 17;
  }else{
    ch_clk = -999;
    printf("error!!!! channel %d board %d not associated to a clk!!\n", ch_num, bo_num);
  }

  return make_pair(ch_clk,bo_clk);




}

