/*!
  \file
  \version $Id: TABMparCal.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TABMparCal.
*/

#include <string.h>

#include <fstream>
#include <math.h>
#include <vector>

using namespace std;
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"

#include "TABMparCal.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TABMparCal TABMparCal.hxx "TABMparCal.hxx"
  \brief conf parameters for the beam monitor. **
*/

ClassImp(TABMparCal);

//------------------------------------------+-----------------------------------
//! Default constructor.
TABMparCal::TABMparCal()
  :  TAGparTools()
{
  fkDefaultParName = "./calib/TABM_T0_Calibration.cal";
  vector<Float_t> myt0s(36,-10000);
  fT0Vec = myt0s;
  fpResoFunc=new TF1("bmResoFunc","0.0245237+0.106748*x+0.229201*x*x-24.0304*x*x*x+183.529*x*x*x*x-619.259*x*x*x*x*x+1080.97*x*x*x*x*x*x-952.989*x*x*x*x*x*x*x+335.937*x*x*x*x*x*x*x*x",0.,0.8);
  fpSTrel=new TF1("McStrel","0.00773*x -5.1692440e-05*x*x + 1.8928600e-07*x*x*x -2.4652420e-10*x*x*x*x", 0., 350.);
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMparCal::~TABMparCal(){
  delete fpResoFunc;
  delete fpSTrel;
}


Bool_t TABMparCal::FromFile(const TString& inputname) {
  ifstream infile;
  TString filename;

  if (inputname.IsNull()){
    cout<<"Warning in TABMparCal::FromFile:: the input filename is Null, I will load the default par file:"<<fkDefaultParName.Data()<<endl;
    filename = fkDefaultParName;
  }else
     filename = inputname;

  Info("FromFile", "Open file: %s for T0 calibration\n", filename.Data());

  gSystem->ExpandPathName(filename);
  infile.open(filename,ios::in);
  if(infile.is_open()==kFALSE){
    cout<<"TABMparCal::ERROR: Cannot open T0 file: "<<filename<<endl;
    return kTRUE;
  }
  char tmp_char[200];
  vector<Float_t> fileT0(36,-10000.);
  Int_t tmp_int=-1;
  infile>>tmp_char>>tmp_char>>tmp_char>>tmp_char;

  for(Int_t i=0;i<36;i++){
    if(!infile.eof() && tmp_int==i-1)
      infile>>tmp_char>>tmp_int>>tmp_char>>fileT0.at(i);
    else{
      cout<<"TABMparCal::FromFile::Error in the T0 file "<<filename<<"!!!!!! check if it is write properly"<<endl;
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
        cout<<"WARNING IN TABMparCal::FromFile: channel not considered in tdc map tdc_cha=i="<<i<<" T0 for this channel is set to -10000"<<endl;
      else if(fT0Vec[i]==-20000)
        cout<<"WARNING IN TABMparCal::FromFile: channel with too few elements to evaluate T0: tdc_cha=i="<<i<<" T0 for this channel is set to -20000"<<endl;
    }
  }

  //charge the resolution function
  Int_t parnum;
  Double_t par;
  infile>>tmp_char>>tmp_char;
  delete fpResoFunc;
  fpResoFunc=new TF1("bmResoFunc",tmp_char,0.,0.8);
  infile>>tmp_char>>parnum;
  for(Int_t i=0;i<parnum;++i){
    infile>>par;
    fpResoFunc->SetParameter(i,par);
  }

  //charge the strel function
  infile>>tmp_char>>tmp_char;
  delete fpSTrel;
  fpSTrel=new TF1("bmParSTrel",tmp_char,0.,400);
  infile>>tmp_char>>parnum;
  for(Int_t i=0;i<parnum;++i){
    infile>>par;
    fpSTrel->SetParameter(i,par);
  }

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
    cout<<"STrel maximum bin at "<<fpSTrel->GetMaximumX()<<endl;
  }

  infile.close();

  return kFALSE;
}

//********************************** T0 ********************************

void TABMparCal::PrintT0s(TString output_filename, TString input_filename, Long64_t tot_num_ev){
  ofstream outfile;
  outfile.open(output_filename.Data(),ios::out);
  outfile<<"calculated_from: "<<input_filename.Data()<<"    number_of_events= "<<tot_num_ev<<endl<<endl;
  for(Int_t i=0;i<36;i++)
    outfile<<"cellid= "<<i<<"  T0_time= "<<fT0Vec[i]<<endl;
  outfile<<endl;
  outfile.close();
  return;
}


void TABMparCal::SetT0s(vector<Float_t> t0s) {

  if(t0s.size() == 36) {
    fT0Vec = t0s;
  } else {
    Error("Parameter()","Vectors size mismatch:: fix the t0 vector inmput size!!! %d ",(int) t0s.size());
  }

  return;
}


void TABMparCal::SetT0(Int_t cha, Float_t t0in){

if(cha<36 && cha>=0)
  fT0Vec[cha]=t0in;
else {
    Error("Parameter()","Channel out of Range!!! cha=%d",cha);
  }

  return;
}

void TABMparCal::CoutT0(){
  cout<<"Print BM T0 time:"<<endl;
  for(Int_t i=0;i<fT0Vec.size();i++)
    cout<<"cell_id="<<i<<"  T0="<<fT0Vec[i]<<endl;
  cout<<endl;
}

