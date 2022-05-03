

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
#include "TATOEdetector.hxx"
#include "TATOEmatcher.hxx"
#include "TATOElogger.hxx"
#include "TATOEmomentum.hxx"
#include "TATOEchecker.hxx"

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


#include "TFile.h"
#include "TTree.h"

#include <random>

template<class T>
class TD;



template<class UKF, class DetectorList>
struct TATOEactGlb {};


template<class UKF, class ... DetectorProperties>
struct TATOEactGlb< UKF, detector_list< details::finished_tag,
                                        DetectorProperties...  >   >
            : TATOEbaseAct
{
    friend class TATOEmatcher<TATOEactGlb, empty_writer>;
    
    using detector_list_t = detector_list< details::finished_tag, DetectorProperties...  >;
    
    using vector_matrix = typename underlying< typename details::filter_traits<UKF>::state >::vector::type ;
    using covariance_matrix = typename underlying< typename details::filter_traits<UKF>::state >::covariance::type ;
    using corrected_state = corrected_state_impl<vector_matrix, covariance_matrix>;
    using state = state_impl<vector_matrix, covariance_matrix>;
    
    using data_type = TAGcluster;
    using full_state = aggregator< corrected_state, data_handle<data_type>, step_register, block_weight_register >;
    
    using node_type = node<full_state>;
    
    
    struct track{
        particle_hypothesis hypothesis;
        
        double total_chisquared;
        
        std::vector<full_state> cluster_c;
        std::vector<full_state>& get_clusters() { return cluster_c; }
        std::vector<full_state> const & get_clusters() const { return cluster_c; }
        
        double momentum{0};
        double mass{0};
        double tof{0};
        double length{0};
        double beta{0};
        std::size_t nucleon_number;
        
        polynomial_fit_parameters parameters;
        double determination_coefficient_scan{0};
        
        std::size_t clone{0};
    };
    
private:
    particle_hypothesis current_hypothesis_m{};
    UKF ukf_m;
    detector_list_t list_m;
    TAGntuGlbTrack* reconstructed_track_mhc;
    TADIgeoField const* field_mh;
    
    double beam_energy_m;
    int beam_mass_number_m;
    double target_position_m;
    double st_position_m;
    matcher<TATOEactGlb> matcher_m;
    TATOElogger logger_m;
    std::vector<computation_checker> computation_checker_mc;
    std::vector<histogram_checker> histogram_checker_mc;

    node_type const * current_node_mh;
    std::size_t event{0};
    
    std::vector<track> track_mc;

    std::random_device random_device_m;
    std::mt19937 random_generator_m{ random_device_m() };
    
public:
    
    TATOEactGlb( UKF&& ukf_p,
                 detector_list_t&& list_p,
                 TAGntuGlbTrack* track_phc,
                 TAGparGeo const * global_parameters_ph,
                 TADIgeoField const* field_ph,
                 bool use_matcher = false) :
        ukf_m{ std::move(ukf_p) },
        list_m{ std::move(list_p) },
        reconstructed_track_mhc{ track_phc },
        beam_energy_m{ global_parameters_ph->GetBeamPar().Energy * 1000 }, //Mev/u
        beam_mass_number_m{ static_cast<int>(global_parameters_ph->GetBeamPar().AtomicMass) },
        target_position_m{ global_parameters_ph->GetTargetPar().Position.Z() },
        st_position_m{ static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()))->GetSTCenter().Z() },
    field_mh{field_ph},
        matcher_m{ empty_matcher<TATOEactGlb>{} }
    {
        if( use_matcher ){
            using namespace checker;
            matcher_m = TATOEmatcher<TATOEactGlb, empty_writer>{global_parameters_ph, *this};
        }
        ukf_m.call_stepper().specify_tolerance(1e-14);
//        fill_histogram_checker<>();
//        fill_histogram_checker<checker::correct_tof_predicate>();
//        fill_histogram_checker<checker::charge_well_matched>();
//        fill_histogram_checker<checker::well_matched>();
//        fill_histogram_checker<checker::badly_matched>();
//        fill_histogram_checker<checker::purity_predicate>();
//        fill_histogram_checker<checker::shearing_predicate>();
//        fill_histogram_checker<checker::beta_predicate>();
//        fill_histogram_checker<checker::out_of_resolution_predicate>();
//        fill_histogram_checker<checker::negative_track_slope_predicate>();
//        fill_histogram_checker<checker::positive_track_slope_predicate>();
//        fill_histogram_checker<checker::aberrant_arc_length_predicate>();
        
        ukf_m.call_stepper().ode.model().particle_h = &current_hypothesis_m;
    }

    void Action() override {
//        puts(__PRETTY_FUNCTION__);
        
        ++event;
//        std::cout << "---event---\n";
        logger_m.clear();
        
//        matcher_m.clear();
        matcher_m.generate_candidate_indices();
        
        auto hypothesis_c = form_hypothesis();
        
//        std::cout << "hypothesis: " << hypothesis_c.size() << '\n';
        
        for(auto & hypothesis : hypothesis_c){
            current_hypothesis_m = hypothesis;
//            std::cout << "hypothesis: " << hypothesis.charge << " - " << hypothesis.mass << " - " << hypothesis.momentum << " -- " << hypothesis.get_end_points().size() << '\n';
            reconstruct();
        }
        
        logger_m.add_root_header( "END_RECONSTRUCTION" );
        auto track_c = shear_suboptimal_tracks( std::move(track_mc) );
        
//        logger_m << "track_size: " << track_c.size() << '\n';
//
//        for(auto const& track : track_c){
//            std::cout << "track_cluster_count: " << track.get_clusters().size() << '\n';
//            logger_m << "track_cluster_count: " << track.get_clusters().size() << '\n';
//            for(auto const& cluster : track.get_clusters() ){
//                logger_m << "cluster: " << cluster.vector(0,0) << " - " << cluster.vector(1,0) << " - " << cluster.evaluation_point << '\n';
//            }
//        }

//        track_c = compute_track_parameters( std::move(track_c) );
        track_c = compute_track_parameters_splitted( std::move(track_c) );
        track_c = compute_arc_length( std::move(track_c) );
        track_c = compute_time_of_flight( std::move(track_c));
        track_c = compute_momentum( std::move(track_c ));
        track_c = refine_hypotheses( std::move(track_c) );
        
        register_tracks_upward( std::move( track_c ) );
        
//        logger_m.freeze_everything();
//        logger_m.output();
        
        auto result_c = matcher_m.retrieve_results();
        for( auto& checker : computation_checker_mc ){
            checker.apply( result_c );
        }
        for( auto & checker : histogram_checker_mc ){
            checker.apply( result_c );
        }
        
        matcher_m.end_event();
        
        //form list of hypothesis
        
        //for each
        // - set current_hypothesis_m
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
    
    void Output() override {
        
        if( !computation_checker_mc.empty() ){
            logger_m.add_root_header("RESULTS");
            auto result_c = computation_checker_mc[0].output();
            auto efficiency = result_c[0];
            logger_m.template add_header<1, details::immutable_tag>("efficiency");
            logger_m << "global_efficiency: " << efficiency.value * 100 << '\n';
            logger_m << "global_efficiency_error: " << efficiency.error * 100<< '\n';
//
            auto purity = result_c[1];
            logger_m.template add_header<1, details::immutable_tag>("purity");
            logger_m << "global_purity: " << purity.value * 100 << '\n';
            logger_m << "global_purity_error: " << purity.error * 100<< '\n';
//
            auto reconstructed = result_c[2];
            auto fake = result_c[3];
            logger_m.template add_header<1, details::immutable_tag>("fake_yield");
            auto fake_yield = fake.value /(reconstructed.value+fake.value);
            logger_m << "global_fake_yield: " << fake_yield * 100 << '\n';
            auto fake_yield_error = sqrt( fake_yield * (1+ fake_yield)/reconstructed.value);
            logger_m << "fake_yield_error: " << fake_yield_error * 100<< '\n';
//
            auto clone = result_c[4];
            logger_m.template add_header<1, details::immutable_tag>("clone_yield");
            auto clone_multiplicity = clone.value /reconstructed.value;
            logger_m << "global_multiplicity: " << clone_multiplicity << '\n';
            auto clone_multiplicity_error = sqrt( clone_multiplicity * (1+ clone_multiplicity)/reconstructed.value);
            logger_m << "clone_multiplicty_error: " << clone_multiplicity_error << '\n';
//
            auto mass_yield = result_c[5];
            logger_m.template add_header<1, details::immutable_tag>("mass_yield");
            logger_m << "mass_yield: " << mass_yield.value << '\n';
            logger_m << "mass_yield_error: " << mass_yield.error << '\n';

            auto momentum_residuals = result_c[6];
            logger_m.template add_header<1, details::immutable_tag>("momentum_residuals");
            logger_m << "momentum_residuals: " << momentum_residuals.value << '\n';
            logger_m << "momentum_residuals_error: " << momentum_residuals.error << '\n';
            
            auto beta = result_c[7];
            logger_m.template add_header<1, details::immutable_tag>("beta");
            logger_m << "beta_resolution: " << beta.value * 100 << '\n';
            logger_m << "beta_resolution_error: " << beta.error * 100<< '\n';
//
            
//            auto result_charge1_c = computation_checker_mc[1].output();
//            auto efficiency1 = result_charge1_c[0];
//            auto purity1 = result_charge1_c[1];
//            auto beta1 = result_charge1_c[2];
//            logger_m.template add_header<1, details::immutable_tag>("charge1");
//            logger_m << "[efficiency, purity, beta]: [";
//            logger_m << efficiency1.value * 100 << " +/- " << efficiency1.error * 100 << ", ";
//            logger_m << purity1.value * 100 << " +/- " << purity1.error * 100 << ", ";
//            logger_m << beta1.value * 100 << " +/- " << beta1.error * 100 << ", ";
//            logger_m << "]\n";
//
//            auto result_charge2_c = computation_checker_mc[2].output();
//            auto efficiency2 = result_charge2_c[0];
//            auto purity2 = result_charge2_c[1];
//            auto beta2 = result_charge2_c[2];
//            logger_m.template add_header<1, details::immutable_tag>("charge2");
//            logger_m << "[efficiency, purity, beta]: [";
//            logger_m << efficiency2.value * 100 << " +/- " << efficiency2.error * 100 << ", ";
//            logger_m << purity2.value * 100 << " +/- " << purity2.error * 100 << ", ";
//            logger_m << beta2.value * 100 << " +/- " << beta2.error * 100 << ", ";
//            logger_m << "]\n";
//
//
//            auto result_charge3_c = computation_checker_mc[3].output();
//            auto efficiency3 = result_charge3_c[0];
//            auto purity3 = result_charge3_c[1];
//            auto beta3 = result_charge3_c[2];
//            logger_m.template add_header<1, details::immutable_tag>("charge3");
//            logger_m << "[efficiency, purity, beta]: [";
//            logger_m << efficiency3.value * 100 << " +/- " << efficiency3.error * 100 << ", ";
//            logger_m << purity3.value * 100 << " +/- " << purity3.error * 100 << ", ";
//            logger_m << beta3.value * 100 << " +/- " << beta3.error * 100 << ", ";
//            logger_m << "]\n";
//
//            auto result_charge4_c = computation_checker_mc[4].output();
//            auto efficiency4 = result_charge4_c[0];
//            auto purity4 = result_charge4_c[1];
//            auto beta4 = result_charge4_c[2];
//            logger_m.template add_header<1, details::immutable_tag>("charge4");
//            logger_m << "[efficiency, purity, beta]: [";
//            logger_m << efficiency4.value * 100 << " +/- " << efficiency4.error * 100 << ", ";
//            logger_m << purity4.value * 100 << " +/- " << purity4.error * 100 << ", ";
//            logger_m << beta4.value * 100 << " +/- " << beta4.error * 100 << ", ";
//            logger_m << "]\n";
//
//            auto result_charge5_c = computation_checker_mc[5].output();
//            auto efficiency5 = result_charge5_c[0];
//            auto purity5 = result_charge5_c[1];
//            auto beta5 = result_charge5_c[2];
//            logger_m.template add_header<1, details::immutable_tag>("charge5");
//            logger_m << "[efficiency, purity, beta]: [";
//            logger_m << efficiency5.value * 100 << " +/- " << efficiency5.error * 100 << ", ";
//            logger_m << purity5.value * 100 << " +/- " << purity5.error * 100 << ", ";
//            logger_m << beta5.value * 100 << " +/- " << beta5.error * 100 << ", ";
//            logger_m << "]\n";
//
//            auto result_charge6_c = computation_checker_mc[6].output();
//            auto efficiency6 = result_charge6_c[0];
//            auto purity6 = result_charge6_c[1];
//            auto beta6 = result_charge6_c[2];
//            logger_m.template add_header<1, details::immutable_tag>("charge6");
//            logger_m << "[efficiency, purity, beta]: [";
//            logger_m << efficiency6.value * 100 << " +/- " << efficiency6.error * 100 << ", ";
//            logger_m << purity6.value * 100 << " +/- " << purity6.error * 100 << ", ";
//            logger_m << beta6.value * 100 << " +/- " << beta6.error * 100 << ", ";
//            logger_m << "]\n";
//
//
//            auto result_charge7_c = computation_checker_mc[7].output();
//            auto efficiency7 = result_charge7_c[0];
//            auto purity7 = result_charge7_c[1];
//            auto beta7 = result_charge7_c[2];
//            logger_m.template add_header<1, details::immutable_tag>("charge7");
//            logger_m << "[efficiency, purity, beta]: [";
//            logger_m << efficiency7.value * 100 << " +/- " << efficiency7.error * 100 << ", ";
//            logger_m << purity7.value * 100 << " +/- " << purity7.error * 100 << ", ";
//            logger_m << beta7.value * 100 << " +/- " << beta7.error * 100 << ", ";
//            logger_m << "]\n";
//
//
//            auto result_charge8_c = computation_checker_mc[8].output();
//            auto efficiency8 = result_charge8_c[0];
//            auto purity8 = result_charge8_c[1];
//            auto beta8 = result_charge8_c[2];
//            logger_m.template add_header<1, details::immutable_tag>("charge8");
//            logger_m << "[efficiency, purity, beta]: [";
//            logger_m << efficiency8.value * 100 << " +/- " << efficiency8.error * 100 << ", ";
//            logger_m << purity8.value * 100 << " +/- " << purity8.error * 100 << ", ";
//            logger_m << beta8.value * 100 << " +/- " << beta8.error * 100 << ", ";
//            logger_m << "]\n";
        }
        
        logger_m.output();
        
        //scan change limits + number of points -> count how many actually go through the recomputation
        if(!histogram_checker_mc.empty()){
            TFile file{"tol_16O400C_1e5_refineh_tris_basec.root", "RECREATE"};
//            TFile file{"tol_16O200C_1e5_refineh_basec.root", "RECREATE"};
//            TFile file{"tol_16O200C2H4_1e5_refineh_basec.root", "RECREATE"};
//            TFile file{"tol_12C200C_3e4_refineh_tris_basec.root", "RECREATE"};
//            TFile file{"tol_test.root", "RECREATE"};
            for( auto & checker : histogram_checker_mc ){ checker.output(); }
            file.Close();
        }
    }
    
public:
    std::vector<computation_checker>& get_computation_checker() override { return computation_checker_mc; }
    
private:
    template<class MO = checker::no_requirement>
    void fill_histogram_checker() {
        using namespace checker;
        histogram_checker_mc.push_back(
                TATOEchecker<
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based> >, no_requirement, MO >,
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<8>, MO >,
                    purity< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<9>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based> >, no_requirement, MO >,
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<8>, MO >,
                    purity< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<9>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based> >, no_requirement, MO >,
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<8>, MO >,
                    purity< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<9>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based> >, no_requirement, MO >,
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<8>, MO >,
                    efficiency< histogram< reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<9>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based> >, no_requirement, MO >,
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<8>, MO >,
                    efficiency< histogram< reconstruction_base< theta_based, reconstructible_based>>, isolate_charge<9>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based> >, no_requirement, MO >,
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<8>, MO >,
                    efficiency< histogram< reconstruction_base< phi_based, reconstructible_based>>, isolate_charge<9>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    reconstructed_distribution< histogram< reconstruction_base<momentum_based, reconstructed_based>>, no_requirement, MO >
//                    reconstructed_distribution< histogram<per_nucleon, reconstructed_based>, isolate_charge<1>, MO >,
//                    reconstructed_distribution< histogram<per_nucleon, reconstructed_based>, isolate_charge<2>, MO >,
//                    reconstructed_distribution< histogram<per_nucleon, reconstructed_based>, isolate_charge<3>, MO >,
//                    reconstructed_distribution< histogram<per_nucleon, reconstructed_based>, isolate_charge<4>, MO >,
//                    reconstructed_distribution< histogram<per_nucleon, reconstructed_based>, isolate_charge<5>, MO >,
//                    reconstructed_distribution< histogram<per_nucleon, reconstructed_based>, isolate_charge<6>, MO >,
//                    reconstructed_distribution< histogram<per_nucleon, reconstructed_based>, isolate_charge<7>, MO >,
//                    reconstructed_distribution< histogram<per_nucleon, reconstructed_based>, isolate_charge<8>, MO >,
//                    reconstructed_distribution< histogram<per_nucleon, reconstructed_based>, isolate_charge<9>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    reconstructible_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, no_requirement, MO >
//                    reconstructible_distribution< histogram<per_nucleon>, isolate_charge<1>, MO >,
//                    reconstructible_distribution< histogram<per_nucleon>, isolate_charge<2>, MO >,
//                    reconstructible_distribution< histogram<per_nucleon>, isolate_charge<3>, MO >,
//                    reconstructible_distribution< histogram<per_nucleon>, isolate_charge<4>, MO >,
//                    reconstructible_distribution< histogram<per_nucleon>, isolate_charge<5>, MO >,
//                    reconstructible_distribution< histogram<per_nucleon>, isolate_charge<6>, MO >,
//                    reconstructible_distribution< histogram<per_nucleon>, isolate_charge<7>, MO >,
//                    reconstructible_distribution< histogram<per_nucleon>, isolate_charge<8>, MO >,
//                    reconstructible_distribution< histogram<per_nucleon>, isolate_charge<9>, MO >
                            >{} );

//        histogram_checker_mc.push_back(
//                TATOEchecker<
//                    missed_distribution< histogram<absolute>, no_requirement, MO >
////                    missed_distribution< histogram<absolute>, isolate_charge<1>, MO >,
////                    missed_distribution< histogram<absolute>, isolate_charge<2>, MO >,
////                    missed_distribution< histogram<absolute>, isolate_charge<3>, MO >,
////                    missed_distribution< histogram<absolute>, isolate_charge<4>, MO >,
////                    missed_distribution< histogram<absolute>, isolate_charge<5>, MO >,
////                    missed_distribution< histogram<absolute>, isolate_charge<6>, MO >,
////                    missed_distribution< histogram<absolute>, isolate_charge<7>, MO >,
////                    missed_distribution< histogram<absolute>, isolate_charge<8>, MO >,
////                    missed_distribution< histogram<absolute>, isolate_charge<9>, MO >
//                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, no_requirement, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<8>, MO >
//                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<9>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, no_requirement, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, isolate_charge<1>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, isolate_charge<2>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, isolate_charge<3>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, isolate_charge<4>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, isolate_charge<5>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, isolate_charge<6>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, isolate_charge<7>, MO >,
                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, isolate_charge<8>, MO >
//                    mass_distribution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, isolate_charge<9>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    mass_identification< histogram<>, no_requirement, MO >,
                    mass_identification< histogram<>, isolate_charge<1>, MO >,
                    mass_identification< histogram<>, isolate_charge<2>, MO >,
                    mass_identification< histogram<>, isolate_charge<3>, MO >,
                    mass_identification< histogram<>, isolate_charge<4>, MO >,
                    mass_identification< histogram<>, isolate_charge<5>, MO >,
                    mass_identification< histogram<>, isolate_charge<6>, MO >,
                    mass_identification< histogram<>, isolate_charge<7>, MO >,
                    mass_identification< histogram<>, isolate_charge<8>, MO >,
                    mass_identification< histogram<>, isolate_charge<9>, MO >
                            >{} );
        
//        histogram_checker_mc.push_back(
//                TATOEchecker<
//                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructed_based>>, no_requirement, MO >
//                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    momentum_resolution< histogram<reconstruction_base<theta_based, reconstructible_based>>, no_requirement, MO >,
                    momentum_resolution< histogram<reconstruction_base<theta_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    momentum_resolution< histogram<reconstruction_base<theta_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    momentum_resolution< histogram<reconstruction_base<theta_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    momentum_resolution< histogram<reconstruction_base<theta_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    momentum_resolution< histogram<reconstruction_base<theta_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    momentum_resolution< histogram<reconstruction_base<theta_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    momentum_resolution< histogram<reconstruction_base<theta_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    momentum_resolution< histogram<reconstruction_base<theta_based, reconstructible_based>>, isolate_charge<8>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    momentum_resolution< histogram<reconstruction_base<phi_based, reconstructible_based>>, no_requirement, MO >,
                    momentum_resolution< histogram<reconstruction_base<phi_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    momentum_resolution< histogram<reconstruction_base<phi_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    momentum_resolution< histogram<reconstruction_base<phi_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    momentum_resolution< histogram<reconstruction_base<phi_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    momentum_resolution< histogram<reconstruction_base<phi_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    momentum_resolution< histogram<reconstruction_base<phi_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    momentum_resolution< histogram<reconstruction_base<phi_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    momentum_resolution< histogram<reconstruction_base<phi_based, reconstructible_based>>, isolate_charge<8>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, no_requirement, MO >,
                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    momentum_resolution< histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<8>, MO >
                            >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    momentum_resolution< histogram<reconstruction_base<angle_based, reconstructible_based>>, no_requirement, MO >,
                    momentum_resolution< histogram<reconstruction_base<angle_based, reconstructible_based>>, isolate_charge<1>, MO >,
                    momentum_resolution< histogram<reconstruction_base<angle_based, reconstructible_based>>, isolate_charge<2>, MO >,
                    momentum_resolution< histogram<reconstruction_base<angle_based, reconstructible_based>>, isolate_charge<3>, MO >,
                    momentum_resolution< histogram<reconstruction_base<angle_based, reconstructible_based>>, isolate_charge<4>, MO >,
                    momentum_resolution< histogram<reconstruction_base<angle_based, reconstructible_based>>, isolate_charge<5>, MO >,
                    momentum_resolution< histogram<reconstruction_base<angle_based, reconstructible_based>>, isolate_charge<6>, MO >,
                    momentum_resolution< histogram<reconstruction_base<angle_based, reconstructible_based>>, isolate_charge<7>, MO >,
                    momentum_resolution< histogram<reconstruction_base<angle_based, reconstructible_based>>, isolate_charge<8>, MO >
                            >{} );
