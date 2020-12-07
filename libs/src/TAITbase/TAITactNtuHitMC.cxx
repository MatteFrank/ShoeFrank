/*!
  \file
  \version $Id: TAITactNtuHitMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAITactNtuHitMC.
*/

#include "TH2F.h"
#include <map>
#include "TF1.h"
#include "TMath.h"
#include "TDirectory.h"

#include "TAITparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAMCntuHit.hxx"
#include "TAITntuRaw.hxx"

#include "TAITactNtuHitMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

#include "TAMCflukaParser.hxx"

#include "GlobalPar.hxx"


/*!
  \class TAITactNtuHitMC"
  \brief NTuplizer for vertex raw hits. **
*/

using namespace std;

ClassImp(TAITactNtuHitMC);


//------------------------------------------+-----------------------------------
//
TAITactNtuHitMC::TAITactNtuHitMC(const char* name, TAGdataDsc* pNtuMC, TAGdataDsc* pNtuEve, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, EVENT_STRUCT* evStr)
: TAVTactBaseNtuMC(name, pGeoMap),
   fpNtuMC(pNtuMC),
   fpNtuEve(pNtuEve),
   fpNtuRaw(pNtuRaw),
   fEventStruct(evStr)
{
   if (fEventStruct == 0x0) {
     AddDataIn(pNtuMC, "TAMCntuHit");
     AddDataIn(pNtuEve, "TAMCntuEve");
   } 
   AddDataOut(pNtuRaw, "TAITntuRaw");
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
	
	TAITntuRaw* pNtuRaw = (TAITntuRaw*) fpNtuRaw->Object();
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

//------------------------------------------+-----------------------------------
void TAITactNtuHitMC::Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents)
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
      if (fgPileup && storedEvents <= fgPileupEventsN) {
         mcHit.id  = sensorId;
         mcHit.de  = de;
         mcHit.x   = posIn.X();
         mcHit.y   = posIn.Y();
         mcHit.zi  = posIn.Z();
         mcHit.zo  = posOut.Z();
         storedEvtInfo.push_back(mcHit);
      }
      
      // Digitizing
      posIn  = pGeoMap->Detector2Sensor(sensorId, posIn);
      posOut = pGeoMap->Detector2Sensor(sensorId, posOut);
      
      DigitizeHit(sensorId, de, posIn, posOut, i, trackIdx);
   }
}

//------------------------------------------+-----------------------------------
void TAITactNtuHitMC::DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackIdx)
{
  TAMCntuEve* pNtuEve  = 0;
  
  if (fEventStruct == 0x0)
    pNtuEve = (TAMCntuEve*) fpNtuEve->Object();
  else
    pNtuEve = TAMCflukaParser::GetTracks(fEventStruct, fpNtuEve);
  
  TAMCeveTrack*  track = pNtuEve->GetTrack(trackIdx);
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

//------------------------------------------+-----------------------------------
void TAITactNtuHitMC::FillPixels(Int_t sensorId, Int_t hitId, Int_t trackIdx )
{
	TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
	TAITntuRaw* pNtuRaw = (TAITntuRaw*) fpNtuRaw->Object();
 
	map<int, double> digiMap = fDigitizer->GetMap();
	int nPixelX = fDigitizer->GetNPixelX();
 
	// fill pixels from map
   int count = 0;
	for ( map< int, double >::iterator it = digiMap.begin(); it != digiMap.end(); ++it) {

	   if ( digiMap[it->first] > 0 ) {
         count++;
			int line = it->first / nPixelX;
			int col  = it->first % nPixelX;
         Double_t value = digiMap[it->first];

         TAITntuHit* pixel = 0x0;
         pair<int, int> p(sensorId, it->first);

         if (fMap[p] == 0x0) {
            pixel = (TAITntuHit*)pNtuRaw->NewPixel(sensorId, value, line, col);
            fMap[p] = pixel;
         } else
            pixel = fMap[p];

         pixel->AddMcTrackIdx(trackIdx, hitId);


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
void TAITactNtuHitMC::FillNoise()
{
	TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
	for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
	   FillNoise(i);
	}
}

//___________________________________
void TAITactNtuHitMC::FillNoise(Int_t sensorId)
{
	TAITntuRaw* pNtuRaw = (TAITntuRaw*) fpNtuRaw->Object();

	Int_t pixelsN = gRandom->Uniform(0, fNoisyPixelsN);
	for (Int_t i = 0; i < pixelsN; ++i) {
	   Int_t col  = gRandom->Uniform(0,fDigitizer->GetNPixelX());
	   Int_t line = gRandom->Uniform(0,fDigitizer->GetNPixelY());
	   TAITntuHit* pixel = pNtuRaw->NewPixel(sensorId, 1., line, col);
	   pixel->AddMcTrackIdx(fgMcNoiseId);
	}
}
