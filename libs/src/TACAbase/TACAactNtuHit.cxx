/*!
  \file
  \version $Id: TACAactNtuHit.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TACAactNtuHit.
*/

#include "TACAparMap.hxx"
#include "TACAparCal.hxx"
#include "TACAactNtuHit.hxx"

/*!
  \class TACAactNtuHit TACAactNtuHit.hxx "TACAactNtuHit.hxx"
  \brief Get Beam Monitor raw data from WD. **
*/

ClassImp(TACAactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAactNtuHit::TACAactNtuHit(const char* name,
                             TAGdataDsc* p_datraw,
                             TAGdataDsc* p_nturaw,
                             TAGparaDsc* p_parmap,
                             TAGparaDsc* p_parcal)
  : TAGaction(name, "TACAactNtuHit - Unpack CA raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw),
    fpParMap(p_parmap),
    fpParCal(p_parcal),
    fTcorr1Par1(-0.0011),
    fTcorr1Par0(0.167),
    fTcorr2Par1(4.94583e-03),
    fTcorr2Par0(9000.),
    fT1(270.),
    fT2(380.)
{
  AddDataIn(p_datraw, "TACAntuRaw");
  AddDataOut(p_nturaw, "TACAntuHit");

  AddPara(p_parmap, "TACAparMap");
  AddPara(p_parcal, "TACAparCal");

  SetTemperatureFunctions();
  SetParFunction();

}

//------------------------------------------+-----------------------------------
//! Destructor.
TACAactNtuHit::~TACAactNtuHit()
{
  delete fTcorr1;
  delete fTcorr2;
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TACAactNtuHit::Action() {

   TACAntuRaw*   p_datraw = (TACAntuRaw*) fpDatRaw->Object();
   TACAntuHit*   p_nturaw = (TACAntuHit*) fpNtuRaw->Object();
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
    Double_t amplitude  = aHi->GetAmplitude();

  

    // here needed mapping file
    Int_t crysId = p_parmap->GetCrystalId(bo_num, ch_num);
    if (crysId == -1) // pb with mapping
      continue;

    Double_t type=0; // I define a fake type (I do not know what it really is...) (gtraini)

    // here we need the calibration file
   // Double_t charge_tcorr = GetTemperatureCorrection(charge, crysId);
    //AS:: we wait for a proper integration of T inside the DAQ
    Double_t charge_tcorr = charge;
    Double_t charge_equalis = GetEqualisationCorrection(charge_tcorr, crysId);
    Double_t energy = GetEnergy(charge_equalis, crysId);
    Double_t tof    = GetTime(time, crysId);

    TACAhit* createdhit=p_nturaw->NewHit(crysId, energy, time,type);
    createdhit->SetValid(true);
  
    fhCharge[crysId]->Fill(energy);
    fhChannelMap->Fill(crysId);
    fhAmplitude[crysId]->Fill(amplitude);
    
  }


  fpNtuRaw->SetBit(kValid);


   return kTRUE;
}

// --------------------------------------------------------------------------------------
void  TACAactNtuHit::SetTemperatureFunctions()
{

   fTcorr1 = new TF1("Tcorr1", this, &TACAactNtuHit::TemperatureCorrFunction, 0, 100000, 2, "TACAactNtuRaw", "TemperatureCorrFunction");
   fTcorr2 = new TF1("Tcorr2", this, &TACAactNtuHit::TemperatureCorrFunction, 0, 100000, 2, "TACAactNtuRaw", "TemperatureCorrFunction");

}

// --------------------------------------------------------------------------------------
void  TACAactNtuHit::SetParFunction()
{
   fTcorr1->SetParameters(fTcorr1Par0, fTcorr1Par1);
   fTcorr2->SetParameters(fTcorr2Par0, fTcorr2Par1);
}

// --------------------------------------------------------------------------------------
Double_t TACAactNtuHit::TemperatureCorrFunction(Double_t* x, Double_t* par)
{
   Float_t xx = x[0];
   Float_t m0 = par[0] + xx*par[1];

   return m0;
}

//------------------------------------------+-----------------------------------
Double_t TACAactNtuHit::GetTemperatureCorrection(Double_t charge, Int_t  crysId)
{
 // TACAparCal* parcal = (TACAparCal*) fpParCal->Object();


 //  Double_t T0 = parcal->GetTemperatureCry(crysId);

 //  Double_t m1 = fTcorr1->Eval(charge);
 //  Double_t m2 = fTcorr2->Eval(charge);

 //  Double_t m0 = m1 + ((m2-m1)/(fT2-fT1))*(T0-fT1);

 //  Double_t delta = (fT1 - T0) * m0;

 //  Double_t charge_tcorr = charge + delta;

  return charge; //ricordarsi di cambiare quando avremo la calibrazione!!!!!!
  
  //return charge_tcorr;

}
//------------------------------------------+-----------------------------------
Double_t TACAactNtuHit::GetEqualisationCorrection(Double_t charge_tcorr, Int_t  crysId)
{
  TACAparCal* parcal = (TACAparCal*) fpParCal->Object();


  Double_t Equalis0 = parcal->getCalibrationMap()->GetEqualiseCry(crysId);
  Double_t charge_equalis = charge_tcorr*Equalis0;

  return charge_equalis;

}

//------------------------------------------+-----------------------------------
Double_t TACAactNtuHit::GetEnergy(Double_t rawenergy, Int_t  crysId)
{
   TACAparCal* p_parcal = (TACAparCal*) fpParCal->Object();

   Double_t p0 = p_parcal->GetElossParam(crysId,0);
   Double_t p1 = p_parcal->GetElossParam(crysId,1);

   return p0 + p1 * rawenergy;


  //fake calibration (gtraini)  return raw value meanwhile
 // return rawenergy;
}

//------------------------------------------+-----------------------------------
Double_t TACAactNtuHit::GetTime(Double_t RawTime, Int_t  crysId)
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

void TACAactNtuHit::CreateHistogram(){

  DeleteHistogram();

  char histoname[100]="";
  if(FootDebugLevel(1))
     cout<<"I have created the CA histo. "<<endl;

  // sprintf(histoname,"stEvtTime");
  // fhEventTime = new TH1F(histoname, histoname, 6000, 0., 60.);
  // AddHistogram(fhEventTime);

  // sprintf(histoname,"stTrigTime");
  // fhTrigTime = new TH1F(histoname, histoname, 256, 0., 256.);
  // AddHistogram(fhTrigTime);

  sprintf(histoname,"caTotCharge");
  
  fhTotCharge = new TH1F(histoname, histoname, 400, -0.1, 3.9);
  AddHistogram(fhTotCharge);

  sprintf(histoname,"caChMap");

  fhChannelMap = new TH1F(histoname, histoname, 9, 0, 9);
  AddHistogram(fhChannelMap);


  for(int iCh=0;iCh<9;iCh++){
    sprintf(histoname,"caDeltaTime_ch%d", iCh);
    fhArrivalTime[iCh]= new TH1F(histoname, histoname, 100, -5., 5.);
    AddHistogram(fhArrivalTime[iCh]);

    sprintf(histoname,"caCharge_ch%d", iCh);
    fhCharge[iCh]= new TH1F(histoname, histoname, 200, -0.1, 100);
    AddHistogram(fhCharge[iCh]);

    sprintf(histoname,"caMaxAmp_ch%d", iCh);
    fhAmplitude[iCh]= new TH1F(histoname, histoname, 120, -0.1, 1.1);
    AddHistogram(fhAmplitude[iCh]);
  }

  SetValidHistogram(kTRUE);

}
