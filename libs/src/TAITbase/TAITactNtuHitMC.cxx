/*!
  \file TAITactNtuHitMC.cxx
  \brief   NTuplizer for ITR MC hits
*/

#include "TH2F.h"
#include <map>
#include "TF1.h"
#include "TMath.h"
#include "TDirectory.h"

#include "TAITparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAMCntuHit.hxx"
#include "TAITntuHit.hxx"

#include "TAITactNtuHitMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"

#include "TAMCflukaParser.hxx"

#include "TAGrecoManager.hxx"


/*!
  \class TAITactNtuHitMC
  \brief NTuplizer for ITR MC hits
*/

using namespace std;

//! Class Imp
ClassImp(TAITactNtuHitMC);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuMC MC hit container descriptor
//! \param[in] pNtuEve MC event container descriptor
//! \param[out] pNtuRaw hit container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] evStr Fluka structure pointer
TAITactNtuHitMC::TAITactNtuHitMC(const char* name, TAGdataDsc* pNtuMC, TAGdataDsc* pNtuEve, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, EVENT_STRUCT* evStr)
: TAVTactBaseNtuHitMC(name, pGeoMap),
   fpNtuMC(pNtuMC),
   fpNtuEve(pNtuEve),
   fpNtuRaw(pNtuRaw),
   fEventStruct(evStr)
{
   if (fEventStruct == 0x0) {
     AddDataIn(pNtuMC, "TAMCntuHit");
     AddDataIn(pNtuEve, "TAMCntuPart");
   } 
   AddDataOut(pNtuRaw, "TAITntuHit");
   AddPara(pGeoMap, "TAITparGeo");
   
   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Create histogram
void TAITactNtuHitMC::CreateDigitizer()
{
   TAITparGeo* pGeoMap  = (TAITparGeo*) fpGeoMap->Object();
   fDigitizer = new TAITdigitizerE(pGeoMap);
   if (fgSigmaNoiseLevel > 0)
      ComputeNoiseLevel();
}

//------------------------------------------+-----------------------------------
//! Action.
bool TAITactNtuHitMC::Action()
{
	
	TAITntuHit* pNtuRaw = (TAITntuHit*) fpNtuRaw->Object();
	pNtuRaw->Clear();

   static Int_t storedEvents = 0;
   std::vector<RawMcHit_t> storedEvtInfo;
   
   Digitize(storedEvtInfo, storedEvents);
   
   // Pileup
   if (fgPileup && storedEvents <= fgPileupEventsN) {
      fStoredEvents.push_back(storedEvtInfo);
      storedEvents++;
   }
   
   if (fgPileup && storedEvents >= fgPileupEventsN)
   GeneratePileup();
   
   if(FootDebugLevel(1)) {
      std::vector<RawMcHit_t> mcInfo;
      if (fgPileup && storedEvents <= fgPileupEventsN) {
         for (Int_t i = 0; i < fStoredEvents.size(); ++i) {
            printf("Event %d\n", i);
            mcInfo = fStoredEvents[i];
            for (Int_t j = 0; j < mcInfo.size(); ++j) {
               RawMcHit_t hit = mcInfo[j];
               printf("id %d de %.4f x %.4f y %.4f\n", hit.id, hit.de, hit.x, hit.y);
            }
         }
      }
   }
   
   if (fgSigmaNoiseLevel > 0)
      FillNoise();
  
   if (fEventStruct != 0x0) {
     fpNtuMC->SetBit(kValid);
     fpNtuEve->SetBit(kValid);
   }
   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}

//______________________________________________________________________________
//! Digitize for MC hit and stored MC event when pileup active
//!
//! \param[in] storedEvtInfo list of MC events stored
//! \param[in] storedEvents number fo MC events stored
void TAITactNtuHitMC::Digitize(vector<RawMcHit_t>& storedEvtInfo, Int_t storedEvents)
{
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();

   TAMCntuHit* pNtuMC  = 0;
  
   if (fEventStruct == 0x0)
     pNtuMC = (TAMCntuHit*) fpNtuMC->Object();
   else
     pNtuMC = TAMCflukaParser::GetItrHits(fEventStruct, fpNtuMC);
  
   RawMcHit_t mcHit;
   fMap.clear();
   
   if(FootDebugLevel(1))
   Info("TAVTactNtuMC::Action()", "start  -->  ITRn : %d  ", pNtuMC->GetHitsN());
   
   // Loop over all MC hits
   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hit = pNtuMC->GetHit(i);
      
      TVector3 posIn(hit->GetInPosition());
      TVector3 posOut(hit->GetOutPosition());
      Int_t sensorId = hit->GetSensorId(); // sensorId
      Float_t de     = hit->GetDeltaE();
      Int_t trackIdx = hit->GetTrackIdx()-1;
      
      // used for pileup ...
      if (fgPileup && storedEvents <= fgPileupEventsN)
         FillPileup(storedEvtInfo, hit, i);
      
      // Transformations
      posIn  = fpGeoTrafo->FromGlobalToITLocal(posIn);
      posOut = fpGeoTrafo->FromGlobalToITLocal(posOut);
      
      // Digitizing
      posIn  = pGeoMap->Detector2Sensor(sensorId, posIn);
      posOut = pGeoMap->Detector2Sensor(sensorId, posOut);
      
      DigitizeHit(sensorId, de, posIn, posOut, i, trackIdx);
   }
}

