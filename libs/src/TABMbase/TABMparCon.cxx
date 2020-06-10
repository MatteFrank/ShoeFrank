/*!
  \file
  \version $Id: TABMparCon.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TABMparCon.
*/

#include <string.h>

#include <fstream>
#include <math.h>
#include <vector>

using namespace std;
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"

#include "TABMparCon.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TABMparCon TABMparCon.hxx "TABMparCon.hxx"
  \brief conf parameters for the beam monitor. **
*/

ClassImp(TABMparCon);

//------------------------------------------+-----------------------------------
//! Default constructor.
TABMparCon::TABMparCon()
  :  TAGparTools(),
  fRDriftErr(0.015),
  fPlaneHitCut(3),
  fChi2Cut (5.),
  fMinHitCut(6),
  fMaxHitCut(20),
  fRejMaxCut(36),
  fNumIte(20),
  fParMove(0.0001),
  fLegMBin(40),
  fLegMRange(0.1),
  fLegRBin(75),
  fLegRRange(2.)
{
  fkDefaultParName = "./config/TABMdetector.cfg";
  vector<Float_t> myt0s(36,-10000);
  fT0Vec = myt0s;
  fpResoFunc=new TF1("BMResoFunc","0.032891770+0.0075746330*x-5.1692440e-05*x*x+1.8928600e-07*x*x*x-2.4652420e-10*x*x*x*x",0.,0.8);
  fpSTrel=new TF1("McStrel","0.0097110679*x-(6.6272359e-05)*x*x+(2.4267436e-07)*x*x*x-(3.1605667e-10)*x*x*x*x", 0., 320.);
  fAssHitErr=(gTAGroot->CurrentCampaignNumber()==1) ? 15. : 5.;
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMparCon::~TABMparCon(){
  delete fpResoFunc;
  delete fpSTrel;
}


//------------------------------------------+-----------------------------------
//! Read mapping data from file \a name .


Bool_t TABMparCon::FromFile(const TString& name) {
  Clear();
  TString nameExp;

  if (name.IsNull())
     nameExp = fkDefaultParName;
  else
     nameExp = name;

  if (!Open(nameExp)) return false;

  if(FootDebugLevel(1))
     cout<<"TABMparCon::FromFile:: read config file from "<<nameExp.Data()<<endl<<"Now I'll printout the BM FromFile read parameters"<<endl;

  //cuts
  ReadItem(fMinHitCut);
  if(FootDebugLevel(1))
     cout<<"fMinHitCut="<<fMinHitCut<<endl;
  ReadItem(fMaxHitCut);
  if(FootDebugLevel(1))
     cout<<"fMaxHitCut="<<fMaxHitCut<<endl;
  ReadItem(fRejMaxCut);
  if(FootDebugLevel(1))
     cout<<"fRejMaxCut="<<fRejMaxCut<<endl;
  ReadItem(fChi2Cut);
  if(FootDebugLevel(1))
     cout<<"fChi2Cut="<<fChi2Cut<<endl;

  //track reco
  ReadItem(fNumIte);
  if(FootDebugLevel(1))
     cout<<"fNumIte="<<fNumIte<<endl;
  ReadItem(fParMove);
  if(FootDebugLevel(1))
     cout<<"fParMove="<<fParMove<<endl;
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

  //other parameters
  ReadItem(fHitTimeCut);
  if(FootDebugLevel(1))
     cout<<"fHitTimeCut="<<fHitTimeCut<<endl;

  //MC parameters
  ReadItem(fSmearRDrift);
  if(FootDebugLevel(1))
     cout<<"fSmearRDrift="<<fSmearRDrift<<endl;
  ReadItem(fSmearHits);
  if(FootDebugLevel(1))
     cout<<"fSmearHits="<<fSmearHits<<endl;

return false;
}


Bool_t TABMparCon::FromFileOld(const TString& name) {

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
        fParMove=myArg1;
      }else {
	      Error(""," Plane Map Error:: check config file!! (F)");
	      return kTRUE;
        }
      }
  }//end of readline

  return kFALSE;
}




void TABMparCon::PrintT0s(TString output_filename, TString input_filename, Long64_t tot_num_ev){
  ofstream outfile;
  outfile.open(output_filename.Data(),ios::out);
  outfile<<"calculated_from: "<<input_filename.Data()<<"    number_of_events= "<<tot_num_ev<<"     fT0Choice= "<<fT0Choice<<endl;
  for(Int_t i=0;i<36;i++)
    outfile<<"cellid= "<<i<<"  T0_time= "<<fT0Vec[i]<<endl;
  outfile.close();
  return;
}


