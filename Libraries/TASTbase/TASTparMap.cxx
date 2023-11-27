/*!
  \file TASTparMap.cxx
  \brief   Implementation of TASTparMap.
*/

#include <string.h>

#include <fstream>

#include "TSystem.h"
#include "TString.h"

#include "TASTparMap.hxx"

//##############################################################################

/*!
  \class TASTparMap
  \brief Map parameters for ST
*/

//! Class Imp
ClassImp(TASTparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTparMap::TASTparMap()
{
  vector<int> tdchaID;             tdchaID.clear();   
  vector<int> tdboaID;             tdboaID.clear();   

  fTDchaID =    tdchaID;   
  fTDboaID =    tdboaID;   
  fReso.clear();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TASTparMap::~TASTparMap()
{}


//------------------------------------------+-----------------------------------
//! Read mapping data from file \a name .
Bool_t TASTparMap::IsSTChannel(int iCha)
{
  if(std::find(fTDchaID.begin(),fTDchaID.end(),iCha) == fTDchaID.end()){
    return false;
  }
  return true;
}

//------------------------------------------+-----------------------------------
Bool_t TASTparMap::IsSTClock(int iCha)
{
  if(std::find(fTDclkID.begin(),fTDclkID.end(),iCha) == fTDclkID.end()){
    return false;
  }
  return true;
}

//------------------------------------------+-----------------------------------
Bool_t TASTparMap::IsSTBoard(int iBo)
{
  if(std::find(fTDboaID.begin(),fTDboaID.end(),iBo) == fTDboaID.end()){
    return false;
  }
  return true;
}

//------------------------------------------+-----------------------------------
Bool_t TASTparMap::FromFile(const TString& name)
{
  Clear();
  
  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);

   char bufConf[1024];
   int myArg1(0), myArg2(0);
   double myArg3(0.);
   
   ifstream incF;
   incF.open(name_exp.Data());
   
   if (!incF) {
      Error("FromFile()", "failed to open file '%s'", name_exp.Data());
      return kTRUE;
   }
   
   while (incF.getline(bufConf, 200, '\n')) {
      if(strchr(bufConf,'!')) {
         //      Info("FromFile()","Skip comment line:: %s",bufConf);
      } else if(strchr(bufConf,'T')) {
         //Cha id and board
         sscanf(bufConf, "T%d %d %lf",&myArg1,&myArg2, &myArg3);
         if((myArg1>-1 && myArg1<18) && (myArg2>-1 && myArg2<100)) {
            if(myArg1<16){
               fTDchaID.push_back(myArg1);
               fTDboaID.push_back(myArg2);
               fReso[make_pair(myArg1, myArg2)] = myArg3;
            }else{
               fTDclkID.push_back(myArg1);
               fTDboaID.push_back(myArg2);
            }
         } else {
            Error(""," Plane Map Error:: check config file!!");
            return kTRUE;
         }
      }
   }


   
   return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TASTparMap::Clear(Option_t*)
{
  TAGpara::Clear();
  fTDchaID.clear();
  return;
}

//------------------------------------------+-----------------------------------
double TASTparMap::GetChannelWeight(int ch_num, int bo_num){

  double sigma = fReso.find(make_pair(ch_num, bo_num))->second;
  
  return 1/sigma/sigma;
}

//------------------------------------------+-----------------------------------
pair<int,int> TASTparMap::GetClockChannel(int ch_num, int bo_num){

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

