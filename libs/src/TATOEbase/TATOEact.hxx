

#ifndef _TATOEact_HXX
#define _TATOEact_HXX


#include <utility>
#include <iostream>

#include "detector_list.hpp"
#include "arborescence.hpp"

#include "TATOEutilities.hxx"
//______________________________________________________________________________
//

#include "TAMCntuEve.hxx"
#include "TAVTcluster.hxx"
#include "TAVTntuVertex.hxx"

#include "TAGntuGlbTrack.hxx"
#include "TAGcluster.hxx"

template<class T>
class TD;


struct TATOEbaseAct {
    virtual void Action()  = 0;
    virtual ~TATOEbaseAct() = default;
};




template<class UKF, class DetectorList>
struct TATOEactGlb {};


template<class UKF, class ... DetectorProperties>
struct TATOEactGlb< UKF, detector_list< details::finished_tag,
                                        DetectorProperties...  >   >
            : TATOEbaseAct
{
    using detector_list_t = detector_list< details::finished_tag, DetectorProperties...  >;
    
    using vector_matrix = typename underlying< typename details::filter_traits<UKF>::state >::vector::type ;
    using covariance_matrix = typename underlying< typename details::filter_traits<UKF>::state >::covariance::type ;
    using corrected_state = corrected_state_impl<vector_matrix, covariance_matrix>;
    using state = state_impl<vector_matrix, covariance_matrix>;
    
    using data_type = TAGcluster;
    using full_state = aggregator< corrected_state, data_handle<data_type> >;
    
    using node_type = node<full_state>;
    
private:
    particle_properties particle_m{};
    UKF ukf_m;
    detector_list_t list_m;
    TAGntuGlbTrack* track_mhc;
    
public:
    
    TATOEactGlb( UKF&& ukf_p,
                 detector_list_t&& list_p,
                 TAGntuGlbTrack* track_phc ) :
        ukf_m{ std::move(ukf_p) },
        list_m{ std::move(list_p) },
        track_mhc{ track_phc }
    {
        ukf_m.call_stepper().ode.model().particle_h = &particle_m;
    }
    
    void Action() override {

        std::cout << " ---- GLOBAL_RECONSTRUCTION --- \n";
        
        auto hypothesis_c = form_hypothesis();
        
        for(auto & hypothesis : hypothesis_c){
            particle_m = hypothesis;
            
            std::cout << std::scientific << " -- Particle -- \n";
            std::cout << "charge = " << particle_m.charge << '\n';
            std::cout << "mass = " << particle_m.mass << '\n';
            std::cout << "momentum = " << particle_m.momentum << '\n';
            
            if(particle_m.charge == 0){ continue; }
            reconstruct();
        }
        
        
        //form list of hypothesis
        
        //for each
        // - set particle_m
        // - reconstruct: either consume the arborescence and release global tracks -> better

     
        
        
        //reconstruct
        // - create arborescence from first layer of vertex
        // - iterate over the list to retrieve each detector properties
        // - advance_reconstruction for each
        //-- control tracks
           // -- fill
        
        // advance_reconstruction
        // - retrieve handler for arborescence -> arborscence is a parameter
        // - propagate from the nodes
        // - retrieve the list of candidates, sort them according to the cut value
        // - correct the good ones and add to the arborescence
        
        
    };
    
private:
    
    std::vector<particle_properties> form_hypothesis()
    {
        
        std::cout << " ---- FORM_HYPOTHESIS --- \n";
        
        auto tof = list_m.last();

        auto candidate_c = tof.generate_candidates();
        std::vector<particle_properties> hypothesis_c;
        hypothesis_c.reserve( candidate_c.size()*2 );
        
        
        auto * data_hc = static_cast<TAMCntuEve*>( gTAGroot->FindDataDsc( "eveMc" )->Object() );
        
        for( const auto& candidate : candidate_c ) {

            std::cout << "---- candidate ----\n";
            for( int i{0}; i < candidate.data->GetColumnHit()->GetMcTracksN() ; ++i  ){
                auto id = candidate.data->GetColumnHit()->GetMcTrackIdx(i);
                std::cout << "A/Z :" << data_hc->GetHit(id)->GetMass() << "/" << data_hc->GetHit(id)->GetCharge() << '\n';
                std::cout << "initial_momentum : " << data_hc->GetHit(id)->GetInitP().Mag() * 1e3 << '\n';
                std::cout << "real_track_slope_x : " << data_hc->GetHit(id)->GetInitP().X()/data_hc->GetHit(id)->GetInitP().Z() << '\n';
                std::cout << "real_track_slope_y : " << data_hc->GetHit(id)->GetInitP().Y()/data_hc->GetHit(id)->GetInitP().Z() << '\n';
            }
            
            
            auto charge = candidate.data->GetChargeZ();
            
            auto add_current_end_point = [&candidate]( particle_properties & hypothesis_p  )
                                 { hypothesis_p.get_end_points().push_back( candidate.data ); };
            
            std::for_each( hypothesis_c.begin(), hypothesis_c.end(),
                           [&charge, &candidate, &add_current_end_point]( particle_properties & h_p ){
                               if( h_p.charge == charge ){ add_current_end_point( h_p ); }
                           } );
            
            
            
            auto first_matching_hypothesis = std::find_if( hypothesis_c.begin(), hypothesis_c.end(),
                                                          [&charge]( particle_properties const & h_p ){ return h_p.charge == charge; } );
            
            if( first_matching_hypothesis == hypothesis_c.end() ){
                auto beam_energy = 200.; //MeV/u
            
                auto add_hypothesis = [&]( int mass_number_p, double light_ion_boost_p = 1 ){
                    auto momentum = sqrt( (beam_energy * mass_number_p) * (beam_energy * mass_number_p) + 2 *  (beam_energy * mass_number_p) * (938 * mass_number_p)  );
                    hypothesis_c.push_back( particle_properties{ charge, mass_number_p, momentum, light_ion_boost_p } );
                    add_current_end_point( hypothesis_c.back() );
                };
            
                switch(charge){
                    case 1:
                    {
                        auto light_ion_boost = 2;
                        add_hypothesis(1, light_ion_boost);
                    
//                      light_ion_boost = 1.3;
                        add_hypothesis(2, light_ion_boost);
                    
                        add_hypothesis(3);
                        break;
                    }
                    case 2:
                    {
                        add_hypothesis(3);
                        add_hypothesis(4);
                        break;
                    }
                    default:
                    {
                        auto mass_number = charge * 2;
                        add_hypothesis(mass_number);
                        break;
                    }
                        
                        
                }
                
                
            }
            
        }
        
        return hypothesis_c;
    }
    

    corrected_state generate_corrected_state( TAVTvertex const * vertex_ph,
                                              TAVTbaseCluster const * cluster_ph ) const
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>(
                                     gTAGroot->FindAction( TAGgeoTrafo::GetDefaultActName().Data() )
                                                            );
        
        auto start = transformation_h->FromVTLocalToGlobal( vertex_ph->GetVertexPosition() );
        auto end = transformation_h->FromVTLocalToGlobal( cluster_ph->GetPositionG() );
        
        auto length = end - start;
        
        auto track_slope_x = length.X()/length.Z();
        auto track_slope_y = length.Y()/length.Z();
        
        std::cout << "track_slope_x: " << track_slope_x << '\n';
        std::cout << "track_slope_y: " << track_slope_y << '\n';
        
        
        auto length_error_x = sqrt( pow( vertex_ph->GetVertexPosError().X(), 2 ) +
                                    pow( cluster_ph->GetPosErrorG().X(), 2)             );
        auto length_error_y = sqrt( pow( vertex_ph->GetVertexPosError().Y(), 2 ) +
                                    pow( cluster_ph->GetPosErrorG().Y(), 2)           );
        
        
        auto track_slope_error_x = abs( track_slope_x ) *
                                   sqrt( pow( length_error_x/length.X(), 2) +
                                         pow( 0.05/length.Z(), 2)              );
        auto track_slope_error_y = abs( track_slope_y ) *
                                   sqrt( pow( length_error_y/length.Y(), 2) +
                                         pow( 0.05/length.Z(), 2)               );
        
        
        using vector = typename underlying<state>::vector;
        using covariance = typename underlying<state>::covariance;
        
        return corrected_state{
            state{
                evaluation_point{ start.Z() },
                vector{{ start.X(), start.Y(), track_slope_x, track_slope_y }},
                covariance{{   pow(length_error_x, 2  ),     0,    0,    0,
                               0,     pow(length_error_y, 2),      0,    0,
                               0,    0,   pow(track_slope_error_x, 2),   0,
                               0,    0,    0,   pow( track_slope_error_y, 2)  }}
            },
            chisquared{0}
        };
    }
    
    void reconstruct()
    {
        std::cout << " ---- RECONSTRUCT --- \n";
        
        auto arborescence = make_arborescence< full_state >();
        

        list_m.apply_for_each( [this, &arborescence ]( const auto& detector_p )
                               {
                                   cross_check_origin( arborescence, detector_p );
                                   advance_reconstruction( arborescence, detector_p );
                               } );
        
        
        auto& node_c = arborescence.get_handler();
        std::cout << "----- reconstructed_track : "<< node_c.size() << " -----\n";
        
        for(auto& node : node_c){
            register_track(node);

        }
    }
    
    //-------------------------------------------------------------------------------------
    //                           cross_check_origin
    //
    
    template< class T,
              typename std::enable_if_t< !std::is_same< T,  detector_properties< details::vertex_tag > >::value ,
                                          std::nullptr_t  > = nullptr    >
    void cross_check_origin( arborescence< node_type >& arborescence_p,
                             const T& detector_p )
    {
        auto confirm_origin = []( node_type & node_p, auto const & detector_p)
        {
            return node_p.get_value().evaluation_point >= detector_p.layer_depth(0);
        };
        
        auto& node_c = arborescence_p.get_handler();
        for(auto & node : node_c ){
            if( !confirm_origin( node, list_m.before( detector_p ) ) ){ node.mark_invalid(); }
        }
    }
    
    void cross_check_origin( arborescence< node_type >& /*arborescence_p*/,
                             const detector_properties<details::vertex_tag>& /*vertex_p*/ )
    {}
    
    //-------------------------------------------------------------------------------------
    //                            advance_reconstruction_impl
    // - should be the same, always : only the confrontation part differs !
    
    
    template< class T >
    std::vector<full_state> advance_reconstruction_impl( state s_p,
                                                         const T& layer_p )
    {
        std::cout << "\nstarting_state : ( " << s_p.vector(0,0) << ", " << s_p.vector(1,0) ;
        std::cout << ") -- (" << s_p.vector(2,0) << ", " << s_p.vector(3,0)  ;
        std::cout << ") -- " << s_p.evaluation_point << '\n';
//
        auto sigma_c = ukf_m.generate_sigma_points( s_p );
        
        sigma_c = ukf_m.propagate_while(
                                        std::move(sigma_c),
                                        [this, &layer_p](const auto& os_p)
                                        { return os_p.evaluation_point + ukf_m.step_length() < layer_p.depth ; }
                                        );
        
        auto step = layer_p.depth - sigma_c.front().evaluation_point;
        
        if(step<0){ std::cout << "WARNING : going backwards !\n"; }
        
        sigma_c = ukf_m.force_propagation( std::move(sigma_c), step );
        auto ps = ukf_m.generate_propagated_state( std::move(sigma_c) );
//
        std::cout << "propagated_state : ( " << ps.vector(0,0) << ", " << ps.vector(1,0) ;
        std::cout << ") -- (" << ps.vector(2,0) << ", " << ps.vector(3,0)  ;
        std::cout << ") -- " << ps.evaluation_point << '\n';
        
        return confront(ps, layer_p);
    }
    
    
    
    //-------------------------------------------------------------------------------------
    //                            advance_reconstruction overload set
    
    template< class T,
              typename std::enable_if_t< !std::is_same< T,  detector_properties< details::tof_tag > >::value ,
                                          std::nullptr_t  > = nullptr,
              typename std::enable_if_t< !std::is_same< T,  detector_properties< details::vertex_tag > >::value ,
                                          std::nullptr_t  > = nullptr    >
    void advance_reconstruction( arborescence<node_type>& arborescence_p,
                                 const T& layer_pc )
    {
        
        std::cout << " ---- ADVANCE_RECONSTRUCTION --- \n";
        
        
        for(auto && layer : layer_pc ) {
        
            auto& leaf_c = arborescence_p.get_handler();
            std::cout << " --- layer --- \n";
        
            if( layer.get_candidates().empty() ){ continue; }
            
            
            for(auto& leaf : leaf_c){
                
                std::cout << " --- leaf --- \n";
            
                ukf_m.step_length() = 1e-3;
            
                auto s = make_state( leaf.get_value() );
                auto fs_c = advance_reconstruction_impl( s, layer );
            
                for( auto & fs : fs_c ){
//
                    std::cout << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                    std::cout << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                    std::cout << ") -- " << fs.evaluation_point << " -- " << fs.chisquared << '\n';
//
                    leaf.add_child( std::move(fs) );
                }

            }
 
        }
    
    }
    
    

    void advance_reconstruction( arborescence<node_type>& arborescence_p,
                                 const detector_properties<details::tof_tag>& tof_p )
    {
        
        std::cout << " ---- FINALISE_RECONSTRUCTION --- \n";
        
        
        auto& leaf_c = arborescence_p.get_handler();
        
        
        for(auto& leaf : leaf_c){
            
            ukf_m.step_length() = 1e-3;
            
            auto s = make_state(leaf.get_value());
            auto fs_c = advance_reconstruction_impl( s, tof_p.form_layer() );
            
            if( fs_c.empty() ){ leaf.mark_invalid(); }
            else{
                auto fs = fs_c.front();
                
                std::cout << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                std::cout << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                std::cout << ") -- " << fs.evaluation_point << " -- " << fs.chisquared << '\n';
                
                leaf.add_child( std::move(fs_c.front()) );
            }
        }
        
    }
    
    
    