Bool_t TABMparCon::loadT0s(TString filename) {
  ifstream infile;
  if(gTAGroot->CurrentRunNumber()==2210 || gTAGroot->CurrentRunNumber()==2211 || gTAGroot->CurrentRunNumber()==2212)
    filename.Insert(filename.Last('.'),"_7April");
  else if(gTAGroot->CurrentRunNumber()==2239 || gTAGroot->CurrentRunNumber()==2240 || gTAGroot->CurrentRunNumber()==2241 || gTAGroot->CurrentRunNumber()==2242 || gTAGroot->CurrentRunNumber()==2251)
    filename.Insert(filename.Last('.'),"_8April");

  gSystem->ExpandPathName(filename);
  infile.open(filename,ios::in);
  if(infile.is_open()==kFALSE){
    cout<<"TABMparCon::ERROR: Cannot open T0 file: "<<filename<<endl;
    return kTRUE;
  }
  char tmp_char[200];
  vector<Float_t> fileT0(36,-10000.);
  Int_t tmp_int=-1, status=0;
  infile>>tmp_char>>tmp_char>>tmp_char>>tmp_char>>tmp_char>>tmp_char>>tmp_char>>fT0Choice;

  for(Int_t i=0;i<36;i++){
    if(!infile.eof() && tmp_int==i-1)
      infile>>tmp_char>>tmp_int>>tmp_char>>fileT0.at(i);
    else{
      cout<<"TABMparCon::loadT0s::Error in the T0 file "<<filename<<"!!!!!! check if it is write properly"<<endl;
      infile.close();
      return kTRUE;
      }
  }

  fT0Vec=fileT0;
  //check if the T0 are ok
  if(FootDebugLevel(1)) {
    for(Int_t i=0;i<36;i++) {
      cout<<"BM T0: "<<fT0Vec[i]<<endl;
      if(fT0Vec[i]==-10000)
      cout<<"WARNING IN TABMparCon::loadT0s: channel not considered in tdc map tdc_cha=i="<<i<<" T0 for this channel is set to -10000"<<endl;
      else if(fT0Vec[i]==-20000)
      cout<<"WARNING IN TABMparCon::loadT0s! channel with too few elements to evaluate T0: tdc_cha=i="<<i<<" T0 for this channel is set to -20000"<<endl;
    }
  }

  //charge the resolution function
  Int_t parnum;
  Double_t par;
  infile>>tmp_char>>tmp_char;
  delete fpResoFunc;
  fpResoFunc=new TF1("BMResoFunc",tmp_char,0.,0.8);
  infile>>tmp_char>>parnum;
  for(Int_t i=0;i<parnum;++i){
    infile>>par;
    fpResoFunc->SetParameter(i,par);
  }

  //charge the strel function
  infile>>tmp_char>>tmp_char;
  delete fpSTrel;
  fpSTrel=new TF1("BM_STrel",tmp_char,0.,400);
  infile>>tmp_char>>parnum;
  for(Int_t i=0;i<parnum;++i){
    infile>>par;
    fpSTrel->SetParameter(i,par);
  }
  fMaxSTrel=fpSTrel->GetMaximumX();

  if(FootDebugLevel(1)){
    cout<<"BM resolution formula="<<fpResoFunc->GetFormula()->GetExpFormula().Data()<<endl;
    cout<<"number of parameters="<<fpResoFunc->GetNpar()<<endl;
    cout<<"parameters:  (";
    for(Int_t i=0;i<fpResoFunc->GetNpar();++i)
      cout<<fpResoFunc->GetParameter(i)<<" , ";
    cout<<")"<<endl;
    cout<<"BM space time relation formula="<<fpSTrel->GetFormula()->GetExpFormula().Data()<<endl;
    cout<<"number of parameters="<<fpSTrel->GetNpar()<<endl;
    cout<<"parameters:  (";
    for(Int_t i=0;i<fpSTrel->GetNpar();++i)
      cout<<fpSTrel->GetParameter(i)<<" , ";
    cout<<")"<<endl;
    cout<<"STrel maximum bin at "<<fMaxSTrel<<endl;
  }

  infile.close();

  return kFALSE;
}



void TABMparCon::SetT0s(vector<Float_t> t0s) {

  if(t0s.size() == 36) {
    fT0Vec = t0s;
  } else {
    Error("Parameter()","Vectors size mismatch:: fix the t0 vector inmput size!!! %d ",(int) t0s.size());
  }

  return;
}


void TABMparCon::SetT0(Int_t cha, Float_t t0in){

if(cha<36 && cha>=0)
  fT0Vec[cha]=t0in;
else {
    Error("Parameter()","Channel out of Range!!! cha=%d",cha);
  }

  return;
}

void TABMparCon::CoutT0(){
  cout<<"Print BM T0 time:"<<endl;
  for(Int_t i=0;i<fT0Vec.size();i++)
    cout<<"cell_id="<<i<<"  T0="<<fT0Vec[i]<<endl;
  cout<<endl;
}


//------------------------------------------+-----------------------------------
//! Clear geometry info.

void TABMparCon::Clear(Option_t*)
{
  fRDriftErr=0.015;
  fPlaneHitCut=3;
  fChi2Cut = 5.;
  fMinHitCut=6;
  fMaxHitCut=20;
  fRejMaxCut=36;
  fNumIte=20;
  fParMove=0.0001;
  fLegMBin=40;
  fLegMRange=0.1;
  fLegRBin=75;
  fLegRRange=2.;
  // fAssHitErr=3.;
  fAssHitErr=(gTAGroot->CurrentCampaignNumber()==1) ? 10. : 5.;

  vector<Float_t> myt0s(36,-10000);
  fT0Vec = myt0s;

  return;
}

/*------------------------------------------+---------------------------------*/

void TABMparCon::ResetStrelFunc(){
  delete fpSTrel;
  fpSTrel=new TF1("McStrel","0.0097110679*x-(6.6272359e-05)*x*x+(2.4267436e-07)*x*x*x-(3.1605667e-10)*x*x*x*x", 0., 320.);
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TABMparCon::ToStream(ostream& os, Option_t*) const
{
  os << "TABMparCon " << GetName() << endl;
  return;
}


//~ Float_t TABMparCon::ResoEval(Float_t dist) {
  //~ Float_t sigma;
  //~ Int_t mybin(-1);
  //~ if(fpResoFunc) {
    //~ mybin = fpResoFunc->FindBin(dist);
    //~ sigma = fpResoFunc->GetBinContent(mybin)/10000;
  //~ }
  //~ return sigma>0 ? sigma:0.12;

//~ }
