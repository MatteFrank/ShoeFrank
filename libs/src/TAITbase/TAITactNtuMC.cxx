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

#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

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
 : TAVTactBaseNtuMC(name, pGeoMap),
   fpEvtStr(evStr),
   fpNtuRaw(pNtuRaw)
{
	AddDataOut(pNtuRaw, "TAITntuRaw");
	AddPara(pGeoMap, "TAITparGeo");

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
   
   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}

//------------------------------------------+-----------------------------------
void TAITactNtuMC::Digitize(vector<RawMcHit_t> storedEvtInfo, Int_t storedEvents)
{
   TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();
   
   RawMcHit_t mcHit;
   fMap.clear();
   
   if(FootDebugLevel(1))
      Info("TAITactNtuMC::Action()", "start  -->  ITn : %d  ", fpEvtStr->ITRn);
   
   // Loop over all MC hits
   for (Int_t i = 0; i < fpEvtStr->ITRn; i++) {
      if(FootDebugLevel(1))    cout<< endl << "FLUKA id =   " << fpEvtStr->TRfx[i] << "  "<< fpEvtStr->TRfy[i] << "  "<< fpEvtStr->TRfz[i] << endl;
      
      TVector3 posIn(fpEvtStr->ITRxin[i], fpEvtStr->ITRyin[i], fpEvtStr->ITRzin[i]);
      TVector3 posOut(fpEvtStr->ITRxout[i], fpEvtStr->ITRyout[i], fpEvtStr->ITRzout[i]);
      Int_t sensorId   = fpEvtStr->ITRisens[i];
      Int_t genPartIdx = fpEvtStr->ITRid[i] - 1;

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
      posIn = pGeoMap->Detector2Sensor(sensorId, posIn);
      posOut = pGeoMap->Detector2Sensor(sensorId, posOut);
      
      DigitizeHit(sensorId, fpEvtStr->ITRde[i], posIn, posOut, i, genPartIdx);
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
	int nPixelX = fDigitizer->GetPixelsNx();
 
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
void TAITactNtuMC::FillNoise()
{
	TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
	for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
	   FillNoise(i);
	}
}

//___________________________________
void TAITactNtuMC::FillNoise(Int_t sensorId)
{
	TAITntuRaw* pNtuRaw = (TAITntuRaw*) fpNtuRaw->Object();

	Int_t pixelsN = gRandom->Uniform(0, fNoisyPixelsN);
	for (Int_t i = 0; i < pixelsN; ++i) {
	   Int_t col  = gRandom->Uniform(0,fDigitizer->GetPixelsNx());
	   Int_t line = gRandom->Uniform(0,fDigitizer->GetPixelsNy());
	   TAITntuHit* pixel = pNtuRaw->NewPixel(sensorId, 1., line, col);
	   pixel->AddMcTrackIdx(fgMcNoiseId);
	}
}
