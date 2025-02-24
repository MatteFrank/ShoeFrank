/*!
  \file
  \version $Id: TABMparConf.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TABMparConf.
*/

#include <string.h>

#include <fstream>
#include <math.h>
#include <vector>

using namespace std;
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"

#include "TABMparConf.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \file TABMparConf.hxx
  \brief Configuration file for the Beam Monitor
*/

ClassImp(TABMparConf);

//------------------------------------------+-----------------------------------
//! Default constructor.
TABMparConf::TABMparConf()
  :  TAGparTools(),
  fChi2Cut (5.),
  fPlaneHitCut(3),
  fMinHitCut(6),
  fMaxHitCut(20),
  fRejMaxCut(36),
  fHitTimeCut(330),
  fNumIte(20),
  fParMoveM(0.0001),
  fParMoveQ(0.0001),
  fParToll(0.0001),
  fLegMBin(40),
  fLegMRange(0.1),
  fLegRBin(75),
  fLegRRange(2.),
  fAssHitErr(3.),
  fInvertTrack(0),
  fSmearHits(0),
  fSmearRDrift(4),
  fRDriftErr(0.015),
  fEnThresh(0.0001)
{
  fDeadCha.Set(0);
  fkDefaultParName = "./config/TABMdetector.cfg";
}

//------------------------------------------+-----------------------------------
//! Default Destructor.
TABMparConf::~TABMparConf(){}


//------------------------------------------+-----------------------------------
//! Read config input file
//!
//! \param[in] name input file name
Bool_t TABMparConf::FromFile(const TString& name) {
  Clear();
  TString nameExp;

  if (name.IsNull()){
    cout<<"Warning in TABMparConf::FromFile:: the input filename is Null, I will load the default par file:"<<fkDefaultParName.Data()<<endl;
    nameExp = fkDefaultParName;
  }else
     nameExp = name;

  if (!Open(nameExp)) return false;

  if(FootDebugLevel(1))
     cout<<"TABMparConf::FromFile:: read config file from "<<nameExp.Data()<<endl<<"Now I'll printout the BM FromFile read parameters"<<endl;

  //cuts
  ReadItem(fMinHitCut);
  if(FootDebugLevel(1))
     cout<<"fMinHitCut="<<fMinHitCut<<endl;
  ReadItem(fMaxHitCut);
  if(FootDebugLevel(1))
     cout<<"fMaxHitCut="<<fMaxHitCut<<endl;
  ReadItem(fPlaneHitCut);
  if(FootDebugLevel(1))
     cout<<"fPlaneHitCut="<<fPlaneHitCut<<endl;
  ReadItem(fChi2Cut);
  if(FootDebugLevel(1))
     cout<<"fChi2Cut="<<fChi2Cut<<endl;

  //track reco
  ReadItem(fNumIte);
  if(FootDebugLevel(1))
     cout<<"fNumIte="<<fNumIte<<endl;
  ReadItem(fParMoveM);
  if(FootDebugLevel(1))
     cout<<"fParMoveM="<<fParMoveM<<endl;
  ReadItem(fParMoveQ);
  if(FootDebugLevel(1))
     cout<<"fParMoveQ="<<fParMoveQ<<endl;
  ReadItem(fParToll);
  if(FootDebugLevel(1))
     cout<<"fParToll="<<fParToll<<endl;
  ReadItem(fLegMBin);
  if(FootDebugLevel(1))
     cout<<"fLegMBin="<<fLegMBin<<endl;
  ReadItem(fLegMRange);
  if(FootDebugLevel(1))
     cout<<"fLegMRange="<<fLegMRange<<endl;
  ReadItem(fLegRBin);
  if(FootDebugLevel(1))
     cout<<"fLegRBin="<<fLegRBin<<endl;
  ReadItem(fLegRRange);
  if(FootDebugLevel(1))
     cout<<"fLegRRange="<<fLegRRange<<endl;
  ReadItem(fAssHitErr);
  if(FootDebugLevel(1))
     cout<<"fAssHitErr="<<fAssHitErr<<endl;
  ReadItem(fInvertTrack);
  if(FootDebugLevel(1))
     cout<<"fInvertTrack="<<fInvertTrack<<endl;

  //other parameters
  ReadItem(fHitTimeCut);
  if(FootDebugLevel(1))
     cout<<"fHitTimeCut="<<fHitTimeCut<<endl;

  ReadItem(fDeadCha);
  if(FootDebugLevel(1)){
    cout<<"fDeadCha= ";
    for(Int_t i=0;i<fDeadCha.GetSize();i++)
      cout<<" "<<fDeadCha.At(i);
    cout<<endl;
  }

  //MC parameters
  ReadItem(fSmearRDrift);
  if(FootDebugLevel(1))
     cout<<"fSmearRDrift="<<fSmearRDrift<<endl;
  ReadItem(fSmearHits);
  if(TAGrecoManager::GetPar()->IsElecNoiseMc())
    fSmearHits=2;
  if(FootDebugLevel(1))
     cout<<"fSmearHits="<<fSmearHits<<endl;
  ReadItem(fEnThresh);
  if(FootDebugLevel(1))
     cout<<"fEnThresh="<<fEnThresh<<endl;

  Close();

return false;
}

