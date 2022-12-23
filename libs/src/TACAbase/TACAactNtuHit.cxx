/*!
  \file TACAactNtuHit.cxx
  \brief   Implementation of TACAactNtuHit.
*/

#include "TACAparMap.hxx"
#include "TACAparCal.hxx"
#include "TACAactNtuHit.hxx"

/*!
  \class TACAactNtuHit
  \brief Action to create CA hits from RAW data. **
*/

//! Class Imp
ClassImp(TACAactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] p_datraw raw data input container descriptor
//! \param[out] p_nturaw data output container descriptor
//! \param[in] p_parmap mapping parameter descriptor
//! \param[in] p_parcal calibration parameter descriptor
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
    // this should be read from calib file for each crystal ??
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
Bool_t TACAactNtuHit::Action()
{
   TACAntuRaw*   p_datraw = (TACAntuRaw*) fpDatRaw->Object();
   TACAntuHit*   p_nturaw = (TACAntuHit*) fpNtuRaw->Object();
   TACAparMap*   p_parmap = (TACAparMap*) fpParMap->Object();

   int nhit = p_datraw->GetHitsN();

   int ch_num, bo_num;

   int nCry = p_parmap->GetCrystalsN();
   Double_t totCharge = 0;

   for (int ih = 0; ih < nhit; ++ih) {
      TACArawHit *aHi = p_datraw->GetHit(ih);

      Int_t ch_num     = aHi->GetChID();
      Int_t bo_num     = aHi->GetBoardId();
      Double_t time    = aHi->GetTime();
      Double_t timeOth = aHi->GetTimeOth();
      Double_t charge  = aHi->GetCharge();
      Double_t amplitude  = aHi->GetAmplitude();
      Int_t z = 1; //N.B. only for testing pourposes!!!!
      // Double_t tempADC  = aHi->GetTemperature(); // in ADC count
      //Double_t temp = ADC2Temp(tempADC);

      // here needed mapping file
      Int_t crysId = p_parmap->GetCrystalId(bo_num, ch_num);
       if (crysId < 0 || crysId >= nCry) {  // should not happen, already check on Raw hit creation 
        Error("Action", " --- Not well mapped WD vs crystal ID. board: %d  ch: %d -> crysId %d", bo_num, ch_num, crysId);
         continue;
      }

      Double_t type = 0; // I define a fake type (I do not know what it really is...) (gtraini)

      // Temperature correction
      //Double_t charge_tcorr = GetTemperatureCorrection(charge, temp, crysId);
      //AS:: we wait for a proper integration of T inside the DAQ
      Double_t charge_tcorr = charge;
      Double_t charge_equalis = GetEqualisationCorrection(charge_tcorr, crysId);
      totCharge += charge_equalis;
      Double_t energy = GetEnergy(charge_equalis, crysId, z);
      Double_t tof    = GetTime(time, crysId);

      TACAhit* createdhit = p_nturaw->NewHit(crysId, energy, time,type);
      createdhit->SetValid(true);
  
      if (ValidHistogram()) {
         if (crysId < 9) { // Only 9 histograms
            fhCharge[crysId]->Fill(energy);
            fhChannelMap->Fill(crysId);
            fhAmplitude[crysId]->Fill(amplitude);
         }
      }
   }
   if (ValidHistogram()) fhTotCharge->Fill(totCharge);


   fpNtuRaw->SetBit(kValid);

   return kTRUE;
}

// --------------------------------------------------------------------------------------
//! Convert ADC counts from sensor to Temperature to Celsius
Double_t TACAactNtuHit::ADC2Temp(Double_t adc, Int_t crysId) 
{

   TACAparCal* p_parcal = (TACAparCal*) fpParCal->Object();

   Double_t p0_SH = p_parcal->GetADC2TempParam(crysId, 0);
   Double_t p1_SH = p_parcal->GetADC2TempParam(crysId, 1);
   Double_t p2_SH = p_parcal->GetADC2TempParam(crysId, 2);
   
   // the NTC (negative temperature coefficient) sensor

   const double VCC = 5.04; // voltage divider supply voltage (V) measured at VME crate 
   const double R0 = 10000.0; // series resistance in the voltage divider (Ohm)
   const double Ron = 50.;// value of the multiplexer Ron (Ohm) for ADG406B (dual supply)
   
   double Vadc = (VCC/1023.0) * adc; // 10-bit ADC: max. value is 1023
   double Rt = (Vadc/(VCC-Vadc))*R0 - Ron; // voltage divider equation with Ron correction

   // The Steinhart-Hart formula is given below with the nominal coefficients a, b and c, 
   // which after calibration could be replaced by three constants for each crystal:

   Double_t temp = 1./ (p0_SH + p1_SH * log(Rt) + p2_SH * pow(log(Rt), 3)) - 273.15;
   
   return temp;
}

// --------------------------------------------------------------------------------------
//! Set functions
void  TACAactNtuHit::SetTemperatureFunctions()
{
   // Angular slope as function of amplitude/charge for temp fT1
   fTcorr1 = new TF1("Tcorr1", this, &TACAactNtuHit::TemperatureCorrFunction, 0, 100000, 2, "TACAactNtuRaw", "TemperatureCorrFunction");
   // Angular slope as function of amplitude/charge for temp fT2
   fTcorr2 = new TF1("Tcorr2", this, &TACAactNtuHit::TemperatureCorrFunction, 0, 100000, 2, "TACAactNtuRaw", "TemperatureCorrFunction");

}

