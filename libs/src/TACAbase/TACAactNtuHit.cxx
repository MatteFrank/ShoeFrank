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
    fpParCal(p_parcal)
{
  AddDataIn(p_datraw, "TACAdatRaw");
  AddDataOut(p_nturaw, "TACAntuHit");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACAactNtuHit::~TACAactNtuHit()
{}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TACAactNtuHit::Action() {

   TACAdatRaw*   p_datraw = (TACAdatRaw*) fpDatRaw->Object();
   TACAntuHit*   p_nturaw = (TACAntuHit*) fpNtuRaw->Object();
   TACAparMap*   p_parmap = (TACAparMap*) fpParMap->Object();
   // TACAparCal*   p_parcal = (TACAparCal*) fpParCal->Object();
  
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
    Double_t energy = GetEnergy(charge, crysId);
    Double_t tof    = GetTime(time, crysId);
    p_nturaw->NewHit(crysId, energy, time,type);
    cout << "Energy: " << energy << endl;
  }
   
  fpNtuRaw->SetBit(kValid);

     
   return kTRUE;
}

//------------------------------------------+-----------------------------------
Double_t TACAactNtuHit::GetEnergy(Double_t rawenergy, Int_t  crysId)
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
  // hEventTime = new TH1F(histoname, histoname, 6000, 0., 60.);
  // AddHistogram(hEventTime);

  // sprintf(histoname,"stTrigTime");
  // hTrigTime = new TH1F(histoname, histoname, 256, 0., 256.);
  // AddHistogram(hTrigTime);

  // sprintf(histoname,"caTotCharge");
  // hTotCharge = new TH1F(histoname, histoname, 400, -0.1, 3.9);
  // AddHistogram(hTotCharge);
  
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
