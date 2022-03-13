/*!
  \file TAVTactBaseNtuHitMC.cxx
  \brief   Base class to NTuplize MC hits
*/

#include "TH2F.h"
#include <map>
#include "TF1.h"
#include "TMath.h"
#include "TDirectory.h"


#include "TAVTparGeo.hxx"

#include "TAVTparConf.hxx"

#include "TAVTntuHit.hxx"
#include "TAMCntuHit.hxx"


#include "TAVTactBaseNtuHitMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "TAGrecoManager.hxx"

/*!
  \class TAVTactBaseNtuHitMC
  \brief Base class to NTuplize VTX MC hits
*/

using namespace std;

//! Class imp
ClassImp(TAVTactBaseNtuHitMC);

Bool_t  TAVTactBaseNtuHitMC::fgPileup          = false;
Float_t TAVTactBaseNtuHitMC::fgPoissonPar      = 0.736; // ajust for FIRST
Int_t   TAVTactBaseNtuHitMC::fgPileupEventsN   = 100;
Float_t TAVTactBaseNtuHitMC::fgSigmaNoiseLevel = -1.;
Int_t   TAVTactBaseNtuHitMC::fgMcNoiseId       = -99;

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pGeoMap geometry parameter descriptor
TAVTactBaseNtuHitMC::TAVTactBaseNtuHitMC(const char* name,  TAGparaDsc* pGeoMap)
 : TAGaction(name, "TAVTactBaseNtuHitMC - NTuplize hit MC data"),
   fpGeoMap(pGeoMap),
	fNoisyPixelsN(0)
{   
	fpHisPoisson = (TH1F*)gDirectory->FindObject("vtPoisson");
	if (fpHisPoisson == 0x0) {
	   
		Double_t tot = 0.;
		Double_t par = fgPoissonPar;

		for (Int_t i = 1; i < 60; ++i) {
			tot += TMath::PoissonI(i, par);
		}

		fpHisPoisson = new TH1F("vtPoisson", "Poisson", 62, -0.5, 61.5);

		for (Int_t i = 1; i < 60; ++i) {
			Float_t val = TMath::PoissonI(i, par)/tot*100.;
			fpHisPoisson->Fill(i, val);
		}
	}
   
   TString tmp(name);
   fPrefix = tmp(0,2);
   
   fTitleDev = fPrefix;
  if (fPrefix.Contains("vt"))
     fTitleDev = "Vertex";
  else if (fPrefix.Contains("it"))
     fTitleDev = "Inner Tracker";
   else
      printf("Wrong prefix for histograms !");
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseNtuHitMC::CreateHistogram()
{

   DeleteHistogram();
   
   TAVTbaseParGeo* pGeoMap = (TAVTbaseParGeo*) fpGeoMap->Object();
   
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisPixel[i] = new TH1F(Form("%sMcPixel%d", fPrefix.Data(), i+1), Form("%s - MC # pixels per clusters for sensor %d", fTitleDev.Data(), i+1), 100, 0., 100.);
      AddHistogram(fpHisPixel[i]);
   }
   
   fpHisPixelTot = new TH1F(Form("%sMcPixelTot", fPrefix.Data()), Form("%s - MC # total pixels per clusters", fTitleDev.Data()), 100, 0., 100.);
   AddHistogram(fpHisPixelTot);

   fpHisDeTot = new TH1F(Form("%sMcDeTot", fPrefix.Data()), Form("%s - MC total energy loss", fTitleDev.Data()), 1000, 0., 10000.);
   AddHistogram(fpHisDeTot);

   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisDeSensor[i] = new TH1F(Form("%sMcDe%d", fPrefix.Data(), i+1), Form("%s - MC energy loss for sensor %d", fTitleDev.Data(), i+1), 1000, 0., 2000.);
      AddHistogram(fpHisDeSensor[i]);
   }
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisPixelMap[i]  = new TH2F(Form("%sMcPixelMap%d", fPrefix.Data(), i+1) , Form("%s - MC pixel map for sensor %d", fTitleDev.Data(), i+1),
                                   pGeoMap->GetPixelsNx(), 0, pGeoMap->GetPixelsNx(),
                                   pGeoMap->GetPixelsNy(), 0, pGeoMap->GetPixelsNy());
      fpHisPixelMap[i]->SetStats(kFALSE);
      AddHistogram(fpHisPixelMap[i]);
   }
   
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      fpHisPosMap[i] =  new TH2F(Form("%sMcPosMap%d", fPrefix.Data(), i+1), Form("%s - MC position map for sensor %d", fTitleDev.Data(), i+1),
                                 100, -pGeoMap->GetPitchX()/2.*pGeoMap->GetPixelsNx(), pGeoMap->GetPitchX()/2.*pGeoMap->GetPixelsNx(),
                                 100, -pGeoMap->GetPitchY()/2.*pGeoMap->GetPixelsNy(), pGeoMap->GetPitchY()/2.*pGeoMap->GetPixelsNy());
      fpHisPosMap[i]->SetStats(kFALSE);
      AddHistogram(fpHisPosMap[i]);
   }
   
   SetValidHistogram(kTRUE);
}


