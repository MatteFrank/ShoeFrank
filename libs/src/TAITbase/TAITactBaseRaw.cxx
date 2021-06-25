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
   fPrefix = "it";
   fTitleDev = "Inner Tracker";
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAITactBaseRaw::~TAITactBaseRaw()
{
}


// --------------------------------------------------------------------------------------
void TAITactBaseRaw::AddPixel(Int_t iSensor, Int_t value, Int_t aLine, Int_t aColumn)
{
   // Add a pixel to the vector of pixels
   // require the following info
   // - input = number of the sensors
   // - value = analog value of this pixel
   // - line & column = position of the pixel in the matrix
   
   TAITntuHit*  pNtuRaw = (TAITntuHit*)  fpNtuRaw->Object();
   TAITparGeo*  pGeoMap = (TAITparGeo*)  fpGeoMap->Object();
   TAITparMap*  pParMap = (TAITparMap*)  fpParMap->Object();
   TAITparConf* pConfig = (TAITparConf*) fpConfig->Object();
   
   Int_t planeId = pParMap->GetPlaneId(iSensor, fDataLink);
   
   std::pair<int, int> pair(aLine, aColumn);
   if (pConfig->GetSensorPar(planeId).DeadPixelMap[pair] == 1) return;
   
   TAIThit* pixel   = (TAIThit*)pNtuRaw->NewPixel(planeId, value, aLine, aColumn);
   
   double v = pGeoMap->GetPositionV(aLine);
   double u = pGeoMap->GetPositionU(aColumn);
   TVector3 pos(u,v,0);
   pixel->SetPosition(pos);

   
   if (ValidHistogram()) {
      fpHisPixelMap[planeId]->Fill(aLine, aColumn);
	  
	  fpHisRateMap[planeId]->Fill(aColumn);
	  
	  for (Int_t i = 0; i < 4; ++i) {
		 if (aColumn >= 258*i && aColumn < (i+1)*258)
			fpHisRateMapQ[planeId]->Fill(i+1);
	  }
   }
}

