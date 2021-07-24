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
				   TAGdataDsc* pNtuCluster, TAGdataDsc* pNtuPoint, TAGparaDsc* pGeoMap)
  : TAGaction(name, "TAMSDactNtuHit - NTuplize hits"),
    fpNtuCluster(pNtuCluster),
    fpNtuPoint(pNtuPoint),
    fpGeoMap(pGeoMap)
{
  AddDataIn(pNtuCluster,   "TAMSDntuCluster");
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
  TAMSDntuCluster* pNtuCluster  = (TAMSDntuCluster*) fpNtuCluster->Object();
  TAMSDntuPoint* pNtuPoint      = (TAMSDntuPoint*) fpNtuPoint->Object();
  TAMSDparGeo* pGeoMap          = (TAMSDparGeo*) fpGeoMap->Object();
  
  bool xyOrder = true;
  
  for ( int iLayer = 0; iLayer< pGeoMap->GetSensorsN(); iLayer+=2 ){

    // fill points
    for (int iClus = 0; iClus < pNtuCluster->GetClustersN(iLayer); iClus++) {

      TAMSDcluster* colHit = (TAMSDcluster*) pNtuCluster->GetCluster(iLayer,iClus);
      if (colHit == 0) continue;

      for (int iClus_ = 0; iClus_ < pNtuCluster->GetClustersN(iLayer+1); iClus_++) {

         TAMSDcluster* rowHit = (TAMSDcluster*) pNtuCluster->GetCluster(iLayer+1,iClus);
         if (rowHit == 0) continue;
         
         TAMSDpoint* point = pNtuPoint->NewPoint(iLayer/2,
                                                 colHit->GetPosition().Y(), colHit->GetPosError().Y(), colHit,
                                                 rowHit->GetPosition().X(), rowHit->GetPosError().X(), rowHit);
       
         auto posz = (colHit->GetPositionG().Z() + rowHit->GetPositionG().Z())/2.;
         TVector3 pos(rowHit->GetPositionG().X(), colHit->GetPositionG().Y(), posz);
         point->SetPositionG(pos);
         point->SetValid();
      }
    }
  }
  return true;
}
