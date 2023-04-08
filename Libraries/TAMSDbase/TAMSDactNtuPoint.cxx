/*!
 \file TAMSDactNtuPoint.cxx
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
#include "TAGrecoManager.hxx"

/*!
 \class TAMSDactNtuPoint
 \brief NTuplizer for micro strip point
 */

//! Class imp
ClassImp(TAMSDactNtuPoint);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuCluster cluster input container descriptor
//! \param[out] pNtuPoint point output container descriptor
//! \param[in] pGeoMap geometry parameter descriptor
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
//! Action
Bool_t TAMSDactNtuPoint::Action()
{
   Bool_t ok = FindPoints();

   fpNtuPoint->SetBit(kValid);

   return true;
}

//______________________________________________________________________________
//! Find points
Bool_t TAMSDactNtuPoint::FindPoints()
{
  TAMSDntuCluster* pNtuCluster  = (TAMSDntuCluster*) fpNtuCluster->Object();
  TAMSDntuPoint* pNtuPoint      = (TAMSDntuPoint*) fpNtuPoint->Object();
  TAMSDparGeo* pGeoMap          = (TAMSDparGeo*) fpGeoMap->Object();
   
  if(FootDebugLevel(1)) {  
    cout<<"****************************"<<endl;
    cout<<"  NtuPoint hits "<<endl;
    cout<<"****************************"<<endl;
  }
  
  int plane(0);
  TAMSDcluster* rowHit;
  TAMSDcluster* colHit;

  for ( int iLayer = 0; iLayer< pGeoMap->GetSensorsN(); iLayer+=2 ){
   rowHit = nullptr;
   colHit = nullptr;
    // fill points
    for (int iClus = 0; iClus < pNtuCluster->GetClustersN(iLayer); iClus++) {

      if( ((TAMSDcluster*) pNtuCluster->GetCluster(iLayer,iClus))->GetPlaneView() == 0) //Plane X -> colHit
         colHit = (TAMSDcluster*) pNtuCluster->GetCluster(iLayer,iClus);
      else
         rowHit = (TAMSDcluster*) pNtuCluster->GetCluster(iLayer,iClus);

      if (colHit == 0 && rowHit == 0) continue;

      for (int iClus2 = 0; iClus2 < pNtuCluster->GetClustersN(iLayer+1); iClus2++) {

         if( ((TAMSDcluster*) pNtuCluster->GetCluster(iLayer+1,iClus2))->GetPlaneView() == 0) //Plane X -> colHit
            colHit = (TAMSDcluster*) pNtuCluster->GetCluster(iLayer+1,iClus2);
         else
            rowHit = (TAMSDcluster*) pNtuCluster->GetCluster(iLayer+1,iClus2);
         
         if (rowHit == 0 || colHit == 0)
         {
            Warning("FindPoints()", "Found two consecutive MSD layers w/ the same plane view!");
            continue;
         }
         
         TAMSDpoint* point = pNtuPoint->NewPoint(iLayer/2, colHit, rowHit);
       
         auto posx =  rowHit->GetPositionG().X() + colHit->GetPositionG().X();
         auto posy =  rowHit->GetPositionG().Y() + colHit->GetPositionG().Y();
         auto posz = (colHit->GetPositionG().Z() + rowHit->GetPositionG().Z())/2.;
         
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
    plane++;
  }
   
  return true;
}
