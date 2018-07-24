/*!
  \file
  \version $Id: TAVTactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAVTactNtuMC.
*/

#include "TH2F.h"
#include <map>
#include "TF1.h"
#include "TMath.h"
#include "TDirectory.h"


#include "TAVTparGeo.hxx"
#include "TAVTparMap.hxx"
#include "TAVTparConf.hxx"

#include "TAVTntuRaw.hxx"


#include "TAVTactNtuMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "GlobalPar.hxx"

/*!
  \class TAVTactNtuMC"
  \brief NTuplizer for vertex raw hits. **
*/

using namespace std;

ClassImp(TAVTactNtuMC);


Bool_t  TAVTactNtuMC::fgPileup          = false;
Float_t TAVTactNtuMC::fgPoissonPar      = 0.736; // ajust for FIRST
Int_t   TAVTactNtuMC::fgPileupEventsN   = 10;
Float_t TAVTactNtuMC::fgSigmaNoiseLevel = -1.;
Int_t   TAVTactNtuMC::fgMcNoiseId       = -99;


//------------------------------------------+-----------------------------------
//
TAVTactNtuMC::TAVTactNtuMC(const char* name, TAGdataDsc* pNtuRaw,  TAGparaDsc* pGeoMap, TAGparaDsc* pParMap, EVENT_STRUCT* evStr)
 : TAGaction(name, "TAVTactNtuMC - NTuplize Vertex MC data"),
	fpNtuRaw(pNtuRaw),
	fpGeoMap(pGeoMap),
	fpParMap(pParMap),
	fpEvtStr(evStr),
	fDigitizer(new TAVTdigitizerE(pGeoMap)),
	fNoisyPixelsN(0),
	fDebugLevel(0)
{
	AddDataOut(pNtuRaw, "TAVTntuRaw");
	AddPara(pGeoMap, "TAVTparGeo");
	AddPara(pParMap, "TAVTparMap");


	fpHisPoisson = (TH1F*)gDirectory->FindObject("vtPoisson");
	if (fpHisPoisson == 0x0) {
	   
		Double_t tot = 0.;
		Double_t par = fgPoissonPar;

		for (Int_t i = 1; i < 10; ++i) {
			tot += TMath::PoissonI(i, par);
		}

		fpHisPoisson = new TH1F("vtPoisson", "Poisson", 12, -0.5, 11.5);

		for (Int_t i = 1; i < 10; ++i) {
			Float_t val = TMath::PoissonI(i, par)/tot*100.;
			fpHisPoisson->Fill(i, val);
		}
	}
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactNtuMC::CreateHistogram()
{

   DeleteHistogram();
   
   TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();
   
   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
      fpHisPixel[i] = new TH1F(Form("vtMcPixel%d", i+1), Form("Vertex - MC # pixels per clusters for sensor %d", i+1), 100, 0., 100.);
      AddHistogram(fpHisPixel[i]);
   }
   
   fpHisPixelTot = new TH1F("vtMcPixelTot", "Vertex - MC # total pixels per clusters", 100, 0., 100.);
   AddHistogram(fpHisPixelTot);

   fpHisDeTot = new TH1F("vtMcDeTot", "Vertex - MC total energy loss", 1000, 0., 10000.);
   AddHistogram(fpHisDeTot);

   
   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
      fpHisDeSensor[i] = new TH1F(Form("vtMcDe%d", i+1), Form("Vertex - MC energy loss for sensor %d", i+1), 1000, 0., 10000.);
      AddHistogram(fpHisDeSensor[i]);
   }
   
   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
      if (TAVTparConf::IsMapHistOn()) {
         fpHisPixelMap[i]  = new TH2F(Form("vtMcPixelMap%d", i+1) , Form("Vertex - pixel map for sensor %d", i+1),
                                      pGeoMap->GetNPixelX(), 0, pGeoMap->GetNPixelX(),
                                      pGeoMap->GetNPixelY(), 0, pGeoMap->GetNPixelY());
         fpHisPixelMap[i]->SetStats(kFALSE);
         AddHistogram(fpHisPixelMap[i]);
      }
   }
   
   for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
      if (TAVTparConf::IsMapHistOn()) {
         fpHisPosMap[i] =  new TH2F(Form("vtMcPosMap%d", i+1), Form("Vertex - position map for sensor %d", i+1),
                                    100, -pGeoMap->GetPitchX()/2.*pGeoMap->GetNPixelX(), pGeoMap->GetPitchX()/2.*pGeoMap->GetNPixelX(),
                                    100, -pGeoMap->GetPitchY()/2.*pGeoMap->GetNPixelY(), pGeoMap->GetPitchY()/2.*pGeoMap->GetNPixelY());
         fpHisPosMap[i]->SetStats(kFALSE);
         AddHistogram(fpHisPosMap[i]);
      }
   }
   
   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
