/*!
 \file TAGactNtuGlbTrack.cxx
 \brief   Implementation of TAGactNtuGlbTrack.
 */

#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"
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
#include "flag_set.hpp"

//! Class Imp
ClassImp(TAGactNtuGlbTrack)

namespace details{
struct ms2d_tag; ///< MSD 2D flag
}

/*!
 \class TAGactNtuGlbTrack
 \brief  Read back detector clusters to reconstruct global tracks with TOE**
 */

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] p_vtxclus  VTX cluster container descriptor
//! \param[in] p_vtxtrack  VTX track container descriptor
//! \param[in] p_vtxvertex  vertex container descriptor
//! \param[in] p_itrclus ITR cluster container descriptor
//! \param[in] p_msdclus MSD cluster container descriptor
//! \param[in] p_msdpoint MSD point container descriptor
//! \param[in] p_twpoint TW point container descriptor
//! \param[out] p_glbtrack global track container descriptor
//! \param[in] p_geoG target geometry parameter descriptor
//! \param[in] p_geodi dipole geometry parameter descriptor
//! \param[in] p_geoVtx VTX geometry parameter descriptor
//! \param[in] p_geoItr ITR geometry parameter descriptor
//! \param[in] p_geoMsd MSD geometry parameter descriptor
//! \param[in] p_geoTof TW geometry parameter descriptor
//! \param[in] field field map
TAGactNtuGlbTrack::TAGactNtuGlbTrack( const char* name,
                                      TAGdataDsc* p_vtxclus,
                                      TAGdataDsc* p_vtxtrack,
                                      TAGdataDsc* p_vtxvertex,
                                      TAGdataDsc* p_itrclus,
                                      TAGdataDsc* p_msdclus,
                                      TAGdataDsc* p_msdpoint,
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
   fpMsdPoint(p_msdpoint),
   fpTwPoint(p_twpoint),
   fpGlbTrack(p_glbtrack),
   fpGGeoMap(p_geoG),
   fpDiGeoMap(p_geodi),
   fpVtxGeoMap(p_geoVtx),
   fpItrGeoMap(p_geoItr),
   fpMsdGeoMap(p_geoMsd),
   fpTofGeoMap(p_geoTof),
   fField(field),
   fActEvtReader(nullptr)
{
   AddDataOut(p_glbtrack, "TAGntuGlbTrack");
   
   // VT mandatory
    if (TAGrecoManager::GetPar()->IncludeVT()) {
        AddDataIn(p_vtxclus, "TAVTntuCluster");
        AddDataIn(p_vtxvertex, "TAVTntuVertex");
    }
   
   if (TAGrecoManager::GetPar()->IncludeIT())
      AddDataIn(p_itrclus, "TAITntuCluster");
   
   if (TAGrecoManager::GetPar()->IncludeMSD())
      AddDataIn(p_msdclus, "TAMSDntuCluster");
   
   // TW mandatory

   AddDataIn(p_twpoint, "TATWntuPoint");
   
   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   
   TAGparGeo* pGGeoMap = static_cast<TAGparGeo*>( fpGGeoMap->Object());
   
   
   TString tgtName = pGGeoMap->GetTargetPar().Material;//"C";
   fCutVtx  = TAGrecoManager::GetPar()->GetVtxTagCuts(tgtName.Data());
   fCutItr  = TAGrecoManager::GetPar()->GetItrTagCuts(tgtName.Data());
   fCutMsd  = TAGrecoManager::GetPar()->GetMsdTagCuts(tgtName.Data());
   fCutMsd2 = TAGrecoManager::GetPar()->GetMsd2TagCuts(tgtName.Data());
   fCutTw   = TAGrecoManager::GetPar()->GetTwTagCuts(tgtName.Data());
   
   fActTOE = SetupAction();

}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGactNtuGlbTrack::~TAGactNtuGlbTrack()
{
    delete fActTOE;
}

//------------------------------------------+-----------------------------------
//! Setup action.
 TATOEbaseAct* TAGactNtuGlbTrack::SetupAction() const
{
    using state_vector     =  matrix<4,1> ;
    using state_covariance =  matrix<4, 4> ;
    using state            = state_impl< state_vector, state_covariance  >;
    
    auto * clusterTW_hc    = static_cast<TATWntuPoint*>( fpTwPoint->Object() );
    auto * geoTW_h         = static_cast<TATWparGeo*>( fpTofGeoMap->Object() );
    
    auto ode               = make_ode< matrix<2,1>, 2>( model{ GetFootField() } );
    auto stepper           = make_stepper<data_grkn56>( std::move(ode) );
    auto ukf               = make_ukf<state>( std::move(stepper) );
    
    uint8_t opcode{ flag_set<details::tof_tag>{} };
    if( fpVtxClus  && fpVtxVertex && fpVtxGeoMap ){ opcode |= flag_set<details::vertex_tag>{};  }
    if( fpItrClus  && fpItrGeoMap )               { opcode |= flag_set<details::it_tag>{}; }
    if( fpMsdClus  && fpMsdGeoMap )               { opcode |= flag_set<details::msd_tag>{}; }
    if( fpMsdPoint && fpMsdGeoMap && !fpVtxClus ) { opcode &= ~flag_set<details::msd_tag>{}; opcode |= flag_set<details::ms2d_tag>{}; }
    
#include<bitset>

    switch( opcode ){
        case flag_set<details::vertex_tag, details::it_tag, details::tof_tag>{}: {
            auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( fpVtxClus->Object() );
            auto * vertex_hc     = static_cast<TAVTntuVertex*>( fpVtxVertex->Object() );
            auto * geoVTX_h      = static_cast<TAVTparGeo*>( fpVtxGeoMap->Object() );
            
            auto * clusterIT_hc  = static_cast<TAITntuCluster*>( fpItrClus->Object() );
            auto * geoIT_h       = static_cast<TAITparGeo*>( fpItrGeoMap->Object() );
            
            auto list = start_list( detector_properties<details::vertex_tag>(vertex_hc, clusterVTX_hc,
                                                                             geoVTX_h) )
                            .add( detector_properties<details::it_tag>(clusterIT_hc, geoIT_h) )
                            .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h) )
                            .finish();

            auto* action_h =  new_TATOEactGlb(std::move(ukf),
                                              std::move(list),
                                              static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                                              static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                                              GetFootField(), true
                                              );
           
           if (fCutVtx.size() != 0) {
              details::vertex_tag vtx_tag;
              action_h->set_cuts(vtx_tag, fCutVtx);
           }
           
           if (fCutItr.size() != 0) {
              details::it_tag it_tag;
              action_h->set_cuts(it_tag, fCutItr);
           }
           
           if (fCutTw.size() != 0) {
              details::tof_tag tof_tag;
              action_h->set_cuts(tof_tag, fCutTw);
           }


            auto& computation_checker_c = action_h->get_computation_checker();
            using namespace checker;
            computation_checker_c.push_back( TATOEchecker< efficiency<computation >,
                                                           purity<computation>,
                                                           reconstructed_distribution<computation>,
                                                           fake_distribution<computation>,
                                                           clone_distribution<computation>,
                                                           mass_identification<computation> ,
                                                           momentum_resolution<computation>,
                                                           beta_ratio<computation> >{} );
            
            return action_h;
        }
          
        case flag_set<details::vertex_tag, details::msd_tag, details::tof_tag>{}: {
            auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( fpVtxClus->Object() );
            auto * vertex_hc = static_cast<TAVTntuVertex*>( fpVtxVertex->Object() );
            auto * geoVTX_h = static_cast<TAVTparGeo*>( fpVtxGeoMap->Object() );
            
            auto * clusterMSD_hc = static_cast<TAMSDntuCluster*>( fpMsdClus->Object() );
            auto * geoMSD_h = static_cast<TAMSDparGeo*>( fpMsdGeoMap->Object() );
            
            auto list = start_list( detector_properties<details::vertex_tag>(vertex_hc, clusterVTX_hc,
                                                                             geoVTX_h) )
                            .add( detector_properties<details::msd_tag>(clusterMSD_hc, geoMSD_h) )
                            .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h) )
                            .finish();

            auto* action_h =  new_TATOEactGlb(std::move(ukf),
                                              std::move(list),
                                              static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                                              static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                                              GetFootField(), true
                                              );
            if (fCutVtx.size() != 0) {
              details::vertex_tag vtx_tag;
              action_h->set_cuts(vtx_tag, fCutVtx);
            }
           
            if (fCutMsd.size() != 0) {
              details::msd_tag msd_tag;
              action_h->set_cuts(msd_tag, fCutMsd);
            }
           
            if (fCutTw.size() != 0) {
              details::tof_tag tof_tag;
              action_h->set_cuts(tof_tag, fCutTw);
            }

            auto& computation_checker_c = action_h->get_computation_checker();
            using namespace checker;
            computation_checker_c.push_back( TATOEchecker< efficiency<computation >,
                                                           purity<computation>,
                                                           reconstructed_distribution<computation>,
                                                           fake_distribution<computation>,
                                                           clone_distribution<computation>,
                                                           mass_identification<computation> ,
                                                           momentum_resolution<computation>,
                                                           beta_ratio<computation> >{} );
            
            return action_h;
        }
          
        case flag_set<details::vertex_tag, details::it_tag, details::msd_tag, details::tof_tag>{}: {
            auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( fpVtxClus->Object() );
            auto * vertex_hc     = static_cast<TAVTntuVertex*>( fpVtxVertex->Object() );
            auto * geoVTX_h      = static_cast<TAVTparGeo*>( fpVtxGeoMap->Object() );
            
            auto * clusterIT_hc  = static_cast<TAITntuCluster*>( fpItrClus->Object() );
            auto * geoIT_h       = static_cast<TAITparGeo*>( fpItrGeoMap->Object() );
            
            auto * clusterMSD_hc = static_cast<TAMSDntuCluster*>( fpMsdClus->Object() );
            auto * geoMSD_h      = static_cast<TAMSDparGeo*>( fpMsdGeoMap->Object() );
            
            auto list = start_list( detector_properties<details::vertex_tag>(vertex_hc, clusterVTX_hc,
                                                                             geoVTX_h) )
                            .add( detector_properties<details::it_tag>(clusterIT_hc, geoIT_h) )
                            .add( detector_properties<details::msd_tag>(clusterMSD_hc, geoMSD_h) )
                            .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h) )
                            .finish();

           auto* action_h =  new_TATOEactGlb(std::move(ukf),
                                             std::move(list),
                                             static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                                             static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                                             GetFootField(), true
                                             );
           
           if (fCutVtx.size() != 0) {
              details::vertex_tag vtx_tag;
              action_h->set_cuts(vtx_tag, fCutVtx);
           }
           
           if (fCutItr.size() != 0) {
              details::it_tag it_tag;
              action_h->set_cuts(it_tag, fCutItr);
           }
           
           if (fCutMsd.size() != 0) {
              details::msd_tag msd_tag;
              action_h->set_cuts(msd_tag, fCutMsd);
           }
           
           if (fCutTw.size() != 0) {
              details::tof_tag tof_tag;
              action_h->set_cuts(tof_tag, fCutTw);
           }
           
            auto& computation_checker_c = action_h->get_computation_checker();
            using namespace checker;
            computation_checker_c.push_back( TATOEchecker< efficiency<computation >,
                                                           purity<computation>,
                                                           reconstructed_distribution<computation>,
                                                           fake_distribution<computation>,
                                                           clone_distribution<computation>,
                                                           mass_identification<computation> ,
                                                           momentum_resolution<computation>,
                                                           beta_ratio<computation> >{} );
            return action_h;
        }
          
        case flag_set<details::ms2d_tag, details::tof_tag>{}: {
            auto * pointMSD_hc = static_cast<TAMSDntuPoint*>( fpMsdPoint->Object() );
            auto * geoMSD_h    = static_cast<TAMSDparGeo*>( fpMsdGeoMap->Object() );
            
            auto list = start_list( detector_properties<details::ms2d_tag>(pointMSD_hc, geoMSD_h) )
                            .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h) )
                            .finish();

            auto* action_h =  new_TATOEactGlb(std::move(ukf),
                                              std::move(list),
                                              static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                                              static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                                              GetFootField(), true
                                              );
           
           
           if (fCutMsd2.size() != 0) {
              details::ms2d_tag msd_tag;
              action_h->set_cuts(msd_tag, fCutMsd2);
           }
           
           if (fCutTw.size() != 0) {
              details::tof_tag tof_tag;
              action_h->set_cuts(tof_tag, fCutTw);
           }

            auto& computation_checker_c = action_h->get_computation_checker();
            using namespace checker;
            computation_checker_c.push_back( TATOEchecker< efficiency<computation >,
                                                           purity<computation>,
                                                           reconstructed_distribution<computation>,
                                                           fake_distribution<computation>,
                                                           clone_distribution<computation>,
                                                           mass_identification<computation> ,
                                                           momentum_resolution<computation>,
                                                           beta_ratio<computation> >{} );
            
            return action_h;
        }
    }
    
    auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( fpVtxClus->Object() );
    auto * vertex_hc     = static_cast<TAVTntuVertex*>( fpVtxVertex->Object() );
    auto * geoVTX_h      = static_cast<TAVTparGeo*>( fpVtxGeoMap->Object() );
    
    auto list = start_list( detector_properties<details::vertex_tag>(vertex_hc, clusterVTX_hc,
                                                                     geoVTX_h) )
                    .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h) )
                    .finish();
    
    
    auto* action_h = new_TATOEactGlb(std::move(ukf),
                                     std::move(list),
                                     static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                                     static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                                     GetFootField(), true
                                     );
   
    if (fCutVtx.size() != 0) {
       details::vertex_tag vtx_tag;
       action_h->set_cuts(vtx_tag, fCutVtx);
    }
   
    auto& computation_checker_c = action_h->get_computation_checker();
    using namespace checker;
    computation_checker_c.push_back( TATOEchecker< efficiency<computation >,
                                                   purity<computation>,
                                                   reconstructed_distribution<computation>,
                                                   fake_distribution<computation>,
                                                   clone_distribution<computation>,
                                                   mass_identification<computation> ,
                                                   momentum_resolution<computation>,
                                                   beta_ratio<computation> >{} );
    return action_h;
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
//! Write histograms
void TAGactNtuGlbTrack::WriteHistogram()
{
    Output();
//    RegisterHistograms();
    TAGaction::WriteHistogram();
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAGactNtuGlbTrack::Action()
{
  fpGlbTrack->Clear();

  fActTOE->Action();
    
  
  fpGlbTrack->SetBit(kValid);
  
  return kTRUE;
}



