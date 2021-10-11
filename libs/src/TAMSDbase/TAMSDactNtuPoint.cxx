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
  : TAGaction(name, "TAMSDactNtuPoint - NTuplize points"),
    fpNtuCluster(pNtuCluster),
    fpNtuPoint(pNtuPoint),
    fpGeoMap(pGeoMap)
{
  AddDataIn(pNtuCluster, "TAMSDntuCluster");
  AddDataOut(pNtuPoint,  "TAMSDntuPoint");
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

   TString prefix = "ms";
   TString titleDev = "Micro Strip Detector";
      
   TAMSDparGeo* pGeoMap  = (TAMSDparGeo*) fpGeoMap->Object();
   
   for (Int_t i = 0; i < pGeoMap->GetStationsN(); ++i) {
      fpHisPointMap[i] = new TH2F(Form("%sPointMap%d", prefix.Data(), i+1), Form("%s - point map for sensor %d", titleDev.Data(), i+1),
                                  pGeoMap->GetStripsN(), -pGeoMap->GetPitch()*pGeoMap->GetStripsN()/2., pGeoMap->GetPitch()*pGeoMap->GetStripsN()/2.,
                                  pGeoMap->GetStripsN(), -pGeoMap->GetPitch()*pGeoMap->GetStripsN()/2., pGeoMap->GetPitch()*pGeoMap->GetStripsN()/2.);
      
      AddHistogram(fpHisPointMap[i]);
   }
   
   for (Int_t i = 0; i < pGeoMap->GetStationsN(); ++i) {
      fpHisPointCharge[i] = new TH1F(Form("%sPointCharge%d",prefix.Data(), i+1), Form("%s - point charge for sensor %d", titleDev.Data(), i+1), 500, 0., 10000);
      AddHistogram(fpHisPointCharge[i]);
   }
   
   fpHisPointChargeTot = new TH1F(Form("%sPointChargeTot",prefix.Data()), Form("%s - total point charge", titleDev.Data()), 500, 0., 10000);
   AddHistogram(fpHisPointChargeTot);

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
       
         auto posx = 0.;
         auto posy = 0.;
         auto posz = (colHit->GetPositionG().Z() + rowHit->GetPositionG().Z())/2.;

         if (pGeoMap->GetSensorPar(iLayer).TypeIdx == 1) {
            posx = rowHit->GetPositionG().X();
            posy = colHit->GetPositionG().Y();
         } else {
            posx = colHit->GetPositionG().X();
            posy = rowHit->GetPositionG().Y();
         }
         TVector3 pos(posx, posy, posz);
         
         
         point->SetPositionG(pos);
         point->SetValid();
         point->SetSensorIdx(iLayer);
         if (ValidHistogram()) {
            fpHisPointMap[iLayer/2]->Fill(pos[0], pos[1]);
            fpHisPointCharge[iLayer/2]->Fill(point->GetEnergyLoss());
            fpHisPointChargeTot->Fill(point->GetEnergyLoss());
         }
      }
    }
  }
  return true;
}
