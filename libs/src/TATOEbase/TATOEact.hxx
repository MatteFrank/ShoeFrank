

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
#include "TATOEmomentum.hxx"

#include "grkn_data.hpp"
//#include "stepper.hpp"
template< class Callable, class Data >
struct stepper;
template<class Data, class Callable>
constexpr auto make_stepper(Callable c_p) -> stepper<Callable, Data>;


template<class OperatingType, std::size_t Order, class Callable>
struct ode;
template<class OperatingType, std::size_t Order, class Callable>
auto make_ode(Callable&& c_p) -> ode<OperatingType, Order, Callable>;



//______________________________________________________________________________
//

#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"

#include "TAVTcluster.hxx"
#include "TAVTntuVertex.hxx"

#include "TAGntuGlbTrack.hxx"
#include "TAGcluster.hxx"
#include "TAGparGeo.hxx"
#include "TAITcluster.hxx"




template<class T>
class TD;



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
    using full_state = aggregator< corrected_state, data_handle<data_type>, step_register >;
    
    using node_type = node<full_state>;
    
    
    struct track{
        
        particle_properties particle;
        
        double total_chisquared;
        
        std::vector<full_state> cluster_c;
        std::vector<full_state>& get_clusters() { return cluster_c; }
        std::vector<full_state> const & get_clusters() const { return cluster_c; }
        
        double momentum{0};
        double tof{0};
        double length{0};
        
        TAGtrack::polynomial_fit_parameters parameters;
        
        std::size_t clone{0};
    };
    
private:
    particle_properties particle_m{};
    UKF ukf_m;
    detector_list_t list_m;
    TAGntuGlbTrack* reconstructed_track_mhc;
    TADIgeoField const* field_mh;
    
    double beam_energy_m;
    int beam_mass_number_m;
    double target_position_m;
    double st_position_m;
    checker<TATOEactGlb> checker_m;
    TATOElogger logger_m;
    

    node_type const * current_node_mh;
    std::size_t event{0};
    
    std::vector<track> track_mc;