//        histogram_checker_mc.push_back(
//                TATOEchecker<
//                    residuals< histogram<>, no_requirement, MO >,
//                    residuals< histogram<>, isolate_charge<1>, MO >
////                    residuals< histogram<>, isolate_charge<2>, MO >,
////                    residuals< histogram<>, isolate_charge<3>, MO >,
////                    residuals< histogram<>, isolate_charge<4>, MO >,
////                    residuals< histogram<>, isolate_charge<5>, MO >,
////                    residuals< histogram<>, isolate_charge<6>, MO >,
////                    residuals< histogram<>, isolate_charge<7>, MO >,
////                    residuals< histogram<>, isolate_charge<8>, MO >,
////                    residuals< histogram<>, isolate_charge<9>, MO >
//                            >{} );
//        histogram_checker_mc.push_back(
//                TATOEchecker<
//                    shearing_factor_distribution< histogram<>, no_requirement, MO >,
//                    shearing_factor_distribution< histogram<>, isolate_charge<1>, MO >,
//                    shearing_factor_distribution< histogram<>, isolate_charge<2>, MO >,
//                    shearing_factor_distribution< histogram<>, isolate_charge<3>, MO >,
//                    shearing_factor_distribution< histogram<>, isolate_charge<4>, MO >,
//                    shearing_factor_distribution< histogram<>, isolate_charge<5>, MO >,
//                    shearing_factor_distribution< histogram<>, isolate_charge<6>, MO >,
//                    shearing_factor_distribution< histogram<>, isolate_charge<7>, MO >,
//                    shearing_factor_distribution< histogram<>, isolate_charge<8>, MO >,
//                    shearing_factor_distribution< histogram<>, isolate_charge<9>, MO >
//                                       >{} );
//        histogram_checker_mc.push_back(
//                TATOEchecker<
//                    cluster_size_distribution< histogram<>, no_requirement, MO >,
//                    cluster_size_distribution< histogram<>, isolate_charge<1>, MO >,
//                    cluster_size_distribution< histogram<>, isolate_charge<2>, MO >,
//                    cluster_size_distribution< histogram<>, isolate_charge<3>, MO >,
//                    cluster_size_distribution< histogram<>, isolate_charge<4>, MO >,
//                    cluster_size_distribution< histogram<>, isolate_charge<5>, MO >,
//                    cluster_size_distribution< histogram<>, isolate_charge<6>, MO >,
//                    cluster_size_distribution< histogram<>, isolate_charge<7>, MO >,
//                    cluster_size_distribution< histogram<>, isolate_charge<8>, MO >,
//                    cluster_size_distribution< histogram<>, isolate_charge<9>, MO >
//                                       >{} );
//        histogram_checker_mc.push_back(
//                TATOEchecker<
//                    splitted_mc_distribution< histogram<>, no_requirement, MO >
//                                       >{} );
//        histogram_checker_mc.push_back(
//                TATOEchecker<
//                    angle_distribution< histogram<>, no_requirement, MO >,
//                    angle_distribution< histogram<>, isolate_charge<1>, MO >,
//                    angle_distribution< histogram<>, isolate_charge<2>, MO >,
//                    angle_distribution< histogram<>, isolate_charge<3>, MO >,
//                    angle_distribution< histogram<>, isolate_charge<4>, MO >,
//                    angle_distribution< histogram<>, isolate_charge<5>, MO >,
//                    angle_distribution< histogram<>, isolate_charge<6>, MO >,
//                    angle_distribution< histogram<>, isolate_charge<7>, MO >,
//                    angle_distribution< histogram<>, isolate_charge<8>, MO >,
//                    angle_distribution< histogram<>, isolate_charge<9>, MO >
//                                       >{} );
//        histogram_checker_mc.push_back(
//                TATOEchecker<
//                    arc_length_ratio< histogram<>, no_requirement, MO >
//                    arc_length_ratio< histogram<>, isolate_charge<1>, MO >,
//                    arc_length_ratio< histogram<>, isolate_charge<2>, MO >,
//                    arc_length_ratio< histogram<>, isolate_charge<3>, MO >,
//                    arc_length_ratio< histogram<>, isolate_charge<4>, MO >,
//                    arc_length_ratio< histogram<>, isolate_charge<5>, MO >,
//                    arc_length_ratio< histogram<>, isolate_charge<6>, MO >,
//                    arc_length_ratio< histogram<>, isolate_charge<7>, MO >,
//                    arc_length_ratio< histogram<>, isolate_charge<8>, MO >,
//                    arc_length_ratio< histogram<>, isolate_charge<9>, MO >
//                                       >{} );
//        histogram_checker_mc.push_back(
//                TATOEchecker<
//                    tof_ratio< histogram<>, no_requirement, MO >
//                    tof_ratio< histogram<>, isolate_charge<1>, MO >,
//                    tof_ratio< histogram<>, isolate_charge<2>, MO >,
//                    tof_ratio< histogram<>, isolate_charge<3>, MO >,
//                    tof_ratio< histogram<>, isolate_charge<4>, MO >,
//                    tof_ratio< histogram<>, isolate_charge<5>, MO >,
//                    tof_ratio< histogram<>, isolate_charge<6>, MO >,
//                    tof_ratio< histogram<>, isolate_charge<7>, MO >,
//                    tof_ratio< histogram<>, isolate_charge<8>, MO >,
//                    tof_ratio< histogram<>, isolate_charge<9>, MO >
//                                       >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    beta_ratio< histogram<>, no_requirement, MO >,
                    beta_ratio< histogram<>, isolate_charge<1>, MO >,
                    beta_ratio< histogram<>, isolate_charge<2>, MO >,
                    beta_ratio< histogram<>, isolate_charge<3>, MO >,
                    beta_ratio< histogram<>, isolate_charge<4>, MO >,
                    beta_ratio< histogram<>, isolate_charge<5>, MO >,
                    beta_ratio< histogram<>, isolate_charge<6>, MO >,
                    beta_ratio< histogram<>, isolate_charge<7>, MO >,
                    beta_ratio< histogram<>, isolate_charge<8>, MO >
                             >{} );
        histogram_checker_mc.push_back(
                       TATOEchecker<
                           clone_distribution< histogram<>, no_requirement, MO >,
                           clone_distribution< histogram<>, isolate_charge<1>, MO >
//                           clone_distribution< histogram<>, isolate_charge<2>, MO >,
//                           clone_distribution< histogram<>, isolate_charge<3>, MO >,
//                           clone_distribution< histogram<>, isolate_charge<4>, MO >,
//                           clone_distribution< histogram<>, isolate_charge<5>, MO >,
//                           clone_distribution< histogram<>, isolate_charge<6>, MO >,
//                           clone_distribution< histogram<>, isolate_charge<7>, MO >,
//                           clone_distribution< histogram<>, isolate_charge<8>, MO >
                                   >{} );
        histogram_checker_mc.push_back(
                TATOEchecker<
                    determination_coefficient_distribution< histogram<>, no_requirement, MO >
                            >{} );
    }
    
