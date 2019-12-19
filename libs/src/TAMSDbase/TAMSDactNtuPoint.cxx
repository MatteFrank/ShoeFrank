/*!
 \file
 \version $Id: TAMSDactNtuPoint.cxx $
 \brief   Implementation of TAMSDactNtuPoint.
 */
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDntuPoint.hxx"

#include "TAMSDactNtuPoint.hxx"

/*!
 \class TAMSDactNtuPoint
 \brief NTuplizer for micro strip. **
 */

ClassImp(TAMSDactNtuPoint);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDactNtuPoint::TAMSDactNtuPoint(const char* name,
									 TAGdataDsc* pNtuRaw, TAGdataDsc* pNtuPoint, TAGparaDsc* pGeoMap)
 : TAGaction(name, "TAMSDactNtuHit - NTuplize hits"),
   fpNtuRaw(pNtuRaw),
   fpNtuPoint(pNtuPoint),
   fpGeoMap(pGeoMap)
{
   AddDataIn(pNtuRaw,   "TAMSDntuRaw");
   AddDataOut(pNtuPoint, "TAMSDntuPoint");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDactNtuPoint::~TAMSDactNtuPoint()
{

}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAMSDactNtuPoint::CreateHistogram()
{
   DeleteHistogram();

   // fpHisDist = new TH1F("twDist", "ToF Wall - Minimal distance between planes", 200, 0., 100);
   // AddHistogram(fpHisDist);
	 //
   // fpHisCharge1 = new TH1F("twCharge1", "ToF Wall - Charge layer 1", 1000, 0., 5e6);
   // AddHistogram(fpHisCharge1);
	 //
   // fpHisCharge2 = new TH1F("twCharge2", "ToF Wall - Charge layer 2", 1000, 0., 5e6);
   // AddHistogram(fpHisCharge2);
	 //
   // fpHisChargeTot = new TH1F("twChargeTot", "ToF Wall - Total charge", 1000, 0., 5e6);
   // AddHistogram(fpHisChargeTot);

   SetValidHistogram(kTRUE);
}

//______________________________________________________________________________
//
Bool_t TAMSDactNtuPoint::Action()
{
   Bool_t ok = FindPoints();

   fpNtuPoint->SetBit(kValid);

   return true;
}

//______________________________________________________________________________
//
Bool_t TAMSDactNtuPoint::FindPoints()
{
	TAMSDntuRaw* pNtuHit      = (TAMSDntuRaw*) fpNtuRaw->Object();
	TAMSDntuPoint* pNtuPoint  = (TAMSDntuPoint*) fpNtuPoint->Object();
	TAMSDparGeo* pGeoMap      = (TAMSDparGeo*) fpGeoMap->Object();

	bool xyOrder = true;

	for ( int iLayer = 0; iLayer< pGeoMap->GetSensorsN(); iLayer+=2 ){
		m_listOfStripsCol = pNtuHit->GetListOfStrips(iLayer);

		if (m_listOfStripsCol->GetEntries() == 0) continue;

		// fill points
		for (int iStrip = 0; iStrip < m_listOfStripsCol->GetEntries(); iStrip++) {
			TAMSDntuHit* colHit = (TAMSDntuHit*) m_listOfStripsCol->At(iStrip);
			if ( colHit->GetView() == 0 ) 	xyOrder = true;
			else xyOrder = false;
			m_listOfStripsRow = pNtuHit->GetListOfStrips(iLayer + 1);
			for (int iStrip_ = 0; iStrip_ < m_listOfStripsRow->GetEntries(); iStrip_++) {
				TAMSDntuHit* rowHit = (TAMSDntuHit*)m_listOfStripsRow->At(iStrip_);
				if ( !(rowHit->GetView() == 1 && xyOrder) ) 	cout << "ERROR on TAMSDactNtuPoint" << endl;
				TVector3 localPointPosition;
				localPointPosition.SetXYZ(colHit->GetPosition(), rowHit->GetPosition(), pGeoMap->GetSensorPosition(iLayer).Z());
				TAMSDpoint* point = pNtuPoint->NewPoint( iLayer/2, colHit->GetPosition(), rowHit->GetPosition(), localPointPosition );
				int colGenParticleID = colHit->GetMcTrackIdx(0);
				int colMCHitID = colHit->GetMcIndex(0);
				int rowGenParticleID = rowHit->GetMcTrackIdx(0);
				int rowMCHitID = rowHit->GetMcIndex(0);
				point->SetGeneratedParticle( colGenParticleID, rowGenParticleID, colMCHitID, rowMCHitID );
			}
		}
	}
	return true;
}
