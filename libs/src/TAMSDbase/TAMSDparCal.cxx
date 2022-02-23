#include "TError.h"

#include "TAMSDparCal.hxx"

ClassImp(TAMSDparCal)

//_____________________________________________________________________
TAMSDparCal::TAMSDparCal(int strip_number_p)
: TAGparTools()

{
  // Standard constructor
  fMapCal = new TAMSDcalibrationMap(strip_number_p);
   
   SetFunctions();
}

//------------------------------------------+-----------------------------------
TAMSDparCal::~TAMSDparCal()
{
  if (fMapCal)
    delete fMapCal;
   
   if (fFuncEta)
      delete fFuncEta;
   
   if (fFunc1)
      delete fFunc1;
   
   if (fFunc2)
   delete fFunc2;
   
   if (fFunc3)
      delete fFunc3;
}

//_________________________________________
void TAMSDparCal::SetFunctions()
{
   fFuncEta = new TF1("fFuncEta","pol6",0,1);
   fFuncEta->SetParameter(0,2458.8);
   fFuncEta->SetParameter(1,606.9);
   fFuncEta->SetParameter(2,-59574.6);
   fFuncEta->SetParameter(3,276349);
   fFuncEta->SetParameter(4,-533689);
   fFuncEta->SetParameter(5,474078);
   fFuncEta->SetParameter(6,-157766);
   
   fFunc1 = new TF1("fFunc1","pol8",0,0.33);
   fFunc1->SetParameter(0,-1.64509e-05);
   fFunc1->SetParameter(1,0.00764971);
   fFunc1->SetParameter(2,-0.820312);
   fFunc1->SetParameter(3,30.5879);
   fFunc1->SetParameter(4,-416.444);
   fFunc1->SetParameter(5,2846.94);
   fFunc1->SetParameter(6,-10502.1);
   fFunc1->SetParameter(7,19985.3);
   fFunc1->SetParameter(8,-15388.5);
   
   fFunc2 = new TF1("fFunc2","pol5",0.33,0.66);
   fFunc2->SetParameter(0,-0.102357);
   fFunc2->SetParameter(1,1.04854);
   fFunc2->SetParameter(2,-4.12363);
   fFunc2->SetParameter(3,8.22154);
   fFunc2->SetParameter(4,-8.23574);
   fFunc2->SetParameter(5,3.31505);
   
   fFunc3 = new TF1("fFunc3","pol5",0.66,1);
   fFunc3->SetParameter(0,2.60998);
   fFunc3->SetParameter(1,-17.2915);
   fFunc3->SetParameter(2,45.2007);
   fFunc3->SetParameter(3,-58.2217);
   fFunc3->SetParameter(4,37.0426);
   fFunc3->SetParameter(5,-9.32504);
}

//_________________________________________
Bool_t TAMSDparCal::LoadEnergyCalibrationMap(TString name)
{
   Clear();
   
   fMapCal->LoadEnergyCalibrationMap(name.Data());
   
   Info("LoadEnergyCalibrationMap()", "Open file %s for Energy calibration\n", name.Data());
   
   return true;
}

//_____________________________________________________________________
TAMSDcalibrationMap::ElossParameter_t TAMSDparCal::GetElossParameters()
{
    return fMapCal->GetElossParameters();
}

//_________________________________________
Bool_t TAMSDparCal::LoadPedestalMap(TString name)
{
   Clear();
   
   fMapCal->LoadPedestalMap(name.Data());
   
   Info("LoadPedestalMap()", "Open file %s for Pedestals\n", name.Data());
   
   return true;
}

//_____________________________________________________________________
TAMSDcalibrationMap::PedParameter_t TAMSDparCal::GetPedestal(Int_t sensorId, Int_t stripId) {
    return fMapCal->GetPedestal( sensorId, stripId );
}

//_____________________________________________________________________
Double_t TAMSDparCal::GetPedestalValue(Int_t sensorId, TAMSDcalibrationMap::PedParameter_t const& pedestal_p, Bool_t seed)
{
   Double_t sigLevel = 0;
   
   if(seed){
      sigLevel = fMapCal->GetPedestalSeedLevel(sensorId);
   }else {
      sigLevel = fMapCal->GetPedestalHitLevel(sensorId);
   }

   Double_t mean     = pedestal_p.mean;
   Double_t sigma    = pedestal_p.sigma;

   Double_t value = mean + sigLevel*sigma;
   
   if (pedestal_p.status)
      return value;
   else
      return -1;
}

//______________________________________________________________________________
//
Float_t TAMSDparCal::ComputeEtaChargeCorrection(Float_t eta)
{   
   //TODO: template for charge loss correction wrt eta, will need to use parameters loaded from "TAMSD_Energy_Calibration.cal"
   Float_t correction = 1.0;
   
   if(eta == 0 || eta == 1)
      return correction;
   
   correction = fFuncEta->Eval(eta)/fFuncEta->Eval(1);
   
   return correction;
}

//______________________________________________________________________________
//
Float_t TAMSDparCal::ComputeEtaPosCorrection(Float_t eta)
{
   //TODO: template for hit position correction wrt eta, will need to use parameters loaded from "TAMSD_Energy_Calibration.cal"
   Float_t correction = 0.0;
   
   if(eta < 0.33)
   {
      correction = fFunc1->Eval(eta);
   } else if (eta >= 0.33 && eta <= 0.66)
   {
      correction = fFunc2->Eval(eta);
   } else
   {
      correction = fFunc3->Eval(eta);
   }
   
   return correction;
}
