/*!
  \file TAMSDactNtuHitMC.cxx
  \brief   Implementation of TAMSDactNtuHitMC.
*/

#include "TH2F.h"
#include <map>
#include "TF1.h"
#include "TMath.h"
#include "TRandom.h"

#include "TAMSDparGeo.hxx"
#include "TAMSDparCal.hxx"

#include "TAMSDntuHit.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"

#include "TAMSDactNtuHitMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "TAMCflukaParser.hxx"

#include "TAGrecoManager.hxx"

/*!
  \class TAMSDactNtuHitMC"
  \brief NTuplizer for vertex raw hits.
*/

using namespace std;

//! Class imp
ClassImp(TAMSDactNtuHitMC);

Int_t   TAMSDactNtuHitMC::fgMcNoiseId       = -99;

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuMC MC input container descriptor
//! \param[in] pNtuEve MC particle input container descriptor
//! \param[out] pNtuRaw raw data output container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] evStr old Fluka container descriptor
TAMSDactNtuHitMC::TAMSDactNtuHitMC(const char* name, TAGdataDsc* pNtuMC, TAGdataDsc* pNtuEve,TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pCalMap, EVENT_STRUCT* evStr)
:  TAGaction(name, "TAMSDactNtuHitMC - NTuplize MSD MC data"),
   fpNtuMC(pNtuMC),
   fpNtuEve(pNtuEve),
   fpNtuRaw(pNtuRaw),
   fpGeoMap(pGeoMap),
   fpCalMap(pCalMap),
   fNoiseLevel(false),
   fEventStruct(evStr)
{
   if (fEventStruct == 0x0) {
     AddDataIn(pNtuMC, "TAMCntuHit");
     AddDataIn(pNtuEve, "TAMCntuPart");
   }
   AddDataOut(pNtuRaw, "TAMSDntuHit");
   AddPara(pGeoMap, "TAMSDparGeo");
   AddPara(pCalMap, "TAMSDparCal");

   fpGeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   if ((fNoiseLevel = TAGrecoManager::GetPar()->IsElecNoiseMc())) {
      TAMSDparCal* parCal = (TAMSDparCal*) fpCalMap->Object();

      for (Int_t i = 0; i < parCal->GetPedestaLevelSize(); ++i) {
         fSigmaNoiseLevel.push_back(parCal->GetPedestalHitLevel(i));
      }
   }
   
   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAMSDactNtuHitMC::CreateHistogram()
{
  
  DeleteHistogram();
  
  TString prefix = "ms";
  TString titleDev = "Micro Strip Detector";
  
  TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisStrip[i] = new TH1F(Form("%sMcStripl%d", prefix.Data(), i+1), Form("%s - MC # strip per clusters for sensor %d", titleDev.Data(), i+1), 100, 0., 100.);
    AddHistogram(fpHisStrip[i]);
  }
  
  fpHisStripTot = new TH1F(Form("%sMcStripTot", prefix.Data()), Form("%s - MC # total strips per clusters", titleDev.Data()), 100, 0., 100.);
  AddHistogram(fpHisStripTot);
  
  fpHisDeTot = new TH1F(Form("%sMcDeTot", prefix.Data()), Form("%s - MC total energy loss", titleDev.Data()), 1000, 0., 10000.);
  AddHistogram(fpHisDeTot);
  
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisDeSensor[i] = new TH1F(Form("%sMcDe%d", prefix.Data(), i+1), Form("%s - MC energy loss for sensor %d", titleDev.Data(), i+1), 1000, 0., 10000.);
    AddHistogram(fpHisDeSensor[i]);
  }
  
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisAdc[i] = new TH1F(Form("%sStripAdc%d", prefix.Data(), i+1), Form("%s - Charge value per cluster for sensor %d", titleDev.Data(), i+1),
                             pGeoMap->GetStripsN(), 0, pGeoMap->GetStripsN());
      AddHistogram(fpHisAdc[i]);
   }
   
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisStripMap[i]  = new TH1F(Form("%sMcStripMap%d", prefix.Data(), i+1) , Form("%s - MC strip map for sensor %d", titleDev.Data(), i+1),
				 pGeoMap->GetStripsN(), 0, pGeoMap->GetStripsN());
    AddHistogram(fpHisStripMap[i]);
  }
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisPosMap[i] =  new TH1F(Form("%sMcPosMap%d", prefix.Data(), i+1), Form("%s - MC position map for sensor %d", titleDev.Data(), i+1),
			       pGeoMap->GetStripsN(), -pGeoMap->GetPitch()*pGeoMap->GetStripsN(), pGeoMap->GetPitch()*pGeoMap->GetStripsN());
    AddHistogram(fpHisPosMap[i]);
  }
   
  SetValidHistogram(kTRUE);
}


