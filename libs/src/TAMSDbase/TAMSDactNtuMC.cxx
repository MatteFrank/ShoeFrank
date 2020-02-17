/*!
  \file
  \version $Id: TAMSDactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAMSDactNtuMC.
*/

#include "TH2F.h"
#include <map>
#include "TF1.h"
#include "TMath.h"
#include "TDirectory.h"


#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"

#include "TAMSDntuRaw.hxx"


#include "TAMSDactNtuMC.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "GlobalPar.hxx"

/*!
  \class TAMSDactNtuMC"
  \brief NTuplizer for vertex raw hits. **
*/

using namespace std;

ClassImp(TAMSDactNtuMC);

Float_t TAMSDactNtuMC::fgSigmaNoiseLevel = -1.;
Int_t   TAMSDactNtuMC::fgMcNoiseId       = -99;

//------------------------------------------+-----------------------------------
//
TAMSDactNtuMC::TAMSDactNtuMC(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, EVENT_STRUCT* evStr)
:  TAGaction(name, "TAMSDactNtuMC - NTuplize MSD MC data"),
   fpNtuRaw(pNtuRaw),
   fpGeoMap(pGeoMap),
   fpEvtStr(evStr),
   fNoisyStripsN(0)
{
	AddDataOut(pNtuRaw, "TAMSDntuRaw");
	AddPara(pGeoMap, "TAMSDparGeo");

   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAMSDactNtuMC::CreateHistogram()
{
  
  DeleteHistogram();
  
  TString prefix = "ms";
  TString titleDev = "Multi Strip Detector";
  
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
                             pGeoMap->GetNStrip(), 0, pGeoMap->GetNStrip());
      AddHistogram(fpHisAdc[i]);
   }
   
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisStripMap[i]  = new TH1F(Form("%sMcStripMap%d", prefix.Data(), i+1) , Form("%s - MC strip map for sensor %d", titleDev.Data(), i+1),
				 pGeoMap->GetNStrip(), 0, pGeoMap->GetNStrip());
    AddHistogram(fpHisStripMap[i]);
  }
  
  for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
    fpHisPosMap[i] =  new TH1F(Form("%sMcPosMap%d", prefix.Data(), i+1), Form("%s - MC position map for sensor %d", titleDev.Data(), i+1),
			       pGeoMap->GetNStrip(), -pGeoMap->GetPitch()*pGeoMap->GetNStrip(), pGeoMap->GetPitch()*pGeoMap->GetNStrip());
    AddHistogram(fpHisPosMap[i]);
  }
   
  SetValidHistogram(kTRUE);
}


//------------------------------------------+-----------------------------------
//! Create histogram
void TAMSDactNtuMC::CreateDigitizer()
{
   TAMSDparGeo* pGeoMap  = (TAMSDparGeo*) fpGeoMap->Object();
   fDigitizer = new TAMSDdigitizer(pGeoMap);
   if (fgSigmaNoiseLevel > 0)
      ComputeNoiseLevel();
}