//______________________________________________________________________________
//! Digitize from energy loss, position in/out and hit index and track index for a given sensor
//!
//! \param[in] sensorId sensor index
//! \param[in] de energy loss
//! \param[in] posIn position entering the expitaxial layer
//! \param[in] posOut position exiting the expitaxial layer
//! \param[in] idx MC hit index
//! \param[in] trackIdx MC event index
void TAITactNtuHitMC::DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackIdx)
{
  TAMCntuPart* pNtuEve  = 0;
  
  if (fEventStruct == 0x0)
    pNtuEve = (TAMCntuPart*) fpNtuEve->Object();
  else
    pNtuEve = TAMCflukaParser::GetTracks(fEventStruct, fpNtuEve);
  
  TAMCpart*  track = pNtuEve->GetTrack(trackIdx);
  Int_t  Z = track->GetCharge();
  
   if (!fDigitizer->Process(de, posIn[0], posIn[1], posIn[2], posOut[2], 0, 0, Z)) return;
   FillPixels(sensorId, idx, trackIdx);
  
   if (ValidHistogram()) {
      fpHisDeTot->Fill(de*TAGgeoTrafo::GevToKev());
      fpHisDeSensor[sensorId]->Fill(de*TAGgeoTrafo::GevToKev());
      
      Int_t pixelsN = fDigitizer->GetMap().size();
      fpHisPixel[sensorId]->Fill(pixelsN);
      fpHisPixelTot->Fill(pixelsN);
   }
}

//______________________________________________________________________________
//! Fill pixel from hit index and track index for a given sensor
//!
//! \param[in] sensorId sensor index
//! \param[in] idx MC hit index
//! \param[in] trackIdx MC event index
void TAITactNtuHitMC::FillPixels(Int_t sensorId, Int_t hitId, Int_t trackIdx, Bool_t pileup)
{
	TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
	TAITntuHit* pNtuRaw = (TAITntuHit*) fpNtuRaw->Object();
 
	map<int, double> digiMap = fDigitizer->GetMap();
	int nPixelX = fDigitizer->GetPixelsNx();
 
	// fill pixels from map
   int count = 0;
	for ( map< int, double >::iterator it = digiMap.begin(); it != digiMap.end(); ++it) {

	   if ( digiMap[it->first] > 0 ) {
         count++;
			int line = it->first / nPixelX;
			int col  = it->first % nPixelX;
         Double_t value = digiMap[it->first];

         TAIThit* pixel = 0x0;
         pair<int, int> p(sensorId, it->first);

         if (fMap[p] == 0x0) {
            pixel = (TAIThit*)pNtuRaw->NewPixel(sensorId, value, line, col);
            fMap[p] = pixel;
         } else
            pixel = fMap[p];

         pixel->AddMcTrackIdx(trackIdx, hitId);
         if (pileup)
            pixel->SetPileUp();

         if(FootDebugLevel(1))
				printf("line %d col %d\n", line, col);

			double v = pGeoMap->GetPositionV(line);
			double u = pGeoMap->GetPositionU(col);
         TVector3 pos(u,v,0);
         pixel->SetPosition(pos);

			if (ValidHistogram()) {
				fpHisPixelMap[sensorId]->Fill(line, col);
				fpHisPosMap[sensorId]->Fill(u, v);
			}
		}
   }
}

//___________________________________
//! Fill noisy pixels for all sensors
void TAITactNtuHitMC::FillNoise()
{
	TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
	for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
	   FillNoise(i);
	}
}

//___________________________________
//! Fill noisy pixel for a given sensor
//!
//! \param[in] sensorId sensor index
void TAITactNtuHitMC::FillNoise(Int_t sensorId)
{
	TAITntuHit* pNtuRaw = (TAITntuHit*) fpNtuRaw->Object();

	Int_t pixelsN = gRandom->Uniform(0, fNoisyPixelsN);
	for (Int_t i = 0; i < pixelsN; ++i) {
	   Int_t col  = gRandom->Uniform(0,fDigitizer->GetPixelsNx());
	   Int_t line = gRandom->Uniform(0,fDigitizer->GetPixelsNy());
	   TAIThit* pixel = pNtuRaw->NewPixel(sensorId, 1., line, col);
	   pixel->AddMcTrackIdx(fgMcNoiseId);
	}
}