// --------------------------------------------------------------------------------------
//! Set parameters functions
void  TACAactNtuHit::SetParFunction(/* Int_t  crysId*/)
{
   // if we manage to have a set of calibration for each crystal
   //TACAparCal* p_parcal = (TACAparCal*) fpParCal->Object();
   //Double_t fTcorr1Par0 = p_parcal->GetTemp1Param(crysId,0);
   //Double_t fTcorr1Par1 = p_parcal->GetTemp1Param(crysId,1);
   //Double_t fTcorr2Par0 = p_parcal->GetTemp2Param(crysId,0);
   //Double_t fTcorr2Par1 = p_parcal->GetTemp2Param(crysId,1);

   fTcorr1->SetParameters(fTcorr1Par0, fTcorr1Par1);
   fTcorr2->SetParameters(fTcorr2Par0, fTcorr2Par1);
}

// --------------------------------------------------------------------------------------
//! Correlation temperature function
//!
//! \param[in] x input vector
//! \param[in] par parameter vector
Double_t TACAactNtuHit::TemperatureCorrFunction(Double_t* x, Double_t* par)
{
   // Angular slope as function of amplitude/charge
   Float_t xx = x[0];
   Float_t m0 = par[0] + xx*par[1];

   return m0;
}

//------------------------------------------+-----------------------------------
//! Return temperature correlation
//!
//! \param[in] charge input charge
//! \param[in] temp input temperature
//! \param[in] crysId crystal id
Double_t TACAactNtuHit::GetTemperatureCorrection(Double_t charge, Double_t temp, Int_t  crysId)
{
   // Current temperature from sensor for crysId
   Double_t T0 = temp;

   Double_t m1 = fTcorr1->Eval(charge);
   Double_t m2 = fTcorr2->Eval(charge);

   Double_t m0 = m1 + ((m2-m1)/(fT2-fT1))*(T0-fT1);

   Double_t delta = (fT1 - T0) * m0;

   Double_t charge_tcorr = charge + delta;

   return charge_tcorr;
}

//------------------------------------------+-----------------------------------
//! Returns equalized correction
//!
//! \param[in] charge_tcorr input charge correlation
//! \param[in] crysId crystal id
Double_t TACAactNtuHit::GetEqualisationCorrection(Double_t charge_tcorr, Int_t  crysId)
{
   TACAparCal* p_parcal = (TACAparCal*) fpParCal->Object();

   Double_t Equalis0 = p_parcal->GetADC2EnergyParam(crysId, 12);

   Double_t charge_equalis = charge_tcorr * Equalis0;

   return charge_equalis;
}


//------------------------------------------+-----------------------------------
//! Get curve for fragments of different Z obtained as a ratio between the parameter curve for protons and the parameter curve for other He, C, O
//!
//! \param[in] p0 of exponential slope
//! \param[in] p1 of exponential slope
//! \param[in] p2 of exponential slope
//! \param[in] z particle charge (atomic number)
Double_t TACAactNtuHit::GetZCurve(Double_t p0, Double_t  p1, Double_t p2, Int_t z)
{

   return p0 + p1*exp(-(z/p2));


  //calibration AValetti's analysis 22.12.22
  //return energy from ADC  
}

//------------------------------------------+-----------------------------------
//! Get calibrated energy
//!
//! \param[in] rawenergy raw energy
//! \param[in] crysId crystal id
//! \param[in] z particle charge (atomic number)
Double_t TACAactNtuHit::GetEnergy(Double_t rawenergy, Int_t  crysId, Int_t z)
{
   TACAparCal* p_parcal = (TACAparCal*) fpParCal->Object();

   Double_t p0 = p_parcal->GetADC2EnergyParam(crysId, 0);
   Double_t p1 = p_parcal->GetADC2EnergyParam(crysId, 1);
   Double_t p2 = p_parcal->GetADC2EnergyParam(crysId, 2);

   Double_t p3 = p_parcal->GetADC2EnergyParam(crysId, 3);
   Double_t p4 = p_parcal->GetADC2EnergyParam(crysId, 4);
   Double_t p5 = p_parcal->GetADC2EnergyParam(crysId, 5);

   Double_t p6 = p_parcal->GetADC2EnergyParam(crysId, 6);
   Double_t p7 = p_parcal->GetADC2EnergyParam(crysId, 7);
   Double_t p8 = p_parcal->GetADC2EnergyParam(crysId, 8);

   Double_t p9 = p_parcal->GetADC2EnergyParam(crysId, 9);
   Double_t p10 = p_parcal->GetADC2EnergyParam(crysId, 10);
   Double_t p11 = p_parcal->GetADC2EnergyParam(crysId, 11);

   p0 = p0*GetZCurve(p3,p4,p5,z);
   p1 = p1*GetZCurve(p6,p7,p8,z);
   p2 = p2*GetZCurve(p9,p10,p11,z);

   Double_t ac = p0-p2*rawenergy;

   return (p1 * rawenergy + sqrt( p1 * p1 * rawenergy * rawenergy + 4 * ac ))/(2 * ac);


  //calibration AValetti's analysis 22.12.22
  //return energy from ADC  
}

//------------------------------------------+-----------------------------------
//! Get calibrated time
//!
//! \param[in] RawTime raw time
//! \param[in] crysId crystal id
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
void TACAactNtuHit::CreateHistogram()
{

   DeleteHistogram();

   char histoname[100]="";
   if (FootDebugLevel(1))
      cout << "Calorimeter Histograms created. "<<endl;

   // sprintf(histoname,"stEvtTime");
   // fhEventTime = new TH1F(histoname, histoname, 6000, 0., 60.);
   // AddHistogram(fhEventTime);

   // sprintf(histoname,"stTrigTime");
   // fhTrigTime = new TH1F(histoname, histoname, 256, 0., 256.);
   // AddHistogram(fhTrigTime);
   
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