//------------------------------------------+-----------------------------------
//! Action.
bool TAMSDactNtuMC::Action()
{
	if ( fDebugLevel> 0 )     
	  Info("TAMSDactNtuMC::Action()", "start  -->  VTn : %d  ", fpEvtStr->MSDn);

   TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
	TAMSDntuRaw* pNtuRaw = (TAMSDntuRaw*) fpNtuRaw->Object();
	pNtuRaw->Clear();
   fMap.clear();
   
	// Loop over all MC hits
	for (Int_t i = 0; i < fpEvtStr->MSDn; i++) {
		if ( fDebugLevel> 0 )     cout<< endl << "FLUKA id =   " << fpEvtStr->TRfx[i] << "  "<< fpEvtStr->TRfy[i] << "  "<< fpEvtStr->TRfz[i] << endl;

		Int_t sensorId = fpEvtStr->MSDilay[i];

		// Digitizing
      if (ValidHistogram()) {
         fpHisDeTot->Fill(fpEvtStr->MSDde[i]*TAGgeoTrafo::GevToKev());
         fpHisDeSensor[sensorId]->Fill(fpEvtStr->MSDde[i]*TAGgeoTrafo::GevToKev());
      }
            
      // Go to sensor frame
      TVector3 posIn(fpEvtStr->MSDxin[i], fpEvtStr->MSDyin[i], fpEvtStr->MSDzin[i]);
      TVector3 posOut(fpEvtStr->MSDxout[i], fpEvtStr->MSDyout[i], fpEvtStr->MSDzout[i]);
      posIn  = pGeoMap->Detector2Sensor(sensorId, posIn);
      posOut = pGeoMap->Detector2Sensor(sensorId, posOut);
      
      if (!fDigitizer->Process(fpEvtStr->MSDde[i], posIn[0], posIn[1], posIn[2], posOut[2])) continue;
		FillStrips(sensorId, i);
		
		if (ValidHistogram()) {
         Int_t stripsN = fDigitizer->GetMap().size();
         fpHisStrip[sensorId]->Fill(stripsN);
         fpHisStripTot->Fill(stripsN);
		}
   }

   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}


//------------------------------------------+-----------------------------------
void TAMSDactNtuMC::FillStrips(Int_t sensorId, Int_t hitId )
{
  TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
  TAMSDntuRaw* pNtuRaw = (TAMSDntuRaw*) fpNtuRaw->Object();
  
  Int_t view = pGeoMap->GetSensorPar(sensorId).TypeIdx;
  
  map<int, double> digiMap = fDigitizer->GetMap();
  
  // fill strips from map
  int count = 0;
  for ( map< int, double >::iterator it = digiMap.begin(); it != digiMap.end(); ++it) {
    
    if ( digiMap[it->first] > 0 ) {
      count++;
      int stripId = it->first;
      
       TAMSDntuHit* strip  = 0x0;
       
       if (fMap[stripId] == 0) {
          strip = (TAMSDntuHit*)pNtuRaw->NewStrip(sensorId, digiMap[stripId], view, stripId);
          fMap[stripId] = strip;
       } else
          strip = fMap[stripId];
       
      Int_t genPartID = fpEvtStr->MSDid[hitId] - 1;
      strip->AddMcTrackIdx(genPartID, hitId);
      
       if ( fDebugLevel> 0 )
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
void TAMSDactNtuMC::FillNoise()
{
   TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
      FillNoise(i);
   }
}

//___________________________________
void TAMSDactNtuMC::FillNoise(Int_t sensorId)
{
  TAMSDntuRaw* pNtuRaw = (TAMSDntuRaw*) fpNtuRaw->Object();
  TAMSDparGeo* pGeoMap = (TAMSDparGeo*) fpGeoMap->Object();
  
  Int_t view = pGeoMap->GetSensorPar(sensorId).TypeIdx;
  
  Int_t stripsN = gRandom->Uniform(0, fNoisyStripsN);
  for (Int_t i = 0; i < stripsN; ++i) {
    Int_t stripId  = gRandom->Uniform(0,fDigitizer->GetStripsN());
    TAMSDntuHit* strip = pNtuRaw->NewStrip(sensorId, 1., view, stripId);
    strip->AddMcTrackIdx(fgMcNoiseId);
  }
}

// --------------------------------------------------------------------------------------
void TAMSDactNtuMC::ComputeNoiseLevel()
{
  // computing number of noise pixels (sigma level) from gaussian
  TF1* f = new TF1("f", "gaus", -10, 10);
  f->SetParameters(1,0,1);
  Float_t fraction = 0;
  
  if (fgSigmaNoiseLevel > 0) {
    fraction = f->Integral(-fgSigmaNoiseLevel, fgSigmaNoiseLevel)/TMath::Sqrt(2*TMath::Pi());
    fNoisyStripsN = TMath::Nint(fDigitizer->GetStripsN()*(1.-fraction));
  }
  
  if (fDebugLevel)
      printf("Number of noise pixels %d\n", fNoisyStripsN);
  
  delete f;
}
