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
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMparMap::~TABMparMap()
{}


//------------------------------------------+-----------------------------------
//! Read mapping data from file \a name .

Bool_t TABMparMap::FromFile(const TString& name, TABMparGeo *bmgeo) {

  Clear();
  TString nameExp;
  TArrayI readArrayI;

  if (name.IsNull()){
    Error("TABMparMap::FromFile()","Input file not set!!!!");
    return kTRUE;
  }else
     nameExp = name;

  if (!Open(nameExp)) return kTRUE;

  Info("FromFile()", "Open file %s for mapping\n", nameExp.Data());

  if(FootDebugLevel(1))
     cout<<"TABMparMap::FromFile:: read config file from "<<nameExp.Data()<<endl<<"Now I'll printout the BM FromFile read parameters"<<endl;

  //Read TDC channels
  fTdcCha.Set(0);
  ReadItem(fTdcCha);
  for(int k=0;k<fTdcCha.GetSize();k++)
    if(fTdcCha[k]==999)
      fTdcCha[k]=-1;

  //Read Scaler channels
  fScaCha.Set(0);
  ReadItem(fScaCha);
  for(int k=0;k<fScaCha.GetSize();k++)
    if(fScaCha[k]==999)
      fScaCha[k]=-1;

  //Read Adc channels
  fAdcCha.Set(0);
  ReadItem(fAdcCha);
  for(int k=0;k<fAdcCha.GetSize();k++)
    if(fAdcCha[k]==999)
      fAdcCha[k]=-1;

  //Let's do some checks on the values
  if(fTdcCha.GetSize()<2 || fScaCha.GetSize()<1 || fAdcCha.GetSize()<1){
    Error("TABMparMap::FromFile()","tdc, scaler and adc values not initialized");
    cout<<"fTdcCha.GetSize()="<<fTdcCha.GetSize()<<"  fScaCha.GetSize()="<<"  fAdcCha.GetSize()="<<fAdcCha.GetSize()<<endl;
    return kTRUE;
  }
  if(fTdcCha[0]<=0){
    Error("TABMparMap::FromFile()","Tdc max number of channel not set");
    return kTRUE;
  }

  //set the start values
  for(Int_t i=0;i<fTdcCha[0];i++)
    fTdc2CellVec[i]=-1;
  if(fTdcCha[1]>=0)
    fTdc2CellVec[fTdcCha[1]]=-1000;
  if(fTdcCha[2]>=0)
    fTdc2CellVec[fTdcCha[2]]=-1001;
  for(Int_t i=0;i<36;i++)
    fCell2TdcVec[i]=-1;


  fboardNum=513; //512 is fixed + 1=tdc (mandatory)
  if(fScaCha[0]>=0)
  fboardNum+= 256;
  if(fScaCha[0]>=0)
  fboardNum+= 16;

  //Read tdc channel mapping
  for(Int_t i=0;i<36;++i){
    readArrayI.Set(0);
    ReadItem(readArrayI);
  if(readArrayI[0]<fTdcCha[0] && readArrayI[0]>=0 && readArrayI[1]<36 && readArrayI[1]>=0 && fTdc2CellVec[readArrayI[0]]==-1 && fCell2TdcVec[readArrayI[1]]==-1) {
      fTdc2CellVec[readArrayI[0]]=readArrayI[1];
      fCell2TdcVec[readArrayI[1]]=readArrayI[0];
    }else{
      Error("TABMparMap::FromFile()","wrong channel id and identifiers!!!! check the map file");
      cout<<"read value 0="<<readArrayI[0]<<"  read value 1="<<readArrayI[1]<<endl;
      cout<<"fTdc2CellVec="<<fTdc2CellVec[readArrayI[0]]<<"  fCell2TdcVec="<<fCell2TdcVec[readArrayI[1]]<<endl;
      return kTRUE;
    }
  }

  if(FootDebugLevel(1)){
    cout<<"TABMparMap::print fTdcCha"<<endl;
    for(Int_t i=0;i<fTdcCha.GetSize();i++)
      cout<<"index="<<i<<"  fTdcCha="<<fTdcCha[i]<<endl;

    cout<<"TABMparMap::print fScaCha"<<endl;
    for(Int_t i=0;i<fScaCha.GetSize();i++)
      cout<<"index="<<i<<"  fScaCha="<<fScaCha[i]<<endl;

    cout<<"TABMparMap::print fAdcCha"<<endl;
    for(Int_t i=0;i<fAdcCha.GetSize();i++)
      cout<<"index="<<i<<"  fAdcCha="<<fAdcCha[i]<<endl;

    cout<<"TABMparMap::print fTdc2CellVec"<<endl;
    for(auto it = fTdc2CellVec.cbegin(); it != fTdc2CellVec.cend(); ++it)
      cout<<"Tdc channel:"<<it->first<<"  cellid:"<< it->second<<endl;

    cout<<endl<<"TABMparMap::print fCell2TdcVec"<<endl;
    for(auto it = fCell2TdcVec.cbegin(); it != fCell2TdcVec.cend(); ++it)
      cout<<"cellid:"<<it->first<<"  tdc channel:"<< it->second<<endl;
    cout<<endl;
  }

  Close();

  return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TABMparMap::Clear(Option_t*){
  TAGpara::Clear();
  fTdc2CellVec.clear();
  fCell2TdcVec.clear();
  fTdcCha.Set(0);
  fScaCha.Set(0);
  fAdcCha.Set(0);
  return;
}
