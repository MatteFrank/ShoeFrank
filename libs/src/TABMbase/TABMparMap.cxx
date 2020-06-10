/*!
  \file
  \version $Id: TABMparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TABMparMap.
*/

#include <string.h>

#include <fstream>

#include "TSystem.h"
#include "TString.h"

#include "TABMparMap.hxx"

//##############################################################################

/*!
  \class TABMparMap TABMparMap.hxx "TABMparMap.hxx"
  \brief Map and Geometry parameters for Tof wall. **
*/

ClassImp(TABMparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMparMap::TABMparMap() {
  fTdcMaxCha=64;
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMparMap::~TABMparMap()
{}


//------------------------------------------+-----------------------------------
//! Read mapping data from file \a name .

Bool_t TABMparMap::FromFile(const TString& name, TABMparGeo *bmgeo) {

  Clear();

  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);

  char bufConf[1024];
  Int_t myArg1(-100), myArg2(-100), myArg3(-100), myArg4(-100), myArg5(-100);

  ifstream incF;
  incF.open(name_exp.Data());
  if (!incF) {
    Error("ERROR in TABMparMap::FromFile()", "failed to open file '%s'", name_exp.Data());
    return kTRUE;
  }

  while (incF.getline(bufConf, 200, '\n')) {
    if(strchr(bufConf,'!')) {
      //      Info("FromFile()","Skip comment line:: %s",bufConf);
    }else if(strchr(bufConf,'#')) {
      sscanf(bufConf, "#%d %d %d %d %d",&myArg1,&myArg2,&myArg3,&myArg4, &myArg5);
       if(myArg1>-1 && myArg1<fTdcMaxCha && myArg2<36 && myArg2>-1 && (myArg3>=0 || myArg3<=5) && (myArg4==1 || myArg4==0) && myArg5>-1 && myArg5<3) {
        if(fTdc2CellVec[myArg1]<0){
          if(bmgeo->GetBMNcell(myArg3,myArg4,myArg5)!=myArg2)
            Error("TABMparMap::FromFile()","channel wrong channel id and identifiers!!!!");
          else{
            fTdc2CellVec[myArg1]=myArg2;
            fCell2TdcVec[myArg2]=myArg1;
          }
        }
        else{
          Error("TABMparMap::FromFile()","channel already set; check config file!!");
          return kTRUE;
        }
      } else {
        Error(""," TABMparMap Plane Map Error:: check config file!! (#)");
        cout<<"myArg1="<<myArg1<<"  myArg2="<<myArg2<<"  myArg3="<<myArg3<<"  myArg4="<<myArg4<<"  myArg5="<<myArg5<<"  fTdcMaxCha="<<fTdcMaxCha<<endl;
        return kTRUE;
      }
    } else if(strchr(bufConf,'T')) {
        sscanf(bufConf, "T %d",&myArg1);
      if(myArg1<0 || myArg1>fTdcMaxCha) {
        Error("FromFile()","TABMparMap::Reference Tr channel:: check config file!!(T)");
        return kTRUE;
      }else{
        fTrefCh=myArg1;
        fTdc2CellVec[fTrefCh]=-1000;
        fCell2TdcVec[36]=fTrefCh;
      }
    } else if(strchr(bufConf,'S')) {
        sscanf(bufConf, "S %d",&fSca830ch);
    } else if(strchr(bufConf,'A')) {
        sscanf(bufConf, "A %d",&fAdc792ch);
    } else if(strchr(bufConf,'M')) {
        sscanf(bufConf, "M %d",&myArg1);
        if(!(myArg1==64 || myArg1==128 || myArg1==256)) {
          Error("FromFile()","TABMparMap::fTdcMaxCha not = 64 || 128 || 256:: check config file!!(M)");
          return kTRUE;
        }else{
          fTdcMaxCha=myArg1;
          for(Int_t i=0;i<fTdcMaxCha;i++)
            fTdc2CellVec.push_back(-1);
          for(Int_t i=0;i<37;i++)
            fCell2TdcVec.push_back(-1);
          }
    }
  }

  fboardNum=513; //512 is fixed + 1=tdc (mandatory)
  if(fSca830ch>=0)
    fboardNum+= 256;
  if(fAdc792ch>=0)
    fboardNum+= 16;

  //check on the map
  Int_t tmp_int(0);
  for(Int_t i=0;i<fTdcMaxCha;i++)
    if(fTdc2CellVec[i]>=0)
      tmp_int++;
  if(tmp_int!=36){
    cout<<"ERROR in TABMparMap:FromFile():error in the map! you haven't set 36 channel for the BM! number of channel set="<<tmp_int<<endl;
    return kTRUE;
  }

  if(FootDebugLevel(1)){
    cout<<"TABMparMap::print fTdc2CellVec"<<endl;
    for(Int_t i=0;i<fTdc2CellVec.size();i++)
      cout<<"i="<<i<<"  fTdc2CellVec[i]="<<fTdc2CellVec[i]<<endl;
    cout<<endl<<"print fCell2TdcVec"<<endl;
    for(Int_t i=0;i<fCell2TdcVec.size();i++)
      cout<<"i="<<i<<"   fCell2TdcVec[i]="<<fCell2TdcVec[i]<<endl;
    cout<<endl;
  }

  return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TABMparMap::Clear(Option_t*){
  TAGpara::Clear();
  fTdc2CellVec.clear();
  fCell2TdcVec.clear();
  return;
}
