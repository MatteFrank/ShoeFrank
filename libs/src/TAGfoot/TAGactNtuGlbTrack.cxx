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
#include "TAGactTreeWriter.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TAVTntuCluster.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTtrack.hxx"
#include "TABMntuTrack.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"
#include "TAGntuPoint.hxx"

#include <utility>
#include <numeric>
#include <iostream>


#include "TAGactNtuGlbTrack.hxx"


#include "TATOEutilities.hxx"

#include "grkn_data.hpp"
#include "stepper.hpp"
#include "ukf.hpp"


/*!
 \class TAGactNtuGlbTrack TAGactNtuGlbTrack.hxx "TAGactNtuGlbTrack.hxx"
 \brief  Read back detector clusters to reconstruct global tracks**
 */

Bool_t  TAGactNtuGlbTrack::fgStdAloneFlag = false;

ClassImp(TAGactNtuGlbTrack)

//------------------------------------------+-----------------------------------
//! Default constructor.
TAGactNtuGlbTrack::TAGactNtuGlbTrack( const char* name,
                                      TAGdataDsc* p_vtxclus,
                                      TAGdataDsc* p_vtxtrack,
                                      TAGdataDsc* p_vtxvertex,
                                      TAGdataDsc* p_itrclus,
                                      TAGdataDsc* p_msdclus,
                                      TAGdataDsc* p_twpoint,
                                      TAGdataDsc* p_glbtrack,
                                      TAGparaDsc* p_geoG,
                                      TAGparaDsc* p_geodi,
                                      TAGparaDsc* p_geoVtx,
                                      TAGparaDsc* p_geoItr,
                                      TAGparaDsc* p_geoMsd,
                                      TAGparaDsc* p_geoTof,
                                      TADIgeoField* field)
 : TAGaction(name, "TAGactNtuGlbTrack - Global Tracker"),
   fpVtxClus(p_vtxclus),
   fpVtxTrack(p_vtxtrack),
   fpVtxVertex(p_vtxvertex),
   fpItrClus(p_itrclus),
   fpMsdClus(p_msdclus),
   fpTwPoint(p_twpoint),
   fpGlbTrack(p_glbtrack),
   fpGGeoMap(p_geoG),
   fpDiGeoMap(p_geodi),
   fpVtxGeoMap(p_geoVtx),
   fpItrGeoMap(p_geoItr),
   fpMsdGeoMap(p_geoMsd),
   fpTofGeoMap(p_geoTof),
   fField(field),
   fActEvtReader(nullptr),
   fActTOE( SetupAction() )
{
   AddDataOut(p_glbtrack, "TAGntuGlbTrack");
   
   if (GlobalPar::GetPar()->IncludeVT()) //should not be if
   {
      AddDataIn(p_vtxclus, "TAVTntuCluster");
      AddDataIn(p_vtxvertex, "TAVTntuVertex");
   }
   
   if (GlobalPar::GetPar()->IncludeIT())
      AddDataIn(p_itrclus, "TAITntuCluster");
   
   if (GlobalPar::GetPar()->IncludeMSD())
      AddDataIn(p_msdclus, "TAMSDntuCluster");
   
   if(GlobalPar::GetPar()->IncludeTW()) //neither
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
   
   if (GlobalPar::GetPar()->IncludeVT()) {
     fActEvtReader->SetupBranch(fpVtxTrack,  TAVTntuTrack::GetBranchName());
     fActEvtReader->SetupBranch(fpVtxClus,   TAVTntuCluster::GetBranchName());
     fActEvtReader->SetupBranch(fpVtxVertex, TAVTntuVertex::GetBranchName());
   }

   if (GlobalPar::GetPar()->IncludeIT())
      fActEvtReader->SetupBranch(fpItrClus,  TAITntuCluster::GetBranchName());
   
   if (GlobalPar::GetPar()->IncludeMSD())
      fActEvtReader->SetupBranch(fpMsdClus,  TAMSDntuCluster::GetBranchName());
   
   if(GlobalPar::GetPar()->IncludeTW())
      fActEvtReader->SetupBranch(fpTwPoint,  TATWntuPoint::GetBranchName());
}