//********************************** ADC ********************************
Bool_t TABMparCal::LoadAdc(TString inputname, TABMparMap *p_bmmap){
  ifstream infile;
  TString filename;
  ResetAdc(p_bmmap->GetAdcMaxCh());

  if (inputname.IsNull()){
    cout<<"ERROR in TABMparCal::LoadAdc:: the input filename is Null, please add a file name!"<<endl;
    return kTRUE;
  }else
    filename = inputname;

  Info("LoadAdc", "Loading BM Adc pedestals from file: %s\n", filename.Data());

  gSystem->ExpandPathName(filename);
  infile.open(filename,ios::in);
  if(infile.is_open()==kFALSE){
    cout<<"TABMparCal::ERROR: Cannot open Adc Pedestals file: "<<filename<<endl;
    return kTRUE;
  }

  char tmp_char[200];
  Int_t channel=-1;
  Float_t mean, stddev;
  infile>>tmp_char>>tmp_char>>tmp_char>>tmp_char;

  for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++){
    if(!infile.eof() && channel==i-1){
      infile>>tmp_char>>channel>>tmp_char>>mean>>tmp_char>>stddev;
      fAdcPedVec.at(channel)=make_pair(mean,stddev);
    }else{
      cout<<"TABMparCal::LoadAdc::Error in the Adc file "<<filename<<"!!!!!! check if it is write properly"<<endl;
      infile.close();
      return kTRUE;
      }
  }

  //check if the Adc are ok
  if(FootDebugLevel(1))
    CoutAdc();

  return kFALSE;
}

void TABMparCal::PrintAdc(TString output_filename, TString input_filename, Long64_t tot_num_ev){
  ofstream outfile;
  outfile.open(output_filename.Data(),ios::out);
  outfile<<"calculated_from: "<<input_filename.Data()<<"    number_of_events= "<<tot_num_ev<<endl<<endl;
  for(Int_t i=0;i<fAdcPedVec.size();i++)
    outfile<<"adc_cha= "<<i<<"  pedestal= "<<fAdcPedVec[i].first<<"  devstd= "<<fAdcPedVec[i].second<<endl;
  outfile<<endl;
  outfile.close();
  return;
}

void TABMparCal::CoutAdc(){
  cout<<"Print BM ADC pedestals:"<<endl;
  for(Int_t i=0;i<fAdcPedVec.size();i++)
    cout<<"adc_channel= "<<i<<"  pedestal= "<<fAdcPedVec[i].first<<"   devstd="<<fAdcPedVec[i].second<<endl;
  cout<<endl;
}

void TABMparCal::ResetAdc(Int_t size){
  fAdcPedVec.clear();
  if(size<=0)
    return;
  fAdcPedVec.resize(size,make_pair(0,0));
  return;
};

//********************************** STREL AND RESOLUTION ********************************

void TABMparCal::PrintResoStrel(TString output_filename){
  ofstream outfile;
  outfile.open(output_filename.Data(),ios::app);
  outfile<<"Resolution_function:  "<<fpResoFunc->GetFormula()->GetExpFormula().Data()<<endl;
  outfile<<"Resolution_number_of_parameters:  "<<fpResoFunc->GetNpar()<<endl;
  for(Int_t i=0;i<fpResoFunc->GetNpar();i++)
    outfile<<fpResoFunc->GetParameter(i)<<"   ";
  outfile<<endl<<endl;
  outfile<<"STrel_function:  "<<fpSTrel->GetFormula()->GetExpFormula().Data()<<endl;
  outfile<<"STrel_number_of_parameters:  "<<fpSTrel->GetNpar()<<endl;
  for(Int_t i=0;i<fpSTrel->GetNpar();i++)
    outfile<<fpSTrel->GetParameter(i)<<"   ";
  outfile<<endl;
  outfile.close();
  return;
}


void TABMparCal::ResetStrelFunc(){
  delete fpSTrel;
  fpSTrel=new TF1("McStrel","0.0096625*x -6.461555e-05*x*x + 2.366075e-07*x*x*x -3.0815525e-10*x*x*x*x", 0., 330.);
}

void TABMparCal::SetResoFunc(TF1* inreso){
  delete fpResoFunc;
  fpResoFunc=(TF1*)(inreso->Clone("bmResoFunc"));
}

void TABMparCal::SetSTrelFunc(TF1* instrel){
  delete fpSTrel;
  fpSTrel=(TF1*)(instrel->Clone("bmParSTrel"));
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TABMparCal::ToStream(ostream& os, Option_t*) const
{
  os << "TABMparCal " << GetName() << endl;
  return;
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.

void TABMparCal::Clear(Option_t*)
{

  delete fpResoFunc;
  delete fpSTrel;

  vector<Float_t> myt0s(36,-10000);
  fT0Vec = myt0s;
  fpResoFunc=new TF1("bmResoFunc","0.0245237+0.106748*x+0.229201*x*x-24.0304*x*x*x+183.529*x*x*x*x-619.259*x*x*x*x*x+1080.97*x*x*x*x*x*x-952.989*x*x*x*x*x*x*x+335.937*x*x*x*x*x*x*x*x",0.,0.8);
  fpSTrel=new TF1("McStrel","0.00773*x -5.1692440e-05*x*x + 1.8928600e-07*x*x*x -2.4652420e-10*x*x*x*x", 0., 350.);
  return;
}
