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
#include "TAGrecoManager.hxx"

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
   fpSens[0] = new TH2F("msdSens1_xy", "MSD - X - Y Dist", 500, -5., 5., 500, -5., 5.);
   AddHistogram(fpSens[0]);

   fpSens[1] = new TH2F("msdSens2_xy", "MSD - X - Y Dist", 500, -5., 5., 500, -5., 5.);
   AddHistogram(fpSens[1]);

   fpSens[2] = new TH2F("msdSens3_xy", "MSD - X - Y Dist", 500, -5., 5., 500, -5., 5.);
   AddHistogram(fpSens[2]);

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
  if(FootDebugLevel(2)) {
    cout<<"****************************"<<endl;
    cout<<"  NtuPoint hits "<<endl;
    cout<<"****************************"<<endl;
  }
  
  int plane(0);
  for ( int iLayer = 0; iLayer< pGeoMap->GetSensorsN(); iLayer+=2 ){

    // fill points
    
    for (int iStrip = 0; iStrip < pNtuCluster->GetClustersN(iLayer); iStrip++) {

      TAMSDcluster* colHit = (TAMSDcluster*) pNtuCluster->GetCluster(iLayer,iStrip);
      if (colHit == 0) continue;
      if (colHit->GetPlaneView() == 0 ) 	xyOrder = true;
      else xyOrder = false;

      for (int iStrip_ = 0; iStrip_ < pNtuCluster->GetClustersN(iLayer+1); iStrip_++) {

         TAMSDcluster* rowHit = (TAMSDcluster*) pNtuCluster->GetCluster(iLayer+1,iStrip);

         if (rowHit == 0) continue;

         if ( !(rowHit->GetPlaneView() == 1 && xyOrder) ) 	cout << "ERROR on TAMSDactNtuPoint" << endl;
         
         TVector3 localPointPosition;
         localPointPosition.SetXYZ(colHit->GetPositionG().X(), rowHit->GetPositionG().Y(), pGeoMap->GetSensorPosition(iLayer).Z());

         TAMSDpoint* point = pNtuPoint->NewPoint( iLayer/2, colHit->GetPositionG().X(), rowHit->GetPositionG().Y(),
                                                 localPointPosition );
         TVector3 posErr(colHit->GetPosError().X(), rowHit->GetPosError().Y(), 0.01);
         point->SetPosErrorG(posErr);
         
         // tmp solution, considered only one particle
         if (colHit->GetMcTracksN())
            point->AddMcTrackIdx(colHit->GetMcTrackIdx(0));
         if (rowHit->GetMcTracksN())
            point->AddMcTrackIdx(rowHit->GetMcTrackIdx(0));
      }
    } 
    plane++;
  }
  return true;
}
