/*!
  \file TANEactNtuHit.cxx
  \brief   Implementation of TANEactNtuHit.
*/

#include "TANEactNtuHit.hxx"

/*!
  \class TANEactNtuHit
  \brief Action to create CA hits from RAW data. **
*/

//! Class Imp
ClassImp(TANEactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] p_datraw raw data input container descriptor
//! \param[out] p_nturaw data output container descriptor
//! \param[in] p_parmap mapping parameter descriptor
//! \param[in] p_parcal calibration parameter descriptor
TANEactNtuHit::TANEactNtuHit(const char* name,
                             TAGdataDsc* p_datraw,
                             TAGdataDsc* p_nturaw)
  : TAGaction(name, "TANEactNtuHit - Unpack neutron raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw)
{
   AddDataIn(p_datraw, "TANEntuRaw");
   AddDataOut(p_nturaw, "TANEntuHit");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TANEactNtuHit::~TANEactNtuHit()
{
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TANEactNtuHit::Action()
{
   TANEntuRaw*   p_datraw = (TANEntuRaw*) fpDatRaw->Object();
   TANEntuHit*   p_nturaw = (TANEntuHit*) fpNtuRaw->Object();

   int nhit = p_datraw->GetHitsN();

   for (int ih = 0; ih < nhit; ++ih) {
      TANErawHit *aHi = p_datraw->GetHit(ih);

      Int_t ch_num     = aHi->GetChID();
      Int_t bo_num     = aHi->GetBoardId();
      Double_t time    = aHi->GetTime();
      Double_t timeOth = aHi->GetTimeOth();
      Double_t charge  = aHi->GetCharge();
      Double_t amplitude  = aHi->GetAmplitude();

      TANEhit* createdhit = p_nturaw->NewHit(ch_num, charge, time);
      createdhit->SetValid(true);
  
      if (ValidHistogram()) {

      }
   }


   fpNtuRaw->SetBit(kValid);

   return kTRUE;
}



//------------------------------------------+-----------------------------------
//! Histograms
void TANEactNtuHit::CreateHistogram()
{

   DeleteHistogram();

   char histoname[100]="";
   
   fhTotCharge = new TH1F("caTotCharge", "caTotCharge", 400, -0.1, 3.9);
   AddHistogram(fhTotCharge);

   fhChannelMap = new TH1F("caChMap", "caChMap", 9, 0, 9);
   AddHistogram(fhChannelMap);

   for(int iCh=0; iCh<9; iCh++) {
      fhArrivalTime[iCh]= new TH1F(Form("caDeltaTime_ch%d", iCh), Form("caDeltaTime_ch%d", iCh), 100, -5., 5.);
      AddHistogram(fhArrivalTime[iCh]);

      fhCharge[iCh]= new TH1F(Form("caCharge_ch%d", iCh), Form("caCharge_ch%d", iCh), 200, -0.1, 100);
      AddHistogram(fhCharge[iCh]);

      fhAmplitude[iCh]= new TH1F(Form("caMaxAmp_ch%d", iCh), Form("caMaxAmp_ch%d", iCh), 120, -0.1, 1.1);
      AddHistogram(fhAmplitude[iCh]);
   }

   SetValidHistogram(kTRUE);
}
