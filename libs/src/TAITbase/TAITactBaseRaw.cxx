/*!
 \file
 \version $Id: TAITactBaseRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
 \brief   Implementation of TAITactBaseRaw.
 */

#include "DECardEvent.hh"

#include "TH2F.h"

#include "TAGrecoManager.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparMap.hxx"

#include "TAITntuHit.hxx"
#include "TAITactBaseRaw.hxx"
#include "TAVTmi26Type.hxx"

/*!
 \class TAITactBaseRaw TAITactBaseRaw.hxx "TAITactBaseRaw.hxx"
 \brief Base class to get ITR/VTX raw data. **
 */

ClassImp(TAITactBaseRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAITactBaseRaw::TAITactBaseRaw(const char* name, TAGdataDsc* pNtuRaw, TAGparaDsc* pGeoMap, TAGparaDsc* pConfig, TAGparaDsc* pParMap)
: TAVTactBaseRaw(name, pNtuRaw, pGeoMap, pConfig, pParMap)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactBaseRaw::~TAITactBaseRaw()
{
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAITactBaseRaw::CreateHistogram()
{
   DeleteHistogram();
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
   for (Int_t i = 0; i < pGeoMap->GetSensorsN(); ++i) {
     fpHisPixelMap[i] = new TH2F(Form("itPixelMap%d", i+1), Form("Vertex - pixel map for sensor %d", i+1),
                                  pGeoMap->GetPixelsNy(), 0, pGeoMap->GetPixelsNy(),
                                  pGeoMap->GetPixelsNx(), 0, pGeoMap->GetPixelsNx());
     fpHisPixelMap[i]->SetStats(kFALSE);
     AddHistogram(fpHisPixelMap[i]);
      
	  fpHisRateMap[i] = new TH1F(Form("itRateMap%d", i+1), Form("Vertex - rate per line for sensor %d", i+1), 
								 pGeoMap->GetPixelsNx(), 0, pGeoMap->GetPixelsNx());
	  AddHistogram(fpHisRateMap[i]);

	  fpHisRateMapQ[i] = new TH1F(Form("itRateMapQ%d", i+1), Form("Vertex - rate per quadrant for sensor %d", i+1), 10, 0, 5);
	  AddHistogram(fpHisRateMapQ[i]);
	  
	  fpHisEvtLength[i] = new TH1F(Form("itEvtLength%d", i+1), Form("Vertex - event length sensor %d", i+1), 1000, 0, 1000);
	  AddHistogram(fpHisEvtLength[i]);
      
     fpHisTriggerEvt[i] = new TH1F(Form("itTriggerEvt%d", i+1), Form("Vertex - Trigger difference in event sensor %d", i+1),  20, -9.5, 10.5);
     AddHistogram(fpHisTriggerEvt[i]);
      
     fpHisEvtNumber[i] = new TH1F(Form("itNumberEvt%d", i+1), Form("Vertex -  Event number difference per event sensor %d", i+1), 20, -9.5, 10.5);
     AddHistogram(fpHisEvtNumber[i]);
      
     fpHisTimeStampEvt[i] = new TH1F(Form("itTimeStampEvt%d", i+1), Form("Vertex -  Time stamp difference per event sensor %d", i+1), 1000, -200, 200);
     AddHistogram(fpHisTimeStampEvt[i]);
  
      fpHisTriggerFrame[i] = new TH1F(Form("itTriggerFrame%d", i+1), Form("Vertex - Trigger difference in sensor %d", i+1),  20, -9.5, 10.5);
      AddHistogram(fpHisTriggerFrame[i]);
      
      fpHisTimeStampFrame[i] = new TH1F(Form("itTimeStampFrame%d", i+1), Form("Vertex - Time stamp difference in sensor% d", i+1),  1000, -20000, 20000);
      AddHistogram(fpHisTimeStampFrame[i]);
      
      fpHisFrameCnt[i] = new TH1F(Form("itFrameCnt%d", i+1), Form("Vertex - Frame cnt difference in sensor %d", i+1),  510, -9.5, 500.5);
      AddHistogram(fpHisFrameCnt[i]);

   }

   SetValidHistogram(kTRUE);
   return;
}

// --------------------------------------------------------------------------------------
Int_t TAITactBaseRaw::GetSensor(UInt_t key)
{
   TAITparGeo*  pGeoMap = (TAITparGeo*)  fpGeoMap->Object();
   
   key = (key >> 16) & 0xFFFF;
   for (Int_t i = 0; i <  pGeoMap->GetSensorsN(); ++i) {
      if (fgkKeyHeader[i] == key)
         return i;
   }
   return -1;
}



// --------------------------------------------------------------------------------------
void TAITactBaseRaw::AddPixel( Int_t iSensor, Int_t value, Int_t aLine, Int_t aColumn)
{
   // Add a pixel to the vector of pixels
   // require the following info
   // - input = number of the sensors
   // - value = analog value of this pixel
   // - line & column = position of the pixel in the matrix
   
   TAITntuHit* pNtuRaw = (TAITntuHit*) fpNtuRaw->Object();
   TAITparGeo* pGeoMap = (TAITparGeo*) fpGeoMap->Object();
   
   TAIThit* pixel   = (TAIThit*)pNtuRaw->NewPixel(iSensor, value, aLine, aColumn);
   
   double v = pGeoMap->GetPositionV(aLine);
   double u = pGeoMap->GetPositionU(aColumn);
   TVector3 pos(u,v,0);
   pixel->SetPosition(pos);

   
   if (ValidHistogram()) {
      fpHisPixelMap[iSensor]->Fill(aLine, aColumn);
	  
	  fpHisRateMap[iSensor]->Fill(aColumn);
	  
	  for (Int_t i = 0; i < 4; ++i) {
		 if (aColumn >= 258*i && aColumn < (i+1)*258)
			fpHisRateMapQ[iSensor]->Fill(i+1);
	  }
   }
}