//------------------------------------------+-----------------------------------
//! Create histogram
void TAMSDactNtuHitMC::CreateDigitizer()
{
   TAMSDparGeo* pGeoMap  = (TAMSDparGeo*) fpGeoMap->Object();
   fDigitizer = new TAMSDdigitizer(pGeoMap);
   
   if (fNoiseLevel)
      ComputeNoiseLevel();
}


//------------------------------------------+-----------------------------------
//! Action.
bool TAMSDactNtuHitMC::Action()
{
   TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
   TAMSDntuHit* pNtuRaw = (TAMSDntuHit*) fpNtuRaw->Object();
  
   TAMCntuHit* pNtuMC  = 0;
  
   if (fEventStruct == 0x0)
     pNtuMC = (TAMCntuHit*) fpNtuMC->Object();
   else
     pNtuMC = TAMCflukaParser::GetMsdHits(fEventStruct, fpNtuMC);
  
	pNtuRaw->Clear();
   fMap.clear();
 
   if(FootDebugLevel(1))
      Info("TAMSDactNtuHitMC::Action()", "start  -->  MSDn : %d  ", pNtuMC->GetHitsN());

   
	// Loop over all MC hits
   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hit = pNtuMC->GetHit(i);

		Int_t sensorId = hit->GetSensorId();
      Int_t trackIdx = hit->GetTrackIdx()-1;

		// Digitizing
      if (ValidHistogram()) {
         fpHisDeTot->Fill(hit->GetDeltaE()*TAGgeoTrafo::GevToKev());
         fpHisDeSensor[sensorId]->Fill(hit->GetDeltaE()*TAGgeoTrafo::GevToKev());
      }
            
      // Go to sensor frame
      TVector3 posIn(hit->GetInPosition());
      TVector3 posOut(hit->GetOutPosition());
      
      posIn  = fpGeoTrafo->FromGlobalToMSDLocal(posIn);
      posOut = fpGeoTrafo->FromGlobalToMSDLocal(posOut);
      posIn[2] = posOut[2] = 0;

      posIn  = pGeoMap->Detector2Sensor(sensorId, posIn);
      posOut = pGeoMap->Detector2Sensor(sensorId, posOut);
      
      if (!fDigitizer->Process(hit->GetDeltaE(), posIn[0], posIn[1], posIn[2], posOut[2])) continue;
		FillStrips(sensorId, i, trackIdx);
		
		if (ValidHistogram()) {
         Int_t stripsN = fDigitizer->GetMap().size();
         fpHisStrip[sensorId]->Fill(stripsN);
         fpHisStripTot->Fill(stripsN);
		}
   }

   if (fNoiseLevel > 0)
      FillNoise();

   
   if (fEventStruct != 0x0) {
     fpNtuMC->SetBit(kValid);
     fpNtuEve->SetBit(kValid);
   }
   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}


