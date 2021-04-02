/*!
  \file
  \version $Id: TACAactNtuRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TACAactNtuRaw.
*/

#include "TACAparMap.hxx"
#include "TACAparCal.hxx"
#include "TACAactNtuRaw.hxx"

/*!
  \class TACAactNtuRaw TACAactNtuRaw.hxx "TACAactNtuRaw.hxx"
  \brief Get Beam Monitor raw data from WD. **
*/

ClassImp(TACAactNtuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAactNtuRaw::TACAactNtuRaw(const char* name,
                             TAGdataDsc* p_datraw,
                             TAGdataDsc* p_nturaw,
                             TAGparaDsc* p_parmap,
                             TAGparaDsc* p_parcal)
  : TAGaction(name, "TACAactNtuRaw - Unpack CA raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw),
    fpParMap(p_parmap),
    fpParCal(p_parcal),
    fTcorr1Par1(-0.0011),
    fTcorr1Par0(0.167),
    fTcorr2Par1(4.94583e-03),
    fTcorr2Par0(9000.),
    T1(270.),
    T2(380.)
{
  AddDataIn(p_datraw, "TACAdatRaw");
  AddDataOut(p_nturaw, "TACAntuRaw");

  AddPara(p_parmap, "TACAparMap");
  AddPara(p_parcal, "TACAparCal");

  f_parcal = (TACAparCal*) fpParCal->Object();
  f_parmap = (TACAparMap*) fpParMap->Object();

}

//------------------------------------------+-----------------------------------
//! Destructor.

TACAactNtuRaw::~TACAactNtuRaw()
{
  delete fTcorr1;
  delete fTcorr2;
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TACAactNtuRaw::Action() {

   TACAdatRaw*   p_datraw = (TACAdatRaw*) fpDatRaw->Object();
   TACAntuRaw*   p_nturaw = (TACAntuRaw*) fpNtuRaw->Object();
   TACAparMap*   p_parmap = (TACAparMap*) fpParMap->Object();
  
  int nhit = p_datraw->GetHitsN();

  
  int ch_num, bo_num;

  for(int ih = 0; ih < nhit; ++ih) {
    TACArawHit *aHi = p_datraw->GetHit(ih);

    Int_t ch_num     = aHi->GetChID();
    Int_t bo_num     = aHi->GetBoardId();
    Double_t time    = aHi->GetTime();
    Double_t timeOth = aHi->GetTimeOth();
    Double_t charge  = aHi->GetCharge();
  
    // here needed mapping file
    Int_t crysId = p_parmap->GetCrystalId(bo_num, ch_num);
    
    if (crysId == -1) // pb with mapping
      continue;

    Double_t type=0; // I define a fake type (I do not know what it really is...) (gtraini)
    
    // here we need the calibration file
    Double_t charge_tcorr = GetTemperatureCorrection(charge, crysId);
    Double_t charge_equalis = GetEqualisationCorrection(charge_tcorr, crysId);
    Double_t energy = GetEnergy(charge_equalis, crysId);
    Double_t tof    = GetTime(time, crysId);
    p_nturaw->NewHit(crysId, energy, time,type);
    
  }

   
  fpNtuRaw->SetBit(kValid);

     
   return kTRUE;
}

// --------------------------------------------------------------------------------------
void  TACAactNtuRaw::SetTemperatureFunctions()
{

   fTcorr1 = new TF1("Tcorr1", this, &TACAactNtuRaw::TemperatureCorrFunction, 0, 100000, 2, "TACAactNtuRaw", "TemperatureCorrFunction");
   fTcorr2 = new TF1("Tcorr2", this, &TACAactNtuRaw::TemperatureCorrFunction, 0, 100000, 2, "TACAactNtuRaw", "TemperatureCorrFunction");

}

// --------------------------------------------------------------------------------------
void  TACAactNtuRaw::SetParFunction()
{
   fTcorr1->SetParameters(fTcorr1Par0, fTcorr1Par1);
   fTcorr2->SetParameters(fTcorr2Par0, fTcorr2Par1);
}

// --------------------------------------------------------------------------------------
Double_t TACAactNtuRaw::TemperatureCorrFunction(Double_t* x, Double_t* par)
{
   Float_t xx = x[0];
   Float_t m0 = par[0] + xx*par[1];

   return m0;
}

//------------------------------------------+-----------------------------------
Double_t TACAactNtuRaw::GetTemperatureCorrection(Double_t charge, Int_t  crysId)
{

  SetTemperatureFunctions();
  SetParFunction();
  Double_t T0 = f_parcal->getCalibrationMap()->GetTemperatureCry(crysId);
  Double_t m1 = fTcorr1->Eval(charge);
  Double_t m2 = fTcorr2->Eval(charge);

  Double_t m0 = m1 + ((m2-m1)/(T2-T1))*(T0-T1);

  Double_t delta = (T1 - T0) * m0;

  Double_t charge_tcorr = charge + delta;

  return charge_tcorr;
  
}
//------------------------------------------+-----------------------------------
Double_t TACAactNtuRaw::GetEqualisationCorrection(Double_t charge_tcorr, Int_t  crysId)
{

  Double_t Equalis0 = f_parcal->getCalibrationMap()->GetEqualiseCry(crysId);
  Double_t charge_equalis = charge_tcorr*Equalis0;
  
  return charge_equalis;

}

//------------------------------------------+-----------------------------------
Double_t TACAactNtuRaw::GetEnergy(Double_t rawenergy, Int_t  crysId)
{
  // TACAparCal* p_parcal = (TACAparCal*) fpParCal->Object();

  // Double_t p0 = p_parcal->GetElossParameter(crysId,0);
  // Double_t p1 = p_parcal->GetElossParameter(crysId,1);
  // Double_t p2 = p_parcal->GetElossParameter(crysId,2);
  // Double_t p3 = p_parcal->GetElossParameter(crysId,3);
  
  // return p0 + p1 * rawenergy;


  //fake calibration (gtraini)  return raw value meanwhile
  return rawenergy;
  
}

//------------------------------------------+-----------------------------------
Double_t TACAactNtuRaw::GetTime(Double_t RawTime, Int_t  crysId)
{
  // TACAparCal* p_parcal = (TACAparCal*) fpParCal->Object();

  // Double_t p0 = p_parcal->GetTofParameter(crysId,0);
  // Double_t p1 = p_parcal->GetTofParameter(crysId,1);
  // Double_t p2 = p_parcal->GetTofParameter(crysId,2);
  // Double_t p3 = p_parcal->GetTofParameter(crysId,3);

  // return p0 + p1 * RawTime;
  
  //fake calibration (gtraini), return raw value meanwhile
  return RawTime;

}

//------------------------------------------+-----------------------------------
//! Histograms

void TACAactNtuRaw::CreateHistogram(){

  DeleteHistogram();

  char histoname[100]="";
  if(FootDebugLevel(1))
     cout<<"I have created the CA histo. "<<endl;

  // sprintf(histoname,"stEvtTime");
  // hEventTime = new TH1F(histoname, histoname, 6000, 0., 60.);
  // AddHistogram(hEventTime);

  // sprintf(histoname,"stTrigTime");
  // hTrigTime = new TH1F(histoname, histoname, 256, 0., 256.);
  // AddHistogram(hTrigTime);

  sprintf(histoname,"caTotCharge");
  hTotCharge = new TH1F(histoname, histoname, 400, -0.1, 3.9);
  AddHistogram(hTotCharge);
  
  // for(int iCh=0;iCh<8;iCh++){
  //   sprintf(histoname,"stDeltaTime_ch%d", iCh);
  //   hArrivalTime[iCh]= new TH1F(histoname, histoname, 100, -5., 5.);
  //   AddHistogram(hArrivalTime[iCh]);

  //   sprintf(histoname,"stCharge_ch%d", iCh);
  //   hCharge[iCh]= new TH1F(histoname, histoname, 200, -0.1, 1.9);
  //   AddHistogram(hCharge[iCh]);

  //   sprintf(histoname,"stMaxAmp_ch%d", iCh);
  //   hAmplitude[iCh]= new TH1F(histoname, histoname, 120, -0.1, 1.1);
  //   AddHistogram(hAmplitude[iCh]);
  // }

  SetValidHistogram(kTRUE);
  
}