public:
    
    TATOEactGlb( UKF&& ukf_p,
                 detector_list_t&& list_p,
                 TAGntuGlbTrack* track_phc,
                 TAGparGeo const * global_parameters_ph,
                 TADIgeoField const* field_ph,
                 bool use_checker = false) :
        ukf_m{ std::move(ukf_p) },
        list_m{ std::move(list_p) },
        reconstructed_track_mhc{ track_phc },
        beam_energy_m{ global_parameters_ph->GetBeamPar().Energy * 1000 }, //Mev/u
        beam_mass_number_m{ static_cast<int>(global_parameters_ph->GetBeamPar().AtomicMass) },
        target_position_m{ global_parameters_ph->GetTargetPar().Position.Z() },
        st_position_m{ static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()))->GetSTCenter().Z() },
    field_mh{field_ph},
        checker_m{ empty_checker<TATOEactGlb>{} }
    {
        if( use_checker ){ checker_m = TATOEchecker<TATOEactGlb>{global_parameters_ph, *this};}
        ukf_m.call_stepper().ode.model().particle_h = &particle_m;
    }
  
    void Output() override {
        checker_m.compute_results( details::all_mixed_tag{} );
        checker_m.compute_results( details::all_separated_tag{} );
//        logger_m.freeze_everything();
        logger_m.output();
    }
    
    void RegisterHistograms() override {
        checker_m.register_histograms( details::all_separated_tag{} );
    }
    
    void Action() override {
        
        
        ++event;
        logger_m.clear();
        checker_m.start_event();
        
        auto hypothesis_c = form_hypothesis();
        
//        std::cout << "hypothesis: " << hypothesis_c.size() << '\n';
        
        for(auto & hypothesis : hypothesis_c){
            particle_m = hypothesis;
//            std::cout << "hypothesis: " << hypothesis.charge << " - " << hypothesis.mass << " - " << hypothesis.momentum << " -- " << hypothesis.get_end_points().size() << '\n';
            reconstruct();
        }
        
        logger_m.add_root_header( "END_RECONSTRUCTION" );
        auto track_c = shear_suboptimal_tracks( std::move(track_mc) );
//        track_c = compute_momentum_old( std::move(track_c) );
        track_c = compute_momentum_new( std::move(track_c) );
        register_tracks_upward( std::move( track_c ) );
        
        checker_m.end_event();
//        logger_m.freeze_everything();
//        logger_m.output();
        
        
        
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
    void reset_event_number() override { event = 0 ; }
    void set_cuts( details::vertex_tag, double cut_p) override {
        list_m.template set_cuts<detector_properties<details::vertex_tag>>( cut_p );
    }
    void set_cuts( details::it_tag, std::array<double, 2> const& cut_pc) override {
        list_m.template set_cuts<detector_properties<details::it_tag>>( cut_pc );
    }
    void set_cuts( details::msd_tag, std::array<double, 3> const& cut_pc) override {
        list_m.template set_cuts<detector_properties<details::msd_tag>>( cut_pc );
    }
    void set_cuts( details::tof_tag, double cut_p) override  {
        list_m.template set_cuts<detector_properties<details::tof_tag>>( cut_p );
    }
    void set_cuts( details::ms2d_tag, double cut_p) override  {
        list_m.template set_cuts<detector_properties<details::ms2d_tag>>( cut_p );
    }
    
    reconstruction_result retrieve_results( ) override {
        return checker_m.retrieve_results( );
    }
    
    std::vector<particle_properties> form_hypothesis()
    {
        
        logger_m.add_root_header( "FORM_HYPOTHESIS" );
        logger_m << "event: "<< event << '\n';
//        std::cout << "event: "<< event << '\n';
        auto tof = list_m.last();

//        std::cout << "form_hypothesis:\n";
        auto candidate_c = tof.generate_candidates();
        std::vector<particle_properties> hypothesis_c;
        hypothesis_c.reserve( candidate_c.size()*2 );

//        std::cout << "candidates: " << candidate_c.size() << '\n';
        for( const auto& candidate : candidate_c ) {
            auto charge = candidate.data->GetChargeZ();
            if(charge == 0) {continue;}
            
            logger_m.add_header<1>("candidate");
            logger_m << "charge: " << charge << '\n';
            
//            auto* data_h =  static_cast<TAMCntuPart*>( gTAGroot->FindDataDsc( "eveMc" )->Object() );
//            std::cout << "TAMCntuPart: " << data_h->GetBranchName() << "\n";
//            logger_m << "id_charge_couple: ";
//            for( int i{0} ; i < candidate.data->GetMcTracksN() ; ++ i){
//                auto index = candidate.data->GetMcTrackIdx(i);
//                logger_m << "(" << index << ", ";
//                logger_m << data_h->GetTrack( index )->GetCharge() << ")";
//            }
//            logger_m << '\n';
//
            // -----------------------------
            checker_m.submit_reconstructible_track( candidate );
            // -----------------------------
            
            auto add_current_end_point = [&candidate]( particle_properties & hypothesis_p  )
                                 { hypothesis_p.get_end_points().push_back( candidate.data ); };
//
            



            auto first_matching_hypothesis_i = std::find_if( hypothesis_c.begin(), hypothesis_c.end(),
                                                          [&charge]( particle_properties const & h_p ){ return h_p.charge == charge; } );

            if( first_matching_hypothesis_i != hypothesis_c.end() ) {
                std::for_each( first_matching_hypothesis_i, hypothesis_c.end(),
                              [&charge, &candidate, &add_current_end_point]( particle_properties & h_p ){
                                  if( h_p.charge == charge ){ add_current_end_point( h_p ); }
                              } );
                break;
            }
        
            
            auto add_hypothesis = [&]( int mass_number_p,
                                        double light_ion_boost_p = 1,
                                        double energy_modifier_p = 1 )
                        {
                   // auto true_momentum = checker_m.retrieve_momentum( candidate );
                   // auto true_mass = checker_m.retrieve_mass( candidate );
                  //  auto true_charge = checker_m.retrieve_charge( candidate );
                  //  logger_m << "momentum: " << momentum <<  '\n';
                  //  logger_m << "real_momentum: " << true_momentum << '\n';
                 //   if( true_momentum > 0. ){ momentum = true_momentum;}
                   // if( true_charge > 0. ){ charge = true_charge;}
                 //   if( true_mass > 0. ){ mass_number_p = true_mass;}
                auto momentum = sqrt( pow(beam_energy_m * mass_number_p, 2)  +
                                      2 *  (beam_energy_m * mass_number_p) * (938 * mass_number_p)  ) *
                                      energy_modifier_p;
                                          
                hypothesis_c.push_back( particle_properties{ charge, mass_number_p, momentum, light_ion_boost_p } );
                add_current_end_point( hypothesis_c.back() );
                        };
            
            switch(charge){
                case 1:
                {
                    auto light_ion_boost = 2.5;
                    add_hypothesis(1, light_ion_boost);
                    add_hypothesis(1, light_ion_boost, 0.5);
//                     light_ion_boost = 1.3;
                    add_hypothesis(2, light_ion_boost);
                    add_hypothesis(3);
                    break;
                }
                case 2:
                {
                    add_hypothesis(4);
                    add_hypothesis(3);
                    break;
                }
                case 3 :
                {
                    add_hypothesis(6);
                    add_hypothesis(7);
                    add_hypothesis(8);
                    break;
                }
                case 4 :
                {
                    add_hypothesis(7);
                    add_hypothesis(9);
                    add_hypothesis(10);
                    break;
                }
                case 5 :
                {
                    add_hypothesis(10);
                    add_hypothesis(11);
                    break;
                }
                case 6 :
                {
                    add_hypothesis(11);
                    add_hypothesis(12);
                    add_hypothesis(13);
                    break;
                }
                case 7:
                {
                    add_hypothesis(14);
                    add_hypothesis(15);
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
        
        return hypothesis_c;
    }
    

    corrected_state generate_corrected_state( TAVTvertex const * vertex_ph,
                                              TAVTbaseCluster const * cluster_ph )
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>(
                                     gTAGroot->FindAction( TAGgeoTrafo::GetDefaultActName().Data() )
                                                            );
        
        auto start = transformation_h->FromVTLocalToGlobal( vertex_ph->GetPosition() );
        auto end = transformation_h->FromVTLocalToGlobal( cluster_ph->GetPositionG() );
        
        auto length = end - start;
        
        auto track_slope_x = length.X()/length.Z();
        auto track_slope_y = length.Y()/length.Z();
        
        
        auto length_error_x = sqrt( pow( vertex_ph->GetPosError().X(), 2 ) +
                                    pow( cluster_ph->GetPosErrorG().X(), 2)             );
        auto length_error_y = sqrt( pow( vertex_ph->GetPosError().Y(), 2 ) +
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
    
    std::vector<corrected_state> generate_corrected_state( TAMSDpoint const * msd_ph )
    {
        std::vector<corrected_state> result_c;

        auto * transformation_h = static_cast<TAGgeoTrafo*>(
                                     gTAGroot->FindAction( TAGgeoTrafo::GetDefaultActName().Data() )
                                                            );
        
        auto start = transformation_h->FromMSDLocalToGlobal( msd_ph->GetPositionG() );
        
        auto tof = list_m.last();
        auto candidate_c = tof.generate_candidates();
        result_c.reserve( candidate_c.size() );
        
        for(auto && candidate : candidate_c) {
            auto end = transformation_h->FromTWLocalToGlobal( candidate.data->GetPositionG() );
        
            auto length = end - start;
        
            auto track_slope_x = length.X()/length.Z();
            auto track_slope_y = length.Y()/length.Z();
        
        
            auto length_error_x = sqrt( pow( msd_ph->GetPosError().X(), 2 ) +
                                       pow( candidate.data->GetPosErrorG().X(), 2)             );
            auto length_error_y = sqrt( pow( msd_ph->GetPosError().Y(), 2 ) +
                                       pow( candidate.data->GetPosErrorG().Y(), 2)           );
        
        
            auto track_slope_error_x = abs( track_slope_x ) *
                                       sqrt( pow( length_error_x/length.X(), 2) +
                                            pow( 0.05/length.Z(), 2)              );
            auto track_slope_error_y = abs( track_slope_y ) *
                                        sqrt( pow( length_error_y/length.Y(), 2) +
                                              pow( 0.05/length.Z(), 2)               );
        
        
            using vector = typename underlying<state>::vector;
            using covariance = typename underlying<state>::covariance;
        
            result_c.push_back(
                        corrected_state{
                            state{
                                evaluation_point{ start.Z() },
                                vector{{ start.X(), start.Y(), track_slope_x, track_slope_y }},
                                covariance{{   pow(length_error_x, 2  ),     0,    0,    0,
                                                0,     pow(length_error_y, 2),      0,    0,
                                                0,    0,   pow(track_slope_error_x, 2),   0,
                                                0,    0,    0,   pow( track_slope_error_y, 2)  }}
                                },
                            chisquared{0}
                                        }
                               );
        }
        return result_c;
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
    
    void cross_check_origin( arborescence< node_type >& /*arborescence_p*/,
                             const detector_properties<details::ms2d_tag>& /*vertex_p*/ )
    {}
    
    //cross_check_origin msd -> same as vertex
    
    //-------------------------------------------------------------------------------------
    //                            advance_reconstruction_impl
    // - should be the same, always : only the confrontation part differs !
    
    
    template< class T >
    std::vector<full_state> advance_reconstruction_impl( state s_p,
                                                         const T& layer_p )
    {
        logger_m.add_sub_header("advance_reconstruction_impl");
//        std::cout << "\nstarting_state : ( " << s_p.vector(0,0) << ", " << s_p.vector(1,0) ;
//        std::cout << ") -- (" << s_p.vector(2,0) << ", " << s_p.vector(3,0)  ;
//        std::cout << ") -- " << s_p.evaluation_point << '\n';
        logger_m << "\nstarting_state : ( " << s_p.vector(0,0) << ", " << s_p.vector(1,0) ;
        logger_m << ") -- (" << s_p.vector(2,0) << ", " << s_p.vector(3,0)  ;
        logger_m << ") -- " << s_p.evaluation_point << '\n';

        
        auto sigma_c = ukf_m.generate_sigma_points( s_p );
        
        sigma_c = ukf_m.propagate_while(
                                        std::move(sigma_c),
                                        [this, &layer_p](const auto& os_p)
                                        { return os_p.evaluation_point + ukf_m.step_length() < layer_p.depth ; }
                                        );
        
        auto step = layer_p.depth - sigma_c.front().evaluation_point;
        
        if(step<0){ logger_m << "<Warning>::going_backwards\n"; logger_m.freeze() ; std::cout << "WARNING : going backwards !" << std::endl; }
        
        sigma_c = ukf_m.force_propagation( std::move(sigma_c), step );
        auto ps = ukf_m.generate_propagated_state( std::move(sigma_c) );
        
        
//
        
//        std::cout << "propagated_state : ( " << ps.vector(0,0) << ", " << ps.vector(1,0) ;
//        std::cout << ") -- (" << ps.vector(2,0) << ", " << ps.vector(3,0)  ;
//        std::cout << ") -- " << ps.evaluation_point << '\n';
//
        

        logger_m << "propagated_state : ( " << ps.vector(0,0) << ", " << ps.vector(1,0) ;
        logger_m << ") -- (" << ps.vector(2,0) << ", " << ps.vector(3,0)  ;
        logger_m << ") -- " << ps.evaluation_point << '\n';
        
        
        return confront(ps, layer_p);
    }
    

    
    //-------------------------------------------------------------------------------------
    //                            advance_reconstruction overload set
    
    template< class T   >
    void advance_reconstruction( arborescence<node_type>& arborescence_p,
                                 const T& detector_p )
    {
        
        logger_m.add_root_header("ADVANCE_RECONSTRUCTION");
//        std::cout << "ADVANCE_RECONSTRUCTION\n";
        
        auto layer_c = detector_p.form_layers();
        
        for(auto && layer : layer_c ) {
        
            auto& leaf_c = arborescence_p.get_handler();
            logger_m.add_header<1>("layer");
        
            if( layer.get_candidates().empty() ){ continue; }
            
            
            for(auto& leaf : leaf_c){
                
                logger_m.add_header<2>( "leaf" );
                
                // -----------------------------
                checker_m.update_current_node( &leaf );
                checker_m.output_current_hypothesis();
                // -----------------------------
            
                ukf_m.step_length() = 1e-3;
            
                auto s = make_state( leaf.get_value() );
                auto fs_c = advance_reconstruction_impl( s, layer );
            
                
                for( auto & fs : fs_c ){
//
                    fs.step_length = compute_step_length( s, fs );
                    
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
//        std::cout << "FINALISE_RECONSTRUCTION\n";
//        std::cout << "advance_reconstruction<tof>:\n";
        auto& leaf_c = arborescence_p.get_handler();
        
        auto const& layer = tof_p.form_layer();
        
        for(auto& leaf : leaf_c){
        
            logger_m.add_header<1>("leaf");
            
            // -----------------------------
            checker_m.update_current_node( &leaf );
            checker_m.output_current_hypothesis();
            // -----------------------------
            
            ukf_m.step_length() = 1e-3;
            
            auto s = make_state(leaf.get_value());
            
            auto fs_c = advance_reconstruction_impl( s, layer );
            
            if( fs_c.empty() ){ leaf.mark_invalid(); }
            else{
                for( auto&& fs : fs_c ){
                
                    fs.step_length = compute_step_length( s, fs );
                    
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
//        std::cout << "START_RECONSTRUCTION\n";
        
        auto track_c = vertex_p.get_track_list( );
        for( auto& track : track_c ){
            auto vertex_h = track.vertex();
            auto first_h = track.first_cluster();
            
            auto cs = generate_corrected_state( vertex_h, first_h );
            auto fs = full_state{
                std::move(cs),
                data_handle<data_type>{nullptr},
                step_register{}
            };
            
            auto * leaf_h = arborescence_p.add_root( std::move(fs) );
            // -----------------------------
            checker_m.update_current_node( leaf_h );
            checker_m.output_current_hypothesis();
            // -----------------------------
            logger_m.add_header<1>( "vertex_track_reconstruction" );
            
            
            for( auto layer : track ){
                
                ukf_m.step_length() = 1e-3;
                
                auto s = make_state( leaf_h->get_value() );
                auto fs_c = advance_reconstruction_impl( s, layer );
                
                if( !fs_c.empty() ){
                    auto fs = fs_c.front();
                    
                    fs.step_length = compute_step_length( s, fs );
//
                    logger_m << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                    logger_m << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                    logger_m << ") -- " << fs.evaluation_point << " -- " << fs.chisquared << '\n';

//                    std::cout << " in [" << fs.vector(0, 0) << ", " << fs.vector(1, 0) << ", " << fs.evaluation_point << "]\n";
//                    TVector3 reco_position{
//                        fs.vector(0, 0),
//                        fs.vector(1,0),
//                        fs.evaluation_point
//                    };
//                    auto const field = field_mh->GetField(reco_position);
//                    std::cout << " field: [" << field.X() << ", " << field.Y() << ", " << field.Z() << "]\n";
                    
                    
                    leaf_h = leaf_h->add_child( std::move(fs_c.front()) );
                    // -----------------------------
                    checker_m.update_current_node( leaf_h );
                    // -----------------------------
                }
                
                
                
            }
            
//            std::cout << "cluster_added: " << leaf_h->depth()-1 << '\n';
//            std::cout << "cluster_intrack: " << track.size() << '\n';
            if( leaf_h->depth()-1 < track.size()-1 ){ leaf_h->mark_invalid(); }
            
        }
        
    }
    
    
    
     void advance_reconstruction( arborescence<node_type>& arborescence_p,
                                  const detector_properties<details::ms2d_tag>& ms2d_p )
     {
         
         logger_m.add_root_header("START_RECONSTRUCTION");
 //        std::cout << "START_RECONSTRUCTION\n";
         
         auto layer_c = ms2d_p.form_layers();
         auto generating_candidate_c = layer_c.generating_candidates();
            
         for( auto&& candidate : generating_candidate_c ){
             auto cs_c = generate_corrected_state( candidate );
             
             for( auto && cs : cs_c){
                 auto fs = full_state{
                    std::move(cs),
                    data_handle<data_type>{candidate.data},
                    step_register{}
                };
             
                 auto * leaf_h = arborescence_p.add_root( std::move(fs) );
             // -----------------------------
                 checker_m.update_current_node( leaf_h );
                 checker_m.output_current_hypothesis();
             }
         }
         
         for(auto && layer : layer_c) {
             logger_m.add_header<1>("layer");
         
             if( layer.get_candidates().empty() ){ continue; }
             
             auto& leaf_c = arborescence_p.get_handler();
             for(auto& leaf : leaf_c){
                 
                 logger_m.add_header<2>( "leaf" );
                 
                 // -----------------------------
                 checker_m.update_current_node( &leaf );
                 checker_m.output_current_hypothesis();
                 // -----------------------------
             
                 ukf_m.step_length() = 1e-3;
             
                 auto s = make_state( leaf.get_value() );
                 auto fs_c = advance_reconstruction_impl( s, layer );
             
                 for( auto & fs : fs_c ){
 //
                     fs.step_length = compute_step_length( s, fs );
                     
                     logger_m << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                     logger_m << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                     logger_m << ") -- " << fs.evaluation_point << " -- " << fs.chisquared << '\n';
 //
                     leaf.add_child( std::move(fs) );
                 }

             }
  
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
    
        logger_m.add_sub_header(  "confront" );
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
                data_handle<data_type>{ iterator->data },
                step_register{}  };
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
        logger_m.add_sub_header(  "confront" );
        
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
                                  data_handle<data_type>{ iterator->data } ,
                                  step_register{}
            };
            
            fs_c.push_back( std::move(fs) );
        }

        return fs_c;
    }
    


    std::vector<full_state> confront( const state& ps_p,
                                      const track_list< detector_properties<details::vertex_tag> >::pseudo_layer& layer_p )//optionnal is more relevant here
    {
        using candidate = typename detector_properties< details::vertex_tag >::candidate;
        using enriched_candidate = enriched_candidate_impl< candidate >;
    
        logger_m.add_sub_header(  "confront" );
        
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
                                      data_handle<data_type>{ ec.data },
                step_register{}
            };
            
            fs_c.push_back( std::move(fs) );
            
        }
    
        return fs_c;
    }
           
           

    template<class Enriched, class T>
    bool pass_selection( Enriched const& ec_p, const state& ps_p, T const& layer_p )
    {
        logger_m.add_sub_header( "pass_selection" );
        
        auto error = ec_p.data->GetPosError();
        logger_m << "error: (" << error.X() << ", " << error.Y() << ")\n";
        
        
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
        
        logger_m << "cluster_mc_id: ";
        for( int i{0} ; i < ec_p.data->GetMcTracksN() ; ++ i){
            logger_m << ec_p.data->GetMcTrackIdx(i) << " ";
        }
        logger_m << '\n';
        
        return ec_p.chisquared < cutter.chisquared;
    }
    
    template<class Enriched>
    bool pass_selection( Enriched const& ec_p,
                         state const& ps_p,
                         layer_generator<detector_properties<details::msd_tag>>::layer const& layer_p )
    {
        logger_m.add_sub_header( "pass_selection" );
        
        auto error = ec_p.data->GetPosError();
        matrix<1,1> v = ec_p.measurement_matrix * ps_p.vector;
        if( ec_p.measurement_matrix(0,0) > 0 ) {
            logger_m << " -- x orientation -- \n";
            logger_m << "error: (" << error.X() << ", " << error.Y() << ")\n";
            v(0,0) += layer_p.cut * particle_m.light_ion_boost * error.X();
        }
        else {
            logger_m << " -- y orientation -- \n";
            logger_m << "error: (" << error.X() << ", " << error.Y() << ")\n";
            v(0,0) += layer_p.cut * particle_m.light_ion_boost * error.Y();
        }
        
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
        logger_m << "cutter : (" << cutter_candidate.vector(0, 0) << ")\n";
        logger_m << "cutter_chisquared : " << cutter.chisquared << '\n';
        logger_m << "candidate : (" << ec_p.vector(0, 0) << ")\n";
        logger_m << "candidate_chisquared : " << ec_p.chisquared << '\n';

        
        logger_m << "cluster_mc_id: ";
        for( int i{0} ; i < ec_p.data->GetMcTracksN() ; ++ i){
            logger_m << ec_p.data->GetMcTrackIdx(i) << " ";
        }
        logger_m << '\n';
        
        
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
        double shearing_factor = total_chisquared / value_c.size();
        
//        track_mc.push_back( track{ particle_m, shearing_factor, 0, 0, std::move(value_c)} );
        track_mc.push_back( track{ particle_m, shearing_factor, std::move(value_c) } );
    }
    
    std::vector< track > shear_suboptimal_tracks( std::vector<track>&& track_pc )
    {
        
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

        
        for( auto const * end_point_h : end_point_ch ){
            
            auto end_iterator = std::partition( track_pc.begin(), track_pc.end(),
                                                [&end_point_h](track const & track_p)
                                                { return track_p.get_clusters().back().data == end_point_h; } );
            
            std::sort( track_pc.begin(), end_iterator,
                       [](track const & track1_p, track const & track2_p)
                      { return track1_p.total_chisquared < track2_p.total_chisquared ; } );

            final_track_c.push_back( track_pc.front() );
            final_track_c.back().clone = std::distance(track_pc.begin(), end_iterator);
            
            track_pc.erase(track_pc.begin(), end_iterator);
        }
        
        return final_track_c;
    }
    
    
    
    
    //-------------------------------------------------------------------------------------
    //                           compute_step_length
    
    constexpr double compute_step_length( state const& s_p,
                                         full_state const& fs_p) const
    {
        double x = fs_p.vector(0,0) - s_p.vector(0,0);
        double y = fs_p.vector(0,1) - s_p.vector(0,1);
        double z = fs_p.evaluation_point - s_p.evaluation_point;
        return sqrt( pow(x, 2) + pow(y, 2) + pow(z, 2) );
    }
    
    //-------------------------------------------------------------------------------------
    //                      compute_momemtum
    
    constexpr std::vector< track > compute_momentum_old( std::vector<track>&& track_pc ) const {
        for( auto& track : track_pc) {
            auto && cluster_c = track.get_clusters();
        
            double total_step_length{0};
            for( auto && cluster : cluster_c ){
                total_step_length += cluster.step_length;
            }
        
            double beam_speed = sqrt( pow(beam_energy_m*beam_mass_number_m, 2) + 2 * beam_mass_number_m * beam_mass_number_m * 938 * beam_energy_m )/(beam_mass_number_m * 938 + beam_mass_number_m * beam_energy_m) * 30;
//            double additional_time = (target_position_m - st_position_m)/beam_speed;
            double additional_time = (cluster_c.front().evaluation_point - st_position_m)/beam_speed;
        
            double speed = total_step_length/(static_cast<TATWpoint const *>(cluster_c.back().data)->GetToF() - additional_time);
            double beta = speed/30;
            double gamma = 1./sqrt(1 - pow(beta, 2));
            double momentum = gamma * 938 * track.particle.mass * beta;
        
            track.momentum = momentum;
            
       //     std::cout << "arc_length: " << total_step_length << '\n';
       //     std::cout << "momentum: " << momentum << '\n';
        }
        
        return std::move( track_pc );
    }
    
    
    template<std::size_t N>
    double compute_arc_length( std::vector<full_state> const& cluster_pc ) const {
        
        //fit in x/y
        //retrieve fit parameters
        //compute arc-length through integration
        
        std::array<double, N> z_c{};
        
        std::array<double, N> x_c{};
        std::array<double, N * N> weight_x_c{};
        
        std::array<double, N> y_c{};
        std::array<double, N * N> weight_y_c{};
        
        //zip !
        for( std::size_t i{0}; i < N; ++i ) {
            auto const& cluster = cluster_pc[i];
            
            z_c[i] = cluster.evaluation_point;
            
            
            
            x_c[i] = cluster.vector(0, 0);
            y_c[i] = cluster.vector(1, 0);
            
        //    std::cout << "z: " << z_c[i] << " - x: " << x_c[i] << '\n';
    
            weight_x_c[i + i*N] = cluster.covariance(0, 0);
            weight_y_c[i + N*i] = cluster.covariance(1, 1);
        }
        
        
        constexpr std::size_t const order_x = 4;
        auto const regressor_x = make_custom_matrix<N, order_x>(
                           [&z_c, &order_x]( std::size_t index_p ){
                           std::size_t column_index = index_p % order_x;
                           std::size_t row_index = index_p / order_x;
                           return pow( z_c[row_index], column_index );
                       }
                                                          );
        auto const observation_x = matrix<N, 1>{ std::move(x_c) };
        auto const weight_x = matrix<N, N>{ std::move(weight_x_c)};
        
        //computation splitted to reduce instantiation depth (not allowed over 900 for gcc by default)
        auto const part1_x = form_inverse( expr::compute( transpose(regressor_x) * weight_x * regressor_x ) );
        auto const part2_x = expr::compute( transpose( regressor_x ) * weight_x * observation_x );
        auto const parameter_x = expr::compute( part1_x * part2_x );
//        auto const parameter_x = expr::compute( form_inverse( expr::compute( transpose(regressor_x) * weight_x * regressor_x ) ) * transpose( regressor_x ) * weight_x * observation_x );
        
     //   std::cout << "parameter_x: \n" << parameter_x;
        
        
        constexpr std::size_t const order_y = 2;
        auto const regressor_y = make_custom_matrix<N, order_y>(
                           [&z_c, &order_y]( std::size_t index_p ){
                           std::size_t column_index = index_p % order_y;
                           std::size_t row_index = index_p / order_y;
                           return pow( z_c[row_index], column_index );
                       }
                                                          );
        auto const observation_y = matrix<N, 1>{ std::move(y_c) };
        auto const weight_y = matrix<N, N>{ std::move(weight_y_c)};
        
        //computation splitted to reduce instantiation depth (not allowed over 900 for gcc by default)
            auto const part1_y = form_inverse( expr::compute( transpose(regressor_y) * weight_y * regressor_y ) );
            auto const part2_y = expr::compute( transpose( regressor_y ) * weight_y * observation_y );
            auto const parameter_y = expr::compute( part1_y * part2_y );
//        auto const parameter_y = expr::compute( form_inverse( expr::compute( transpose(regressor_y) * weight_y * regressor_y ) ) * transpose( regressor_y ) * weight_y * observation_y );
        
     //   std::cout << "parameter_y: \n" << parameter_y;
        
        
        // ====================  arc length computation ===================
        constexpr std::size_t const exponent = 5; //max exponent of z in derivatives

        
        // ------------ mixing_matrices ----------------
        auto const mixing_x = make_custom_matrix<exponent, order_x>(
                       [&order_x]( std::size_t index_p ){
                           std::size_t column_index = index_p % order_x;
                           std::size_t row_index = index_p / order_x;
                           return static_cast<double>( ( column_index == row_index+1 ) ?  column_index : 0. );
                       }
                                                                    );
//        std::cout << "mixing_x: \n" << mixing_x;
        
        auto const mixing_y = make_custom_matrix<exponent, order_y>(
                       [&order_y]( std::size_t index_p ){
                           std::size_t column_index = index_p % order_y;
                           std::size_t row_index = index_p / order_y;
                           return static_cast<double>( ( column_index == row_index+1 ) ?  column_index : 0. );
                       }
                                                                    );
//        std::cout << "mixing_y: \n" << mixing_y;
        
        //------------ "ode" ----------------
        auto ode = make_ode<double, 1>(
              [&parameter_x, &mixing_x, &parameter_y, &mixing_y, exponent]
              (operating_state<double, 1> const& os_p){
                  auto const polynomial = make_custom_matrix<1, exponent>(
                        [value = os_p.evaluation_point, &exponent]( std::size_t index_p ){
                            std::size_t column_index = index_p % exponent;
                            return pow( value, column_index );
                        }
                                                                          );
                  double const dx_dz = expr::compute( polynomial * mixing_x * parameter_x );
                  double const dy_dz = expr::compute( polynomial * mixing_y * parameter_y );
                  
                  return sqrt( pow(dx_dz, 2) + pow(dy_dz, 2) + 1 );
              }
                                       );
        
        //------------- numerical_integration -------------------
        auto stepper = make_stepper<data_rkf45>( std::move(ode) );
        auto os = operating_state<double, 1>{ z_c[0], 0 };
        stepper.specify_tolerance(1e-5);
        
        auto step = 1e-3;
        while( os.evaluation_point + step < z_c[N-1] ){
            auto step_result = stepper.step( std::move(os), step );
            if( step_result.second != 0 ){
                step = stepper.optimize_step_length(step, step_result.second);
            }
            os = std::move(step_result.first);
        }
        step = z_c[N-1] - os.evaluation_point;
        os = stepper.force_step( std::move(os), step );
        
        return os.state( details::order_tag<0>{} );
//        return 0;
    }
    
    template<std::size_t N>
    TAGtrack::polynomial_fit_parameters compute_polynomial_parameters( std::vector<full_state> const& cluster_pc ) const {
        
        //fit in x/y
        //retrieve fit parameters
        
        std::array<double, N> z_c{};
        
        std::array<double, N> x_c{};
        std::array<double, N * N> weight_x_c{};
        
        std::array<double, N> y_c{};
        std::array<double, N * N> weight_y_c{};
        
        //zip !
        for( std::size_t i{0}; i < N; ++i ) {
            auto const& cluster = cluster_pc[i];
            
            z_c[i] = cluster.evaluation_point;
            x_c[i] = cluster.vector(0, 0);
            y_c[i] = cluster.vector(1, 0);
            
            weight_x_c[i + i*N] = cluster.covariance(0, 0);
            weight_y_c[i + N*i] = cluster.covariance(1, 1);
        }
        
        
        constexpr std::size_t const order_x = 4;
        auto const regressor_x = make_custom_matrix<N, order_x>(
                                            [&z_c, &order_x]( std::size_t index_p ){
                                                std::size_t column_index = index_p % order_x;
                                                std::size_t row_index = index_p / order_x;
                                                return pow( z_c[row_index], column_index );
                                                                                    }
                                                                );
        auto const observation_x = matrix<N, 1>{ std::move(x_c) };
        auto const weight_x = matrix<N, N>{ std::move(weight_x_c)};
        
        //computation splitted to reduce instantiation depth (not allowed over 900 for gcc by default)
        auto const part1_x = form_inverse( expr::compute( transpose(regressor_x) * weight_x * regressor_x ) );
        auto const part2_x = expr::compute( transpose( regressor_x ) * weight_x * observation_x );
        auto const parameter_x = expr::compute( part1_x * part2_x );
        
        
        constexpr std::size_t const order_y = 2;
        auto const regressor_y = make_custom_matrix<N, order_y>(
                                                                [&z_c, &order_y]( std::size_t index_p ){
                                                                    std::size_t column_index = index_p % order_y;
                                                                    std::size_t row_index = index_p / order_y;
                                                                    return pow( z_c[row_index], column_index );
                                                                }
                                                                );
        auto const observation_y = matrix<N, 1>{ std::move(y_c) };
        auto const weight_y = matrix<N, N>{ std::move(weight_y_c)};
        
        //computation splitted to reduce instantiation depth (not allowed over 900 for gcc by default)
        auto const part1_y = form_inverse( expr::compute( transpose(regressor_y) * weight_y * regressor_y ) );
        auto const part2_y = expr::compute( transpose( regressor_y ) * weight_y * observation_y );
        auto const parameter_y = expr::compute( part1_y * part2_y );
        
        return {std::move(parameter_x.data()), std::move(parameter_y.data())};
    }
    
    constexpr std::vector< track > compute_momentum_new( std::vector<track>&& track_pc ) const {
        for( auto& track : track_pc) {
            
            auto cluster_c = track.get_clusters();
            double arc_length{0};
            switch( cluster_c.size() ){
                case 4:{
                    arc_length = compute_arc_length<4>( cluster_c );
                    track.parameters = compute_polynomial_parameters<4>( cluster_c );
                    break;
                }
                case 5:{
                    arc_length = compute_arc_length<5>( cluster_c );
                    track.parameters = compute_polynomial_parameters<5>( cluster_c );
                    break;
                }
                case 6:{
                    arc_length = compute_arc_length<6>( cluster_c );
                    track.parameters = compute_polynomial_parameters<6>( cluster_c );
                    break;
                }
                case 7:{
                    arc_length = compute_arc_length<7>( cluster_c );
                    track.parameters = compute_polynomial_parameters<7>( cluster_c );
                    break;
                }
                case 8:{
                    arc_length = compute_arc_length<8>( cluster_c );
                    track.parameters = compute_polynomial_parameters<8>( cluster_c );
                    break;
                }
                case 9:{
                    arc_length = compute_arc_length<9>( cluster_c );
                    track.parameters = compute_polynomial_parameters<9>( cluster_c );
                    break;
                }
                case 10:{
                    arc_length = compute_arc_length<10>( cluster_c );
                    track.parameters = compute_polynomial_parameters<10>( cluster_c );
                    break;
                }
                case 11:{
                    arc_length = compute_arc_length<11>( cluster_c );
                    track.parameters = compute_polynomial_parameters<11>( cluster_c );
                    break;
                }
                case 12:{
                    arc_length = compute_arc_length<12>( cluster_c );
                    track.parameters = compute_polynomial_parameters<12>( cluster_c );
                    break;
                }
                case 13:{
                    arc_length = compute_arc_length<13>( cluster_c );
                    track.parameters = compute_polynomial_parameters<13>( cluster_c );
                    break;
                }
                case 14:{
                    arc_length = compute_arc_length<14>( cluster_c );
                    track.parameters = compute_polynomial_parameters<14>( cluster_c );
                    break;
                }
                case 15:{
                    arc_length = compute_arc_length<15>( cluster_c );
                    track.parameters = compute_polynomial_parameters<15>( cluster_c );
                    break;
                }
                case 16:{
                    arc_length = compute_arc_length<16>( cluster_c );
                    track.parameters = compute_polynomial_parameters<16>( cluster_c );
                    break;
                }
                default:{
                    std::cerr << "<Warning> in compute_momentum, size of vector was not a considered case " << cluster_c.size() << '\n';
                }
            }
            

            double beam_speed = sqrt( pow(beam_energy_m*beam_mass_number_m, 2) + 2 * beam_mass_number_m * beam_mass_number_m * 938 * beam_energy_m )/(beam_mass_number_m * 938 + beam_mass_number_m * beam_energy_m) * 30;
//            double additional_time = (target_position_m - st_position_m)/beam_speed;
            double additional_time = (cluster_c.front().evaluation_point - st_position_m)/beam_speed;

            double tof = (static_cast<TATWpoint const *>(cluster_c.back().data)->GetToF() - additional_time);
            double speed = arc_length/tof;
            double beta = speed/30;
            
            if( beta < 1){
                double gamma = 1./sqrt(1 - pow(beta, 2));
                double momentum = gamma * 938 * track.particle.mass * beta;
            
       //     std::cout << "arc_length: " << arc_length << '\n';
       //     std::cout << "momentum: " << momentum << '\n';

                track.momentum = momentum;
            }
            else {
                track.momentum = track.particle.momentum;
            }
            track.tof = tof;
            track.length = arc_length;
        }

        return std::move( track_pc );
    }
    
    
    
    void register_tracks_upward( std::vector<track> track_pc )
    {

        
        std::vector<int> index_c;
        
        for( auto & track : track_pc  ) {
            // -----------------------------
            checker_m.submit_reconstructed_track( track );
            // -----------------------------
            if( !reconstructed_track_mhc ){ continue; }
//            std::cout << "particle_mass: " << track.particle.mass << std::endl;
//            std::cout << "reconstruction_momentum: "<< track.particle.momentum << "\n";
            auto * track_h = reconstructed_track_mhc->NewTrack(
                                                  track.particle.mass * 0.938 ,
                                                  track.momentum / 1000.,
                                                  static_cast<double>(track.particle.charge),
                                                  track.tof
                                                               );
//            std::cout << "registered_momentum: " << track_h->GetMomentum() << "\n";
            track_h->SetParameters( track.parameters );
            
            auto & value_c = track.get_clusters();
            for(auto& value : value_c){
                //
//                std::cout << "( " << value.vector(0,0) <<  ", " << value.vector(1,0) <<  " ) -- ( " <<value.vector(2,0) << ", " << value.vector(3,0) <<  " ) -- " << value.evaluation_point << " -- " <<  value.chisquared << std::endl;
//                ////
                TVector3 corrected_position{ value.vector(0,0), value.vector(1,0), value.evaluation_point };
//                corrected_position.Print();
                
                auto momentum_z = sqrt( pow( value.vector(2,0), 2) + pow( value.vector(3,0), 2) + 1 ) * track.momentum ;
                momentum_z /= 1000.;
                auto momentum_x = value.vector(2,0) * momentum_z;
                auto momentum_y = value.vector(3,0) * momentum_z;
                
                TVector3 momentum{ momentum_x , momentum_y, momentum_z };
                TVector3 position_error{ 0.01 ,0.01, 0.01 };
                TVector3 momentum_error{ 10, 10, 10 };
                
                track_h->AddCorrPoint( corrected_position, position_error, momentum, momentum_error ); //corr point not really meas

                if( value.data ){ //needed because first point is vertex, which has no cluster associated
                    auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));

                    
                    auto fill_measured_point_l = [&value, &transformation_h, &position_error, &momentum, &momentum_error, &track_h](
                                                            auto * cluster_ph,
                                                            std::string&& name_p,
                                                            auto transformation_p
                                                                            ){
                                TVector3 measured_position{ (transformation_h->*transformation_p)(value.data->GetPositionG()) };
                                auto* measured_h = track_h->AddMeasPoint( measured_position, position_error, momentum, momentum_error );
                                measured_h->SetDevName( name_p.c_str() );
                                measured_h->SetClusterIdx( cluster_ph->GetClusterIdx() );
                                measured_h->SetSensorIdx( cluster_ph->GetSensorIdx() );
                                                                      };
                    auto const * vertex_h = dynamic_cast<TAVTcluster const*>( value.data );
                    if( vertex_h ){
                        fill_measured_point_l( vertex_h, TAVTparGeo::GetBaseName(), &TAGgeoTrafo::FromVTLocalToGlobal );
                        continue;
                    }
                    auto const * it_h = dynamic_cast<TAITcluster const*>( value.data );
                    if( it_h ){
                         fill_measured_point_l( it_h, TAITparGeo::GetBaseName(), &TAGgeoTrafo::FromITLocalToGlobal );
                        continue;
                    }
                    auto const * msd_h = dynamic_cast<TAMSDcluster const*>( value.data );
                    if( msd_h ){
                         fill_measured_point_l( msd_h, TAMSDparGeo::GetBaseName(), &TAGgeoTrafo::FromMSDLocalToGlobal );
                        continue;
                    }
                    auto const * tw_h = dynamic_cast<TATWpoint const*>( value.data );
                    if( tw_h ){
                         fill_measured_point_l( tw_h, TATWparGeo::GetBaseName(), &TAGgeoTrafo::FromTWLocalToGlobal );
                        continue;
                    }
                    
                }
            }
            
        }
        
        
    }
    
    
    

    
};





template<class UKF, class DetectorList>
auto new_TATOEactGlb(
            UKF ukf_p,
            DetectorList list_p,
            TAGntuGlbTrack* track_phc,
            TAGparGeo* global_parameters_ph,
            TADIgeoField* field_ph,
            bool use_checker = false
                     ) -> TATOEactGlb<UKF, DetectorList> *
{
    return new TATOEactGlb<UKF, DetectorList>{
                    std::move(ukf_p),
                    std::move(list_p),
                    track_phc,
                    global_parameters_ph,
                    field_ph,
                    use_checker};
}


#endif