//! Read old config input file for the old Beam Monitor stand alone data acquisitions
//!
//! \param[in] name input file name
Bool_t TABMparConf::FromFileOld(const TString& name) {

  Clear();

  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);

  char bufConf[1024], tmp_char[200];
  Float_t myArg1(-1),myArg2(-1),myArg3(-1),myArg4(-1),myArg5(-1),myArg6(-1);
  Int_t myArgInt(-1), myArgIntmax(-1), myArgIntmin(-1);

  ifstream incF;
  incF.open(name_exp.Data());
  if (!incF) {
    Error("FromFile()", "failed to open file '%s'", name_exp.Data());
    return kTRUE;
  }

  while (incF.getline(bufConf, 200, '\n')) {
    if(strchr(bufConf,'!')) {
      //      Info("FromFile()","Skip comment line:: %s",bufConf);
    }else if(strchr(bufConf,'H')) {
      sscanf(bufConf, "H %d %d %d", &myArgInt, &myArgIntmin, &myArgIntmax);
      if( myArgInt>0 && myArgInt<7 && myArgIntmin>=0 && myArgIntmax>0 && myArgIntmax>=myArgInt){
        fPlaneHitCut= myArgInt;
        fMinHitCut = myArgIntmin;
        fMaxHitCut = myArgIntmax;
      }else {
	      Error(""," Plane Map Error:: check config file!! (H)");
	      return kTRUE;
        }
    }else if(strchr(bufConf,'Z')) {
      sscanf(bufConf, "Z  %f", &myArg2);
      if( myArg2>=0){
        fHitTimeCut=myArg2;
      }else {
	      Error(""," Plane Map Error:: check config file!! (Z)");
	      return kTRUE;
      }
    }else if(strchr(bufConf,'M')) {
      sscanf(bufConf, "M %d %f %f %f %f %f %d",&myArgInt, &myArg1, &myArg2, &myArg3, &myArg4, &myArg5, &myArgIntmax);
      if((myArgInt==0 || myArgInt==1) && myArg1>=0 && myArg2>=0 && myArg3>=0 && myArg4>=0 && myArg5>=0 && myArgIntmax>=0 && myArgIntmax<6){
        fSmearHits = myArgInt;
        fSmearRDrift=myArgIntmax;
      }else {
	      Error(""," Plane Map Error:: check config file!! (M)");
	      return kTRUE;
        }
    }else if(strchr(bufConf,'J')) {
      sscanf(bufConf, "J %d %f ",&myArgInt, &myArg1);
      if(myArgInt>=0 && myArg1>0){
        fRejMaxCut = myArgInt;
        fChi2Cut=myArg1;
      }else {
	      Error(""," Plane Map Error:: check config file!! (J)");
	      return kTRUE;
        }
    }else if(strchr(bufConf,'F')) {
      sscanf(bufConf, "F %d %f", &myArgIntmin, &myArg1);
      if(myArgIntmin>=0 && myArg1>=0.){
        fNumIte=myArgIntmin;
        fParMoveQ=myArg1;
        fParMoveM=myArg1;
      }else {
	      Error(""," Plane Map Error:: check config file!! (F)");
	      return kTRUE;
        }
      }
  }//end of readline

  return kFALSE;
}



//------------------------------------------+-----------------------------------
//! Clear config info
void TABMparConf::Clear(Option_t*)
{
  fChi2Cut = 5.;
  fPlaneHitCut=3;
  fMinHitCut=6;
  fMaxHitCut=20;
  fRejMaxCut=36;
  fHitTimeCut=330;
  fNumIte=20;
  fParMoveQ=0.0001;
  fParMoveM=0.0001;
  fLegMBin=40;
  fLegMRange=0.1;
  fLegRBin=75;
  fLegRRange=2.;
  fAssHitErr=5.;
  fSmearHits=0;
  fSmearRDrift=4;
  fRDriftErr=0.015;
  fEnThresh=0.0001;
  fDeadCha.Set(0);

  return;
}

//! Check if a channel is set as dead or not (at the moment used only for GSI2019 campaign)
//!
//! \param[in] cha numnber of channel index [0-35]
//! \return true if the channel is set as dead, false otherwise
Bool_t TABMparConf::CheckIsDeadCha(Int_t cha){
  for(Int_t i=0;i<fDeadCha.GetSize();i++)
    if(fDeadCha.At(i)==cha)
      return kTRUE;
  return kFALSE;
}


/*------------------------------------------+---------------------------------*/
//! ostream output
void TABMparConf::ToStream(ostream& os, Option_t*) const
{
  if(false){
  os<<"fMinHitCut="<<fMinHitCut<<endl;
  os<<"fMinHitCut="<<fMinHitCut<<endl;
  os<<"fMaxHitCut="<<fMaxHitCut<<endl;
  os<<"fPlaneHitCut="<<fPlaneHitCut<<endl;
  os<<"fChi2Cut="<<fChi2Cut<<endl;
}

  os<<"fNumIte="<<fNumIte<<endl;
  os<<"fParMoveM="<<fParMoveM<<endl;
  os<<"fParMoveQ="<<fParMoveQ<<endl;
  os<<"fParToll="<<fParToll<<endl;
  os<<"fLegMBin="<<fLegMBin<<endl;
  os<<"fLegMRange="<<fLegMRange<<endl;
  os<<"fLegRBin="<<fLegRBin<<endl;
  os<<"fLegRRange="<<fLegRRange<<endl;
  os<<"fAssHitErr="<<fAssHitErr<<endl;

if(false){
  os<<"fInvertTrack="<<fInvertTrack<<endl;
  os<<"fHitTimeCut="<<fHitTimeCut<<endl;
  os<<"fDeadCha= ";
  for(Int_t i=0;i<fDeadCha.GetSize();i++)
    os<<" "<<fDeadCha.At(i);
  os<<endl;
  os<<"fSmearRDrift="<<fSmearRDrift<<endl;
  os<<"fSmearHits="<<fSmearHits<<endl;
  os<<"fEnThresh="<<fEnThresh<<endl;  
}
  return;

}