private:
    void reset_event_number() override { event = 0 ; }
    void set_cuts( details::vertex_tag, details::vertex_tag::cut_t const& cut_p) override {
        list_m.template set_cuts<detector_properties<details::vertex_tag>>( cut_p );
    }
    void set_cuts( details::it_tag, details::it_tag::cut_t const& cut_pc) override {
        list_m.template set_cuts<detector_properties<details::it_tag>>( cut_pc );
    }
    void set_cuts( details::msd_tag, details::msd_tag::cut_t const& cut_pc) override {
        list_m.template set_cuts<detector_properties<details::msd_tag>>( cut_pc );
    }
    void set_cuts( details::tof_tag, details::tof_tag::cut_t const& cut_p) override  {
        list_m.template set_cuts<detector_properties<details::tof_tag>>( cut_p );
    }
    void set_cuts( details::ms2d_tag, details::ms2d_tag::cut_t const& cut_p) override  {
        list_m.template set_cuts<detector_properties<details::ms2d_tag>>( cut_p );
    }
    
    std::vector<reconstruction_module> const& retrieve_matched_results( ) const override {
        return matcher_m.retrieve_results( );
    }
    
    std::vector<particle_hypothesis> form_hypothesis()
    {
        
        logger_m.add_root_header( "FORM_HYPOTHESIS" );
        logger_m << "event: "<< event << '\n';
//        std::cout << "event: "<< event << '\n';
        auto tof = list_m.last();

//        std::cout << "form_hypothesis:\n";
        auto candidate_c = tof.generate_candidates();
        std::vector<particle_hypothesis> hypothesis_c;
        hypothesis_c.reserve( candidate_c.size()*2 );

        logger_m << "candidates: " << candidate_c.size() << '\n';
        for( const auto& candidate : candidate_c ) {
            auto charge = candidate.data->GetChargeZ();
            logger_m.add_header<1, details::immutable_tag>("candidate");
            logger_m << "charge: " << charge << '\n';
            if(charge == 0) {continue;}
            
            
            
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
            matcher_m.submit_reconstructible_track( candidate );
            // -----------------------------
            
            auto add_current_end_point = [&candidate]( particle_hypothesis & hypothesis_p  )
                                 { hypothesis_p.get_end_points().push_back( candidate.data ); };
//
            



            auto first_matching_hypothesis_i = std::find_if( hypothesis_c.begin(), hypothesis_c.end(),
                                                          [&charge]( particle_hypothesis const & h_p ){ return h_p.properties.charge == charge; } );

            if( first_matching_hypothesis_i != hypothesis_c.end() ) {
                std::for_each( first_matching_hypothesis_i, hypothesis_c.end(),
                              [&charge, &candidate, &add_current_end_point]( particle_hypothesis & h_p ){
                                  if( h_p.properties.charge == charge ){ add_current_end_point( h_p ); }
                              } );
                continue;
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
                                      2 *  (beam_energy_m * mass_number_p) * (931.5 * mass_number_p)  ) *
                                      energy_modifier_p;
                                          
                            hypothesis_c.push_back(
                                            particle_hypothesis{
                                                particle_properties{momentum, charge, mass_number_p, 931.5 * mass_number_p},
                                                light_ion_boost_p
                                                                } );
                add_current_end_point( hypothesis_c.back() );
                        };
            
            switch(charge){
                case 1:
                {
//                    auto light_ion_boost = 2.5;
                    auto light_ion_boost = 2;
                    add_hypothesis(1, light_ion_boost);
                    add_hypothesis(1, light_ion_boost, 0.5);
//                     light_ion_boost = 1.3;
                    add_hypothesis(2, light_ion_boost);
                    add_hypothesis(3, 2);
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
                case 8:
                {
                    add_hypothesis(15);
                    add_hypothesis(16);
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
        
        logger_m.freeze();
//        logger_m.output();
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
    
    corrected_state generate_corrected_state( TAVTvertex const * vertex_ph,
                                              TAVTtrack const * track_ph )
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>(
                                     gTAGroot->FindAction( TAGgeoTrafo::GetDefaultActName().Data() )
                                                            );
        
        auto start = transformation_h->FromVTLocalToGlobal( vertex_ph->GetPosition() );
        auto error = vertex_ph->GetPosError();
        auto error_x = error.X() != 0 ? error.X() : 1e-4;
        auto error_y = error.Y() != 0 ? error.Y() : 1e-4;
        
        auto track_slope_x = track_ph->GetSlopeZ().X();
        auto track_slope_y = track_ph->GetSlopeZ().Y();
        
        auto track_slope_error_x = track_ph->GetSlopeErrZ().X();
        auto track_slope_error_y = track_ph->GetSlopeErrZ().Y();
        
//        std::cout << "errors: [" << error_x<< ", " << error_y << ", " << track_slope_error_x << ", " << track_slope_error_y <<"] \n" ;
        
        
        using vector = typename underlying<state>::vector;
        using covariance = typename underlying<state>::covariance;
        
        return corrected_state{
            state{
                evaluation_point{ start.Z() },
                vector{{ start.X(), start.Y(), track_slope_x, track_slope_y }},
                covariance{{ pow(error_x, 2  ),                 0,                           0,                              0,
                                               0, pow(error_y, 2),                           0,                              0,
                                               0,                 0, pow(track_slope_error_x, 2),                              0,
                                               0,                 0,                           0,   pow( track_slope_error_y, 2) }}
            },
            chisquared{0}
        };
    }
    
    corrected_state generate_corrected_state( TAVTbaseCluster const * cluster_ph,
                                              TAVTtrack const * track_ph )
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>(
                                     gTAGroot->FindAction( TAGgeoTrafo::GetDefaultActName().Data() )
                                                            );
        
        auto start = transformation_h->FromVTLocalToGlobal( cluster_ph->GetPositionG() );
        auto error = cluster_ph->GetPosErrorG();

        auto track_slope_x = track_ph->GetSlopeZ().X();
        auto track_slope_y = track_ph->GetSlopeZ().Y();
        
        auto track_slope_error_x = track_ph->GetSlopeErrZ().X();
        auto track_slope_error_y = track_ph->GetSlopeErrZ().Y();
        
//        std::cout << "errors: [" << error_x<< ", " << error_y << ", " << track_slope_error_x << ", " << track_slope_error_y <<"] \n" ;
        
        
        using vector = typename underlying<state>::vector;
        using covariance = typename underlying<state>::covariance;
        
        return corrected_state{
            state{
                evaluation_point{ start.Z() },
                vector{{ start.X(), start.Y(), track_slope_x, track_slope_y }},
                covariance{{ pow(error.X(), 2  ),                 0,                           0,                              0,
                                               0, pow(error.Y(), 2),                           0,                              0,
                                               0,                 0, pow(track_slope_error_x, 2),                              0,
                                               0,                 0,                           0,   pow( track_slope_error_y, 2) }}
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
        logger_m << "selected_start: " << start.X() << " - " << start.Y() << " - " << start.Z() << '\n';

        
        auto tof = list_m.last();
        auto candidate_c = tof.generate_candidates();
        result_c.reserve( candidate_c.size() );
        
        for(auto && candidate : candidate_c) {
            auto end = transformation_h->FromTWLocalToGlobal( candidate.data->GetPositionG() );
            logger_m <<"selected_end: " << end.X() << " - " << end.Y() << " - " << end.Z() << '\n';
            
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
//        puts(__PRETTY_FUNCTION__);
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
        logger_m << "covariance: \n" << ps.covariance;
        
        
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
                output_current_hypothesis();
                // -----------------------------
            
                ukf_m.step_length() = 5e-1;
            
                auto s = make_state( leaf.get_value() );
                auto fs_c = advance_reconstruction_impl( s, layer );
            
                
                for( auto & fs : fs_c ){
//
                    fs.step_length = compute_step_length( s, fs );
                    
                    logger_m << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                    logger_m << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                    logger_m << ") -- " << fs.evaluation_point;
                    logger_m << " -- {" << fs.prediction << ", " << fs.correction << ", " <<  fs.distance << "}\n";
                    
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
            output_current_hypothesis();
            // -----------------------------
            
            ukf_m.step_length() = 5e-1;
            
            auto s = make_state(leaf.get_value());
            
            auto fs_c = advance_reconstruction_impl( s, layer );
            
            if( fs_c.empty() ){ leaf.mark_invalid(); }
            else{
                for( auto&& fs : fs_c ){
                
                    fs.step_length = compute_step_length( s, fs );
                    
                    logger_m << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                    logger_m << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                    logger_m << ") -- " << fs.evaluation_point;
                    logger_m << " -- {" << fs.prediction << ", " << fs.correction << ", " <<  fs.distance << "}\n";
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
            auto * vertex_h = track.vertex();
//            auto first_h = track.first_cluster();
            auto * track_h = track.get_underlying_track();
            
//            auto cs = generate_corrected_state( vertex_h, first_h );
            auto cs = generate_corrected_state( vertex_h, track_h );
            auto fs = full_state{
                std::move(cs),
                data_handle<data_type>{nullptr},
                step_register{},
                block_weight_register{ details::vertex_tag::block_weight }
            };
            
            auto * leaf_h = arborescence_p.add_root( std::move(fs) );
            // -----------------------------
            output_current_hypothesis();
            // -----------------------------
            logger_m.add_header<1>( "vertex_track_reconstruction" );
            
            if(vertex_h->GetValidity() < 0){
                auto first_h = track.first_cluster();
                auto cs = generate_corrected_state( first_h, track_h );
                
                auto fs = full_state{
                    std::move(cs),
                    data_handle<data_type>{first_h},
                    step_register{},
                    block_weight_register{ details::vertex_tag::block_weight }
                };
                
                leaf_h = leaf_h->add_child( std::move(fs) );
                
                track.skip_first_layer();
            }
            
            for( auto layer : track ){
                
                ukf_m.step_length() = 5e-1;
                
                auto s = make_state( leaf_h->get_value() );
                auto fs_c = advance_reconstruction_impl( s, layer );
                
                if( !fs_c.empty() ){
                    auto fs = fs_c.front();
                    
                    fs.step_length = compute_step_length( s, fs );
//
                    logger_m << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                    logger_m << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                    logger_m << ") -- " << fs.evaluation_point ;
                    logger_m << " -- {" << fs.prediction << ", " << fs.correction << ", " <<  fs.distance << "}\n";

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
//         puts(__PRETTY_FUNCTION__);
         logger_m.add_root_header("START_RECONSTRUCTION");
 //        std::cout << "START_RECONSTRUCTION\n";
         
         auto layer_c = ms2d_p.form_layers();
         auto generating_candidate_c = layer_c.generating_candidates();
        
         logger_m.add_header<1>("candidate_generation");
         logger_m << "generating_candidate_found: " << generating_candidate_c.size() << '\n';
        
         for( auto&& candidate : generating_candidate_c ){
             auto cs_c = generate_corrected_state( candidate.data );
             
             for( auto && cs : cs_c){
                 auto fs = full_state{
                    std::move(cs),
                    data_handle<data_type>{candidate.data},
                    step_register{},
                     block_weight_register{ details::ms2d_tag::block_weight }
                     
                };
             
                 auto * leaf_h = arborescence_p.add_root( std::move(fs) );
             }
         }
         
         for(auto && layer : layer_c) {
             logger_m.add_header<2>("layer");
             logger_m << "candidate_size: " << layer.get_candidates().size() << '\n';
             
             
             if( layer.get_candidates().empty() ){ continue; }
             
             auto& leaf_c = arborescence_p.get_handler();
             for(auto& leaf : leaf_c){
                 
                 logger_m.add_header<3>( "leaf" );
                 
                 // -----------------------------
                 output_current_hypothesis();
                 // -----------------------------
             
                 ukf_m.step_length() = 5e-1;
             
                 auto s = make_state( leaf.get_value() );
                 auto fs_c = advance_reconstruction_impl( s, layer );
             
                 for( auto & fs : fs_c ){
 //
                     fs.step_length = compute_step_length( s, fs );
                     
                     logger_m << "corrected_state : ( " << fs.vector(0,0) << ", " << fs.vector(1,0) ;
                     logger_m << ") -- (" << fs.vector(2,0) << ", " << fs.vector(3,0)  ;
                     logger_m << ") -- " << fs.evaluation_point;
                     logger_m << " -- {" << fs.prediction << ", " << fs.correction << ", " <<  fs.distance << "}\n";
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
    
    //need to correct chi-square computation with the use of cuts values ! error is not accurate enough measurement
    std::vector<full_state> confront(const state& ps_p, const T& layer_p)
    {
        
        auto candidate_c = layer_p.get_candidates();
        
        std::vector<full_state> fs_c;
        fs_c.reserve( candidate_c.size() );
        
        using enriched_candidate = enriched_candidate_impl<typename decltype(candidate_c)::value_type>;
        std::vector< enriched_candidate > enriched_c;
        enriched_c.reserve( candidate_c.size() );
    
//        ps_p.covariance *= layer_p.cut * current_hypothesis_m.light_ion_boost;
        
        logger_m.add_sub_header( "confront" );
        std::for_each( candidate_c.begin(), candidate_c.end(),
                      [this, &ps_p, &enriched_c, &layer_p]( auto candidate_p )
                      {
//                            auto modified_covariance = candidate_p.covariance;
//                            modified_covariance(0,0) *= layer_p.cut_value( orientation::x{} );
//                            modified_covariance(1,1) *= layer_p.cut_value( orientation::y{} );
//                            modified_covariance = modified_covariance* current_hypothesis_m.light_ion_boost;
//                            auto modified_candidate = candidate_p;
//                            modified_candidate.covariance = modified_covariance;
//                            auto chi2_predicted = ukf_m.compute_chisquared(ps_p, modified_candidate);
            
            auto modified_covariance = candidate_p.covariance;
            modified_covariance(0,0) *= pow(double(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost), 2);
            modified_covariance(1,1) *= pow(double(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost), 2);
            auto modified_candidate = candidate_p;
            modified_candidate.covariance = modified_covariance;
            auto chi2_predicted = ukf_m.compute_chisquared(ps_p, modified_candidate);
            
//            candidate_p.covariance(0,0) *= layer_p.cut_value( orientation::x{} );
//            candidate_p.covariance(1,1) *= layer_p.cut_value( orientation::y{} );
//            candidate_p.covariance = candidate_p.covariance* current_hypothesis_m.light_ion_boost;
//            auto chi2_predicted = ukf_m.compute_chisquared(ps_p, candidate_p);
            
//            candidate_p.covariance(0,0) *= pow( double(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost), 2);
//            candidate_p.covariance(1,1) *= pow( double(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost), 2);
//            auto chi2_predicted = ukf_m.compute_chisquared(ps_p, candidate_p);
            
                          enriched_c.push_back( make_enriched_candidate( std::move( candidate_p ) ,
                                                                         chisquared{chi2_predicted} )   );
                      }
                     );
    
        auto select = [this, &ps_p, &layer_p ]( const auto & ec_p ){ return pass_selection( ec_p, ps_p, layer_p); };
        
        auto end = std::partition( enriched_c.begin(), enriched_c.end(), select);
        
        for(auto iterator = enriched_c.begin() ; iterator != end ; ++iterator ){
            auto state = ukf_m.correct_state( ps_p, *iterator ); //should be sliced properly
            
//            auto modified_covariance = iterator->covariance;
//            modified_covariance(0,0) *= layer_p.cut_value( orientation::x{} );
//            modified_covariance(1,1) *= layer_p.cut_value( orientation::y{} );
//            modified_covariance = modified_covariance* current_hypothesis_m.light_ion_boost;
//            auto modified_candidate = *iterator;
//            modified_candidate.covariance = modified_covariance;
//            auto chisquared_corrected = ukf_m.compute_chisquared(state, modified_candidate);
            
            auto modified_covariance = iterator->covariance;
            modified_covariance(0,0) *= pow(double(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost), 2);
            modified_covariance(1,1) *= pow(double(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost), 2);
            auto modified_candidate = *iterator;
            modified_candidate.covariance = modified_covariance;
            auto chisquared_corrected = ukf_m.compute_chisquared(state, modified_candidate);
            
//            auto chisquared_corrected = ukf_m.compute_chisquared(state, *iterator ); //add parameter -> cut value ? // go back to prediction ?
            auto distance = ukf_m.compute_distance( ps_p, state );
            
            auto cs = make_corrected_state( std::move(state),
                                            chisquared{std::move(iterator->prediction), chisquared_corrected, distance} );
            
            auto fs = full_state{ std::move(cs),
                data_handle<data_type>{ iterator->data },
                step_register{},
                block_weight_register{ T::block_weight }  };
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
                                       auto const & end_point_ch = current_hypothesis_m.get_end_points();
                                       return std::any_of( end_point_ch.begin(), end_point_ch.end(),
                                                           [&c_p](auto const & ep_ph ){ return c_p.data == ep_ph;  } );
                                   } );
        logger_m.add_sub_header(  "confront" );
        
        using enriched_candidate = enriched_candidate_impl<typename decltype(candidate_c)::value_type>;
        std::vector< enriched_candidate > enriched_c;
        enriched_c.reserve( std::distance( candidate_c.begin(), candidate_end ) );
        std::for_each( candidate_c.begin(), candidate_end,
                      [this, &ps_p, &enriched_c, &layer_p]( candidate c_p )
                      {
//            auto modified_covariance = c_p.covariance;
//            modified_covariance(0,0) *= layer_p.cut_value( orientation::x{} );
//            modified_covariance(1,1) *= layer_p.cut_value( orientation::y{} );
//            modified_covariance = modified_covariance* current_hypothesis_m.light_ion_boost;
//            auto modified_candidate = c_p;
//            modified_candidate.covariance = modified_covariance;
//            auto chi2_predicted = ukf_m.compute_chisquared(ps_p, modified_candidate);
            
            auto modified_covariance = c_p.covariance;
            modified_covariance(0,0) *= pow(double(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost), 2);
            modified_covariance(1,1) *= pow(double(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost), 2);
            auto modified_candidate = c_p;
            modified_candidate.covariance = modified_covariance;
            auto chi2_predicted = ukf_m.compute_chisquared(ps_p, modified_candidate);
            
//            c_p.covariance(0,0) *= layer_p.cut_value( orientation::x{} );
//            c_p.covariance(1,1) *= layer_p.cut_value( orientation::y{} );
//            c_p.covariance = c_p.covariance* current_hypothesis_m.light_ion_boost;
//            auto chi2_predicted = ukf_m.compute_chisquared(ps_p, c_p);
            
//            c_p.covariance(0,0) *= pow( double(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost), 2);
//            c_p.covariance(1,1) *= pow( double(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost), 2);
//            auto chi2_predicted = ukf_m.compute_chisquared(ps_p, c_p);
            
                            enriched_c.push_back( make_enriched_candidate( std::move( c_p ) ,
                                                                            chisquared{chi2_predicted} )   );
                      }
                      );
        

        
        std::vector<full_state> fs_c;
        fs_c.reserve( std::distance( candidate_c.begin(), candidate_end ) );
        
        auto select = [this, &ps_p, &layer_p ]( const auto & ec_p ){ return pass_selection( ec_p, ps_p, layer_p); };
        
        
        
        auto enriched_end = std::partition( enriched_c.begin(), enriched_c.end(), select );
        for(auto iterator = enriched_c.begin() ; iterator != enriched_end ; ++iterator ){
            auto state = ukf_m.correct_state( ps_p, *iterator ); //should be sliced properly
            
//            auto modified_covariance = iterator->covariance;
//            modified_covariance(0,0) *= layer_p.cut_value( orientation::x{} );
//            modified_covariance(1,1) *= layer_p.cut_value( orientation::y{} );
//            modified_covariance = modified_covariance* current_hypothesis_m.light_ion_boost;
//            auto modified_candidate = *iterator;
//            modified_candidate.covariance = modified_covariance;
//            auto chisquared_corrected = ukf_m.compute_chisquared(state, modified_candidate);
            
            auto modified_covariance = iterator->covariance;
            modified_covariance(0,0) *= pow(double(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost), 2);
            modified_covariance(1,1) *= pow(double(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost), 2);
            auto modified_candidate = *iterator;
            modified_candidate.covariance = modified_covariance;
            auto chisquared_corrected = ukf_m.compute_chisquared(state, modified_candidate);
//
//            auto chisquared_corrected = ukf_m.compute_chisquared(state, *iterator );
            
            auto distance = ukf_m.compute_distance( ps_p, state );
            
            auto cs = make_corrected_state( std::move(state),
                                            chisquared{std::move(iterator->prediction), chisquared_corrected, distance} );
            
            auto fs = full_state{ std::move(cs),
                                  data_handle<data_type>{ iterator->data } ,
                                  step_register{},
                                  block_weight_register{ details::tof_tag::block_weight }
                                  
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
//        auto modified_covariance = c.covariance;
//        modified_covariance(0,0) *= layer_p.cut_value( orientation::x{} );
//        modified_covariance(1,1) *= layer_p.cut_value( orientation::y{} );
//        modified_covariance = modified_covariance* current_hypothesis_m.light_ion_boost;
//        auto modified_candidate = c;
//        modified_candidate.covariance = modified_covariance;
//        auto chi2_predicted = ukf_m.compute_chisquared(ps_p, modified_candidate);
        
        auto modified_covariance = c.covariance;
        modified_covariance(0,0) *= pow(double(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost), 2);
        modified_covariance(1,1) *= pow(double(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost), 2);
        auto modified_candidate = c;
        modified_candidate.covariance = modified_covariance;
        auto chi2_predicted = ukf_m.compute_chisquared(ps_p, modified_candidate);
        
//        c.covariance(0,0) *= layer_p.cut_value( orientation::x{} );
//        c.covariance(1,1) *= layer_p.cut_value( orientation::y{} );
//        c.covariance = c.covariance* current_hypothesis_m.light_ion_boost;
//        auto chi2_predicted = ukf_m.compute_chisquared(ps_p, c);
        
//        c.covariance(0,0) *= pow( double(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost), 2);
//        c.covariance(1,1) *= pow( double(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost), 2);
//        auto chi2_predicted = ukf_m.compute_chisquared(ps_p, c);
        
        enriched_candidate ec = make_enriched_candidate( std::move( c) ,
                                                         chisquared{chi2_predicted}   );
        
    
        std::vector<full_state> fs_c;
    
        if( pass_selection(ec, ps_p, layer_p) ){
            auto state = ukf_m.correct_state( ps_p, ec ); //should be sliced properly
            
            auto chisquared_corrected = ukf_m.compute_chisquared(state, modified_candidate);
//            auto chisquared_corrected = ukf_m.compute_chisquared(state, ec );
            
            auto distance = ukf_m.compute_distance( ps_p, state );
            
            auto cs = make_corrected_state( std::move(state),
                                            chisquared{std::move(ec.prediction), chisquared_corrected, distance} );
            
            auto fs = full_state{ std::move(cs),
                                      data_handle<data_type>{ ec.data },
                step_register{},
                block_weight_register{details::vertex_tag::block_weight}
            };
            
            fs_c.push_back( std::move(fs) );
            
        }
    
        return fs_c;
    }
           
           

    template<class Enriched, class T>
    bool pass_selection( Enriched const& ec_p, const state& ps_p, T const& layer_p )
    {
        logger_m.add_sub_header( "pass_selection" );
        
        //should be retrieved form covariance matrix
        auto error = ec_p.data->GetPosErrorG();
        logger_m << "error: (" << error.X() << ", " << error.Y() << ")\n";

        auto theta = atan2(ec_p.vector(1,0) - ps_p.vector(1,0), ec_p.vector(0,0) - ps_p.vector(0,0) );
        logger_m << "theta: " << theta << '\n';
        auto semi_major_axis = layer_p.cut_value(orientation::x{}) * current_hypothesis_m.light_ion_boost * error.X();
        auto semi_minor_axis = layer_p.cut_value(orientation::y{}) * current_hypothesis_m.light_ion_boost * error.Y();
        auto ellipse_y = semi_major_axis * semi_minor_axis * sin(theta) /
                         sqrt( semi_minor_axis * semi_minor_axis * cos(theta) * cos(theta) +
                               semi_major_axis * semi_major_axis * sin(theta) * sin(theta));
        auto ellipse_x = semi_major_axis * semi_minor_axis * cos(theta) /
                         sqrt( semi_minor_axis * semi_minor_axis * cos(theta) * cos(theta) +
                               semi_major_axis * semi_major_axis * sin(theta) * sin(theta));
        
        auto mps = split_half( ps_p.vector , details::row_tag{});
        mps.first(0,0) += ellipse_x;
        mps.first(1,0) += ellipse_y;
        
        
        using candidate = typename underlying<Enriched>::candidate;
        using vector = typename underlying<candidate>::vector;
        using covariance = typename underlying<candidate>::covariance;
        using measurement_matrix = typename underlying<candidate>::measurement_matrix;
        using data = typename underlying<candidate>::data_type;
        
        auto modified_covariance = ec_p.covariance;
//        modified_covariance(0,0) *= layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost;
//        modified_covariance(1,1) *= layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost;
        
        modified_covariance(0,0) *= pow(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost, 2);
        modified_covariance(1,1) *= pow(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost, 2);
        
        auto cutter_candidate = candidate{
            vector{ std::move(mps.first) },
//            covariance{ ec_p.covariance },
            covariance{modified_covariance},
            measurement_matrix{ ec_p.measurement_matrix },
            data_handle<data>{ ec_p.data }
        };
        
        auto cutter_chisquared = ukf_m.compute_chisquared(ps_p, cutter_candidate);
        //
        logger_m << "cutter : (" << cutter_candidate.vector(0, 0) << ", " << cutter_candidate.vector(1,0) << ")\n";
        logger_m << "cutter_chisquared : " << cutter_chisquared << '\n';
        logger_m << "candidate : (" << ec_p.vector(0, 0) << ", " << ec_p.vector(1,0) << ")\n";
        logger_m << "candidate_chisquared : " << ec_p.prediction << '\n';
        
        logger_m << "cluster_mc_id: ";
        for( int i{0} ; i < ec_p.data->GetMcTracksN() ; ++ i){
            logger_m << ec_p.data->GetMcTrackIdx(i) << " ";
        }
        logger_m << '\n';
        
        return ec_p.prediction < cutter_chisquared;
    }
    
    
    template<class Enriched>
    bool pass_selection( Enriched const& ec_p,
                         state const& ps_p,
                         layer_generator<detector_properties<details::msd_tag>>::layer const& layer_p )
    {
        logger_m.add_sub_header( "pass_selection" );
        
        auto error = ec_p.data->GetPosErrorG();
        matrix<1,1> v = ec_p.measurement_matrix * ps_p.vector;

        using candidate = typename underlying<Enriched>::candidate;
        using vector = typename underlying<candidate>::vector;
        using covariance = typename underlying<candidate>::covariance;
        using measurement_matrix = typename underlying<candidate>::measurement_matrix;
        using data = typename underlying<candidate>::data_type;
        
        auto modified_covariance = ec_p.covariance;
        
        if( ec_p.measurement_matrix(0,0) > 0 ) {
            logger_m << " -- x orientation -- \n";
            logger_m << "error: (" << error.X() << ", " << error.Y() << ")\n";
            v(0,0) += layer_p.cut_value(orientation::x{}) * current_hypothesis_m.light_ion_boost * error.X();
//            modified_covariance(0,0) *= layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost;
            modified_covariance(0,0) *= pow(layer_p.cut_value( orientation::x{} ) * current_hypothesis_m.light_ion_boost, 2);
        }
        else {
            logger_m << " -- y orientation -- \n";
            logger_m << "error: (" << error.X() << ", " << error.Y() << ")\n";
            v(0,0) += layer_p.cut_value(orientation::y{}) * current_hypothesis_m.light_ion_boost * error.Y();
//            modified_covariance(0,0) *= layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost;
            modified_covariance(0,0) *= pow(layer_p.cut_value( orientation::y{} ) * current_hypothesis_m.light_ion_boost, 2);
        }
        
        auto cutter_candidate = candidate{
            vector{ std::move(v) },
//                        covariance{ ec_p.covariance },
            covariance{modified_covariance},
            measurement_matrix{ ec_p.measurement_matrix },
            data_handle<data>{ ec_p.data }
        } ;
        
        auto cutter_chisquared = ukf_m.compute_chisquared(ps_p, cutter_candidate);
        //
        logger_m << "cutter : (" << cutter_candidate.vector(0, 0) << ")\n";
        logger_m << "cutter_chisquared : " << cutter_chisquared << '\n';
        logger_m << "candidate : (" << ec_p.vector(0, 0) << ")\n";
        logger_m << "candidate_covariance: " << double(ec_p.covariance) << '\n';
        logger_m << "candidate_chisquared : " << ec_p.prediction << '\n';
        
        logger_m << "cluster_mc_id: ";
        for( int i{0} ; i < ec_p.data->GetMcTracksN() ; ++ i){
            logger_m << ec_p.data->GetMcTrackIdx(i) << " ";
        }
        logger_m << '\n';
        
//        logger_m.freeze();
        
        return ec_p.prediction < cutter_chisquared;
    }
        
    
    
    
    
    //----------------------------------------------------------------------------------------
    //final cross-check and registering
    
    void register_track( node_type& node_p )
    {
        auto && value_c = node_p.get_branch_values();
        
        double total_chisquared{0};
        for( auto value_i = value_c.begin() +1 ; value_i != value_c.end(); ++value_i){
            auto const& value = *value_i;
//            std::cout << value.prediction << " ";
//            total_chisquared += value.prediction;
            total_chisquared += value.correction;
//            total_chisquared += value.distance;
//            total_chisquared += value.prediction/value.correction;
        }
//        std::cout << '\n';
        double shearing_factor = sqrt( total_chisquared / (value_c.size() -1) );
//        std::cout << "shearing_factor: " << shearing_factor << std::endl;
        if( std::isnan(shearing_factor) ){ std::cout << "shearing_factor is nan\n"; std::terminate(); }
        
        track_mc.push_back( track{ current_hypothesis_m, shearing_factor, std::move(value_c) } );
        track_mc.back().momentum = track_mc.back().hypothesis.properties.momentum;
        track_mc.back().mass     = track_mc.back().hypothesis.properties.mass;
        track_mc.back().nucleon_number     = track_mc.back().hypothesis.properties.nucleon_number;
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
//        std::cout << "track_size: " << track_pc.size() << std::endl;
        
//        std::cout << "looping on endpoints" << std::endl;

//        std::cout<< "total: " << end_point_ch.size() << "\n";
        for( auto const * end_point_h : end_point_ch ){
//            std::cout << "partionning: " << std::endl;
            auto end_iterator = std::partition( track_pc.begin(), track_pc.end(),
                                                [&end_point_h](track const & track_p)
                                                { return track_p.get_clusters().back().data == end_point_h; } );
//            std::cout << "selected: " << std::distance( track_pc.begin(), end_iterator ) << std::endl;
//            std::for_each(track_pc.begin(), end_iterator,
//                      [](track const & track_p)
//                          { std::cout << track_p.hypothesis.properties.charge << "/" << track_p.hypothesis.properties.nucleon_number << " -> " << track_p.total_chisquared << "\n";});
            
            
//            std::cout << "sorting: " <<std::endl;
            std::sort( track_pc.begin(), end_iterator,
                       [](track const & track1_p, track const & track2_p)
                      {  return track1_p.total_chisquared < track2_p.total_chisquared ; } );  //change this to pred/corr ? and see impact

//            std::cout << "selecting" << std::endl;
            final_track_c.push_back( track_pc.front() );
            final_track_c.back().clone = std::distance(track_pc.begin(), end_iterator);
            
//            std::cout << "erasing" << std::endl;
            track_pc.erase(track_pc.begin(), end_iterator);
        }
        
//        std::cout << "all done" << std::endl;
        
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

    
    constexpr std::vector< track > compute_arc_length( std::vector<track>&& track_pc  ) const {
        constexpr std::size_t const exponent = 5; //max exponent of z in derivatives
        constexpr std::size_t const order_x = 4;
        constexpr std::size_t const order_y = 2;
        
        for( auto&& track : track_pc ){
            auto size = track.get_clusters().size();
            if( size < 4 || size > 16 ){
                std::cout << "old method is used\n";
                auto arc_length{0};
                auto cluster_c = track.get_clusters();
                for( auto && cluster : cluster_c ){
                    arc_length += cluster.step_length;
                }
                track.length = arc_length;
                continue;
            }
            
            auto const parameter_x = make_custom_matrix<4,1>( [&track]( std::size_t index_p){ return track.parameters.x[index_p]; } );
            auto const parameter_y = make_custom_matrix<2,1>( [&track]( std::size_t index_p){ return track.parameters.y[index_p]; } );
//            std::cout << "parameter_x: \n" << parameter_x;
//            std::cout << "parameter_y: \n" << parameter_y;
        
            
        // ====================  arc length computation ===================

            
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
                  } );
//            std::cout << "evaluation_point: " << os_p.evaluation_point << '\n';
//            std::cout << "polynomial: \n" << polynomial;
                  double const dx_dz = expr::compute( polynomial * mixing_x * parameter_x );
                  double const dy_dz = expr::compute( polynomial * mixing_y * parameter_y );
                  
                  return sqrt( pow(dx_dz, 2) + pow(dy_dz, 2) + 1 );
              }
                                       );
        
        //------------- numerical_integration -------------------
            auto stepper = make_stepper<data_rkf45>( std::move(ode) );
            auto end_point = track.get_clusters().back().evaluation_point;
            auto os = operating_state<double, 1>{ track.get_clusters().front().evaluation_point, 0 };
//            stepper.specify_tolerance(1e-5);
            stepper.specify_tolerance(1e-13);
        
            auto step = 5e-1;
            
            while( os.evaluation_point + step < end_point ){
                auto step_result = stepper.step( std::move(os), step );
                step = stepper.optimize_step_length(step, step_result.second);
                os = std::move(step_result.first);
            }
            step = end_point - os.evaluation_point;
            os = stepper.force_step( std::move(os), step );
            
            track.length = os.state( details::order_tag<0>{} );
        }
        return std::move(track_pc);
    }
    
    template<std::size_t N, std::size_t Order>
    void compute_polynomial_parameters_x( std::vector<full_state> const& cluster_pc, polynomial_fit_parameters& result_p ) const {
        //fit in x/y
        //retrieve fit parameters
        
        std::array<double, N> z_c{};
        std::array<double, N> x_c{};
        std::array<double, N * N> weight_x_c{};
        
        std::size_t index{0};
        
        for( auto cluster_i{cluster_pc.begin()+1}; cluster_i != cluster_pc.end() ; ++cluster_i ){
//        for( auto cluster_i{cluster_pc.begin()}; cluster_i != cluster_pc.end() ; ++cluster_i ){
            auto * msd_h =dynamic_cast<TAMSDcluster const*>( cluster_i->data );
            if( msd_h && msd_h->GetPlaneView() ){ continue; } //check if msd is in correct orientation
            z_c[index] = cluster_i->evaluation_point;
            x_c[index] = cluster_i->vector(0,0);
            weight_x_c[index + index * N] = cluster_i->covariance(0,0);
            ++index;
        }
        
        
        constexpr std::size_t const order_x = Order;
        auto const regressor_x = make_custom_matrix<N, order_x>(
                                            [&z_c, &order_x]( std::size_t index_p ){
                                                std::size_t column_index = index_p % order_x;
                                                std::size_t row_index = index_p / order_x;
                                                return pow( z_c[row_index], column_index );
                                                                                    }
                                                                );
//        std::cout << "zx:\n";
//        for( auto const& z : z_c ){ std::cout << z << '\n';}
        auto const observation_x = matrix<N, 1>{ std::move(x_c) };
//        std::cout << "observation_x: \n" << observation_x;
        auto const weight_x = matrix<N, N>{ std::move(weight_x_c)};
        
        //computation splitted to reduce instantiation depth (not allowed over 900 for gcc by default)
        auto const part1_x = form_inverse( expr::compute( transpose(regressor_x) * weight_x * regressor_x ) );
        auto const part2_x = expr::compute( transpose( regressor_x ) * weight_x * observation_x );
        auto const parameter_x = expr::compute( part1_x * part2_x );
        
//        auto const centering = expr::compute( make_identity_matrix<N>() - 1./N * make_custom_matrix<N,N>([](std::size_t){return 1.;}) );
//        auto const projection_x = expr::compute( regressor_x * form_inverse( expr::compute( transpose(regressor_x) * regressor_x ) ) * transpose(regressor_x) );
//        double const dcx_p1 = expr::compute( transpose( observation_x) * transpose(projection_x) * expr::compute(centering * projection_x * observation_x ));
//        double const dcx_p2 = 1./ expr::compute( transpose(observation_x) * centering * observation_x );
//        double determination_coefficient_x = dcx_p1 * dcx_p2 ;

        result_p.x = parameter_x.data();
//        result_p.determination_coefficient_x = determination_coefficient_x;

    }
    
    template<std::size_t N, std::size_t Order>
    void compute_polynomial_parameters_y( std::vector<full_state> const& cluster_pc, polynomial_fit_parameters& result_p ) const {
        
        //fit in x/y
        //retrieve fit parameters
        std::array<double, N> z_c{};
        std::array<double, N> y_c{};
        std::array<double, N * N> weight_y_c{};
        
        std::size_t index{0};
        for( auto cluster_i{cluster_pc.begin()+1}; cluster_i != cluster_pc.end() ; ++cluster_i ){
//        for( auto cluster_i{cluster_pc.begin()}; cluster_i != cluster_pc.end() ; ++cluster_i ){
            auto * msd_h =dynamic_cast<TAMSDcluster const*>( cluster_i->data );
            if( msd_h && !msd_h->GetPlaneView() ){ continue; }
            z_c[index] = cluster_i->evaluation_point;
            y_c[index] = cluster_i->vector(1,0);
            weight_y_c[index + index * N] = cluster_i->covariance(1,1);
            ++index;
        }

        constexpr std::size_t const order_y = Order;
        auto const regressor_y = make_custom_matrix<N, order_y>(
                                                                [&z_c, &order_y]( std::size_t index_p ){
                                                                    std::size_t column_index = index_p % order_y;
                                                                    std::size_t row_index = index_p / order_y;
                                                                    return pow( z_c[row_index], column_index );
                                                                }
                                                                );
//        std::cout << "zy:\n";
//        for( auto const& z : z_c ){ std::cout << z << '\n';}
        auto const observation_y = matrix<N, 1>{ std::move(y_c) };
//        std::cout << "observation_y: \n" << observation_y;
        auto const weight_y = matrix<N, N>{ std::move(weight_y_c)};
        
        //computation splitted to reduce instantiation depth (not allowed over 900 for gcc by default)
        auto const part1_y = form_inverse( expr::compute( transpose(regressor_y) * weight_y * regressor_y ) );
        auto const part2_y = expr::compute( transpose( regressor_y ) * weight_y * observation_y );
        auto const parameter_y = expr::compute( part1_y * part2_y );
        
//        auto const centering = expr::compute( make_identity_matrix<N>() - 1./N * make_custom_matrix<N,N>([](std::size_t){return 1.;}) );
//        auto const projection_y = expr::compute( regressor_y * form_inverse( expr::compute( transpose(regressor_y) * regressor_y ) ) * transpose(regressor_y) );
//        double const dcy_p1 = expr::compute( transpose( observation_y) * transpose(projection_y) * expr::compute(centering * projection_y * observation_y));
//        double const dcy_p2 = 1./ expr::compute( transpose(observation_y) * centering * observation_y );
//        double determination_coefficient_y = dcy_p1 * dcy_p2;

        result_p.y = parameter_y.data();
//        result_p.determination_coefficient_y = determination_coefficient_y;
    }
    
    template<std::size_t N>
    polynomial_fit_parameters compute_polynomial_parameters( std::vector<full_state> const& cluster_pc ) const {
        
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
//        std::cout << "parameter_x: \n" << parameter_x;
        
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
//        std::cout << "parameter_y: \n" << parameter_y;
        
//        auto const centering = expr::compute( make_identity_matrix<N>() - 1./N * make_custom_matrix<N,N>([](std::size_t){return 1.;}) );
//        
//        auto const projection_x = expr::compute( regressor_x * form_inverse( expr::compute( transpose(regressor_x) * regressor_x ) ) * transpose(regressor_x) );
//        double const dcx_p1 = expr::compute( transpose( observation_x) * transpose(projection_x) * expr::compute(centering * projection_x * observation_x ));
//        double const dcx_p2 = 1./ expr::compute( transpose(observation_x) * centering * observation_x );
//        double determination_coefficient_x = dcx_p1 * dcx_p2 ;
//        (determination_coefficient_x > 0.95) ? ++over_counter_x : ++below_counter_x;
//        
//        auto const projection_y = expr::compute( regressor_y * form_inverse( expr::compute( transpose(regressor_y) * regressor_y ) ) * transpose(regressor_y) );
//        double const dcy_p1 = expr::compute( transpose( observation_y) * transpose(projection_y) * expr::compute(centering * projection_y * observation_y));
//        double const dcy_p2 = 1./ expr::compute( transpose(observation_y) * centering * observation_y );
//        double determination_coefficient_y = dcy_p1 * dcy_p2;
//        (determination_coefficient_y > 0.95) ? ++over_counter_y : ++below_counter_y;
        
        return {std::move(parameter_x.data()), 0, std::move(parameter_y.data()), 0};
    }
    
    constexpr std::vector< track > compute_track_parameters( std::vector<track>&& track_pc ) const {
        for( auto& track : track_pc) {
            auto cluster_c = track.get_clusters();
            switch( cluster_c.size() ){
                case  4:{ track.parameters = compute_polynomial_parameters< 4>( cluster_c ); break; }
                case  5:{ track.parameters = compute_polynomial_parameters< 5>( cluster_c ); break; }
                case  6:{ track.parameters = compute_polynomial_parameters< 6>( cluster_c ); break; }
                case  7:{ track.parameters = compute_polynomial_parameters< 7>( cluster_c ); break; }
                case  8:{ track.parameters = compute_polynomial_parameters< 8>( cluster_c ); break; }
                case  9:{ track.parameters = compute_polynomial_parameters< 9>( cluster_c ); break; }
                case 10:{ track.parameters = compute_polynomial_parameters<10>( cluster_c ); break; }
                case 11:{ track.parameters = compute_polynomial_parameters<11>( cluster_c ); break; }
                case 12:{ track.parameters = compute_polynomial_parameters<12>( cluster_c ); break; }
                case 13:{ track.parameters = compute_polynomial_parameters<13>( cluster_c ); break; }
                case 14:{ track.parameters = compute_polynomial_parameters<14>( cluster_c ); break; }
                case 15:{ track.parameters = compute_polynomial_parameters<15>( cluster_c ); break; }
                case 16:{ track.parameters = compute_polynomial_parameters<16>( cluster_c ); break; }
            }
            

        }

        return std::move( track_pc );
    }
    
    constexpr std::vector< track > compute_track_parameters_splitted( std::vector<track>&& track_pc ) const {
        for( auto& track : track_pc) {
            auto cluster_c = track.get_clusters();
            std::size_t msd_cluster_x_counter{0};
            std::size_t msd_cluster_y_counter{0};
            for( auto const& cluster : cluster_c){
                auto * msd_h =dynamic_cast<TAMSDcluster const*>( cluster.data );
                if( msd_h ){
                    if( msd_h->GetPlaneView() ){ ++msd_cluster_y_counter; }
                    else{ ++msd_cluster_x_counter; }
                }
            }
            polynomial_fit_parameters result;
//            std::size_t const x_cluster_count = cluster_c.size() -2 - msd_cluster_y_counter; //-tof -vertex - msd_cluster_y
//            std::size_t const x_cluster_count = cluster_c.size() - msd_cluster_y_counter; // -msd_cluster_y
//            std::size_t const x_cluster_count = cluster_c.size(); //all
            std::size_t const x_cluster_count = cluster_c.size() -1 - msd_cluster_y_counter; //-tof/-vertex
            switch( x_cluster_count ){
                case 4:{compute_polynomial_parameters_x<4, 4>( cluster_c, result ); break;}
                case 5:{compute_polynomial_parameters_x<5, 4>( cluster_c, result ); break;}
                case 6:{compute_polynomial_parameters_x<6, 4>( cluster_c, result ); break;}
                case 7:{compute_polynomial_parameters_x<7, 4>( cluster_c, result ); break;}
                case 8:{compute_polynomial_parameters_x<8, 4>( cluster_c, result ); break;}
                case 9:{compute_polynomial_parameters_x<9, 4>( cluster_c, result ); break;}
                case 10:{compute_polynomial_parameters_x<10, 4>( cluster_c, result ); break;}
                case 11:{compute_polynomial_parameters_x<11, 4>( cluster_c, result ); break;}
                case 12:{compute_polynomial_parameters_x<12, 4>( cluster_c, result ); break;}
                case 13:{compute_polynomial_parameters_x<13, 4>( cluster_c, result ); break;}
            }
//            std::size_t const y_cluster_count = cluster_c.size() -2 - msd_cluster_x_counter;//-tof -vertex -msd_cluster_x
//            std::size_t const y_cluster_count = cluster_c.size()  - msd_cluster_x_counter; //-msd_cluster_x
//            std::size_t const y_cluster_count = cluster_c.size(); //all
            std::size_t const y_cluster_count = cluster_c.size() -1 - msd_cluster_x_counter; //-tof
            switch( y_cluster_count ){
                case 4:{compute_polynomial_parameters_y<4, 2>( cluster_c, result ); break;}
                case 5:{compute_polynomial_parameters_y<5, 2>( cluster_c, result ); break;}
                case 6:{compute_polynomial_parameters_y<6, 2>( cluster_c, result ); break;}
                case 7:{compute_polynomial_parameters_y<7, 2>( cluster_c, result ); break;}
                case 8:{compute_polynomial_parameters_y<8, 2>( cluster_c, result ); break;}
                case 9:{compute_polynomial_parameters_y<9, 2>( cluster_c, result ); break;}
                case 10:{compute_polynomial_parameters_y<10, 2>( cluster_c, result ); break;}
                case 11:{compute_polynomial_parameters_y<11, 2>( cluster_c, result ); break;}
                case 12:{compute_polynomial_parameters_y<12, 2>( cluster_c, result ); break;}
                case 13:{compute_polynomial_parameters_y<13, 2>( cluster_c, result ); break;}
            }
            track.parameters = std::move(result);
        }

        return std::move( track_pc );
    }
    
    constexpr std::vector< track > compute_time_of_flight( std::vector<track>&& track_pc ) const {
        for( auto && track : track_pc){
            double beam_speed = sqrt( pow(beam_energy_m*beam_mass_number_m, 2) + 2 * beam_mass_number_m * beam_mass_number_m * 931.5 * beam_energy_m )/(beam_mass_number_m * 931.5 + beam_mass_number_m * beam_energy_m) * 30;
            double additional_time = (track.get_clusters().front().evaluation_point - st_position_m)/beam_speed;
            double tof = (static_cast<TATWpoint const *>(track.get_clusters().back().data)->GetMeanTof() - additional_time);
//            double tof = (static_cast<TATWpoint const *>(track.get_clusters().back().data)->GetToF() - additional_time);
            if(tof < 0){ std::cerr << "Warning: time of flight is below zero\n";}
            track.tof = tof;
        }
        return std::move(track_pc);
    }
    
    constexpr std::vector< track > compute_momentum( std::vector<track>&& track_pc ) const {
        for( auto && track : track_pc){
            double beta = track.length/track.tof * 1./30;
//            puts(__PRETTY_FUNCTION__);
//            std::cout << "track_length: " << track.length << '\n' << "track_tof: " << track.tof << '\n';
            if(std::isnan(beta)){ std::cerr << "beta_is_nan: " << track.length << " - " << track.tof; }
            if( beta < 1 && beta > 0 && !std::isnan(beta)){
                double gamma = 1./sqrt(1 - pow(beta, 2));
                track.beta = beta;
                track.momentum = track.nucleon_number * 931.5 * beta * gamma;
                track.mass = track.momentum/(beta * gamma);
            }
        }
        return std::move( track_pc );
    }
    
    std::vector< track > refine_hypotheses( std::vector<track>&& track_pc ) { 
//        puts(__PRETTY_FUNCTION__);
        int charge{};
        double momentum{};
        struct point{
            double x;
            double y;
            double z;
        };
        
        struct score_and_momentum{
            double score;
            double momentum;
        };
        
        auto position_ode = make_ode< matrix<2,1>, 2>(
                [&charge, &momentum, this](operating_state<matrix<2, 1>, 2> const& os_p){
                    double const dx_dz = os_p.state( details::order_tag<1>{} )(0,0);
                    double const dy_dz = os_p.state( details::order_tag<1>{} )(1,0);
                    double const R = sqrt( dx_dz*dx_dz + dy_dz*dy_dz + 1 );
                    auto field = field_mh->GetField( TVector3{ os_p.state( details::order_tag<0>{} )(0,0), os_p.state( details::order_tag<0>{} )(1,0), os_p.evaluation_point});
                    point field_point{ field.X(), field.Y(), field.Z()};
                    double const change_x = dx_dz*dy_dz * field_point.x - (1+dx_dz*dx_dz) * field_point.y + dy_dz*field_point.z;
                    double const change_y = (1+dy_dz*dy_dz)*field_point.x - dx_dz*dy_dz*field_point.y - dx_dz*field_point.z;
                    return 0.000299792458 * charge/momentum * R * matrix<2,1>{ change_x, change_y };
                }
                                                      );
        auto position_stepper = make_stepper<data_grkn56>( std::move(position_ode) );
//        auto position_stepper = make_stepper<data_grkn4>( std::move(position_ode) );
        position_stepper.specify_tolerance(1e-14);

//        TFile file{"momentum_scan_ol.root", "UPDATE"};
//        std::unique_ptr<TTree> tree_h{nullptr};
//        auto * temp_tree_h = static_cast<TTree*>( file.Get( "data" ) );
//        if( temp_tree_h ){ tree_h.reset( temp_tree_h ); }
//        else{ tree_h.reset( new TTree{ "data", ""} ); }
//        auto * g_h = new TGraph{};
//        double R2{-1};
//        if( tree_h->GetBranch("graphs") ){ tree_h->SetBranchAddress( "graphs" , &g_h ); }
//        else{ tree_h->Branch("graphs", &g_h); }
//        if( tree_h->GetBranch("R2") ){ tree_h->SetBranchAddress( "R2" , &R2); }
//        else{ tree_h->Branch("R2", &R2); }
                        
        auto compute_y_l = []( double z, auto const& track_p ){ return track_p.parameters.y[1] * z + track_p.parameters.y[0];  };
        auto compute_x_l = []( double z, auto const& track_p ){
            return track_p.parameters.x[3] * z * z * z +
                   track_p.parameters.x[2] * z * z +
                   track_p.parameters.x[1] * z +
                   track_p.parameters.x[0];
        };
        auto compute_dydz_l = []( double /*z*/, auto const& track_p ){ return track_p.parameters.y[1];  };
        auto compute_dxdz_l = []( double z, auto const& track_p ){
            return 3*track_p.parameters.x[3] * z * +
                   2*track_p.parameters.x[2] * z +
                   track_p.parameters.x[1];
        };
        
        
        
        for( auto && track : track_pc ){
            charge = track.hypothesis.properties.charge;
            double relative_momentum = track.momentum / track.hypothesis.properties.nucleon_number;
            std::vector<score_and_momentum> refined_c;
//            std::cout << "new_track\n";
//
//            for( double factor{ track.hypothesis.properties.nucleon_number - 1.5 > 0 ? track.hypothesis.properties.nucleon_number - 1.5 : 0.5 };
//                 factor < track.hypothesis.properties.nucleon_number + 1.6 ;
//                 factor+=0.1 ){
            std::uniform_real_distribution<> distribution{(track.get_clusters().begin()+1)->evaluation_point, track.get_clusters().back().evaluation_point};
            std::size_t const scoring_point_number = 10;
            std::vector<double> scoring_point_c(scoring_point_number);
            for(auto i{0}; i < scoring_point_number; ++i){ scoring_point_c.push_back( distribution(random_generator_m)); }
            std::sort( scoring_point_c.begin(), scoring_point_c.end());
            
            for( double factor{ track.hypothesis.properties.nucleon_number - 2 > 0 ? track.hypothesis.properties.nucleon_number - 2 : 0.5 };
                 factor < track.hypothesis.properties.nucleon_number + 2.05 ;
                 factor+=0.05 ){
                momentum = factor * relative_momentum;
                if(momentum < 150){  continue; }
//                std::cout << "momentum: " << momentum << '\n';

                auto starting_point = *(track.get_clusters().begin() +1);
//                auto starting_point = track.get_clusters().front();
                auto position_os = operating_state< matrix<2,1>, 2> {
                    starting_point.evaluation_point,
//                    {
//                        matrix<2,1>{ compute_x_l(starting_point.evaluation_point, track), compute_y_l(starting_point.evaluation_point, track) },
//                        matrix<2,1>{ compute_dxdz_l(starting_point.evaluation_point, track), compute_dydz_l(starting_point.evaluation_point, track) }
//                    }
                    {
                    matrix<2,1>{starting_point.vector(0,0), starting_point.vector(1,0)},
                    matrix<2,1>{starting_point.vector(2,0), starting_point.vector(3,0)}
                    }
                };
                double const z = track.get_clusters().back().evaluation_point;
                auto step = 5e-1;
                
//                std::cout << "current: [" << position_os.state(details::order_tag<0>{})(0,0) << ", ";
//                std::cout << position_os.state(details::order_tag<0>{})(1,0) << "] - [";
//                std::cout << position_os.state(details::order_tag<1>{})(0,0) << ", ";
//                std::cout << position_os.state(details::order_tag<1>{})(1,0) << "] - ";
//                std::cout << position_os.evaluation_point << '\n';
                
//                auto msd_last_z = (track.get_clusters().end()-1)->evaluation_point;
                
                double distance{0};
                for( auto scoring_point : scoring_point_c ){
                while( position_os.evaluation_point + step < scoring_point ){
                    auto step_result = position_stepper.step( std::move(position_os), step );
                    auto new_step_length = position_stepper.optimize_step_length(step, step_result.second);
                    step = ( new_step_length > 10 ) ?
                                10 :
                                (new_step_length < 5e-1) ? 5e-1 : new_step_length;
                    position_os = std::move(step_result.first);
//                    std::cout << "step/position: " << step << "/" << position_os.evaluation_point << '\n';
//                    std::cout << "current: [" << position_os.state(details::order_tag<0>{})(0,0) << ", ";
//                    std::cout << position_os.state(details::order_tag<0>{})(1,0) << "] - [";
//                    std::cout << position_os.state(details::order_tag<1>{})(0,0) << ", ";
//                    std::cout << position_os.state(details::order_tag<1>{})(1,0) << "] - ";
//                    std::cout << position_os.evaluation_point << '\n';
                }
                    step = scoring_point - position_os.evaluation_point;
                    position_os = position_stepper.force_step( std::move(position_os), step );
//                    std::cout << "step/position: " << step << "/" << position_os.evaluation_point << '\n';
                    
                    matrix<2,1> position{ compute_x_l(position_os.evaluation_point, track), compute_y_l(position_os.evaluation_point, track)  };
                    auto residuals = expr::compute(position_os.state( details::order_tag<0>{}) - position );
                    distance += expr::compute( transpose(residuals) * residuals );
                }
//                step = z - position_os.evaluation_point;
//                position_os = position_stepper.force_step( std::move(position_os), step );

//                std::cout << "current: [" << position_os.state(details::order_tag<0>{})(0,0) << ", ";
//                std::cout << position_os.state(details::order_tag<0>{})(1,0) << "] - [";
//                std::cout << position_os.state(details::order_tag<1>{})(0,0) << ", ";
//                std::cout << position_os.state(details::order_tag<1>{})(1,0) << "] - ";
//                std::cout << position_os.evaluation_point << '\n';
                
//                matrix<2,1> position{ compute_x_l(position_os.evaluation_point, track), compute_y_l(position_os.evaluation_point, track)  };
//                matrix<2,1> position{track.get_clusters().back().vector(0, 0), track.get_clusters().back().vector(1,0)  };
//                std::cout << "comparison_point:" << position(0,0) << ", " << position(1,0) << " - > " << compute_dxdz_l(starting_point.evaluation_point, track) << ", " << compute_dydz_l(starting_point.evaluation_point, track)<< '\n';
//                std::cout << "comparison_point:" << position(0,0) << ", " << position(1,0) << " - > " << track.get_clusters().back().vector(2, 0) << ", " << track.get_clusters().back().vector(3, 0)<< '\n';
                
//                auto residuals = expr::compute(position_os.state( details::order_tag<0>{}) - position );
//                double distance = sqrt( expr::compute( transpose(residuals) * residuals ) );
//                std::cout << "distance: " << distance << '\n';
                distance = sqrt(distance/(scoring_point_number-1));
                refined_c.push_back( score_and_momentum{distance, momentum} );
            }
            
//////
//            std::vector<double> score_c, momentum_c;
//            for(auto const& refined : refined_c){
//                score_c.push_back( refined.score );
//                momentum_c.push_back( refined.momentum );
//            }
//            *g_h = TGraph(score_c.size(), momentum_c.data(), score_c.data() );
//            tree_h->Fill();
//            std::sort(refined_c.begin(), refined_c.end(),
//                      [](auto const& v1_p, auto const& v2_p){ return v1_p.score < v2_p.score; });
            
            std::size_t const size{5};
            std::size_t const lower_limit{size/2};
            std::size_t const upper_limit{size/2 +1 };
            
            auto minimum_i = std::min_element( refined_c.begin(), refined_c.end(),
                                               [](auto const& v1_p, auto const& v2_p){ return v1_p.score < v2_p.score; } );
            if( minimum_i == refined_c.end() ){ continue ;}
            if( std::distance(refined_c.begin(), minimum_i) < lower_limit || std::distance(minimum_i, refined_c.end()) < upper_limit ){
                track.momentum = minimum_i->momentum;
            }
            else{
                auto copy_l = [](auto begin_pi, auto end_pi, auto output_i, auto policy_pl){
                    while( begin_pi != end_pi) {
                        *output_i++ = policy_pl( *begin_pi );
                        begin_pi++;
                    }
                };
                
                std::array<double, size> score_c;
                std::array<double, size> momentum_c;
                copy_l( minimum_i-lower_limit, minimum_i+upper_limit, score_c.begin(), [](auto const& value_p){ return value_p.score; } );
                copy_l( minimum_i-lower_limit, minimum_i+upper_limit, momentum_c.begin(), [](auto const& value_p){ return value_p.momentum; } );
                constexpr std::size_t const order = 3;
                auto const regressor = make_custom_matrix<size, order>(
                                   [&momentum_c, &order]( std::size_t index_p ){
                                   std::size_t column_index = index_p % order;
                                   std::size_t row_index = index_p / order;
                                   return pow( momentum_c[row_index], column_index );
                               }
                                                                  );
                auto const observation = matrix<size, 1>{ std::move(score_c) };
                auto const parameter = expr::compute( form_inverse( expr::compute( transpose(regressor) * regressor ) ) *
                                                      expr::compute( transpose( regressor ) * observation ) );
                auto const projection = expr::compute( regressor * form_inverse( expr::compute( transpose(regressor) * regressor ) ) * transpose(regressor) );
                auto const centering = expr::compute( make_identity_matrix<size>() - 1./size * make_custom_matrix<size,size>([](std::size_t){return 1.;}) );
                double determination_coefficient = expr::compute( transpose( observation) * transpose(projection) * centering * projection * observation) * form_inverse( expr::compute( transpose(observation) * centering * observation ) ) ;
                track.determination_coefficient_scan = determination_coefficient;

                track.momentum = (-parameter(1,0)/(2*parameter(2,0)));
            }

            if( track.beta < 1 ){
                double gamma = 1./sqrt(1 - pow(track.beta, 2));
                track.nucleon_number = std::round(track.momentum/( 931.5 * track.beta * gamma ));
                track.mass = track.momentum/(track.beta * gamma);
            }
            
//            tree_h->Fill();
        }
        
//        tree_h->Write();
        return track_pc;
    }
    
    void register_tracks_upward( std::vector<track> track_pc )
    {

        
        std::vector<int> index_c;
        
        for( auto & track : track_pc  ) {
            // -----------------------------
            matcher_m.submit_reconstructed_track( track );
            // -----------------------------
            if( !reconstructed_track_mhc ){ continue; }
//            std::cout << "current_hypothesis_mass: " << track.particle.mass << std::endl;
//            std::cout << "reconstruction_momentum: "<< track.particle.momentum << "\n";
            auto * track_h = reconstructed_track_mhc->NewTrack(
                                                  track.mass /1000 ,
                                                  track.momentum / 1000.,
                                                  static_cast<double>(track.hypothesis.properties.charge),
                                                  track.tof
                                                               );
//            std::cout << "registered_momentum: " << track_h->GetMomentum() << "\n";
            TAGtrack::polynomial_fit_parameters parameters;
            parameters.parameter_x = track.parameters.x;
            parameters.parameter_y = track.parameters.y;
            track_h->SetParameters( parameters );
           
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
               

               // track_h->AddCorrPoint( corrected_position, position_error, momentum, momentum_error ); //corr point not really meas

                if( value.data ){ //needed because first point is vertex, which has no cluster associated
                    auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));

                    
                    auto fill_measured_point_l = [&value, &transformation_h, &position_error, &momentum, &momentum_error, &corrected_position, &track_h](
                                                            auto * cluster_ph,
                                                            std::string&& name_p,
                                                            auto transformation_p
                                                                            ){
                                TVector3 measured_position{ (transformation_h->*transformation_p)(value.data->GetPositionG()) };
                                auto* measured_h = track_h->AddPoint( measured_position, position_error, corrected_position, position_error,
                                                                         momentum, momentum_error );
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
           auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));

           TVector3 vec = GetMomentumAtZ(track_h, transformation_h->GetTGCenter().Z());
           track_h->SetTgtMomentum(vec);
           vec = GetMomentumAtZ(track_h, transformation_h->GetTWCenter().Z());
           track_h->SetTwMomentum(vec);
        }
    }
    
    void output_current_hypothesis()
    {
        logger_m.add_sub_header("current_hypothesis");
        logger_m << "charge: " << current_hypothesis_m.properties.charge << '\n';
        logger_m << "nucleons: " << current_hypothesis_m.properties.nucleon_number << '\n';
        logger_m << "mass: " << current_hypothesis_m.properties.mass << '\n';
        logger_m << "momentum: " << current_hypothesis_m.properties.momentum << '\n';
        
     //   auto * root_h = current_node_mh->get_ancestor();
     //   action_m.logger_m << "track_slope_x: " << root_h->get_value().vector(2, 0) << '\n';
    //    action_m.logger_m << "track_slope_y: " << root_h->get_value().vector(3, 0) << '\n';
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











