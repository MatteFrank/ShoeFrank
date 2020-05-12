

//
//File      : TATOEact.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 10/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef _TATOEact_HXX
#define _TATOEact_HXX


#include <utility>
#include <iostream>
#include <sstream>

#include "detector_list.hpp"
#include "arborescence.hpp"

#include "TATOEutilities.hxx"
#include "TATOEchecker.hxx"
#include "TATOElogger.hxx"
//______________________________________________________________________________
//

#include "TAMCntuEve.hxx"
#include "TAVTcluster.hxx"
#include "TAVTntuVertex.hxx"

#include "TAGntuGlbTrack.hxx"
#include "TAGcluster.hxx"
#include "TAGparGeo.hxx"

template<class T>
class TD;


struct TATOEbaseAct {
    virtual void Action()  = 0;
    virtual void Output() = 0 ;
    virtual void RegisterHistograms() = 0;
    virtual ~TATOEbaseAct() = default;
};




template<class UKF, class DetectorList>
struct TATOEactGlb {};


template<class UKF, class ... DetectorProperties>
struct TATOEactGlb< UKF, detector_list< details::finished_tag,
                                        DetectorProperties...  >   >
            : TATOEbaseAct
{
    friend TATOEchecker<TATOEactGlb>;
    
    using detector_list_t = detector_list< details::finished_tag, DetectorProperties...  >;
    
    using vector_matrix = typename underlying< typename details::filter_traits<UKF>::state >::vector::type ;
    using covariance_matrix = typename underlying< typename details::filter_traits<UKF>::state >::covariance::type ;
    using corrected_state = corrected_state_impl<vector_matrix, covariance_matrix>;
    using state = state_impl<vector_matrix, covariance_matrix>;
    
    using data_type = TAGcluster;
    using full_state = aggregator< corrected_state, data_handle<data_type> >;
    
    using node_type = node<full_state>;
    
    
    struct track{
        particle_properties particle;
        
        double total_chisquared;
        
        std::vector<full_state> cluster_c;
        std::vector<full_state>& get_clusters() { return cluster_c; }
        std::vector<full_state> const & get_clusters() const { return cluster_c; }
    };
    
private:
    particle_properties particle_m{};
    UKF ukf_m;
    detector_list_t list_m;
    TAGntuGlbTrack* reconstructed_track_mhc;
    double beam_energy_m;
    TATOEchecker<TATOEactGlb> checker_m;
    TATOElogger logger_m;

    node_type const * current_node_mh;
    std::size_t event{0};
    
    std::vector<track> track_mc;
public:
    
    TATOEactGlb( UKF&& ukf_p,
                 detector_list_t&& list_p,
                 TAGntuGlbTrack* track_phc,
                 TAGparGeo const * global_parameters_ph) :
        ukf_m{ std::move(ukf_p) },
        list_m{ std::move(list_p) },
        reconstructed_track_mhc{ track_phc },
        beam_energy_m{ global_parameters_ph->GetBeamPar().Energy * 1000 }, //Mev/u
        checker_m{ global_parameters_ph, *this}
    {
        ukf_m.call_stepper().ode.model().particle_h = &particle_m;
    }
    
    void Output() override {
        checker_m.compute_results( details::all_separated_tag{} );
        logger_m.output();
    }
    
    void RegisterHistograms() override {
        checker_m.register_histograms( details::all_separated_tag{} );
    }
    
    void Action() override {
        
        
        ++event;
        logger_m.clear();
        checker_m.reset_local_data();
        
        auto hypothesis_c = form_hypothesis();
        
        for(auto & hypothesis : hypothesis_c){
            particle_m = hypothesis;
            reconstruct();
        }
        
        logger_m.add_root_header( "END_RECONSTRUCTION" );
        auto track_c = shear_suboptimal_tracks( std::move(track_mc) );
        register_tracks_upward( std::move( track_c ) );
        
        logger_m.freeze_everything();
        logger_m.output();
        
        
        
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
        
        logger_m.add_root_header( "FORM_HYPOTHESIS" );
        logger_m << "event: "<< event << '\n';
        
        auto tof = list_m.last();

        auto candidate_c = tof.generate_candidates();
        std::vector<particle_properties> hypothesis_c;
        hypothesis_c.reserve( candidate_c.size()*2 );
        
        
        auto * data_hc = static_cast<TAMCntuEve*>( gTAGroot->FindDataDsc( "eveMc" )->Object() );

        
        for( const auto& candidate : candidate_c ) {
            auto charge = candidate.data->GetChargeZ();
            if(charge == 0) {continue;}
            
            logger_m.add_header<1>("candidate");
            logger_m << "charge: " << charge << '\n';
            checker_m.register_reconstructible_track( candidate );
            
            auto add_current_end_point = [&candidate]( particle_properties & hypothesis_p  )
                                 { hypothesis_p.get_end_points().push_back( candidate.data ); };
            
            std::for_each( hypothesis_c.begin(), hypothesis_c.end(),
                           [&charge, &candidate, &add_current_end_point]( particle_properties & h_p ){
                               if( h_p.charge == charge ){ add_current_end_point( h_p ); }
                           } );
            
            
            
            auto first_matching_hypothesis = std::find_if( hypothesis_c.begin(), hypothesis_c.end(),
                                                          [&charge]( particle_properties const & h_p ){ return h_p.charge == charge; } );
            
            if( first_matching_hypothesis == hypothesis_c.end() ){
            
                auto add_hypothesis = [&]( int mass_number_p,
                                           double light_ion_boost_p = 1,
                                           double energy_modifier_p = 1 )
                                      {
                    auto momentum = sqrt( pow(beam_energy_m * mass_number_p, 2)  +
                                          2 *  (beam_energy_m * mass_number_p) * (938 * mass_number_p)  ) *
                                    energy_modifier_p;
                    hypothesis_c.push_back( particle_properties{ charge, mass_number_p, momentum, light_ion_boost_p } );
                    add_current_end_point( hypothesis_c.back() );
                                      };
            
                switch(charge){
                    case 1:
                    {
                        auto light_ion_boost = 2;
                        add_hypothesis(1, light_ion_boost);
                        add_hypothesis(1, light_ion_boost, 1.5);
                        add_hypothesis(1, light_ion_boost, 0.5);
                    
//                      light_ion_boost = 1.3;
                        add_hypothesis(2, light_ion_boost);
                        add_hypothesis(2, light_ion_boost, 1.5);
                        add_hypothesis(2, light_ion_boost, 0.5);
                    
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
                                              TAVTbaseCluster const * cluster_ph )
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>(
                                     gTAGroot->FindAction( TAGgeoTrafo::GetDefaultActName().Data() )
                                                            );
        
        auto start = transformation_h->FromVTLocalToGlobal( vertex_ph->GetVertexPosition() );
        auto end = transformation_h->FromVTLocalToGlobal( cluster_ph->GetPositionG() );
        
        auto length = end - start;
        
        auto track_slope_x = length.X()/length.Z();
        auto track_slope_y = length.Y()/length.Z();
        
        
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
        logger_m.add_root_header("RECONSTRUCT");
        
        auto arborescence = make_arborescence< full_state >();
        

        list_m.apply_for_each( [this, &arborescence ]( const auto& detector_p )
                               {
                                   cross_check_origin( arborescence, detector_p );
                                   advance_reconstruction( arborescence, detector_p );
                               } );
        
        
        auto& node_c = arborescence.get_handler();
//        std::cout << "----- reconstructed_track : "<< node_c.size() << " -----\n";
        
        
        
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
//        std::cout << "\nstarting_state : ( " << s_p.vector(0,0) << ", " << s_p.vector(1,0) ;
//        std::cout << ") -- (" << s_p.vector(2,0) << ", " << s_p.vector(3,0)  ;
//        std::cout << ") -- " << s_p.evaluation_point << '\n';

        
        auto sigma_c = ukf_m.generate_sigma_points( s_p );
        
        sigma_c = ukf_m.propagate_while(
                                        std::move(sigma_c),
                                        [this, &layer_p](const auto& os_p)
                                        { return os_p.evaluation_point + ukf_m.step_length() < layer_p.depth ; }
                                        );
        
        auto step = layer_p.depth - sigma_c.front().evaluation_point;
        
        if(step<0){ std::cout << "WARNING : going backwards !" << std::endl; }
        
        sigma_c = ukf_m.force_propagation( std::move(sigma_c), step );
        auto ps = ukf_m.generate_propagated_state( std::move(sigma_c) );
//
        
//        std::cout << "propagated_state : ( " << ps.vector(0,0) << ", " << ps.vector(1,0) ;
//        std::cout << ") -- (" << ps.vector(2,0) << ", " << ps.vector(3,0)  ;
//        std::cout << ") -- " << ps.evaluation_point << '\n';
//
        
        logger_m.add_header<3>("advance_reconstruction_impl");
        logger_m << "propagated_state : ( " << ps.vector(0,0) << ", " << ps.vector(1,0) ;
        logger_m << ") -- (" << ps.vector(2,0) << ", " << ps.vector(3,0)  ;
        logger_m << ") -- " << ps.evaluation_point << '\n';
        
        
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
                                 const T& detector_p )
    {
        
        logger_m.add_root_header("ADVANCE_RECONSTRUCTION");
        
        auto layer_c = detector_p.form_layers();
        
        for(auto && layer : layer_c ) {
        
            auto& leaf_c = arborescence_p.get_handler();
            logger_m.add_header<1>("layer");
        
            if( layer.get_candidates().empty() ){ continue; }
            
            
            for(auto& leaf : leaf_c){
                
                logger_m.add_header<2>( "leaf" );
                checker_m.update_current_node( &leaf );
                checker_m.output_current_hypothesis();
            
                ukf_m.step_length() = 1e-3;
            
                auto s = make_state( leaf.get_value() );
                auto fs_c = advance_reconstruction_impl( s, layer );
            
                for( auto & fs : fs_c ){
//
                    
                    logger_m << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                    logger_m << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                    logger_m << ") -- " << fs.evaluation_point << " -- " << fs.chisquared << '\n';
//
                    leaf.add_child( std::move(fs) );
                }

            }
 
        }
    
    }
    
    

    void advance_reconstruction( arborescence<node_type>& arborescence_p,
                                 const detector_properties<details::tof_tag>& tof_p )
    {
        
        logger_m.add_root_header( "FINALISE_RECONSTRUCTION" );
        
        
        auto& leaf_c = arborescence_p.get_handler();
        
        
        
        for(auto& leaf : leaf_c){
        
            logger_m.add_header<1>("leaf");
            checker_m.update_current_node( &leaf );
            
            checker_m.output_current_hypothesis();
            
            
            ukf_m.step_length() = 1e-3;
            
            auto s = make_state(leaf.get_value());
            auto fs_c = advance_reconstruction_impl( s, tof_p.form_layer() );
            
            if( fs_c.empty() ){ leaf.mark_invalid(); }
            else{
                for( auto&& fs : fs_c ){
                
                    logger_m << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                    logger_m << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                    logger_m << ") -- " << fs.evaluation_point << " -- " << fs.chisquared << '\n';
////
                    leaf.add_child( std::move(fs) );
                }
            }
        }
        
    }
    
    
    
//    
    void advance_reconstruction( arborescence<node_type>& arborescence_p,
                                 const detector_properties<details::vertex_tag>& vertex_p )
    {
        
        logger_m.add_root_header("START_RECONSTRUCTION");
    
        auto track_c = vertex_p.get_track_list( );
        for( auto& track : track_c ){
            auto vertex_h = track.vertex();
            auto first_h = track.first_cluster();
            
            auto cs = generate_corrected_state( vertex_h, first_h );
            auto fs = full_state{
                std::move(cs),
                data_handle<data_type>{nullptr}
            };
            
            auto * leaf_h = arborescence_p.add_root( std::move(fs) );
            checker_m.update_current_node( leaf_h );
            checker_m.output_current_hypothesis();
            logger_m.add_header<1>( "vertex_track_reconstruction" );
            
            
            for( auto layer : track ){
                
                ukf_m.step_length() = 1e-3;
                
                auto s = make_state( leaf_h->get_value() );
                auto fs_c = advance_reconstruction_impl( s, layer );
                
                if( !fs_c.empty() ){
                    auto fs = fs_c.front();
//
                    logger_m << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                    logger_m << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                    logger_m << ") -- " << fs.evaluation_point << " -- " << fs.chisquared << '\n';

//
                    leaf_h = leaf_h->add_child( std::move(fs_c.front()) );
                    checker_m.update_current_node( leaf_h );
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
    template<class T >//,
//             typename std::enable_if_t< !std::is_same< T,  detector_properties< details::tof_tag > >::value ,
//                                        std::nullptr_t  > = nullptr,
//             typename std::enable_if_t< !std::is_same< T,  detector_properties< details::vertex_tag > >::value ,
//                                        std::nullptr_t  > = nullptr    >
    std::vector<full_state> confront(const state& ps_p, const T& layer_p)
    {
        
        auto candidate_c = layer_p.get_candidates();
        
        std::vector<full_state> fs_c;
        fs_c.reserve( candidate_c.size() );
        
        using enriched_candidate = enriched_candidate_impl<typename decltype(candidate_c)::value_type>;
        std::vector< enriched_candidate > enriched_c;
        enriched_c.reserve( candidate_c.size() );
    
        logger_m.add_header<4>(  "confront" );
        std::for_each( candidate_c.begin(), candidate_c.end(),
                      [this, &ps_p, &enriched_c]( const auto& candidate_p )
                      {
                          auto chisquared = ukf_m.compute_chisquared(ps_p, candidate_p);
                          enriched_c.push_back( make_enriched_candidate( std::move( candidate_p ) ,
                                                                         std::move( chisquared) )   );
                      }
                     );
    
        auto select = [this, &ps_p, &layer_p ]( const auto & ec_p ){ return pass_selection( ec_p, ps_p, layer_p); };
        
        auto end = std::partition( enriched_c.begin(), enriched_c.end(), select);
        
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
    
    
    
    
    std::vector<full_state> confront(const state& ps_p, const detector_properties<details::tof_tag>::layer& layer_p)  //optionnal is more relevant here
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
        logger_m.add_header<4>(  "confront" );

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
        
        auto select = [this, &ps_p, &layer_p ]( const auto & ec_p ){ return pass_selection( ec_p, ps_p, layer_p); };
        
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
    


    std::vector<full_state> confront( const state& ps_p,
                                      const track_list< detector_properties<details::vertex_tag> >::pseudo_layer& layer_p )//optionnal is more relevant here
    {
        using candidate = typename detector_properties< details::vertex_tag >::candidate;
        using enriched_candidate = enriched_candidate_impl< candidate >;
    
        logger_m.add_header<4>(  "confront" );
        
        auto c = layer_p.get_candidate();
        auto chi2 = ukf_m.compute_chisquared(ps_p, c);
        enriched_candidate ec = make_enriched_candidate( std::move( c) ,
                                                         std::move( chi2 )   );
    
    
        std::vector<full_state> fs_c;
    
        if( pass_selection(ec, ps_p, layer_p) ){
            auto state = ukf_m.correct_state( ps_p, ec ); //should be sliced properly
            auto cs = make_corrected_state( std::move(state),
                                            chisquared{std::move(ec.chisquared)} );
            
            auto fs = full_state{ std::move(cs),
                                      data_handle<data_type>{ ec.data }  };
            
            fs_c.push_back( std::move(fs) );
            
        }
    
        return fs_c;
    }
           
           

    template<class Enriched, class T>
    bool pass_selection( Enriched const& ec_p, const state& ps_p, T const& layer_p )
    {
        logger_m.add_header<5>( "pass_selection" );
        
        auto error = ec_p.data->GetPosError();
        
        auto mps = split_half( ps_p.vector , details::row_tag{});
        mps.first(0,0) += layer_p.cut * particle_m.light_ion_boost * error.X();
        mps.first(1,0) += layer_p.cut * particle_m.light_ion_boost * error.Y();
        
        using candidate = typename underlying<Enriched>::candidate;
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
        logger_m << "cutter : (" << cutter_candidate.vector(0, 0) << ", " << cutter_candidate.vector(1,0) << ")\n";
        logger_m << "cutter_chisquared : " << cutter.chisquared << '\n';
        logger_m << "candidate : (" << ec_p.vector(0, 0) << ", " << ec_p.vector(1,0) << ")\n";
        logger_m << "candidate_chisquared : " << ec_p.chisquared << '\n';
        
        // checker_m.check_validity( ec_p, ec_p.chisquared, cutter.chisquared, details::should_pass_tag{} );
        
        return ec_p.chisquared < cutter.chisquared;
    }
    
    template<class Enriched>
    bool pass_selection( Enriched const& ec_p,
                         state const& ps_p,
                         layer_generator<detector_properties<details::msd_tag>>::layer const& layer_p )
    {
        logger_m.add_header<5>( "pass_selection" );
        
        auto error = ec_p.data->GetPosError();
        matrix<1,1> v = ec_p.measurement_matrix * ps_p.vector;
        if( ec_p.measurement_matrix(0,0) != 0 ) { v(0,0) += layer_p.cut * particle_m.light_ion_boost * error.X(); }
        else { v(0,0) += layer_p.cut * particle_m.light_ion_boost * error.Y(); }
        
        using candidate = typename underlying<Enriched>::candidate;
        using vector = typename underlying<candidate>::vector;
        using covariance = typename underlying<candidate>::covariance;
        using measurement_matrix = typename underlying<candidate>::measurement_matrix;
        using data = typename underlying<candidate>::data_type;
        
        auto cutter_candidate = candidate{
            vector{ std::move(v) },
            covariance{ ec_p.covariance },
            measurement_matrix{ ec_p.measurement_matrix },
            data_handle<data>{ ec_p.data }
        } ;
        
        auto cutter = ukf_m.compute_chisquared(ps_p, cutter_candidate);
        //
        logger_m << "cutter : (" << cutter_candidate.vector(0, 0) << ", " << cutter_candidate.vector(1,0) << ")\n";
        logger_m << "cutter_chisquared : " << cutter.chisquared << '\n';
        logger_m << "candidate : (" << ec_p.vector(0, 0) << ", " << ec_p.vector(1,0) << ")\n";
        logger_m << "candidate_chisquared : " << ec_p.chisquared << '\n';
        
        // checker_m.check_validity( ec_p, ec_p.chisquared, cutter.chisquared, details::should_pass_tag{} );
        
        return ec_p.chisquared < cutter.chisquared;
    }
        
    
    
    
    
    //----------------------------------------------------------------------------------------
    //final cross-check and registering
    
    void register_track( node_type& node_p )
    {
        auto && value_c = node_p.get_branch_values();
        
        double total_chisquared{0};
        for( auto && value : value_c ){
            total_chisquared += value.chisquared;
        }
        total_chisquared /= value_c.size();
        
//        std::cout << "register_track: " << total_chisquared << '\n';
//
//        std::cout << "z/a: " << particle_m.charge << " / " << particle_m.mass << '\n';
//
//        for(auto& value : value_c){
//            std::cout << "( " << value.vector(0,0) <<  ", " << value.vector(1,0) <<  " ) -- ( ";
//            std::cout << value.vector(2,0) << ", " << value.vector(3,0) <<  " ) -- ";
//            std::cout << value.evaluation_point << " -- " <<  value.chisquared;
//            printf(" -- %p", value.data);
//            std::cout << std::endl;
//        }
        
        track_mc.push_back( track{ particle_m, total_chisquared, std::move(value_c)} );
    }
    
    std::vector< track > shear_suboptimal_tracks( std::vector<track>&& track_pc )
    {
//        std::cout << "shear_suboptimal_tracks: " << track_pc.size() << '\n';
        
        std::vector< track > final_track_c;
        final_track_c.reserve( track_pc.size() );
        
        std::vector<data_type const *> end_point_ch;
        end_point_ch.reserve( track_pc.size() );
        
        for( auto & track : track_pc ){
            auto const * end_point_h = track.get_clusters().back().data;
            if( std::none_of( end_point_ch.begin(), end_point_ch.end(),
                             [&end_point_h]( data_type const * data_ph)
                             { return data_ph == end_point_h; }           ) )
            {
                end_point_ch.push_back( end_point_h );
            }
        }
        
//        std::cout << "end_point: " << end_point_ch.size() << '\n';
        
        for( auto const * end_point_h : end_point_ch ){
//            printf("looking_at: %p \n", end_point_h);
            
            auto end_iterator = std::partition( track_pc.begin(), track_pc.end(),
                                                [&end_point_h](track const & track_p)
                                                { return track_p.get_clusters().back().data == end_point_h; } );
            
            std::sort( track_pc.begin(), end_iterator,
                       [](track const & track1_p, track const & track2_p)
                      { return track1_p.total_chisquared < track2_p.total_chisquared ; } );
            
//            std::cout << "track_selected \n";
//            auto& track = track_pc.front();
//            for( auto const & cluster : track.get_clusters() ){
//                std::cout << "( " << cluster.vector(0,0) <<  ", " << cluster.vector(1,0) <<  " ) -- ( ";
//                std::cout << cluster.vector(2,0) << ", " << cluster.vector(3,0) <<  " ) -- ";
//                std::cout << cluster.evaluation_point << " -- " <<  cluster.chisquared << '\n';
//            }
            
            final_track_c.push_back( track_pc.front() );
            
            track_pc.erase(track_pc.begin(), end_iterator);
        }
        
//        std::cout << "final_track: " << final_track_c.size() << std::endl;
        
        return final_track_c;
    }
    
    
    void register_tracks_upward( std::vector<track> track_pc )
    {
//        std::cout << "register_tracks_upward: " << track_pc.size() << std::endl;
        
        
        for( auto & track : track_pc  ) {
            
//            std::cout << "particle_mass: " << track.particle.mass << std::endl;
            
            auto * track_h = reconstructed_track_mhc->NewTrack( track.particle.mass * 0.938 ,
                                                  track.particle.momentum / 1000.,
                                                  static_cast<double>(track.particle.charge),
                                                  1.1   ); //tof value is wrong
            

            auto & value_c = track.get_clusters();
            checker_m.register_reconstructed_track( value_c );
            
//                    std::cout << " --- final_track --- " << '\n';
            for(auto& value : value_c){
                //
//                std::cout << "( " << value.vector(0,0) <<  ", " << value.vector(1,0) <<  " ) -- ( " <<value.vector(2,0) << ", " << value.vector(3,0) <<  " ) -- " << value.evaluation_point << " -- " <<  value.chisquared << std::endl;
//                ////
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
            
        
    }
    
    
    

    
};




template<class UKF, class DetectorList>
auto make_TATOEactGlb(UKF ukf_p, DetectorList list_p, TAGntuGlbTrack* track_phc, TAGparGeo* global_parameters_ph)
        ->TATOEactGlb<UKF, DetectorList>
{
    return {std::move(ukf_p), std::move(list_p), track_phc, global_parameters_ph};
}



template<class UKF, class DetectorList>
auto make_new_TATOEactGlb(UKF ukf_p, DetectorList list_p, TAGntuGlbTrack* track_phc, TAGparGeo* global_parameters_ph)
        -> TATOEactGlb<UKF, DetectorList> *
{
    return new TATOEactGlb<UKF, DetectorList>{std::move(ukf_p), std::move(list_p), track_phc, global_parameters_ph};
}


#endif










