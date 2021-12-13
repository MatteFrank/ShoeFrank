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

namespace details{
struct ms2d_tag;
}

/*!
 \class TAGactNtuGlbTrack
 \brief  Read back detector clusters to reconstruct global tracks with TOE**
 */

ClassImp(TAGactNtuGlbTrack)

//------------------------------------------+-----------------------------------
//! Default constructor.
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
   fActEvtReader(nullptr),
   fActTOE( SetupAction() )
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
    using state_vector =  matrix<4,1> ;
    using state_covariance =  matrix<4, 4> ;
    using state = state_impl< state_vector, state_covariance  >;
    
    auto * clusterTW_hc = static_cast<TATWntuPoint*>( fpTwPoint->Object() );
    auto * geoTW_h = static_cast<TATWparGeo*>( fpTofGeoMap->Object() );
    
    auto ode = make_ode< matrix<2,1>, 2>( model{ GetFootField() } );
    auto stepper = make_stepper<data_grkn56>( std::move(ode) );
    auto ukf = make_ukf<state>( std::move(stepper) );
    
    uint8_t opcode{ flag_set<details::tof_tag>{} };
    if( fpVtxClus && fpVtxVertex && fpVtxGeoMap ){ opcode |= flag_set<details::vertex_tag>{};  }
    if( fpItrClus && fpItrGeoMap ){ opcode |= flag_set<details::it_tag>{}; }
    if( fpMsdClus && fpMsdGeoMap ){ opcode |= flag_set<details::msd_tag>{}; }
    if( fpMsdPoint && fpMsdGeoMap && !fpVtxClus ){ opcode &= ~flag_set<details::msd_tag>{}; opcode |= flag_set<details::ms2d_tag>{}; }
    