// --------------------------------------------------------------------------------------
//! Compute noise level
void TAVTactBaseNtuHitMC::ComputeNoiseLevel()
{
	// computing number of noise pixels (sigma level) from gaussian
	TF1* f = new TF1("f", "gaus", -10, 10);
	f->SetParameters(1,0,1);
	Float_t fraction = 0;
	
	if (fgSigmaNoiseLevel > 0) {
	   fraction = f->Integral(-fgSigmaNoiseLevel, fgSigmaNoiseLevel)/TMath::Sqrt(2*TMath::Pi());
	   fNoisyPixelsN = TMath::Nint(fDigitizer->GetPixelsNx()*fDigitizer->GetPixelsNy()*(1.-fraction));
	}
	
   FootDebugLine(1, "ComputeNoiseLevel()", Form("Number of noise pixels %d\n", fNoisyPixelsN));
	
	delete f;
}

//------------------------------------------+-----------------------------------
//! Generated pileup events
void  TAVTactBaseNtuHitMC::GeneratePileup()
{
   TAVTparGeo* pGeoMap = (TAVTparGeo*) fpGeoMap->Object();

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
		  
         TVector3 posIn(hit.x, hit.y, hit.zi);
         TVector3 posOut(hit.x, hit.y, hit.zo);

         Int_t hitIdx   = hit.htid;
         Int_t trackIdx = hit.tkid;

         posIn  = pGeoMap->Detector2Sensor(hit.id, posIn);
         posOut = pGeoMap->Detector2Sensor(hit.id, posOut);

		  if (!fDigitizer->Process(hit.de, posIn.X(), posIn.Y(), posIn.Z(), posOut.Z())) continue;
		  FillPixels( hit.id, hitIdx, trackIdx, true);
	   }
	}
}

//______________________________________________________________________________
//! Fill pile up information in the structre when pileup active
//!
//! \param[in] storedEvtInfo list of MC events stored
//! \param[in] hit MC hit
//! \param[in] hit MC hit index
void TAVTactBaseNtuHitMC::FillPileup(vector<RawMcHit_t>& storedEvtInfo, TAMChit* hit, Int_t hitIdx)
{
   TVector3 posIn(hit->GetInPosition());
   TVector3 posOut(hit->GetOutPosition());
   Int_t sensorId = hit->GetSensorId(); // sensorId
   Float_t de     = hit->GetDeltaE();
   Int_t trackIdx = hit->GetTrackIdx()-1;
   
   // used for pileup ...
   RawMcHit_t mcHit;
   mcHit.id  = sensorId;
   mcHit.de  = de;
   mcHit.x   = posIn.X();
   mcHit.y   = posIn.Y();
   mcHit.zi  = posIn.Z();
   mcHit.zo  = posOut.Z();
   mcHit.tkid = trackIdx;
   mcHit.htid = hitIdx;
   storedEvtInfo.push_back(mcHit);
}