bool TAVTactNtuMC::Action()
{
	if ( GlobalPar::GetPar()->Debug() > 0 )     
	  Info("TAVTactNtuMC::Action()", "start  -->  VTn : %d  ", fpEvtStr->VTXn);

	TAVTparGeo* pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();     
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();
	pNtuRaw->Clear();

	static Int_t storedEvents = 0;
	std::vector<RawMcHit_t> storedEvtInfo;
	RawMcHit_t mcHit;

	// Loop over all MC hits
	for (Int_t i = 0; i < fpEvtStr->VTXn; i++) {
		if ( GlobalPar::GetPar()->Debug() > 0 )     cout<< endl << "FLUKA id =   " << fpEvtStr->TRfx[i] << "  "<< fpEvtStr->TRfy[i] << "  "<< fpEvtStr->TRfz[i] << endl;

		// !!  in ntuple, the row and col start from 0  !!!
		Int_t myTrow, myTcol;
		myTrow = fpEvtStr->VTXirow[i];
		myTcol = fpEvtStr->VTXicol[i];
		Int_t sensorId = pGeoMap->GetSensorID( fpEvtStr->VTXilay[i], myTcol, myTrow );
	   
		// used for pileup ...
		if (fgPileup && storedEvents <= fgPileupEventsN) {
			mcHit.id  = sensorId;
			mcHit.de  = fpEvtStr->VTXde[i];
			mcHit.x   = fpEvtStr->VTXxin[i];
			mcHit.y   = fpEvtStr->VTXyin[i];
			mcHit.zi  = fpEvtStr->VTXzin[i];
			mcHit.zo  = fpEvtStr->VTXzout[i];
			storedEvtInfo.push_back(mcHit);
		}

		// Digitizing
      if (fpEvtStr->TRcha[i] < 1) continue;
      
      if (ValidHistogram()) {
         fpHisDeTot->Fill(fpEvtStr->VTXde[i]*TAVTbaseDigitizer::GeV2keV());
         fpHisDeSensor[sensorId]->Fill(fpEvtStr->VTXde[i]*TAVTbaseDigitizer::GeV2keV());
      }
      
		if (!fDigitizer->Process(fpEvtStr->VTXde[i], fpEvtStr->VTXxin[i], fpEvtStr->VTXyin[i], fpEvtStr->VTXzin[i], fpEvtStr->VTXzout[i])) continue;
		FillPixels(sensorId, i);
		
		if (ValidHistogram()) {
         Int_t pixelsN = fDigitizer->GetMap().size();
         fpHisPixel[sensorId]->Fill(pixelsN);
         fpHisPixelTot->Fill(pixelsN);
		}
   }

   // Pileup
   if (fgPileup && storedEvents <= fgPileupEventsN) {
      fStoredEvents.push_back(storedEvtInfo);
      storedEvents++;
   }
	
   if (fgPileup && storedEvents >= fgPileupEventsN)
      GeneratePileup();


   if(fDebugLevel) {
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
void TAVTactNtuMC::FillPixels(Int_t sensorId, Int_t hitId )
{
	TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();
 
	map<int, int> digiMap = fDigitizer->GetMap();
	int nPixelX = fDigitizer->GetNPixelX();
 
	// fill pixels from map
   int count = 0;
	for ( map< int, int >::iterator it = digiMap.begin(); it != digiMap.end(); ++it) {

	   if ( digiMap[it->first] == 1 ) {
         count++;
			int line = it->first / nPixelX;
			int col  = it->first % nPixelX;
         
			TAVTntuHit* pixel = (TAVTntuHit*)pNtuRaw->NewPixel(sensorId, 1., line, col);

         Int_t genPartID = fpEvtStr->VTXid[hitId] - 1;
         pixel->SetMCid(genPartID);
         SetMCinfo(pixel, hitId);


         if ( GlobalPar::GetPar()->Debug() > 0 )
				printf("line %d col %d\n", line, col);

			double v = pGeoMap->GetPositionV(line);
			double u = pGeoMap->GetPositionU(col);


			if (ValidHistogram()) {
				fpHisPixelMap[sensorId]->Fill(line, col);
				fpHisPosMap[sensorId]->Fill(u, v);
			}
		}
   }
}



// --------------------------------------------------------------------------------------
void TAVTactNtuMC::ComputeNoiseLevel()
{
	// computing number of noise pixels (sigma level) from gaussian
	TF1* f = new TF1("f", "gaus", -10, 10);
	f->SetParameters(1,0,1);
	Float_t fraction = 0;
	
	if (fgSigmaNoiseLevel > 0) {
	   fraction = f->Integral(-fgSigmaNoiseLevel, fgSigmaNoiseLevel)/TMath::Sqrt(2*TMath::Pi());
	   fNoisyPixelsN = TMath::Nint(fDigitizer->GetNPixelX()*fDigitizer->GetNPixelY()*(1.-fraction));
	}
	
	if (fDebugLevel)
	   printf("Number of noise pixels %d\n", fNoisyPixelsN);
	
	delete f;
}

//___________________________________
void TAVTactNtuMC::FillNoise()
{
	TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();
	for (Int_t i = 0; i < pGeoMap->GetNSensors(); ++i) {
	   FillNoise(i);
	}
}


//___________________________________
void TAVTactNtuMC::FillNoise(Int_t sensorId)
{
	TAVTntuRaw* pNtuRaw = (TAVTntuRaw*) fpNtuRaw->Object();

	Int_t pixelsN = gRandom->Uniform(0, fNoisyPixelsN);
	for (Int_t i = 0; i < pixelsN; ++i) {
	   Int_t col  = gRandom->Uniform(0,fDigitizer->GetNPixelX());
	   Int_t line = gRandom->Uniform(0,fDigitizer->GetNPixelY());
	   TAVTntuHit* pixel = pNtuRaw->NewPixel(sensorId, 1., line, col);
	   pixel->SetMCid(fgMcNoiseId);
	}
}


//------------------------------------------+-----------------------------------
void  TAVTactNtuMC::GeneratePileup()
{
	Int_t pileupEvents = TMath::Nint(fpHisPoisson->GetRandom())-1;

	
	// form pileup events number pull out randomly the stored events
	std::vector<int> rarray;

	for (Int_t i = 0; i < fgPileupEventsN; ++i) {
	   if (i > pileupEvents-1)
		  rarray.push_back(0);
	   else
		  rarray.push_back(1);
	}
	
	std::random_shuffle (rarray.begin(), rarray.end(), TAVTbaseDigitizer::GetRandom);

	std::vector<RawMcHit_t> mcInfo;

	for (Int_t p = 0; p < fgPileupEventsN; ++p) {
	   
	   if (rarray[p] == 0) continue;
	   
	   mcInfo = fStoredEvents[p];
	   
	   for (Int_t j = 0; j < mcInfo.size(); ++j) {
		  RawMcHit_t hit = mcInfo[j];
		  
		  if (!fDigitizer->Process(hit.de, hit.x, hit.y, hit.zi, hit.zo)) continue;
		  FillPixels( hit.id, -1);
	   }
	}
}

//------------------------------------------+-----------------------------------
void TAVTactNtuMC::SetMCinfo(TAVTntuHit* pixel, Int_t hitId)
{
   int genPartID = fpEvtStr->VTXid[hitId] - 1;
   
   // check true particle ID linked to the hit is in the correct range
   if ( genPartID < 0 || genPartID > fpEvtStr->TRn-1 ) {
      Warning("TAVTactNtuMC::SetMCinfo()", "wrong generate particle ID: %d nPart = %d", genPartID, fpEvtStr->TRn);
      return;
   }
   
   if ( GlobalPar::GetPar()->Debug() > 0 )     {
      cout << "Part type: " << fpEvtStr->TRfid[genPartID] << " and charge: " << fpEvtStr->TRcha[genPartID] << endl;
      cout << "Generated Position: " << fpEvtStr->TRix[genPartID] <<" "<<fpEvtStr->TRiy[genPartID]<<" "<<fpEvtStr->TRiz[genPartID] << endl;
      cout << "Generated Momentum: " << fpEvtStr->TRipx[genPartID] <<" "<<fpEvtStr->TRipy[genPartID]<<" "<<fpEvtStr->TRipz[genPartID] << endl;
   }
   
   
   // global coordinates
   TVector3 MCpos = TVector3((fpEvtStr->VTXxin[hitId]  + fpEvtStr->VTXxout[hitId])/2,  (fpEvtStr->VTXyin[hitId]  + fpEvtStr->VTXyout[hitId])/2,  (fpEvtStr->VTXzin[hitId]  + fpEvtStr->VTXzout[hitId])/2);
   TVector3 MCmom = TVector3((fpEvtStr->VTXpxin[hitId] + fpEvtStr->VTXpxout[hitId])/2, (fpEvtStr->VTXpyin[hitId] + fpEvtStr->VTXpyout[hitId])/2, (fpEvtStr->VTXpzin[hitId] + fpEvtStr->VTXpzout[hitId])/2);
   
   if ( GlobalPar::GetPar()->Debug() > 0 )     {
      cout << "Vertex pixel hit n: " << hitId << ". Col " << pixel->GetPixelColumn() << " row "<< pixel->GetPixelLine() << endl;
      cout << "\tGlobal kinematic: \n\t\tPos:\t";
      MCpos.Print();
      cout << "\t\tMom:\t";
      MCmom.Print();
   }
   
   
   pixel->SetMCPosition(MCpos);   // set in local coord (transformation in Hit)
   pixel->SetMCMomentum(MCmom);   // set in local coord
   pixel->SetEneLoss(fpEvtStr->VTXde[hitId]);
   
   // store generated particle info
   pixel->SetGeneratedParticleInfo( genPartID, fpEvtStr->TRfid[genPartID], fpEvtStr->TRcha[genPartID],
                                   fpEvtStr->TRbar[genPartID], fpEvtStr->TRmass[genPartID],
                                   TVector3(fpEvtStr->TRix[genPartID], fpEvtStr->TRiy[genPartID], fpEvtStr->TRiz[genPartID]),
                                   TVector3(fpEvtStr->TRipx[genPartID], fpEvtStr->TRipy[genPartID], fpEvtStr->TRipz[genPartID]) );
}