//    
    void advance_reconstruction( arborescence<node_type>& arborescence_p,
                                 const detector_properties<details::vertex_tag>& vertex_p )
    {
        
        std::cout << " ---- START_RECONSTRUCTION --- \n";
        
        //need to retrieve vertex
        //need to retrieve each track
        //for each track
        //compute track_slope and track_slope error
        //start propagation but feed only following cluster in track
        
        
        auto * vertex_h = vertex_p.retrieve_vertex();
        if( !vertex_h ){ std::cout << "WARNING : no vertex found\n"; }
        
        auto track_c = vertex_p.get_track_list( vertex_h );
        for( auto& track : track_c ){
            auto first_h = track.first_cluster();
            
            auto cs = generate_corrected_state( vertex_h, first_h );
            auto fs = full_state{
                std::move(cs),
                data_handle<data_type>{nullptr}
            };
            
            auto * leaf_h = arborescence_p.add_root( std::move(fs) );
            
            std::cout << " --- track --- \n";
            
            
            for( auto layer : track ){
                
                ukf_m.step_length() = 1e-3;
                
                auto s = make_state( leaf_h->get_value() );
                auto fs_c = advance_reconstruction_impl( s, layer );
                
                if( !fs_c.empty() ){
                    auto fs = fs_c.front();

                    std::cout << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                    std::cout << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                    std::cout << ") -- " << fs.evaluation_point << " -- " << fs.chisquared << '\n';
//
//
                    leaf_h = leaf_h->add_child( std::move(fs_c.front()) );
                    
                }
                
            }
            
//            std::cout << "cluster_added: " << leaf_h->depth()-1 << '\n';
//            std::cout << "cluster_intrack: " << track.size() << '\n';
            if( leaf_h->depth()-1 < track.size()-1 ){ leaf_h->mark_invalid(); }
            
        }
        
    }
    
    
    //-------------------------------------------------------------------------------------
    //                            confront overload set
    
    //SFINAE might not be needed
    template<class T,
             typename std::enable_if_t< !std::is_same< T,  detector_properties< details::tof_tag > >::value ,
                                        std::nullptr_t  > = nullptr,
             typename std::enable_if_t< !std::is_same< T,  detector_properties< details::vertex_tag > >::value ,
                                        std::nullptr_t  > = nullptr    >
    std::vector<full_state> confront(const state& ps_p, const T& layer_p)
    {
        
        auto candidate_c = layer_p.get_candidates();
        
        std::vector<full_state> fs_c;
        fs_c.reserve( candidate_c.size() );
        
        using enriched_candidate = enriched_candidate_impl<typename decltype(candidate_c)::value_type>;
        std::vector< enriched_candidate > enriched_c;
        enriched_c.reserve( candidate_c.size() );
    
        
        std::for_each( candidate_c.begin(), candidate_c.end(),
                      [this, &ps_p, &enriched_c]( const auto& candidate_p )
                      {
                          auto chisquared = ukf_m.compute_chisquared(ps_p, candidate_p);
                          enriched_c.push_back( make_enriched_candidate( std::move( candidate_p ) ,
                                                                         std::move( chisquared) )   );
                      }
                     );
    
        auto end = std::partition(
                                  enriched_c.begin(), enriched_c.end(),
                                  [this, &ps_p, &layer_p ]( const auto & ec_p )
                                  {
                                      auto error = ec_p.data->GetPosError();
                                      
                                      auto mps = split_half( ps_p.vector , details::row_tag{});
                                      mps.first.get(0,0) += layer_p.cut * particle_m.light_ion_boost * error.X();
                                      mps.first.get(1,0) += layer_p.cut * particle_m.light_ion_boost * error.Y();
                                      // TD<decltype(mps)>x;
                                      
                                      using ec = typename underlying<decltype(ec_p)>::type;
                                      using candidate = typename underlying<ec>::candidate;
                                      using vector = typename underlying<candidate>::vector;
                                      using covariance = typename underlying<candidate>::covariance;
                                      using measurement_matrix = typename underlying<candidate>::measurement_matrix;
                                      using data = typename underlying<candidate>::data_type;
                                      
                                      auto cutter_candidate = candidate{
                                          vector{ std::move(mps.first) },
                                          covariance{ ec_p.covariance },
                                          measurement_matrix{ ec_p.measurement_matrix },
                                          data_handle<data>{ ec_p.data }
                                      } ;
                                      
                                      auto cutter = ukf_m.compute_chisquared(ps_p, cutter_candidate);
                                      
                                      std::cout << "cutter_chisquared : " << cutter.chisquared << '\n';
                                      std::cout << "candidate : (" << ec_p.vector(0, 0) << ", " << ec_p.vector(1,0) << ")\n";
                                      std::cout << "candidate_chisquared : " << ec_p.chisquared << '\n';
                                      
                                      return ec_p.chisquared < cutter.chisquared;
                                  }
                                  );
        
        
        for(auto iterator = enriched_c.begin() ; iterator != end ; ++iterator ){
            auto state = ukf_m.correct_state( ps_p, *iterator ); //should be sliced properly
            auto cs = make_corrected_state( std::move(state),
                                           chisquared{std::move(iterator->chisquared)} );
            
            auto fs = full_state{ std::move(cs),
                                  data_handle<data_type>{ iterator->data }  };
            fs_c.push_back( std::move(fs) );
        }
    
    
        return fs_c;
    }
    
    
    
    
    std::vector<full_state> confront(const state& ps_p, const detector_properties<details::tof_tag>::layer& layer_p) //optionnal is more relevant here
    {
        using candidate = typename detector_properties< details::tof_tag >::candidate;
        
        auto candidate_c = layer_p.get_candidates();
        auto candidate_end = std::partition( candidate_c.begin(), candidate_c.end(),
                                   [this, &candidate_c](candidate const & c_p)
                                   {
                                       auto const & end_point_ch = particle_m.get_end_points();
                                       return std::any_of( end_point_ch.begin(), end_point_ch.end(),
                                                           [&c_p](auto const & ep_ph ){ return c_p.data == ep_ph;  } );
                                   } );
        

        using enriched_candidate = enriched_candidate_impl<typename decltype(candidate_c)::value_type>;
        std::vector< enriched_candidate > enriched_c;
        enriched_c.reserve( std::distance( candidate_c.begin(), candidate_end ) );
        std::for_each( candidate_c.begin(), candidate_end,
                      [this, &ps_p, &enriched_c]( candidate const& c_p )
                      {
                          auto chisquared = ukf_m.compute_chisquared(ps_p, c_p);
                          enriched_c.push_back( make_enriched_candidate( std::move( c_p ) ,
                                                                         std::move( chisquared ) )   );
                      }
                      );
        

        
        std::vector<full_state> fs_c;
        fs_c.reserve( std::distance( candidate_c.begin(), candidate_end ) );
        
    
        auto select = [this, &ps_p, &layer_p ]( const auto & ec_p )
                         {
                            auto error = ec_p.data->GetPosError();
            
                            auto mps = split_half( ps_p.vector , details::row_tag{});
                            mps.first.get(0,0) += layer_p.cut * particle_m.light_ion_boost * error.X();
                            mps.first.get(1,0) += layer_p.cut * particle_m.light_ion_boost * error.Y();
            
                             using ec = typename underlying<decltype(ec_p)>::type;
                             using candidate = typename underlying<ec>::candidate;
                             using vector = typename underlying<candidate>::vector;
                             using covariance = typename underlying<candidate>::covariance;
                             using measurement_matrix = typename underlying<candidate>::measurement_matrix;
                             using data = typename underlying<candidate>::data_type;
            
                             auto cutter_candidate = candidate{
                                 vector{ std::move(mps.first) },
                                 covariance{ ec_p.covariance },
                                 measurement_matrix{ ec_p.measurement_matrix },
                                 data_handle<data>{ ec_p.data }
                             } ;
            
                             auto cutter = ukf_m.compute_chisquared(ps_p, cutter_candidate);
                             
                             std::cout << "cutter_chisquared : " << cutter.chisquared << '\n';
                             std::cout << "candidate : (" << ec_p.vector(0, 0) << ", " << ec_p.vector(1,0) << ")\n";
                             std::cout << "candidate_chisquared : " << ec_p.chisquared << '\n';
            
                             return ec_p.chisquared < cutter.chisquared;
                         };
        
        auto enriched_end = std::partition( enriched_c.begin(), enriched_c.end(), select );
        for(auto iterator = enriched_c.begin() ; iterator != enriched_end ; ++iterator ){
            auto state = ukf_m.correct_state( ps_p, *iterator ); //should be sliced properly
            auto cs = make_corrected_state( std::move(state),
                                            chisquared{std::move(iterator->chisquared)} );
            
            auto fs = full_state{ std::move(cs),
                                  data_handle<data_type>{ iterator->data }  };
            fs_c.push_back( std::move(fs) );
        }

        return fs_c;
    }
    


    std::vector<full_state> confront(const state& ps_p, const detector_properties<details::vertex_tag>::track_list::pseudo_layer& layer_p) //optionnal is more relevant here
    {
        using candidate = typename detector_properties< details::vertex_tag >::candidate;
        using enriched_candidate = enriched_candidate_impl< candidate >;
    
    
        auto c = layer_p.get_candidate();
        auto chi2 = ukf_m.compute_chisquared(ps_p, c);
        enriched_candidate ec = make_enriched_candidate( std::move( c) ,
                                                         std::move( chi2 )   );
    
    
        std::vector<full_state> fs_c;
    
        auto selection = [this, &ps_p, &layer_p ]( const auto & ec_p )
                            {
                                auto error = ec_p.data->GetPosError();
        
                                auto mps = split_half( ps_p.vector , details::row_tag{});
                                mps.first.get(0,0) += layer_p.cut * particle_m.light_ion_boost * error.X();
                                mps.first.get(1,0) += layer_p.cut * particle_m.light_ion_boost * error.Y();
        
                                using ec = typename underlying<decltype(ec_p)>::type;
                                using candidate = typename underlying<ec>::candidate;
                                using vector = typename underlying<candidate>::vector;
                                using covariance = typename underlying<candidate>::covariance;
                                using measurement_matrix = typename underlying<candidate>::measurement_matrix;
                                using data = typename underlying<candidate>::data_type;
                            
                                auto cutter_candidate = candidate{
                                    vector{ std::move(mps.first) },
                                    covariance{ ec_p.covariance },
                                    measurement_matrix{ ec_p.measurement_matrix },
                                    data_handle<data>{ ec_p.data }
                                } ;
        
                                auto cutter = ukf_m.compute_chisquared(ps_p, cutter_candidate);
//
                                std::cout << "cutter_chisquared : " << cutter.chisquared << '\n';
                                std::cout << "candidate : (" << ec_p.vector(0, 0) << ", " << ec_p.vector(1,0) << ")\n";
                                std::cout << "candidate_chisquared : " << ec_p.chisquared << '\n';
                                
                                return ec_p.chisquared < cutter.chisquared;
                            };
    
        if( selection(ec) ){
            auto state = ukf_m.correct_state( ps_p, ec ); //should be sliced properly
            auto cs = make_corrected_state( std::move(state),
                                            chisquared{std::move(ec.chisquared)} );
            
            auto fs = full_state{ std::move(cs),
                                   data_handle<data_type>{ ec.data }  };
            
            fs_c.push_back( std::move(fs) );
        }
    
        return fs_c;
    }

    
    
    void register_track( node_type& node_p ) const
    {
        auto * track_h = track_mhc->NewTrack( particle_m.mass * 0.938 , particle_m.momentum / 1000., static_cast<double>(particle_m.charge), 1.1   ); //tof value is wrong
        
        
        auto value_c = node_p.get_branch_values();
        std::cout << " --- final_track --- " << '\n';
        for(auto& value : value_c){
            std::cout << "( " << value.vector(0,0) <<  ", " << value.vector(1,0) <<  " ) -- ( " <<value.vector(2,0) << ", " << value.vector(3,0) <<  " ) -- " << value.evaluation_point << " -- " <<  value.chisquared << '\n';
        
            TVector3 corrected_position{ value.vector(0,0), value.vector(1,0), value.evaluation_point };
            
            
            auto momentum_z = sqrt( pow( value.vector(2,0), 2) + pow( value.vector(3,0), 2) + 1 ) * particle_m.momentum ;
            momentum_z /= 1000.;
            auto momentum_x = value.vector(2,0) * momentum_z;
            auto momentum_y = value.vector(3,0) * momentum_z;

            TVector3 momentum{ momentum_x , momentum_y, momentum_z };
            TVector3 position_error{ 0.01 ,0.01, 0.01 };
            TVector3 momentum_error{ 0.01, 0.01, 0.01 };

            
            track_h->AddCorrPoint( corrected_position, position_error, momentum, momentum_error ); //corr point not really meas
            
            if( value.data ){
                TVector3 measured_position{ value.data->GetPosition().X(), value.data->GetPosition().Y(), corrected_position.Z() };
                track_h->AddMeasPoint( measured_position, position_error, momentum, momentum_error );
            }
        }
    }
    
    
    

    
};




template<class UKF, class DetectorList>
auto make_TATOEactGlb(UKF ukf_p, DetectorList list_p, TAGntuGlbTrack* track_phc)
        ->TATOEactGlb<UKF, DetectorList>
{
    return {std::move(ukf_p), std::move(list_p), track_phc};
}



template<class UKF, class DetectorList>
auto make_new_TATOEactGlb(UKF ukf_p, DetectorList list_p, TAGntuGlbTrack* track_phc)
        -> TATOEactGlb<UKF, DetectorList> *
{
    return new TATOEactGlb<UKF, DetectorList>{std::move(ukf_p), std::move(list_p), track_phc};
}


#endif











