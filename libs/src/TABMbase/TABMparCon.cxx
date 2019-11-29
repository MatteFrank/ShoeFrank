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
  rdrift_err(0.015),
  planehit_cut(3),
  m_isMC (false),
  rdrift_cut (2.),
  chi2red_cut (5.),
  fitter_index (0),
  minnhit_cut(6),
  maxnhit_cut(20),
  rejmax_cut(36),
  num_ite(0),
  par_move(0.0001),
  fakehits_mean(5.57),
  fakehits_sigmaleft(1.8),
  fakehits_sigmaright(2.3),
  mceff_mean(1),
  mceff_sigma(0.2),
  legmbin(40),
  legmrange(0.1),
  legrbin(75),
  legrrange(2.),
  asshitratio(3.)
{
  fkDefaultParName = "./config/TABMdetector.cfg";
  vector<Float_t> myt0s(36,-10000);
  v_t0s = myt0s;
  my_hreso=new TF1("poly","pol10",0.,0.95);
  my_hreso->SetParameters(832.013,-9483.35,42697.4,-57369.7,-53877.2,96272.6,145773.,-67990.3,-284751.,-5173.99,234058.);
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMparCon::~TABMparCon(){}


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
  ReadItem(minnhit_cut);
  if(FootDebugLevel(1))
     cout<<"minnhit_cut="<<minnhit_cut<<endl;
  ReadItem(maxnhit_cut);
  if(FootDebugLevel(1))
     cout<<"maxnhit_cut="<<maxnhit_cut<<endl;
  ReadItem(rejmax_cut);
  if(FootDebugLevel(1))
     cout<<"rejmax_cut="<<rejmax_cut<<endl;
  ReadItem(chi2red_cut);
  if(FootDebugLevel(1))
     cout<<"chi2red_cut="<<chi2red_cut<<endl;

  //track reco
  ReadItem(fitter_index);
  if(FootDebugLevel(1))
     cout<<"fitter_index="<<fitter_index<<endl;
  ReadItem(num_ite);
  if(FootDebugLevel(1))
     cout<<"num_ite="<<num_ite<<endl;
  ReadItem(par_move);
  if(FootDebugLevel(1))
     cout<<"par_move="<<par_move<<endl;
  ReadItem(strel_switch);
  if(FootDebugLevel(1))
     cout<<"strel_switch="<<strel_switch<<endl;

  //other parameters
  ReadItem(hit_timecut);
  if(FootDebugLevel(1))
     cout<<"hit_timecut="<<hit_timecut<<endl;

  //MC parameters
  ReadItem(smearrdrift);
  if(FootDebugLevel(1))
     cout<<"smearrdrift="<<smearrdrift<<endl;
  ReadItem(smearhits);
  if(FootDebugLevel(1))
     cout<<"smearhits="<<smearhits<<endl;

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
    }else if(strchr(bufConf,'R')) {
      sscanf(bufConf, "R %f",&myArg1);
      if(myArg1>0)
        rdrift_cut = myArg1;
      else {
	      Error(""," Plane Map Error:: check config file!! (R)");
	      return kTRUE;
        }
    }else if(strchr(bufConf,'H')) {
      sscanf(bufConf, "H %d %d %d", &myArgInt, &myArgIntmin, &myArgIntmax);
      if( myArgInt>0 && myArgInt<7 && myArgIntmin>=0 && myArgIntmax>0 && myArgIntmax>=myArgInt){
        planehit_cut= myArgInt;
        minnhit_cut = myArgIntmin;
        maxnhit_cut = myArgIntmax;
      }else {
	      Error(""," Plane Map Error:: check config file!! (H)");
	      return kTRUE;
        }
    }else if(strchr(bufConf,'Z')) {
      sscanf(bufConf, "Z  %f %f", &myArg1, &myArg2);
      if(myArg1>=0 && myArg2>=0){
        t0_sigma=myArg1;
        hit_timecut=myArg2;
      }else {
	      Error(""," Plane Map Error:: check config file!! (Z)");
	      return kTRUE;
      }
    }else if(strchr(bufConf,'M')) {
      sscanf(bufConf, "M %d %f %f %f %f %f %d",&myArgInt, &myArg1, &myArg2, &myArg3, &myArg4, &myArg5, &myArgIntmax);
      if((myArgInt==0 || myArgInt==1) && myArg1>=0 && myArg2>=0 && myArg3>=0 && myArg4>=0 && myArg5>=0 && myArgIntmax>=0 && myArgIntmax<6){
        smearhits = myArgInt;
        fakehits_mean=myArg1;
        fakehits_sigmaleft=myArg2;
        fakehits_sigmaright=myArg3;
        mceff_mean=myArg4;
        mceff_sigma=myArg5;
        smearrdrift=myArgIntmax;
      }else {
	      Error(""," Plane Map Error:: check config file!! (M)");
	      return kTRUE;
        }
    }else if(strchr(bufConf,'W')) {
      sscanf(bufConf, "W %d",&myArgInt);
      strel_switch = myArgInt;
    }else if(strchr(bufConf,'J')) {
      sscanf(bufConf, "J %d %f ",&myArgInt, &myArg1);
      if(myArgInt>=0 && myArg1>0){
        rejmax_cut = myArgInt;
        chi2red_cut=myArg1;
      }else {
	      Error(""," Plane Map Error:: check config file!! (J)");
	      return kTRUE;
        }
    }else if(strchr(bufConf,'F')) {
      sscanf(bufConf, "F %d %d %f",&myArgInt, &myArgIntmin, &myArg1);
      if(myArgInt>=0 && myArgIntmin>=0 && myArg1>=0.){
        fitter_index = myArgInt;
        num_ite=myArgIntmin;
        par_move=myArg1;
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
  outfile<<"calculated_from: "<<input_filename.Data()<<"    number_of_events= "<<tot_num_ev<<"     t0_switch= "<<t0_switch<<endl;
  for(Int_t i=0;i<36;i++)
    outfile<<"cellid= "<<i<<"  T0_time= "<<v_t0s[i]<<endl;
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
  infile>>tmp_char>>tmp_char>>tmp_char>>tmp_char>>tmp_char>>t0_switch>>tmp_char>>t0_choice;

  for(Int_t i=0;i<36;i++)
    if(!infile.eof() && tmp_int==i-1)
      infile>>tmp_char>>tmp_int>>tmp_char>>fileT0.at(i);
    else{
      cout<<"TABMparCon::loadT0s::Error in the T0 file "<<filename<<"!!!!!! check if it is write properly"<<endl;
      infile.close();
      return kTRUE;
      }
  infile.close();
  v_t0s=fileT0;

  //check if the T0 are ok
  if(FootDebugLevel(1)) {
     for(Int_t i=0;i<36;i++) {
        cout<<"BM T0: "<<v_t0s[i]<<endl;
        if(v_t0s[i]==-10000)
           cout<<"WARNING IN TABMparCon::loadT0s: channel not considered in tdc map tdc_cha=i="<<i<<" T0 for this channel is set to -10000"<<endl;
        else if(v_t0s[i]==-20000)
           cout<<"WARNING IN TABMparCon::loadT0s! channel with too few elements to evaluate T0: tdc_cha=i="<<i<<" T0 for this channel is set to -20000"<<endl;
     }
  }
  return kFALSE;
}



void TABMparCon::SetT0s(vector<Float_t> t0s) {

  if(t0s.size() == 36) {
    v_t0s = t0s;
  } else {
    Error("Parameter()","Vectors size mismatch:: fix the t0 vector inmput size!!! %d ",(int) t0s.size());
  }

  return;
}


void TABMparCon::SetT0(Int_t cha, Float_t t0in){

if(cha<36 && cha>=0)
  v_t0s[cha]=t0in;
else {
    Error("Parameter()","Channel out of Range!!! cha=%d",cha);
  }

  return;
}

void TABMparCon::CoutT0(){
  cout<<"Print BM T0 time:"<<endl;
  for(Int_t i=0;i<v_t0s.size();i++)
    cout<<"cell_id="<<i<<"  T0="<<v_t0s[i]<<endl;
  cout<<endl;
}


//------------------------------------------+-----------------------------------
//! Clear geometry info.

void TABMparCon::Clear(Option_t*)
{
  rdrift_err=0.015;
  planehit_cut=3;
  m_isMC = false;
  rdrift_cut = 0.11;
  chi2red_cut = 5.;
  fitter_index = 0;
  minnhit_cut=6;
  maxnhit_cut=20;
  rejmax_cut=36;
  num_ite=0;
  par_move=0.0001;
  legmbin=40;
  legmrange=0.1;
  legrbin=75;
  legrrange=2.;
  asshitratio=3.;

  //The following parameters for MC are set from the measurements with protons at Trento
  fakehits_mean=5.57;
  fakehits_sigmaleft=1.8;
  fakehits_sigmaright=2.3;
  mceff_mean=1;
  mceff_sigma=0.2;

  vector<Float_t> myt0s(36,-10000);
  v_t0s = myt0s;

  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TABMparCon::ToStream(ostream& os, Option_t*) const
{
  os << "TABMparCon " << GetName() << endl;

  return;
}


 /*-------------------------------------------------*/

Float_t TABMparCon::FirstSTrel(Float_t tdrift){

  if(tdrift<0 && t0_switch==2)
    return 0.03289 + 0.008*tdrift;

  Float_t rdrift;

  if(strel_switch==1){ //garfield strel
    rdrift=0.00915267+0.00634507*tdrift+2.02527e-05*tdrift*tdrift-7.60133e-07*tdrift*tdrift*tdrift+5.55868e-09*tdrift*tdrift*tdrift*tdrift-1.68944e-11*tdrift*tdrift*tdrift*tdrift*tdrift+1.87124e-14*tdrift*tdrift*tdrift*tdrift*tdrift*tdrift;
  }else if(strel_switch==2){//
    rdrift= 0.00972903*tdrift-8.21676e-05*tdrift*tdrift+3.66446e-07*tdrift*tdrift*tdrift-5.85882e-10*tdrift*tdrift*tdrift*tdrift;
  }else if(strel_switch==3){//
    rdrift= 0.0087776*tdrift-6.41845e-05*tdrift*tdrift+2.4946e-07*tdrift*tdrift*tdrift-3.48422e-10*tdrift*tdrift*tdrift*tdrift;
  }else if(strel_switch==4){//HIT 2014
    rdrift= 0.0092254*tdrift-7.1192e-5*tdrift*tdrift+3.01951e-7*tdrift*tdrift*tdrift-4.66646e-10*tdrift*tdrift*tdrift*tdrift;
  }else if (strel_switch==5){
    rdrift= (0.032891770+0.0075746330*tdrift-(5.1692440e-05)*tdrift*tdrift+(1.8928600e-07)*tdrift*tdrift*tdrift-(2.4652420e-10)*tdrift*tdrift*tdrift*tdrift)/0.78;
  }else if (strel_switch==6){//from strel calibration
    rdrift= -0.118715 + (0.0098028*tdrift) + (-0.000119206*tdrift*tdrift) + (8.75103e-07*tdrift*tdrift*tdrift) + (-3.16015e-09*tdrift*tdrift*tdrift*tdrift) + (4.37948e-12*tdrift*tdrift*tdrift*tdrift*tdrift);
  }else if (strel_switch==7){//from strel calibration
    tdrift+=43.7;
    rdrift= 0.0201024 + (0.00408601*tdrift) + (-4.42738e-05*tdrift*tdrift) + (4.9932e-07*tdrift*tdrift*tdrift) + (-2.45383e-09*tdrift*tdrift*tdrift*tdrift) + (4.08383e-12*tdrift*tdrift*tdrift*tdrift*tdrift);
  }else{
    //FIRST strel embedded in old Framework
    rdrift=(tdrift>320) ? 0.79 : 0.012891770+0.0075746330*tdrift-(5.1692440e-05)*tdrift*tdrift+(1.8928600e-07)*tdrift*tdrift*tdrift-(2.4652420e-10)*tdrift*tdrift*tdrift*tdrift;
  }

  return rdrift<=0 ? 0.0001 : rdrift;

}


Float_t TABMparCon::InverseStrel(Float_t rdrift){
  if(rdrift<0.959){
    TF1 f1("f1","1./0.78*(0.0075746330*x-(5.1692440e-05)*x*x+(1.8928600e-07)*x*x*x-(2.4652420e-10)*x*x*x*x)", 0., 320.);
    return f1.GetX(rdrift);
  }
  return 320;
}


Float_t TABMparCon::FirstSTrelMC(Float_t tdrift, Int_t mc_switch){

  if(tdrift<0 && t0_switch==2)
      return 0.03289 + 0.008*tdrift;

  if(mc_switch==1){ //garfield strel
    return 0.00915267+0.00634507*tdrift+2.02527e-05*tdrift*tdrift-7.60133e-07*tdrift*tdrift*tdrift+5.55868e-09*tdrift*tdrift*tdrift*tdrift-1.68944e-11*tdrift*tdrift*tdrift*tdrift*tdrift+1.87124e-14*tdrift*tdrift*tdrift*tdrift*tdrift*tdrift;
  }else if(mc_switch==2){//
    return 0.00972903*tdrift-8.21676e-05*tdrift*tdrift+3.66446e-07*tdrift*tdrift*tdrift-5.85882e-10*tdrift*tdrift*tdrift*tdrift;
  }else if(mc_switch==3){//
    return 0.0087776*tdrift-6.41845e-05*tdrift*tdrift+2.4946e-07*tdrift*tdrift*tdrift-3.48422e-10*tdrift*tdrift*tdrift*tdrift;
  }else if(mc_switch==4){//HIT 2014
    return 0.0092254*tdrift-7.1192e-5*tdrift*tdrift+3.01951e-7*tdrift*tdrift*tdrift-4.66646e-10*tdrift*tdrift*tdrift*tdrift;
  }else if (mc_switch==5)
    return (0.032891770+0.0075746330*tdrift-(5.1692440e-05)*tdrift*tdrift+(1.8928600e-07)*tdrift*tdrift*tdrift-(2.4652420e-10)*tdrift*tdrift*tdrift*tdrift)/0.78;

  //FIRST strel embedded in old Framework
  return 0.032891770+0.0075746330*tdrift-(5.1692440e-05)*tdrift*tdrift+(1.8928600e-07)*tdrift*tdrift*tdrift-(2.4652420e-10)*tdrift*tdrift*tdrift*tdrift;

}



//~ Float_t TABMparCon::ResoEval(Float_t dist) {
  //~ Float_t sigma;
  //~ Int_t mybin(-1);
  //~ if(my_hreso) {
    //~ mybin = my_hreso->FindBin(dist);
    //~ sigma = my_hreso->GetBinContent(mybin)/10000;
  //~ }
  //~ return sigma>0 ? sigma:0.12;

//~ }

//~ TF1* TABMparCon::GetCalibX() {

  //~ return f_mypol;
//~ }


//~ TF1* TABMparCon::GetCalibY() {

  //~ return f_mypol2;

//~ }

//~ void TABMparCon::ConfigureTrkCalib() {

  //~ /* OLD setup
   //~ 1  p0           1.69143e-02   1.78395e-05   1.25294e-08  -1.12766e-02
   //~ 2  p1           2.21824e+01   4.37184e-03   1.05774e-05  -5.07310e-06
   //~ 3  p2           8.09415e+03   6.11105e-01   3.85959e-03   5.18272e-08
   //~ 4  p3           1.39385e+06   8.36591e+01   6.64637e-01  -3.42786e-10
   //~ 5  p4           1.13567e+08   1.04695e+04   5.41532e+01   3.66862e-13
   //~ 6  p5           3.53414e+09   9.05361e+05   1.68521e+03   3.57968e-14

   //~ 1  p0          -1.15688e-02   2.02851e-05   1.00354e-08  -2.74132e+00
   //~ 2  p1          -1.96039e+00   4.12901e-03   1.36445e-06  -5.52166e-02
   //~ 3  p2          -1.16613e+02   5.45908e-01   1.77382e-04  -1.21665e-03
   //~ 4  p3           2.46797e+04   6.95556e+01   2.22149e-02  -9.68817e-06
   //~ 5  p4           2.01676e+06   8.23400e+03   2.67413e+00  -5.28596e-08
   //~ 6  p5          -2.05344e+06   7.32900e+05   2.08980e+01  -3.10577e-11
  //~ */

  //~ //X correction
  //~ if(m_isMC) {
    //~ f_mypol->SetParameters(0.,1.,0.,0.,0.,0.);
  //~ } else {
    //~ f_mypol->SetName("mypol");
    //~ f_mypol->SetParameters( 70.61e-05,
			    //~ 2.685e-01,
			    //~ 1.203e+01,
			   //~ -2.421e+04,
			    //~ 1.112e+04,
			    //~ 4.325e+08);
  //~ }

  //~ /*
   //~ 1  p0          -1.92457e-05   9.96755e-06   2.67768e-08   2.75109e-03
   //~ 2  p1           3.65559e-01   1.37338e-02   1.88348e-05  -4.51475e-06
   //~ 3  p2          -2.01191e+01   6.94988e+00   7.23064e-03   1.17480e-08
   //~ 4  p3          -4.47220e+04   4.31045e+03   2.29598e+00   4.64828e-11
   //~ 5  p4           2.79566e+05   5.85631e+05   2.16321e+01  -1.73594e-12
   //~ 6  p5           1.90396e+09   2.74307e+08   9.07879e+02   1.93312e-15

   //~ 1  p0          -1.01338e-04   9.87766e-06   2.56656e-08   5.76352e-02
   //~ 2  p1           5.49605e-01   1.32980e-02   1.94777e-05  -4.83048e-04
   //~ 3  p2          -2.40775e+01   7.04670e+00   8.12795e-03   3.90372e-07
   //~ 4  p3          -4.67887e+04   4.61691e+03   2.69340e+00  -3.47984e-09
   //~ 5  p4           1.98095e+06   6.33007e+05   2.08960e+01   3.79568e-12
   //~ 6  p5           1.44651e+09   3.07455e+08   6.89752e+02  -2.36315e-14
  //~ */

  //~ //Y correction
  //~ if(m_isMC) {
    //~ f_mypol2->SetParameters(0.,1.,0.,0.,0.,0.);
  //~ } else {
    //~ f_mypol2->SetName("mypol2");
    //~ f_mypol2->SetParameters(-21.95e-05,
			     //~ 2.953e-01,
			     //~ 17.41e+01,
			    //~ -2.699e+04,
			    //~ -1.061e+07,
			     //~ 1.667e+09);
  //~ }
  //~ return;

//~ }

//~ double TABMparCon::STrel_Delta1(double time) {
  //~ double p0=0.007471, p1=-0.005854, p2= 6.38379e-05, p3=-2.61452e-07, p4= 3.78368e-10;
  //~ double res;
  //~ res = p0 + p1*time + p2*time*time + p3*pow(time,3) + p4*pow(time,4);
  //~ return res;
//~ }

 //~ /*-------------------------------------------------*/

//~ double TABMparCon::STrel_Delta2(double time) {
  //~ double p0=-0.031722, p1=0.00055413, p2= -8.75471e-06, p3= 5.19529e-08, p4=-9.49872e-11 ;
  //~ double res;
  //~ res = p0 + p1*time + p2*time*time + p3*pow(time,3) + p4*pow(time,4);
  //~ return res;
//~ }

//~ /*----------------------------------------*/

//~ double TABMparCon::STrel_Delta3(double time) {
  //~ double p0= -0.00864077, p1= 0.000225237, p2= -3.39075e-06, p3=  2.02131e-08, p4=  -3.68566e-11;
  //~ double res;
  //~ res = p0 + p1*time + p2*time*time + p3*pow(time,3) + p4*pow(time,4);
  //~ return res;
//~ }

//~ //provv, da ottimizzare
//~ double TABMparCon::STrelCorr(double time, int ic, int ip, int iv) {//per ora serve solo il tempo perchè per ogni cella le strel sono le stesse...

  //~ double res = 0;
  //~ bool ana = kTRUE;

  //~ int t_ic, t_ip, t_iv;
  //~ t_ic = ic; t_ip = ip; t_iv = iv;

  //~ int howManyfiles(0);
  //~ if(!ana) {
    //~ howManyfiles = m_myVFunSpl.size();
    //~ for(int ih =0; ih<howManyfiles; ih++) {
      //~ Info("Action()","STrel:: %lf %d %lf %d ",time,ih,res,m_myVFunSpl.size());
      //~ if(m_myVFunSpl.at(ih)) {
        //~ res += (m_myVFunSpl.at(ih))->Eval(time);
        //~ Info("Action()","STrel:: %d %lf ",ih,(m_myVFunSpl.at(ih))->Eval(time));
      //~ }
    //~ }

    //~ //  res *= 0.3;
  //~ } else {
    //~ res -= STrel_Delta1(fabs(time));
    //~ res -= STrel_Delta2(fabs(time));
    //~ res -= STrel_Delta3(fabs(time));
  //~ }

  //~ return res;

//~ }