//------------------------------------------+-----------------------------------
//! Setup action.
 TATOEbaseAct* TAGactNtuGlbTrack::SetupAction() const
{
    using state_vector =  matrix<4,1> ;
    using state_covariance =  matrix<4, 4> ;
    using state = state_impl< state_vector, state_covariance  >;
    
    auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( fpVtxClus->Object() );
    auto * vertex_hc = static_cast<TAVTntuVertex*>( fpVtxVertex->Object() );
    auto * geoVTX_h = static_cast<TAVTparGeo*>( fpVtxGeoMap->Object() );
    
    
    auto * clusterIT_hc = static_cast<TAITntuCluster*>( fpItrClus->Object() );
    auto * geoIT_h = static_cast<TAITparGeo*>( fpItrGeoMap->Object() );
    
    auto * clusterMSD_hc = static_cast<TAMSDntuCluster*>( fpMsdClus->Object() );
    auto * geoMSD_h = static_cast<TAMSDparGeo*>( fpMsdGeoMap->Object() );
    
    auto * clusterTW_hc = static_cast<TATWntuPoint*>( fpTwPoint->Object() );
    auto * geoTW_h = static_cast<TATWparGeo*>( fpTofGeoMap->Object() );
    
    
    auto list = start_list( detector_properties<details::vertex_tag>(vertex_hc, clusterVTX_hc,
                                                                     geoVTX_h, 15) )
                    .add( detector_properties<details::it_tag>(clusterIT_hc, geoIT_h, {33, 38}) )
                    .add( detector_properties<details::msd_tag>(clusterMSD_hc, geoMSD_h, {13, 18, 23}) )
                    .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h, 2.2) )
                    .finish();
    
    
    auto ode = make_ode< matrix<2,1>, 2>( model{ GetFootField() } );
    auto stepper = make_stepper<data_grkn56>( std::move(ode) );
    auto ukf = make_ukf<state>( std::move(stepper) );

    return make_new_TATOEactGlb(
                                std::move(ukf),
                                std::move(list),
                                static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                                static_cast<TAGparGeo*>( fpGGeoMap->Object() )
                               );
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
   //DeleteHistogram();
   fpHisMass = new TH1F("glbMass", "Mass Distribution", 200, 0, 16);
   
   AddHistogram(fpHisMass);
   
   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Register histograms stored in TAGntuGlbTrack.
void TAGactNtuGlbTrack::RegisterHistograms()
{
    fActTOE->RegisterHistograms();
    
    auto efficiency_histogram_ch = GetTrackContainer()->GetEfficiencyHistograms();
    for(auto * histogram_h : efficiency_histogram_ch){
        AddHistogram(histogram_h);
    }
    
    auto id_histogram_ch = GetTrackContainer()->GetIdentificationHistograms();
    for(auto * histogram_h : id_histogram_ch){
        AddHistogram(histogram_h);
    }
    
    SetValidHistogram(kTRUE);
    
    auto writer_h = static_cast<TAGactTreeWriter*>( gTAGroot->FindAction("locRecFile") );
    SetHistogramDir( (TDirectory*)writer_h->File() );
}

//------------------------------------------+-----------------------------------
void TAGactNtuGlbTrack::WriteHistogram()
{
    Output();
    RegisterHistograms();
    TAGaction::WriteHistogram();
}



//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAGactNtuGlbTrack::Action()
{

  if (fgStdAloneFlag)
    fActEvtReader->Process();
  
  
  //    auto* pNtuTrack = static_cast<TAGntuGlbTrack*>(fpGlbTrack->Object() );
  
  fpGlbTrack->Clear();
  
  fActTOE->Action();
  
  fpGlbTrack->SetBit(kValid);
  
  return kTRUE;
}



