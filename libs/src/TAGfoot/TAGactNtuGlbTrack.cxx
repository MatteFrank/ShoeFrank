/*!
 \file
 \version $Id: TAGactNtuGlbTrack.cxx
 \brief   Implementation of TAGactNtuGlbTrack.
 */

#include "TH1F.h"
#include "TTree.h"
#include "TVector3.h"

#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TADIparGeo.hxx"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGactTreeReader.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TAVTntuVertex.hxx"
#include "TAVTtrack.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"
#include "TAGntuPoint.hxx"

#include <utility>
#include <numeric>
#include <iostream>


#include "TAGactNtuGlbTrack.hxx"


/*!
 \class TAGactNtuGlbTrack TAGactNtuGlbTrack.hxx "TAGactNtuGlbTrack.hxx"
 \brief  Read back detector clusters to reconstruct global tracks**
 */

Bool_t  TAGactNtuGlbTrack::fgStdAloneFlag = false;

ClassImp(TAGactNtuGlbTrack)

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGactNtuGlbTrack::TAGactNtuGlbTrack(const char* name, TAGdataDsc* p_vtxvertex, TAGdataDsc* p_itrclus, TAGdataDsc* p_msdclus,
                                     TAGdataDsc* p_twpoint, TAGdataDsc* p_glbtrack, TAGparaDsc* p_geodi, TAGparaDsc* p_geoVtx,
                                     TAGparaDsc* p_geoItr,  TAGparaDsc* p_geoMsd, TAGparaDsc* p_geoTof)
 : TAGaction(name, "TAGactNtuGlbTrack - Global Tracker"),
   fpVtxVertex(p_vtxvertex),
   fpItrClus(p_itrclus),
   fpMsdClus(p_msdclus),
   fpTwPoint(p_twpoint),
   fpGlbTrack(p_glbtrack),
   fpDiGeoMap(p_geodi),
   fpVtxGeoMap(p_geoVtx),
   fpItrGeoMap(p_geoItr),
   fpMsdGeoMap(p_geoMsd),
   fpTofGeoMap(p_geoTof),
   fpNtuPoint(new TAGntuPoint()),
   fActEvtReader(0x0)
{
   AddDataOut(p_glbtrack, "TAGntuGlbTrack");
   if (GlobalPar::GetPar()->IncludeVertex())
      AddDataIn(p_vtxvertex, "TAVTntuVertex");
   
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      AddDataIn(p_itrclus, "TAITntuCluster");
   
   if (GlobalPar::GetPar()->IncludeMSD())
      AddDataIn(p_msdclus, "TAMSDntuCluster");
   
   if(GlobalPar::GetPar()->IncludeTW())
      AddDataIn(p_twpoint, "TATWntuPoint");
   
   if (fgStdAloneFlag)
      SetupBranches();
   
   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuGlbTrack::~TAGactNtuGlbTrack()
{
    delete fActTOE;
}

//__________________________________________________________
// ! Get Tree
TTree* TAGactNtuGlbTrack::GetTree()
{
   if (fgStdAloneFlag)
      return fActEvtReader->GetTree();
   else
      return 0x0;
}

//------------------------------------------+-----------------------------------
//! Setup all branches.
void TAGactNtuGlbTrack::SetupBranches()
{
   fActEvtReader = new TAGactTreeReader("evtReader");
   
   if (GlobalPar::GetPar()->IncludeVertex())
      fActEvtReader->SetupBranch(fpVtxVertex, TAVTntuVertex::GetBranchName());
   
   if (GlobalPar::GetPar()->IncludeInnerTracker())
      fActEvtReader->SetupBranch(fpItrClus,   TAITntuCluster::GetBranchName());
   
   if (GlobalPar::GetPar()->IncludeMSD())
     fActEvtReader->SetupBranch(fpMsdClus,   TAMSDntuCluster::GetBranchName());
   
   if(GlobalPar::GetPar()->IncludeTW())
      fActEvtReader->SetupBranch(fpTwPoint,   TATWntuPoint::GetBranchName());
}

//__________________________________________________________
// ! Open file
void TAGactNtuGlbTrack::Open(TString name)
{
   if (fgStdAloneFlag)
      fActEvtReader->Open(name.Data());
   else
      Error("OpenFile", "Not in stand alone mode");
}

//__________________________________________________________
//! Close file
void TAGactNtuGlbTrack::Close()
{
   fActEvtReader->Close();
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAGactNtuGlbTrack::CreateHistogram()
{
   DeleteHistogram();
   fpHisMass = new TH1F("glbMass", "Mass Distribution", 200, 0, 16);
   
   AddHistogram(fpHisMass);
   
   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAGactNtuGlbTrack::Action()
{
   if (fgStdAloneFlag)
      fActEvtReader->Process();
   
   if(GlobalPar::GetPar()->IncludeVertex())
      FillVtxPoint();
   
   if(GlobalPar::GetPar()->IncludeInnerTracker())
      FillItrPoint();

   if(GlobalPar::GetPar()->IncludeMSD())
      FillMsdPoint();
   
   if(GlobalPar::GetPar()->IncludeTW())
      FillTofPoint();
   
   fpGlbTrack->SetBit(kValid);
   
    std::cout << "TAGactNtuGlbTrack::Action\n";
    fActTOE->Action();
    
    
   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Fill Vtx point.
void TAGactNtuGlbTrack::FillVtxPoint()
{
   Double_t time   = 0.;

   TAVTntuVertex* pNtuVtx  = (TAVTntuVertex*) fpVtxVertex->Object();
   for (Int_t i = 0; i < pNtuVtx->GetVertexN(); ++i) {
      TAVTvertex* vtx = pNtuVtx->GetVertex(i);
      
      if (!vtx->GetVertexValidity()) continue;
      if (!vtx->IsBmMatched()) continue;
      
      for (Int_t j = 0; j < vtx->GetTracksN(); ++j) {
         TAVTtrack* track = vtx->GetTrack(j);
         Double_t charge  = track->GetChargeWithMaxProba();
         Float_t proba    = track->GetChargeMaxProba();
         
         for (Int_t k = 0; k < track->GetClustersN(); ++k) {
            TAVTcluster* clus = (TAVTcluster*)track->GetCluster(k);
            
            TVector3 pos      = clus->GetPositionG();
            TVector3 posG     = fpFootGeo->FromVTLocalToGlobal(pos);
            TVector3 posErr   = clus->GetPosError();
            TVector3 posErrG  = fpFootGeo->FromVTLocalToGlobal(posErr);

            fpNtuPoint->NewPoint(posG, posErrG, time, charge, proba);
         }
      }
   }
}

//------------------------------------------+-----------------------------------
//! Fill Itr point.
void TAGactNtuGlbTrack::FillItrPoint()
{
   Double_t time    = 0.;
   Double_t charge  = 0.;
   Float_t proba    = 0.;
   
   TAITntuCluster* pNtuClus = (TAITntuCluster*) fpItrClus->Object();
   TAITparGeo*     pParGeo  = (TAITparGeo*)     fpItrGeoMap->Object();
   
   for (Int_t i = 0; i < pParGeo->GetSensorsN(); ++i) {
      
      for (Int_t k = 0; k < pNtuClus->GetClustersN(i); ++k) {
         TAITcluster* clus = (TAITcluster*)pNtuClus->GetCluster(i, k);
         
         TVector3 pos      = clus->GetPositionG();
         TVector3 posG     = fpFootGeo->FromITLocalToGlobal(pos);
         TVector3 posErr   = clus->GetPosError();
         TVector3 posErrG  = fpFootGeo->FromITLocalToGlobal(posErr);

         fpNtuPoint->NewPoint(posG, posErrG, time, charge, proba);
      }
   }
}

//------------------------------------------+-----------------------------------
//! Fill Msd point.
void TAGactNtuGlbTrack::FillMsdPoint()
{
   Double_t time    = 0.;
   Double_t charge  = 0.;
   Float_t proba    = 0.;
   
   TAMSDntuCluster* pNtuClus = (TAMSDntuCluster*) fpMsdClus->Object();
   TAMSDparGeo*     pParGeo  = (TAMSDparGeo*)     fpMsdGeoMap->Object();

   for (Int_t i = 0; i < pParGeo->GetSensorsN(); ++i) {
      
      for (Int_t k = 0; k < pNtuClus->GetClustersN(i); ++k) {
         TAMSDcluster* clus = (TAMSDcluster*)pNtuClus->GetCluster(i, k);
         
         TVector3 pos       = clus->GetPositionG();
         TVector3 posG      = fpFootGeo->FromMSDLocalToGlobal(pos);
         Float_t err        = clus->GetPosError();
         TVector3 posErr(err, err, 0); // temporary
         TVector3 posErrG  = fpFootGeo->FromMSDLocalToGlobal(posErr);

         fpNtuPoint->NewPoint(posG, posErrG, time, charge, proba);
      }
   }
}

//------------------------------------------+-----------------------------------
//! Fill Tw point.
void TAGactNtuGlbTrack::FillTofPoint()
{
   TATWntuPoint* pNtuPoint  = (TATWntuPoint*) fpTwPoint->Object();
   for (Int_t i = 0; i < pNtuPoint->GetPointN(); ++i) {
      TATWpoint* point = pNtuPoint->GetPoint(i);

      TVector3 pos    = point->GetPosition();
      Double_t time   = point->GetTime();
      Double_t charge = point->GetChargeZ();
      Double_t proba  = point->GetChargeZProba();
      TVector3 posG   = fpFootGeo->FromTWLocalToGlobal(pos);
      TVector3 posErrG(0,0,0); // for the moment
      
      fpNtuPoint->NewPoint(posG, posErrG, time, charge, proba);
   }
}