//------------------------------------------+-----------------------------------
//! Fill strip hit
//!
//! \param[in] sensorId plane id
//! \param[in] hitId MC hit id
//! \param[in] trackIdx MC particle id
void TAMSDactNtuHitMC::FillStrips(Int_t sensorId, Int_t hitId,  Int_t trackIdx)
{
  TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
  TAMSDntuHit* pNtuRaw = (TAMSDntuHit*) fpNtuRaw->Object();
  TAMSDparCal* parCal  = (TAMSDparCal*) fpCalMap->Object();

  Int_t view = pGeoMap->GetSensorPar(sensorId).TypeIdx;
  
  map<int, double> digiMap = fDigitizer->GetMap();
  
  // fill strips from map
  int count = 0;
  for ( map< int, double >::iterator it = digiMap.begin(); it != digiMap.end(); ++it) {
    
    if ( digiMap[it->first] > 0 ) {
      count++;
      int stripId = it->first;
      
       TAMSDhit* strip  = 0x0;
       pair<int, int> p(sensorId, stripId);

       auto pedestal = parCal->GetPedestal( sensorId, stripId );
       if( pedestal.status )
          continue;
       
       if (fMap[p] == 0) {
          strip = (TAMSDhit*)pNtuRaw->NewStrip(sensorId, digiMap[stripId], view, stripId);
          fMap[p] = strip;
       } else {
          strip = fMap[p];
          strip->SetEnergyLoss(strip->GetEnergyLoss()+digiMap[stripId]);
       }
       
      Int_t genPartID = trackIdx;
      strip->AddMcTrackIdx(genPartID, hitId);
      strip->SetSeed();
      
       if(FootDebugLevel(1))
         printf("strip %d\n", stripId);
      
      double pos = pGeoMap->GetPosition(stripId);
      
      strip->SetPosition(pos);
      
      if (ValidHistogram()) {
         fpHisStripMap[sensorId]->Fill(stripId);
         fpHisPosMap[sensorId]->Fill(pos);
         fpHisAdc[sensorId]->Fill(stripId, digiMap[stripId]);
      }
    }
  }
}

//___________________________________
//! Fill noise
void TAMSDactNtuHitMC::FillNoise()
{
   TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      FillNoise(i);
   }
}

//------------------------------------------+-----------------------------------
//! Fill noisy strip
//!
//! \param[in] sensorId plane id
void TAMSDactNtuHitMC::FillNoise(Int_t sensorId)
{
  TAMSDntuHit* pNtuRaw = (TAMSDntuHit*) fpNtuRaw->Object();
  TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
  
  Int_t view = pGeoMap->GetSensorPar(sensorId).TypeIdx;
  
  Int_t stripsN = gRandom->Uniform(0, fNoisyStripsN[sensorId]);
  for (Int_t i = 0; i < stripsN; ++i) {
    Int_t stripId  = gRandom->Uniform(0,fDigitizer->GetStripsN());
    TAMSDhit* strip = pNtuRaw->NewStrip(sensorId, 1., view, stripId);
    strip->AddMcTrackIdx(fgMcNoiseId);
  }
}

// --------------------------------------------------------------------------------------
//! Compute noise level from sigma
void TAMSDactNtuHitMC::ComputeNoiseLevel()
{
  // computing number of noise strips (sigma level) from gaussian
  TF1* f = new TF1("f", "gaus", -10, 10);
  f->SetParameters(1,0,1);
  Float_t fraction = 0;
  
   TAMSDparCal* parCal = (TAMSDparCal*) fpCalMap->Object();

   for (Int_t i = 0; i < parCal->GetPedestaLevelSize(); ++i) {
      fraction = f->Integral(-fSigmaNoiseLevel[i], fSigmaNoiseLevel[i])/TMath::Sqrt(2*TMath::Pi());
      fNoisyStripsN[i] = TMath::Nint(fDigitizer->GetStripsN()*(1.-fraction));
      
      if(FootDebugLevel(1))
         printf("Number of noise strips %d\n", fNoisyStripsN[i]);
   }
  delete f;
}