#include<bitset>
    std::cout << "opcode: " << std::bitset<8>(opcode) << '\n';
    switch( opcode ){
        case flag_set<details::vertex_tag, details::it_tag, details::tof_tag>{}: {
            auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( fpVtxClus->Object() );
            auto * vertex_hc = static_cast<TAVTntuVertex*>( fpVtxVertex->Object() );
            auto * geoVTX_h = static_cast<TAVTparGeo*>( fpVtxGeoMap->Object() );
            
            auto * clusterIT_hc = static_cast<TAITntuCluster*>( fpItrClus->Object() );
            auto * geoIT_h = static_cast<TAITparGeo*>( fpItrGeoMap->Object() );
            
            auto list = start_list( detector_properties<details::vertex_tag>(vertex_hc, clusterVTX_hc,
                                                                             geoVTX_h) )
                            .add( detector_properties<details::it_tag>(clusterIT_hc, geoIT_h) )
                            .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h) )
                            .finish();

            auto* action_h =  new_TATOEactGlb(
                        std::move(ukf),
                        std::move(list),
                        static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                        static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                        GetFootField(), true
                                       );
            
            auto& computation_checker_c = action_h->get_computation_checker();
            using namespace checker;
            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation>>{} );
            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation>>{} );
            computation_checker_c.push_back( TATOEchecker< purity<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< fake_distribution<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< clone_distribution<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< mass_identification<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< momentum_difference<computation> >{} );
            
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

            auto* action_h =  new_TATOEactGlb(
                        std::move(ukf),
                        std::move(list),
                        static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                        static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                        GetFootField(), true
                                       );
            
            auto& computation_checker_c = action_h->get_computation_checker();
            using namespace checker;
            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation>>{} );
            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation>>{} );
            computation_checker_c.push_back( TATOEchecker< purity<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< fake_distribution<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< clone_distribution<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< mass_identification<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< momentum_difference<computation> >{} );
            
            return action_h;
        }
        case flag_set<details::vertex_tag, details::it_tag, details::msd_tag, details::tof_tag>{}: {
            auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( fpVtxClus->Object() );
            auto * vertex_hc = static_cast<TAVTntuVertex*>( fpVtxVertex->Object() );
            auto * geoVTX_h = static_cast<TAVTparGeo*>( fpVtxGeoMap->Object() );
            
            auto * clusterIT_hc = static_cast<TAITntuCluster*>( fpItrClus->Object() );
            auto * geoIT_h = static_cast<TAITparGeo*>( fpItrGeoMap->Object() );
            
            auto * clusterMSD_hc = static_cast<TAMSDntuCluster*>( fpMsdClus->Object() );
            auto * geoMSD_h = static_cast<TAMSDparGeo*>( fpMsdGeoMap->Object() );
            
            auto list = start_list( detector_properties<details::vertex_tag>(vertex_hc, clusterVTX_hc,
                                                                             geoVTX_h) )
                            .add( detector_properties<details::it_tag>(clusterIT_hc, geoIT_h) )
                            .add( detector_properties<details::msd_tag>(clusterMSD_hc, geoMSD_h) )
                            .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h) )
                            .finish();
            std::cout << "full_config\n";
            auto* action_h =  new_TATOEactGlb(
                        std::move(ukf),
                        std::move(list),
                        static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                        static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                        GetFootField(), true
                                       );
            
            auto& computation_checker_c = action_h->get_computation_checker();
            using namespace checker;
            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation>>{} );
            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation>>{} );
            computation_checker_c.push_back( TATOEchecker< purity<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< fake_distribution<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< clone_distribution<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< mass_identification<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< momentum_difference<computation> >{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation, isolate_charge<1>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation, isolate_charge<1>>>{});
//            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation, isolate_charge<2>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation, isolate_charge<2>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation, isolate_charge<3>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation, isolate_charge<3>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation, isolate_charge<4>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation, isolate_charge<4>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation, isolate_charge<5>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation, isolate_charge<5>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation, isolate_charge<6>>>{} );
//            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation, isolate_charge<6>>>{} );
            
            return action_h;
            
        }
        case flag_set<details::ms2d_tag, details::tof_tag>{}: {
            auto * pointMSD_hc = static_cast<TAMSDntuPoint*>( fpMsdPoint->Object() );
            auto * geoMSD_h = static_cast<TAMSDparGeo*>( fpMsdGeoMap->Object() );
            
            auto list = start_list( detector_properties<details::ms2d_tag>(pointMSD_hc, geoMSD_h) )
                            .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h) )
                            .finish();

            auto* action_h =  new_TATOEactGlb(
                        std::move(ukf),
                        std::move(list),
                        static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                        static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                        GetFootField(), true
                                       );
            
            auto& computation_checker_c = action_h->get_computation_checker();
            using namespace checker;
            computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation>>{} );
            computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation>>{} );
            computation_checker_c.push_back( TATOEchecker< purity<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< fake_distribution<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< clone_distribution<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< mass_identification<computation> >{} );
            computation_checker_c.push_back( TATOEchecker< momentum_difference<computation> >{} );
            
            return action_h;
        }
    }
    
    auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( fpVtxClus->Object() );
    auto * vertex_hc = static_cast<TAVTntuVertex*>( fpVtxVertex->Object() );
    auto * geoVTX_h = static_cast<TAVTparGeo*>( fpVtxGeoMap->Object() );
    
    auto list = start_list( detector_properties<details::vertex_tag>(vertex_hc, clusterVTX_hc,
                                                                     geoVTX_h) )
                    .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h) )
                    .finish();
    
    
    auto* action_h = new_TATOEactGlb(
                std::move(ukf),
                std::move(list),
                static_cast<TAGntuGlbTrack*>( fpGlbTrack->Object() ),
                static_cast<TAGparGeo*>( fpGGeoMap->Object() ),
                GetFootField(), true
                               );
    
    auto& computation_checker_c = action_h->get_computation_checker();
    using namespace checker;
    computation_checker_c.push_back( TATOEchecker< reconstructible_distribution<computation>>{} );
    computation_checker_c.push_back( TATOEchecker< reconstructed_distribution<computation>>{} );
    computation_checker_c.push_back( TATOEchecker< purity<computation> >{} );
    computation_checker_c.push_back( TATOEchecker< fake_distribution<computation> >{} );
    computation_checker_c.push_back( TATOEchecker< clone_distribution<computation> >{} );
    computation_checker_c.push_back( TATOEchecker< mass_identification<computation> >{} );
    computation_checker_c.push_back( TATOEchecker< momentum_difference<computation> >{} );
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



