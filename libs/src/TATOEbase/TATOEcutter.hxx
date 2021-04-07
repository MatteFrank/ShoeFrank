
//
//File      : TATOEcutter.hxx
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 03/04/2021
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//


#ifndef _TATOEcutter_HXX
#define _TATOEcutter_HXX

#include "TAGaction.hxx"

#include "TATOEutilities.hxx"
#include "grkn_data.hpp"
#include "stepper.hpp"
#include "ukf.hpp"



struct TATOEcutter : TAGaction {
    struct glb_features{
        TAGparaDsc* parameter_h;
        TAGparaDsc* dipole_parameter_h;
        TADIgeoField* field_h;
    };
    
    struct vtx_features{
        TAGdataDsc* cluster_h;
        TAGdataDsc* track_h;
        TAGdataDsc* vertex_h;
        TAGparaDsc* parameter_h;
    };
    struct it_features{
        TAGdataDsc* data_h;
        TAGparaDsc* parameter_h;
    };
    struct msd_features{
        TAGdataDsc* data_h;
        TAGparaDsc* parameter_h;
    };
    struct tw_features{
        TAGdataDsc* data_h;
        TAGparaDsc* parameter_h;
    };

    
    TATOEcutter( char const* name_p,
                 glb_features glb_features_p,
                 vtx_features vtx_features_p,
                 it_features  it_features_p,
                 msd_features msd_features_p,
                 tw_features  tw_features_p ) :
        TAGaction(name_p, "TATOEcutter - Tool used to determine optimal cut values according to geometry for use in TOE"),
        glb_features_m{ glb_features_p },
        vtx_features_m{vtx_features_p},
        it_features_m{it_features_p},
        msd_features_m{ msd_features_p},
        tw_features_m{tw_features_p},
        action_mh{setup_action()} {}
    
    void NextIteration() {
        ++cut;
        action_mh->set_cuts( details::vertex_tag{}, cut );
        result_mc.push_back( reconstruction_result{} );
    };
    
    Bool_t Action() override {
        action_mh->Action();
        result_mc.back().add( action_mh->retrieve_result() ) ;
//        std::cout << "results:" << result_c.back().efficiency.value << " - " << result_c.back().purity.value << '\n';
//        action_mh->Output();
        return true;
    }
    
    void Output() {
        
        for( auto const& result : result_mc) {
            std::size_t reconstructed_number{0};
            std::size_t reconstructible_number{0};
            
            std::size_t correct_cluster_number{0};
            std::size_t recovered_cluster_number{0};
            
            std::size_t clone_number{0};
            
            for(auto const & module : result.module_c){
                reconstructed_number += module.reconstructed_number;
                reconstructible_number += module.reconstructible_number;
            
                correct_cluster_number += module.correct_cluster_number;
                recovered_cluster_number += module.recovered_cluster_number;
            
                clone_number += module.clone_number;
            }
        
            std::cout << "===== MIXED_RESULTS ====\n";
            std::cout << "----- efficiency -----\n";
            auto efficiency = reconstructed_number * 1./reconstructible_number;
            std::cout << "global_efficiency: " << efficiency * 100 << '\n';
            auto efficiency_error = sqrt(efficiency* (1+ efficiency)/reconstructible_number);
            std::cout << "global_efficiency_error: " << efficiency_error * 100<< '\n';
        
            std::cout << "----- purity -----\n";

            auto purity = correct_cluster_number * 1./recovered_cluster_number;
            std::cout << "global_purity: " << purity * 100 << '\n';
            auto purity_error = sqrt(purity* (1+purity)/recovered_cluster_number);
            std::cout << "global_purity_error: " << purity_error * 100<< '\n';

        }
    }

private:
    TATOEbaseAct* setup_action() {
        using state_vector =  matrix<4,1> ;
        using state_covariance =  matrix<4, 4> ;
        using state = state_impl< state_vector, state_covariance  >;
        
        auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( vtx_features_m.cluster_h->Object() );
        auto * vertex_hc = static_cast<TAVTntuVertex*>( vtx_features_m.vertex_h->Object() );
        auto * geoVTX_h = static_cast<TAVTparGeo*>( vtx_features_m.parameter_h->Object() );
        
        
        auto * clusterIT_hc = static_cast<TAITntuCluster*>( it_features_m.data_h->Object() );
        auto * geoIT_h = static_cast<TAITparGeo*>( it_features_m.parameter_h->Object() );
        
        auto * clusterMSD_hc = static_cast<TAMSDntuCluster*>( msd_features_m.data_h->Object() );
        auto * geoMSD_h = static_cast<TAMSDparGeo*>( msd_features_m.parameter_h->Object() );
        
        auto * clusterTW_hc = static_cast<TATWntuPoint*>( tw_features_m.data_h->Object() );
        auto * geoTW_h = static_cast<TATWparGeo*>( tw_features_m.parameter_h->Object() );
        
        
        auto list = start_list( detector_properties<details::vertex_tag>(vertex_hc, clusterVTX_hc,
                                                                         geoVTX_h, 15) )
        .add( detector_properties<details::it_tag>(clusterIT_hc, geoIT_h, {33, 38}) )
        .add( detector_properties<details::msd_tag>(clusterMSD_hc, geoMSD_h, {13, 18, 23}) )
        .add( detector_properties<details::tof_tag>(clusterTW_hc, geoTW_h, 2.2) )
        .finish();
        
        
        auto ode = make_ode< matrix<2,1>, 2>( model{ glb_features_m.field_h} );
        auto stepper = make_stepper<data_grkn56>( std::move(ode) );
        auto ukf = make_ukf<state>( std::move(stepper) );
        
        return new_TATOEactGlb(
                               std::move(ukf),
                               std::move(list),
                               nullptr,
                               static_cast<TAGparGeo*>( glb_features_m.parameter_h->Object() ), true
                               );
    }
private:
    glb_features glb_features_m;
    vtx_features vtx_features_m;
    it_features  it_features_m;
    msd_features msd_features_m;
    tw_features  tw_features_m;
    
    std::unique_ptr<TATOEbaseAct> action_mh;

    
    int cut{15};
    std::vector<reconstruction_result> result_mc;
    
};

#endif
