/*!
  \file
  \version $Id: TATWparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TATWparMap.
*/

#include <string.h>

#include <fstream>

#include "TSystem.h"
#include "TString.h"

#include "GlobalPar.hxx"
#include "TATWparMap.hxx"

//##############################################################################

/*!
  \class TATWparMap TATWparMap.hxx "TATWparMap.hxx"
  \brief Map parameters for onion and daisy. **
*/

ClassImp(TATWparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TATWparMap::TATWparMap() {

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
  /*
  char bufConf[1024];
  int myArg1(0), myArg2(0), myArg3(0), myArg4(0), myArg5(0), myArg6(0); 
 
  ifstream incF;
  incF.open(name_exp.Data());
  if (!incF) {
    Error("FromFile()", "failed to open file '%s'", );
    return kTRUE;
  }

  while (incF.getline(bufConf, 200, '\n')) {
    if(strchr(bufConf,'!')) {
      //      Info("FromFile()","Skip comment line:: %s",bufConf);
    } else if(strchr(bufConf,'#')) {
      //Det id, Det channel, tdc, adc, adc board.
      sscanf(bufConf, "#%d %d %d %d %d %d",&myArg1,&myArg2,&myArg3,&myArg4,&myArg5,&myArg6);
      if((myArg1>-1 && myArg1<2) && (myArg2>-1 && myArg2<4) && (myArg3>-1 && myArg3<128) && (myArg4>-1 && myArg4<32) && (myArg5>-1 && myArg5<32)) {
	fDetID.push_back(myArg1);
	DetchaID.push_back(myArg2);
	fTDchaID.push_back(myArg3);
	fTDboaID.push_back(myArg4);
	ADchaID.push_back(myArg5);
	ADboaID.push_back(myArg6);
      } else {
	Error(""," Plane Map Error:: check config file!!");
	return kTRUE;
      }
    }
  }
  */
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

/*
bool TATWparMap::GetIDFromTDC(int channel, int board, int &detID, int &chaID) {

  bool found = kFALSE;
  for(int iw=0; iw<(int)fTDchaID.size(); iw++) {
    if(getTDID(iw) == channel && getfTDboaID(iw) == board)  {
      chaID = getDetChaID(iw);	
      detID = getfDetID(iw);
      found = kTRUE;
      break;
    }
  }

  return found;

}

*/


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

