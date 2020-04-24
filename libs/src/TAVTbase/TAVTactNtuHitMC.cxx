/*!
  \file
  \version $Id: TAVTactNtuHitMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAVTactNtuHitMC.
*/

#include "TH2F.h"
#include <map>
#include "TF1.h"
#include "TMath.h"
#include "TDirectory.h"


#include "TAMCntuHit.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAVTntuRaw.hxx"


#include "TAVTactNtuHitMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

#include "GlobalPar.hxx"

/*!
  \class TAVTactNtuHitMC"
  \brief NTuplizer for vertex raw hits. **
*/

using namespace std;

ClassImp(TAVTactNtuHitMC);

//------------------------------------------+-----------------------------------
//
TAVTactNtuHitMC::TAVTactNtuHitMC(const char* name, TAGdataDsc* pNtuMC, TAGdataDsc* pNtuEve, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap)
: TAVTactBaseNtuMC(name, pGeoMap),
   fpNtuMC(pNtuMC),
   fpNtuEve(pNtuEve),
   fpNtuRaw(pNtuRaw)
{
   AddDataIn(pNtuMC, "TAMCntuHit");
   AddDataIn(pNtuEve, "TAMCntuEve");
   AddDataOut(pNtuRaw, "TAVTntuRaw");
   AddPara(pGeoMap, "TAVTparGeo");
   
   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TAVTactNtuHitMC::CreateDigitizer()
{
   TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();
   if (pGeoMap->GetType() == 1)
      fDigitizer = new TAVTdigitizerG(pGeoMap);
   else
      fDigitizer = new TAVTdigitizerE(pGeoMap);
   
   if (fgSigmaNoiseLevel > 0)
      ComputeNoiseLevel();
}

//------------------------------------------+-----------------------------------
//! Action.
bool TAVTactNtuHitMC::Action()
{
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();
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
   
   fpNtuRaw->SetBit(kValid);
   
   return kTRUE;
}

//------------------------------------------+-----------------------------------
void TAVTactNtuHitMC::Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents)
{
   TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
   TAMCntuHit* pNtuMC  = (TAMCntuHit*) fpNtuMC->Object();

   RawMcHit_t mcHit;
   fMap.clear();
   
   if(FootDebugLevel(1))
      Info("TAVTactNtuHitMC::Action()", "start  -->  VTn : %d  ", pNtuMC->GetHitsN());
   
   // Loop over all MC hits
   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hit = pNtuMC->GetHit(i);
      
      TVector3 posIn(hit->GetInPosition());
      TVector3 posOut(hit->GetOutPosition());
      Int_t sensorId = hit->GetLayer(); // sensorId
      Float_t de     = hit->GetDeltaE();
      Int_t trackIdx = hit->GetTrackId();
      
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
void TAVTactNtuHitMC::DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackIdx)
{
   TAMCntuEve* pNtuEve  = (TAMCntuEve*) fpNtuEve->Object();
   TAMCeveTrack*  track = pNtuEve->GetHit(trackIdx);
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
void TAVTactNtuHitMC::FillPixels(Int_t sensorId, Int_t hitId, Int_t trackIdx)
{
	TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();
 
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
         TAVTntuHit* pixel = 0x0;
         pair<int, int> p(sensorId, it->first);
         
         if (fMap[p] == 0x0) {
            pixel = (TAVTntuHit*)pNtuRaw->NewPixel(sensorId, value, line, col);
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
void TAVTactNtuHitMC::FillNoise()
{
   TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      FillNoise(i);
   }
}

//___________________________________
void TAVTactNtuHitMC::FillNoise(Int_t sensorId)
{
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();

	Int_t pixelsN = gRandom->Uniform(0, fNoisyPixelsN);
	for (Int_t i = 0; i < pixelsN; ++i) {
	   Int_t col  = gRandom->Uniform(0,fDigitizer->GetNPixelX());
	   Int_t line = gRandom->Uniform(0,fDigitizer->GetNPixelY());
	   TAVTntuHit* pixel = pNtuRaw->NewPixel(sensorId, 1., line, col);
	   pixel->AddMcTrackIdx(fgMcNoiseId);
	}
}
