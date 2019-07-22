/*!
  \file
  \version $Id: TAITactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAITactNtuMC.
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

#include "TAITactNtuMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "GlobalPar.hxx"

/*!
  \class TAITactNtuMC"
  \brief NTuplizer for vertex raw hits. **
*/

using namespace std;

ClassImp(TAITactNtuMC);


//------------------------------------------+-----------------------------------
//
TAITactNtuMC::TAITactNtuMC(const char* name, TAGdataDsc* pNtuRaw,  TAGparaDsc* pGeoMap, EVENT_STRUCT* evStr)
 : TAVTactBaseNtuMC(name, pGeoMap, evStr),
   fpNtuRaw(pNtuRaw)
{
	AddDataOut(pNtuRaw, "TAITntuRaw");
	AddPara(pGeoMap, "TAITparGeo");

   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//
TAITactNtuMC::TAITactNtuMC(const char* name, TAGdataDsc* pNtuMC, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap)
: TAVTactBaseNtuMC(name, pGeoMap),
   fpNtuMC(pNtuMC),
   fpNtuRaw(pNtuRaw)
{
   AddDataIn(pNtuMC, "TAMCntuHit");
   AddDataOut(pNtuRaw, "TAVTntuRaw");
   AddPara(pGeoMap, "TAVTparGeo");
   
   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Create histogram
void TAITactNtuMC::CreateDigitizer()
{
   TAITparGeo* pGeoMap  = (TAITparGeo*) fpGeoMap->Object();
   fDigitizer = new TAITdigitizerE(pGeoMap);
   if (fgSigmaNoiseLevel > 0)
      ComputeNoiseLevel();
}


//------------------------------------------+-----------------------------------
//! Action.
bool TAITactNtuMC::Action()
{
	
	TAITntuRaw* pNtuRaw = (TAITntuRaw*) fpNtuRaw->Object();
	pNtuRaw->Clear();

   static Int_t storedEvents = 0;
   std::vector<RawMcHit_t> storedEvtInfo;
   
   if (fpEvtStr == 0x0)
      Digitize(storedEvtInfo, storedEvents);
   else
      DigitizeOld(storedEvtInfo, storedEvents);
   
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
void TAITactNtuMC::DigitizeOld(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents)
{
   TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();
   
   RawMcHit_t mcHit;
   
   if(FootDebugLevel(1))
   Info("TAVTactNtuMC::Action()", "start  -->  VTn : %d  ", fpEvtStr->ITRn);
   
   // Loop over all MC hits
   for (Int_t i = 0; i < fpEvtStr->ITRn; i++) {
      if(FootDebugLevel(1))    cout<< endl << "FLUKA id =   " << fpEvtStr->TRfx[i] << "  "<< fpEvtStr->TRfy[i] << "  "<< fpEvtStr->TRfz[i] << endl;
      
      Int_t sensorId = fpEvtStr->ITRisens[i];
      
      // used for pileup ...
      if (fgPileup && storedEvents <= fgPileupEventsN) {
         mcHit.id  = sensorId;
         mcHit.de  = fpEvtStr->ITRde[i];
         mcHit.x   = fpEvtStr->ITRxin[i];
         mcHit.y   = fpEvtStr->ITRyin[i];
         mcHit.zi  = fpEvtStr->ITRzin[i];
         mcHit.zo  = fpEvtStr->ITRzout[i];
         storedEvtInfo.push_back(mcHit);
      }
      
      // Digitizing
      //      don't change anything ?
      //      Int_t genPartID = fpEvtStr->ITRid[i] - 1;
      //      if (fpEvtStr->TRcha[genPartID] < 1) continue;
      TVector3 posIn(fpEvtStr->ITRxin[i], fpEvtStr->ITRyin[i], fpEvtStr->ITRzin[i]);
      TVector3 posOut(fpEvtStr->ITRxout[i], fpEvtStr->ITRyout[i], fpEvtStr->ITRzout[i]);
      posIn = pGeoMap->Detector2Sensor(sensorId, posIn);
      posOut = pGeoMap->Detector2Sensor(sensorId, posOut);
      Int_t genPartIdx = fpEvtStr->ITRid[i] - 1;
      
      DigitizeHit(sensorId, fpEvtStr->ITRde[i], posIn, posOut, i, genPartIdx);
   }
}

//------------------------------------------+-----------------------------------
void TAITactNtuMC::Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents)
{
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
   TAMCntuHit* pNtuMC  = (TAMCntuHit*) fpNtuMC->Object();
   
   RawMcHit_t mcHit;
   
   if(FootDebugLevel(1))
   Info("TAVTactNtuMC::Action()", "start  -->  ITRn : %d  ", pNtuMC->GetHitsN());
   
   // Loop over all MC hits
   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hit = pNtuMC->GetHit(i);
      
      TVector3 posIn(hit->GetInPosition());
      TVector3 posOut(hit->GetOutPosition());
      Int_t sensorId = hit->GetLayer(); // sensorId
      Float_t de     = hit->GetDeltaE();
      Int_t  trackIdx = hit->GetTrackId();
      
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
void TAITactNtuMC::DigitizeHit(Int_t sensorId, Float_t de, TVector3& posIn, TVector3& posOut, Int_t idx, Int_t trackIdx)
{
   if (!fDigitizer->Process(de, posIn[0], posIn[1], posIn[2], posOut[2])) return;
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
void TAITactNtuMC::FillPixels(Int_t sensorId, Int_t hitId, Int_t trackIdx )
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
         
			TAITntuHit* pixel = (TAITntuHit*)pNtuRaw->NewPixel(sensorId, 1., line, col);

         pixel->AddMcTrackId(trackIdx, hitId);


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
void TAITactNtuMC::FillNoise()
{
	TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
	for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
	   FillNoise(i);
	}
}

//___________________________________
void TAITactNtuMC::FillNoise(Int_t sensorId)
{
	TAITntuRaw* pNtuRaw = (TAITntuRaw*) fpNtuRaw->Object();

	Int_t pixelsN = gRandom->Uniform(0, fNoisyPixelsN);
	for (Int_t i = 0; i < pixelsN; ++i) {
	   Int_t col  = gRandom->Uniform(0,fDigitizer->GetNPixelX());
	   Int_t line = gRandom->Uniform(0,fDigitizer->GetNPixelY());
	   TAITntuHit* pixel = pNtuRaw->NewPixel(sensorId, 1., line, col);
	   pixel->AddMcTrackId(fgMcNoiseId);
	}
}
