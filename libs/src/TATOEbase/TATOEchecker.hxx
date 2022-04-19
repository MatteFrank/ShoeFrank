//
//File      : TATOEchecker.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 28/07/2021
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef TATOEchecker_h
#define TATOEchecker_h

#include "aftereffect.hpp"
#include "flag_set.hpp"

#include <memory>

#include "TATOEutilities.hxx"
#include "TATOEdetector.hxx"

#include "TAGroot.hxx"
#include "TAGcluster.hxx"
#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"

#include "TH2D.h"
#include "TTree.h"

namespace checker {

struct value_and_error{
    double value;
    double error;
};

// -------------------------------------------
//                    Options
struct no_requirement{
    bool predicate(reconstruction_module const& rm_p){ return true; }
};

template<int C>
struct isolate_charge{
    static constexpr int charge = C;
    bool predicate(reconstruction_module const& rm_p){
        return rm_p.reconstructible_o.value().properties.charge == charge;
    }
};

struct well_matched{
    bool predicate(reconstruction_module const& rm_p){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto const& reconstructed = rm_p.reconstructed_o.value();
        return reconstructible.properties.charge == reconstructed.properties.charge &&
               reconstructible.properties.nucleon_number   == reconstructed.properties.nucleon_number;
    }
};

struct charge_well_matched{
    bool predicate(reconstruction_module const& rm_p){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto const& reconstructed = rm_p.reconstructed_o.value();
        return reconstructible.properties.charge == reconstructed.properties.charge;
    }
};

struct badly_matched{
    bool predicate(reconstruction_module const& rm_p){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto const& reconstructed = rm_p.reconstructed_o.value();
        return reconstructible.properties.nucleon_number != reconstructed.properties.nucleon_number;
    }
};

struct purity_predicate{
    bool predicate(reconstruction_module const& rm_p){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto index_c = reconstructible.get_indices();
        std::size_t count{0};
        for( auto const& cluster : reconstructed.get_clusters() ) {
            for( auto const& cluster_index : cluster.mc_index_c ) {
                if( std::any_of( index_c.begin(), index_c.end(),
                                 [&cluster_index]( int index_p ){ return index_p == cluster_index; } ) ){
                    ++count;
                    break;
                }
            }
        }
        return count == reconstructed.get_clusters().size();
    }
};

struct shearing_predicate{
    bool predicate(reconstruction_module const& rm_p){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        return reconstructed.shearing_factor < 0.46;
    }
};

struct beta_predicate{
    bool predicate(reconstruction_module const& rm_p){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        return  reconstructed.length/reconstructed.tof * 1./30 < 1.;
    }
};


struct out_of_resolution_predicate{
    bool predicate(reconstruction_module const& rm_p){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        return abs(reconstructed.properties.momentum - reconstructible.properties.momentum)/reconstructible.properties.momentum > 0.03;
    }
};
//
//struct negative_track_slope_predicate{
//    bool predicate(reconstruction_module const& rm_p){
//        auto const& reconstructible = rm_p.reconstructible_o.value();
//        return reconstructible.parameters.track_slope.x < 0;
//    }
//};
//
//struct positive_track_slope_predicate{
//    bool predicate(reconstruction_module const& rm_p){
//        auto const& reconstructible = rm_p.reconstructible_o.value();
//        return reconstructible.parameters.track_slope.x > 0;
//    }
//};


struct aberrant_arc_length_predicate{
    bool predicate(reconstruction_module const& rm_p){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto const& reconstructed = rm_p.reconstructed_o.value();
        return (reconstructible.parameters.length - reconstructed.length)/(reconstructible.parameters.length) > 0.8;
    }
};


struct correct_tof_predicate{
    bool predicate(reconstruction_module const& rm_p){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        return reconstructible.parameters.tof*1e9 < 20 ;
    }
};


//----------------------------------------------
//                 Format

struct reconstructible_based{};
struct reconstructed_based{};
struct theta_based{};
struct phi_based{};
struct angle_based{};
struct momentum_based{};

template<class Type, class Reference> struct reconstruction_base{
    using base_type = Type;
    using base_reference = Reference;
};
template<class Base = reconstruction_base<momentum_based, reconstructible_based> >
struct histogram{};

struct computation{};



///
template<class Tag, class Base, class CO>
struct generate_histogram{};

template<class Tag, class Base, class CO>
struct generate_tree{};


template<class CO, class MO>
struct successfull_reconstruction_predicate {
    bool predicate( reconstruction_module const& rm_p ) const {
        return rm_p.reconstructible_o.has_value() && rm_p.reconstructed_o.has_value() &&
                CO{}.predicate(rm_p) &&
                MO{}.predicate(rm_p);
    }
};

template<class CO, class MO>
struct is_reconstructible_predicate {
    bool predicate( reconstruction_module const& rm_p ) const {
        return rm_p.reconstructible_o.has_value() &&
                CO{}.predicate(rm_p) &&
                MO{}.predicate(rm_p);
    }
};

template<class Derived, class F> struct single_value_outcome{};
template<class Derived>
struct single_value_outcome<Derived, histogram< reconstruction_base<momentum_based, reconstructible_based>>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructible.properties.momentum/1000 );
    }
};
template<class Derived>
struct single_value_outcome<Derived, histogram< reconstruction_base<momentum_based, reconstructed_based>>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructed.properties.momentum/1000 );
    }
};


template<class Derived>
struct single_value_outcome<Derived, computation>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        ++data.counter;
    }
};



template<class Derived, class F, class CO> struct single_value_output{};
template<class Derived, class CO >
struct single_value_output< Derived, computation, CO >{
    value_and_error output() const {
        value_and_error result;
        auto const& data = static_cast<Derived const&>(*this).data();
        result.value = data.counter;
        result.error = sqrt(data.counter);
        return result;
    }
};

template<class Derived, class B, class CO>
struct single_value_output< Derived, histogram< B>, CO>{
    void output() const { static_cast<Derived const&>(*this).data().value.Write(); }
};


//=====================================================
//                 type

//----------------------------------------------
//                  purity
struct purity_tag{};

template<class Derived, class F> struct purity_outcome{};
template<class Derived>
struct purity_outcome<Derived, histogram<reconstruction_base<momentum_based, reconstructible_based>>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto & data = static_cast<Derived&>(*this).data();
        
        data.recovered_cluster_histogram.Fill(
                            reconstructible.properties.momentum/(1000*reconstructible.properties.nucleon_number),
                            reconstructed.get_clusters().size()
                                          );
        auto index_c = reconstructible.get_indices();
        for( auto const& cluster : reconstructed.get_clusters() ) {
            for( auto const& cluster_index : cluster.mc_index_c ) {
                if( std::any_of( index_c.begin(), index_c.end(),
                                 [&cluster_index]( int index_p ){ return index_p == cluster_index; } ) ){
                    data.correct_cluster_histogram.Fill( reconstructible.properties.momentum/(1000 * reconstructible.properties.nucleon_number) );
                    break;
                }
            }
        }
    }
};

template<class Derived>
struct purity_outcome<Derived, histogram<reconstruction_base<theta_based, reconstructible_based>>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto & data = static_cast<Derived&>(*this).data();
        
        data.recovered_cluster_histogram.Fill(
                            reconstructible.parameters.theta * 180/TMath::Pi(),
                            reconstructed.get_clusters().size()
                                          );
        auto index_c = reconstructible.get_indices();
        for( auto const& cluster : reconstructed.get_clusters() ) {
            for( auto const& cluster_index : cluster.mc_index_c ) {
                if( std::any_of( index_c.begin(), index_c.end(),
                                 [&cluster_index]( int index_p ){ return index_p == cluster_index; } ) ){
                    data.correct_cluster_histogram.Fill( reconstructible.parameters.theta  * 180/TMath::Pi());
                    break;
                }
            }
        }
    }
};

template<class Derived>
struct purity_outcome<Derived, histogram<reconstruction_base<phi_based, reconstructible_based>>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto & data = static_cast<Derived&>(*this).data();
        
        data.recovered_cluster_histogram.Fill(
                            reconstructible.parameters.phi  * 180/TMath::Pi() ,
                            reconstructed.get_clusters().size()
                                          );
        auto index_c = reconstructible.get_indices();
        for( auto const& cluster : reconstructed.get_clusters() ) {
            for( auto const& cluster_index : cluster.mc_index_c ) {
                if( std::any_of( index_c.begin(), index_c.end(),
                                 [&cluster_index]( int index_p ){ return index_p == cluster_index; } ) ){
                    data.correct_cluster_histogram.Fill( reconstructible.parameters.phi  * 180/TMath::Pi() );
                    break;
                }
            }
        }
    }
};

template<class Derived>
struct purity_outcome<Derived, computation>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto & data = static_cast<Derived&>(*this).data();
        
        data.recovered_cluster_count += reconstructed.get_clusters().size();
        auto index_c = reconstructible.get_indices();
        for( auto const& cluster : reconstructed.get_clusters() ) {
            for( auto const& cluster_index : cluster.mc_index_c ) {
                if( std::any_of( index_c.begin(), index_c.end(),
                                 [&cluster_index]( int index_p ){ return index_p == cluster_index; } ) ){
                    data.correct_cluster_count++;
                    break;
                }
            }
        }
    }
};

template<class Derived, class F, class CO> struct purity_output{};
template<class Derived, class CO >
struct purity_output< Derived, computation, CO  >{
    value_and_error output() const {
        value_and_error result;
        auto const& data = static_cast<Derived const&>(*this).data();
        result.value = data.correct_cluster_count* 1./data.recovered_cluster_count;
        result.error = sqrt(result.value* (1+result.value)/data.recovered_cluster_count);
        return result;
    }
};

template<class Derived, class B, class CO>
struct purity_output< Derived, histogram< B>, CO>{
    void output() const {
        auto const& data = static_cast<Derived const&>(*this).data();
        
        TH1D result{ data.recovered_cluster_histogram };
        for(auto i{1}; i < result.GetNbinsX()+1 ; ++i){
            auto recovered = data.recovered_cluster_histogram.GetBinContent(i);
            auto correct = data.correct_cluster_histogram.GetBinContent(i);
            auto purity = recovered > 0 ? correct * 1./recovered : 0;
            auto error = recovered > 0 ? sqrt( purity* (1+ purity)/recovered) : 0;
            if(correct > 50 ){
                result.SetBinContent(i, purity);
                result.SetBinError(i, error );
            }
        }
        result.Write();
    }
};

template<>
struct generate_histogram< purity_tag,  momentum_based, no_requirement >{
    TH1D operator()(){ return TH1D{"purity_mixed_momentum", ";p_{mc} (GeV/u); #psi(p)", 50, 0, 2 };  }
};

template< int C >
struct generate_histogram< purity_tag, momentum_based, isolate_charge<C> >{
    TH1D operator()(){ return TH1D{ Form("purity_charge%d_momentum", C), ";p_{mc} (GeV/u); #psi(p)", 50, 0, 2 };  }
};

template<>
struct generate_histogram< purity_tag, theta_based, no_requirement >{
    TH1D operator()(){ return TH1D{"purity_mixed_theta", ";#theta (#circ); #psi(#theta)", 40, 0, 20 };  }
};

template<int C >
struct generate_histogram< purity_tag, theta_based, isolate_charge<C> >{
    TH1D operator()(){ return TH1D{ Form("purity_charge%d_theta", C), ";#theta (#circ); #psi(#theta)", 40, 0, 20 };  }
};

template<>
struct generate_histogram< purity_tag,  phi_based, no_requirement >{
    TH1D operator()(){ return TH1D{"purity_mixed_phi", ";#phi (#circ); #psi(#phi)", 72, -180, 180 };  }
};

template< int C >
struct generate_histogram< purity_tag,  phi_based, isolate_charge<C> >{
    TH1D operator()(){ return TH1D{ Form("purity_charge%d_phi", C), ";#phi (#circ); #psi(#phi)", 72, -180, 180 };  }
};


template<class F, class CO> struct purity_data{};
template<class B, class CO>
struct purity_data<histogram< B>, CO>{
    TH1D recovered_cluster_histogram{ generate_histogram<purity_tag, typename B::base_type, CO>{}()  };
    TH1D correct_cluster_histogram{generate_histogram<purity_tag, typename B::base_type, CO>{}()};
    
    void clear(){}
};
template<class CO>
struct purity_data< computation, CO >{
    std::size_t recovered_cluster_count{0};
    std::size_t correct_cluster_count{0};

    void clear() { recovered_cluster_count = correct_cluster_count = 0 ;  }
};


//----------------------------------------------
//                  efficiency
struct efficiency_tag{};

template<class Derived, class F> struct efficiency_outcome{};
template<class Derived >
struct efficiency_outcome< Derived, histogram< reconstruction_base< momentum_based, reconstructible_based > > >{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto & data = static_cast<Derived&>(*this).data();
        
        data.reconstructible_distribution.Fill(
                            reconstructible.properties.momentum/(1000*reconstructible.properties.nucleon_number)
                                          );
        
        if( rm_p.reconstructed_o.has_value() ){
            auto const& reconstructed = rm_p.reconstructed_o.value();
            data.reconstructed_distribution.Fill(
                            reconstructible.properties.momentum/(1000*reconstructible.properties.nucleon_number)
                                                 );
        }
    }
};

template<class Derived >
struct efficiency_outcome< Derived, histogram< reconstruction_base< theta_based, reconstructible_based > > >{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto & data = static_cast<Derived&>(*this).data();
        
        data.reconstructible_distribution.Fill(reconstructible.parameters.theta* 180/TMath::Pi());
        
        if( rm_p.reconstructed_o.has_value() ){
            auto const& reconstructed = rm_p.reconstructed_o.value();
            data.reconstructed_distribution.Fill(reconstructible.parameters.theta* 180/TMath::Pi());
        }
    }
};

template<class Derived >
struct efficiency_outcome< Derived, histogram< reconstruction_base< phi_based, reconstructible_based > > >{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto & data = static_cast<Derived&>(*this).data();
        
        data.reconstructible_distribution.Fill( reconstructible.parameters.phi* 180/TMath::Pi() );
        
        if( rm_p.reconstructed_o.has_value() ){
            auto const& reconstructed = rm_p.reconstructed_o.value();
            data.reconstructed_distribution.Fill( reconstructible.parameters.phi* 180/TMath::Pi() );
        }
    }
};

template<class Derived >
struct efficiency_outcome< Derived, computation >{
    void outcome( reconstruction_module const& rm_p ){
        auto & data = static_cast<Derived&>(*this).data();
        
        ++data.reconstructible_counter;
        
        if( rm_p.reconstructed_o.has_value() ){ ++data.reconstructed_counter; }
    }
};

template<class Derived, class F, class CO> struct efficiency_output{};

template<class Derived, class B, class CO>
struct efficiency_output< Derived, histogram< B>, CO>{
    void output() const {
        auto const& data = static_cast<Derived const&>(*this).data();
        
        TH1D result{ data.reconstructed_distribution };
        for(auto i{1}; i < result.GetNbinsX()+1 ; ++i){
            auto reconstructed = data.reconstructed_distribution.GetBinContent(i);
            auto reconstructible = data.reconstructible_distribution.GetBinContent(i);
            auto efficiency = reconstructible > 0 ? reconstructed * 1./reconstructible : 0;
            auto error = reconstructible > 0 ? sqrt( efficiency* (1+ efficiency)/reconstructible) : 0;
            if(reconstructible > 50){
                result.SetBinContent(i, efficiency);
                result.SetBinError(i, error );
            }
        }
        result.Write();
    }
};

template<class Derived, class CO>
struct efficiency_output< Derived, computation, CO>{
    value_and_error output() const {
        value_and_error result;
        auto const& data = static_cast<Derived const&>(*this).data();
        result.value = data.reconstructed_counter* 1./data.reconstructible_counter;
        result.error = sqrt(result.value* (1+result.value)/data.reconstructible_counter);
        return result;
    }
};

template<>
struct generate_histogram< efficiency_tag,  momentum_based, no_requirement >{
    TH1D operator()(){ return TH1D{"efficiency_mixed_momentum", ";p_{mc} (GeV/u); #varepsilon(p)", 50, 0, 2 };  }
};

template< int C >
struct generate_histogram< efficiency_tag, momentum_based, isolate_charge<C> >{
    TH1D operator()(){ return TH1D{ Form("efficiency_charge%d_momentum", C), ";p_{mc} (GeV/u); #varepsilon(p)", 50, 0, 2 };  }
};

template<>
struct generate_histogram< efficiency_tag, theta_based, no_requirement >{
    TH1D operator()(){ return TH1D{"efficiency_mixed_theta", ";#theta (#circ); #varepsilon(#theta)", 40, 0, 20 };  }
};

template<int C >
struct generate_histogram< efficiency_tag, theta_based, isolate_charge<C> >{
    TH1D operator()(){ return TH1D{ Form("efficiency_charge%d_theta", C), ";#theta (#circ); #varepsilon(#theta)", 40, 0, 20 };  }
};

template<>
struct generate_histogram< efficiency_tag,  phi_based, no_requirement >{
    TH1D operator()(){ return TH1D{"efficiency_mixed_phi", ";#phi (#circ); #varepsilon(#phi)", 72, -180, 180 };  }
};

template< int C >
struct generate_histogram< efficiency_tag,  phi_based, isolate_charge<C> >{
    TH1D operator()(){ return TH1D{ Form("efficiency_charge%d_phi", C), ";#phi (#circ); #varepsilon(#phi)", 72, -180, 180 };  }
};


template<class F, class CO> struct efficiency_data{};
template<class B, class CO>
struct efficiency_data<histogram< B>, CO>{
    TH1D reconstructed_distribution{ generate_histogram<efficiency_tag, typename B::base_type, CO>{}()  };
    TH1D reconstructible_distribution{generate_histogram<efficiency_tag, typename B::base_type, CO>{}()};
    
    void clear(){}
};


template<class CO>
struct efficiency_data<computation, CO>{
    std::size_t reconstructed_counter{0};
    std::size_t reconstructible_counter{0};
    
    void clear(){ reconstructed_counter = reconstructible_counter = 0; }
};
//----------------------------------------------
//                  fake_distribution


template<class F, class CO> struct fake_distribution_data{};
template<class CO> struct fake_distribution_data<computation, CO>{
    std::size_t counter{0};
    void clear(){  counter = 0 ; }
};

template<class CO, class MO>
struct fake_distribution_predicate {
    bool predicate( reconstruction_module const& rm_p ) const {
        return !rm_p.reconstructible_o.has_value() && rm_p.reconstructed_o.has_value() &&
                CO{}.predicate(rm_p) &&
                MO{}.predicate(rm_p);
    }
};


//----------------------------------------------
//                  missed_distribution

template<class CO, class MO>
struct missed_distribution_predicate {
    bool predicate( reconstruction_module const& rm_p ) const {
        return rm_p.reconstructible_o.has_value() && !rm_p.reconstructed_o.has_value() &&
                CO{}.predicate(rm_p) &&
                MO{}.predicate(rm_p);
    }
};

template<class F, class CO> struct missed_distribution_data{};
template<class CO> struct missed_distribution_data<computation, CO>{
    std::size_t counter{0};
    void clear(){  counter = 0 ; }
};

template<>
struct missed_distribution_data<histogram<reconstruction_base<momentum_based, reconstructible_based>>, no_requirement>{
    TH1D value{ "missed_distribution_mixed_pmc", ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};

template<int C>
struct missed_distribution_data<histogram<reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<C>>{
    TH1D value{ Form("missed_distribution_charge%d_pmc", C), ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};

//----------------------------------------------
//                  reconstructed_distribution

template<class F, class CO> struct reconstructed_distribution_data{};
template<>
struct reconstructed_distribution_data<histogram< reconstruction_base< momentum_based, reconstructible_based>>, no_requirement>{
    TH1D value{ "reconstructed_distribution_mixed_pmc", ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};
template<int C>
struct reconstructed_distribution_data<histogram<reconstruction_base< momentum_based, reconstructible_based>>, isolate_charge<C>>{
    TH1D value{ Form("reconstructed_distribution_charge%d_pmc", C), ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};
template<>
struct reconstructed_distribution_data<histogram< reconstruction_base< momentum_based, reconstructed_based>>, no_requirement>{
    TH1D value{ "reconstructed_distribution_mixed_prec", ";p_{rec} (GeV); Count", 500, 0, 13 };
    void clear(){}
};
template<int C>
struct reconstructed_distribution_data<histogram< reconstruction_base< momentum_based, reconstructed_based>>, isolate_charge<C>>{
    TH1D value{ Form("reconstructed_distribution_charge%d_prec", C), ";p_{rec} (GeV); Count", 500, 0, 13 };
    void clear(){}
};
template<class CO>
struct reconstructed_distribution_data< computation, CO >{
    std::size_t counter{0};
    void clear(){ puts(__PRETTY_FUNCTION__); counter = 0 ; }
};


//----------------------------------------------
//                  reconstructible_distribution

template<class F, class CO> struct reconstructible_distribution_data{};
template<>
struct reconstructible_distribution_data<histogram< reconstruction_base<momentum_based, reconstructible_based>>, no_requirement>{
    TH1D value{ "reconstructible_distribution_mixed_pmc", ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};

template<int C>
struct reconstructible_distribution_data<histogram< reconstruction_base<momentum_based, reconstructible_based>>, isolate_charge<C>>{
    TH1D value{ Form("reconstructible_distribution_charge%d_pmc", C), ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};

template<class CO>
struct reconstructible_distribution_data< computation, CO >{
    std::size_t counter{0};
    void clear(){ counter = 0 ; }
};

template<class CO, class MO>
struct reconstructible_distribution_predicate {
    bool predicate( reconstruction_module const& rm_p ) const {
        return rm_p.reconstructible_o.has_value() &&
               CO{}.predicate(rm_p) &&
               MO{}.predicate(rm_p);
    }
};


//----------------------------------------------
//           clone_distribution
template<class Derived, class F> struct clone_distribution_outcome{};
template<class Derived>
struct clone_distribution_outcome<Derived, computation>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.counter+=reconstructed.clone_number;
    }
};

template<class Derived, class B>
struct clone_distribution_outcome<Derived, histogram<B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill(reconstructed.clone_number);
    }
};


template<class F, class CO> struct clone_distribution_data{};
template<class CO> struct clone_distribution_data<computation, CO>{
    std::size_t counter{0};
    void clear(){ counter = 0 ; }
};

template<class B> struct clone_distribution_data<histogram<B>, no_requirement>{
    TH1D value{ "clone_distribution_mixed", ";multiplicity; Count", 500, 0, 500 };
    void clear(){}
};

template<class B, int C> struct clone_distribution_data<histogram<B>, isolate_charge<C>>{
    TH1D value{ Form("clone_distribution_charge%d", C), ";multiplicity; Count", 500, 0, 500 };
    void clear(){}
};


//----------------------------------------------
//           mass_identification
template<class Derived, class F> struct mass_identification_outcome{};
template<class Derived>
struct mass_identification_outcome<Derived, computation>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        if(reconstructed.properties.nucleon_number == reconstructible.properties.nucleon_number){ ++data.good_identification_counter; }
        ++data.total_counter;
    }
};

template<class Derived, class B>
struct mass_identification_outcome<Derived, histogram< B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructible.properties.nucleon_number, reconstructed.properties.nucleon_number );
        data.proportion.Fill( reconstructible.properties.nucleon_number, 0.5 );
    }
};

template<class Derived, class F, class CO> struct mass_identification_output{};
template<class Derived, class CO >
struct mass_identification_output< Derived, computation, CO  >{
    value_and_error output() const {
        value_and_error result;
        auto const& data = static_cast<Derived const&>(*this).data();
        result.value = data.good_identification_counter* 1./data.total_counter;
        result.error = sqrt(result.value* (1+result.value)/data.total_counter);
        return result;
    }
};

template<class Derived, class B, class CO >
struct mass_identification_output< Derived, histogram<B>, CO  >{
    void output() const {
        auto const& data = static_cast<Derived const&>(*this).data();
        auto result = TH2D{data.value};
        
        for(auto i{1}; i < data.value.GetNbinsX() +1  ; ++i){
            double total{0};
            for(auto j{1}; j < data.value.GetNbinsY() +1 ; ++ j){ total += data.value.GetBinContent(i,j); }
            if(total != 0){
                for(auto j{1}; j < data.value.GetNbinsY() +1 ; ++ j){
                    double value = std::round(data.value.GetBinContent(i,j)/total * 1000.)/10;
                    result.SetBinContent(i,j, value );
                }
            }
        }
        result.Write();
        
        double total{0};
        for(auto i{1} ; i < data.value.GetNbinsX() +1 ; ++i){ total += data.proportion.GetBinContent(i,1); }
        auto proportion = TH2D{data.proportion};
        if(total != 0){
            for(auto i{1} ; i < data.value.GetNbinsX() +1 ; ++i){
                double value = std::round(data.proportion.GetBinContent(i,1)/total * 10000.)/100;
                proportion.SetBinContent(i, 1, value );
            }
        }
        proportion.Write();
    }
};

template<class F, class CO> struct mass_identification_data{};

template<class CO>
struct mass_identification_data<computation, CO>{
    std::size_t good_identification_counter{0};
    std::size_t total_counter{0};

    void clear() { good_identification_counter = total_counter = 0 ;  }
};

template<class B>
struct mass_identification_data<histogram< B>, no_requirement> {
    TH2D value{"mass_identification_mixed", ";A_{MC};A_{R};%", 20, 0, 20, 20, 0, 20  };
    TH2D proportion{"mass_proportion_mixed", ";A_{MC};;%", 20, 0, 20, 1, 0, 1 };
    void clear(){}
};

template< class B, int C>
struct mass_identification_data<histogram< B>, isolate_charge<C>> {
    TH2D value{Form("mass_identification_charge%d", C), ";A_{MC};A_{R};%", 20, 0, 20, 20, 0, 20  };
    TH2D proportion{Form("mass_proportion_charge%d", C), ";A_{MC};;%", 20, 0, 20, 1, 0, 1 };
    void clear(){}
};

//----------------------------------------------
//           mass_distribution

template<class Derived, class F> struct mass_distribution_outcome{};
template<class Derived, class T>
struct mass_distribution_outcome<Derived, histogram<reconstruction_base<T, reconstructible_based>>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructible.properties.mass/1000 );
    }
};
template<class Derived, class T>
struct mass_distribution_outcome<Derived, histogram< reconstruction_base<T, reconstructed_based>>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructed.properties.mass/1000 );
    }
};


template<class F, class CO> struct mass_distribution_data{};
template<class T>
struct mass_distribution_data<histogram< reconstruction_base<T, reconstructible_based>>, no_requirement>{
    TH1D value{ "mass_distribution_mixed_mc", ";mass (GeV/c^2); Count", 500, 0, 20 };
    void clear(){}
};
template<class T, int C>
struct mass_distribution_data<histogram< reconstruction_base<T, reconstructible_based>>, isolate_charge<C>>{
    TH1D value{ Form("mass_distribution_charge%d_mc", C), ";mass (GeV/c^2); Count", 500, 0, 20 };
    void clear(){}
};

template<class T>
struct mass_distribution_data<histogram< reconstruction_base<T, reconstructed_based>>, no_requirement>{
    TH1D value{ "mass_distribution_mixed_rec", ";mass (GeV/c^2); Count", 500, 0, 20 };
    void clear(){}
};
template<class T, int C>
struct mass_distribution_data<histogram<reconstruction_base<T,  reconstructed_based>>, isolate_charge<C>>{
    TH1D value{ Form("mass_distribution_charge%d_rec", C), ";mass (GeV/c^2); Count", 500, 0, 20 };
    void clear(){}
};

//----------------------------------------------
//           momentum_resolution

struct momentum_resolution_tag{};
struct momentum_bias_tag{};
struct momentum_sigma_tag{};

template<class Derived, class F> struct momentum_resolution_outcome{};

template<class Derived>
struct momentum_resolution_outcome<Derived, computation>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        auto momentum_difference = (reconstructible.properties.momentum - reconstructed.properties.momentum)/reconstructible.properties.momentum;
        ++data.counter;
        data.residuals += pow( momentum_difference, 2);
    }
};

template<class Derived>
struct momentum_resolution_outcome<Derived, histogram< reconstruction_base< momentum_based, reconstructible_based >> >{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        auto momentum_difference =  (reconstructible.properties.momentum - reconstructed.properties.momentum)/reconstructible.properties.momentum;
        data.value.fill_with(reconstructible.properties.momentum/1000, momentum_difference);
    }
};

template<class Derived>
struct momentum_resolution_outcome<Derived, histogram< reconstruction_base< momentum_based, reconstructed_based >> >{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        auto momentum_difference =  (reconstructible.properties.momentum - reconstructed.properties.momentum)/reconstructible.properties.momentum;
        data.value.fill_with(reconstructed.properties.momentum/1000, momentum_difference);
    }
};

template<class Derived>
struct momentum_resolution_outcome<Derived, histogram< reconstruction_base< theta_based, reconstructible_based >> >{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        auto momentum_resolution =  (reconstructible.properties.momentum - reconstructed.properties.momentum)/reconstructible.properties.momentum;
        data.value.fill_with(reconstructible.parameters.theta* 180/TMath::Pi(), momentum_resolution);
    }
};

template<class Derived>
struct momentum_resolution_outcome<Derived, histogram< reconstruction_base< phi_based, reconstructible_based >> >{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        auto momentum_resolution = (reconstructible.properties.momentum - reconstructed.properties.momentum)/reconstructible.properties.momentum;
        data.value.fill_with(reconstructible.parameters.phi* 180/TMath::Pi(), momentum_resolution);
    }
};

template<class Derived>
struct momentum_resolution_outcome<Derived, histogram< reconstruction_base< angle_based, reconstructible_based >> >{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        auto momentum_resolution = (reconstructible.properties.momentum - reconstructed.properties.momentum)/reconstructible.properties.momentum;
        data.value.fill_with( reconstructible.parameters.theta *180/TMath::Pi(), reconstructible.parameters.phi* 180/TMath::Pi(), momentum_resolution);
    }
};

template<class F, class CO> struct momentum_resolution_data{};

template<class CO>
struct momentum_resolution_data<computation, CO>{
    double residuals{0};
    std::size_t counter{0};

    void clear() { residuals = 0.; counter = 0 ;  }
};

template<class B, class CO>
struct momentum_resolution_data<histogram< B>, CO> {
    struct link {
       TTree* tree_h;
        double center;
    };
    
    template<class T> struct bin_size_value{};
    template<> struct bin_size_value< momentum_based >{ static constexpr double value = 0.2; };
    template<> struct bin_size_value< theta_based >{ static constexpr double value = 0.5; };
    template<> struct bin_size_value< phi_based >{ static constexpr double value = 5; };
    
    static constexpr double bin_size = bin_size_value<typename B::base_type>::value;
    
    struct holder{
        std::vector<link> link_c;
        holder() { link_c.reserve(100); }
        void fill_with( double input_p, double momentum_difference_p ){
            auto link_i = std::find_if(
                                link_c.begin(), link_c.end(),
                                [&input_p]( auto const& link_p ){
                                            return link_p.center-bin_size/2 <= input_p && input_p < link_p.center+bin_size/2;
                                                                    }
                                        );
            if( link_i != link_c.cend() ){
//                std::cout << "center: " << input_p << " -> " << link_i->center << '\n';
                link_i->tree_h->SetBranchAddress("value", &momentum_difference_p);
                link_i->tree_h->Fill();
            }
            else{
                auto center = (static_cast<int>( input_p/bin_size ) * bin_size + ( input_p < 0 ? -bin_size/2. : bin_size/2.));
                
                link_c.push_back( link{generate_tree<momentum_resolution_tag,  typename B::base_type, CO>{}(center),
                                       center }  );
                link_c.back().tree_h->Branch("value", &momentum_difference_p);
                link_c.back().tree_h->Fill();
            }
        }
    };
    
    holder value;
    void clear(){}
};


template<class T, class CO>
struct momentum_resolution_data<histogram< reconstruction_base< angle_based, T>>, CO> {
    struct link {
       TTree* tree_h;
        double theta;
        double phi;
    };
    
    static constexpr double bin_size_theta =1.;
    static constexpr double bin_size_phi = 10.;
    
    struct holder{
        std::vector<link> link_c;
        holder() { link_c.reserve(100); }
        void fill_with( double theta_p, double phi_p, double momentum_difference_p ){
            auto link_i = std::find_if(
                                link_c.begin(), link_c.end(),
                                [&theta_p, &phi_p]( auto const& link_p ){
                                            return (link_p.theta-bin_size_theta/2 <= theta_p && theta_p < link_p.theta+bin_size_theta/2 ) &&
                                                    (link_p.phi-bin_size_phi/2 <= phi_p && phi_p < link_p.phi+bin_size_phi/2 );
                                                                    }
                                        );
            if( link_i != link_c.cend() ){
//                std::cout << "center: " << input_p << " -> " << link_i->center << '\n';
                link_i->tree_h->SetBranchAddress("value", &momentum_difference_p);
                link_i->tree_h->Fill();
            }
            else{
                auto theta = (static_cast<int>(theta_p/bin_size_theta ) * bin_size_theta + ( theta_p < 0 ? -bin_size_theta/2. : bin_size_theta/2.));
                auto phi = (static_cast<int>( phi_p/bin_size_phi ) * bin_size_phi + ( phi_p < 0 ? -bin_size_phi/2. : bin_size_phi/2.));
                
                link_c.push_back( link{generate_tree<momentum_resolution_tag, angle_based, CO>{}(theta, phi),
                                       theta, phi }  );
                link_c.back().tree_h->Branch("value", &momentum_difference_p);
                link_c.back().tree_h->Fill();
            }
        }
    };
    
    holder value;
    void clear(){}
};



template<>
struct generate_tree< momentum_resolution_tag, momentum_based, no_requirement>{
    TTree* operator()(double momentum_p){ return new TTree{Form("momentum_difference_%.2fGev/c_mixed", momentum_p), ""}; }
};

template<int C>
struct generate_tree< momentum_resolution_tag, momentum_based, isolate_charge<C>>{
    TTree* operator()(double momentum_p){ return new TTree{Form("momentum_difference_%.2fGev/c_charge%d", momentum_p, C), ""}; }
};

template<>
struct generate_tree< momentum_resolution_tag, theta_based, no_requirement>{
    TTree* operator()(double theta_p){ return new TTree{Form("momentum_difference_%.1fdeg_mixed", theta_p), ""}; }
};

template<>
struct generate_tree< momentum_resolution_tag, phi_based, no_requirement>{
    TTree* operator()(double phi_p){ return new TTree{Form("momentum_difference_%s%.0fdeg_mixed", (phi_p > 0 ? "pos": "neg") ,abs(phi_p)), ""}; }
};

template<int C>
struct generate_tree< momentum_resolution_tag, theta_based, isolate_charge<C>>{
    TTree* operator()(double theta_p){ return new TTree{Form("momentum_difference_%.1fdeg_charge%d", theta_p, C), ""}; }
};

template<int C>
struct generate_tree< momentum_resolution_tag, phi_based, isolate_charge<C>>{
    TTree* operator()(double phi_p){ return new TTree{Form("momentum_difference_%s%.0fdeg_charge%d", (phi_p > 0 ? "pos": "neg") ,abs(phi_p), C), ""}; }
};

template<>
struct generate_tree< momentum_resolution_tag, angle_based, no_requirement>{
    TTree* operator()(double theta_p, double phi_p){ return new TTree{Form("momentum_difference_%.1fdeg_%s%.0fdeg_mixed", theta_p, (phi_p > 0 ? "pos": "neg") ,abs(phi_p)), ""} ; }
};

template<int C>
struct generate_tree< momentum_resolution_tag, angle_based, isolate_charge<C>>{
    TTree* operator()(double theta_p, double phi_p){ return new TTree{Form("momentum_difference_%.1fdeg_%s%.0fdeg_charge%d", theta_p, (phi_p > 0 ? "pos": "neg") ,abs(phi_p), C), ""} ; }
};

    
    

template<>
struct generate_histogram< momentum_resolution_tag,  reconstruction_base<momentum_based, reconstructed_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_resolution_mixed_rec", ";p_{rec} (GeV); #sigma_{p}/p(p)", bin_count, 0, maximum_value };
    }
};

template<>
struct generate_histogram< momentum_resolution_tag,  reconstruction_base<momentum_based, reconstructible_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_resolution_mixed_mc", ";p_{mc} (GeV); #sigma_{p}/p(p)", bin_count, 0, maximum_value };
    }
};

template<int C>
struct generate_histogram< momentum_resolution_tag,  reconstruction_base<momentum_based, reconstructed_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_resolution_charge%d_rec", C), ";p_{rec} (GeV); #sigma_{p}/p(p)", bin_count, 0, maximum_value };
    }
};

template<int C>
struct generate_histogram< momentum_resolution_tag,  reconstruction_base<momentum_based, reconstructible_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_resolution_charge%d_mc", C), ";p_{mc} (GeV); #sigma_{p}/p(p)", bin_count, 0, maximum_value };
    }
};

template<>
struct generate_histogram< momentum_resolution_tag,  reconstruction_base<theta_based, reconstructible_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_resolution_mixed_theta", ";#theta (#circ); #sigma_{p}/p(#theta)", 40, 0, 20 };
    }
};

template<>
struct generate_histogram< momentum_resolution_tag,  reconstruction_base<phi_based, reconstructible_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_resolution_mixed_phi", ";#phi (#circ); #sigma_{p}/p(#phi)", 72, -180, 180 };
    }
};

template<int C>
struct generate_histogram< momentum_resolution_tag,  reconstruction_base<theta_based, reconstructible_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_resolution_charge%d_theta", C), ";#theta (#circ); #sigma_{p}/p(#theta)", 40, 0, 20 };
    }
};

template<int C>
struct generate_histogram< momentum_resolution_tag,  reconstruction_base<phi_based, reconstructible_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_resolution_charge%d_phi", C), ";#phi (#circ); #sigma_{p}/p(#phi)", 72, -180, 180 };
    }
};


template<>
struct generate_histogram< momentum_sigma_tag,  reconstruction_base<momentum_based, reconstructible_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_sigma_mixed_mc", ";p_{mc} (GeV); #sigma_{p}/p(p)", bin_count, 0, maximum_value };
    }
};

template<int C>
struct generate_histogram< momentum_sigma_tag,  reconstruction_base<momentum_based, reconstructible_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_sigma_charge%d_mc", C), ";p_{mc} (GeV); #sigma_{p}/p(p)", bin_count, 0, maximum_value };
    }
};

template<>
struct generate_histogram< momentum_sigma_tag,  reconstruction_base<theta_based, reconstructible_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_sigma_mixed_theta", ";#theta (#circ); #sigma_{p}/p(#theta)", 40, 0, 20 };
    }
};

template<>
struct generate_histogram< momentum_sigma_tag,  reconstruction_base<phi_based, reconstructible_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_sigma_mixed_phi", ";#phi (#circ); #sigma_{p}/p(#phi)", 72, -180, 180 };
    }
};

template<int C>
struct generate_histogram< momentum_sigma_tag,  reconstruction_base<theta_based, reconstructible_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_sigma_charge%d_theta", C), ";#theta (#circ); #sigma_{p}/p(#theta)", 40, 0, 20 };
    }
};

template<int C>
struct generate_histogram< momentum_sigma_tag,  reconstruction_base<phi_based, reconstructible_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_sigma_charge%d_phi", C), ";#phi (#circ); #sigma_{p}/p(#phi)", 72, -180, 180 };
    }
};



template<>
struct generate_histogram< momentum_bias_tag,  reconstruction_base<momentum_based, reconstructed_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_bias_mixed_rec", ";p_{rec} (GeV); bias(p)", bin_count, 0, maximum_value };
    }
};

template<>
struct generate_histogram< momentum_bias_tag,  reconstruction_base<momentum_based, reconstructible_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_bias_mixed_mc", ";p_{mc} (GeV); bias(p)", bin_count, 0, maximum_value };
    }
};

template<int C>
struct generate_histogram< momentum_bias_tag,  reconstruction_base<momentum_based, reconstructed_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_bias_charge%d_rec", C), ";p_{rec} (GeV); bias(p)", bin_count, 0, maximum_value };
    }
};

template<int C>
struct generate_histogram< momentum_bias_tag,  reconstruction_base<momentum_based, reconstructible_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
        double maximum_value = maximum_center_value_p + bin_size_p/2;
        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_bias_charge%d_mc", C), ";p_{mc} (GeV); bias(p)", bin_count, 0, maximum_value };
    }
};




template<>
struct generate_histogram< momentum_bias_tag,  reconstruction_base<theta_based, reconstructible_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_bias_mixed_theta", ";#theta (#circ); bias(#theta)", 40, 0, 20 };
    }
};

template<>
struct generate_histogram< momentum_bias_tag,  reconstruction_base<phi_based, reconstructible_based> , no_requirement >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{"momentum_bias_mixed_phi", ";#theta (#circ); bias(#phi)", 72, -180, 180 };
    }
};

template<int C>
struct generate_histogram< momentum_bias_tag,  reconstruction_base<theta_based, reconstructible_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_bias_charge%d_theta", C), ";#theta (#circ); bias(#theta)", 40, 0, 20 };
    }
};

template<int C>
struct generate_histogram< momentum_bias_tag,  reconstruction_base<phi_based, reconstructible_based> , isolate_charge<C> >{
    TH1D operator()(double bin_size_p, double maximum_center_value_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_bias_charge%d_phi", C), ";#phi (#circ); bias(#phi)", 72, -180, 180 };
    }
};




template<>
struct generate_histogram< momentum_resolution_tag,  angle_based , no_requirement >{
    TH1D operator()(double theta_p){
//       double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bionin_size_p;
        return TH1D{Form("momentum_resolution_mixed_angle_theta%.1fdeg", theta_p), ";#phi (#circ) ; #sigma_{p}/p(#theta, #phi)", 36, -180, 180  };
    }
};

template<int C>
struct generate_histogram< momentum_resolution_tag,  angle_based , isolate_charge<C> >{
    TH1D operator()(double theta_p){
//        double maximum_value = maximum_center_value_p + bin_size_p/2;
//        int bin_count = maximum_value/bin_size_p;
        return TH1D{Form("momentum_resolution_charge%d_angle_theta%.1fdeg", C, theta_p), ";#phi (#circ); #sigma_{p}/p(#theta, #phi)", 36, -180, 180   };
    }
};
    
    
    

template<class Derived, class F, class CO> struct momentum_resolution_output{};
template<class Derived, class B, class CO>
struct momentum_resolution_output< Derived, histogram< B>, CO>{
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        
//        std::vector<link> link_c;
//        link_c.reserve( data.value.link_c.size() );
//        std::for_each( data.value.link_c.begin(), data.value.link_c.end(),
//                      [&link_c](auto const& link_p){ link_c.push_back( link{link_p.tree_h.get(), link_p.center } ); } );
//
        auto link_c = data.value.link_c;
        
        std::sort(link_c.begin(), link_c.end(), [](auto const& l1_p, auto const& l2_p){ return l1_p.center < l2_p.center; });

        
        auto bias_histogram = generate_histogram< momentum_bias_tag, B, CO >{}(data.bin_size, link_c.back().center );
        auto resolution_histogram = generate_histogram< momentum_resolution_tag, B, CO >{}(data.bin_size, link_c.back().center );
        auto sigma_histogram = generate_histogram< momentum_sigma_tag, B, CO >{}(data.bin_size, link_c.back().center );
        
        double bias{0};
        double sigma{0};
        double resolution{0};
        
        for(auto const& link : link_c){
            double value{0};
            int entries = link.tree_h->GetEntries();
            link.tree_h->SetBranchAddress("value", &value);
            if( entries < 100 ){continue;}
            for(auto i{0}; i < entries; ++i){
                link.tree_h->GetEntry(i);
                bias += value;
                resolution += pow(value, 2);
            }
            bias = entries > 0 ? bias/entries : bias ;
            for(auto i{0}; i < entries; ++i){
                link.tree_h->GetEntry(i);
                sigma += pow( value - bias , 2);
            }
            resolution = entries > 1 ? sqrt(resolution/(entries-1)) : sqrt(resolution);
            sigma =  entries > 1 ? sqrt(sigma/(entries-1)) : sqrt(sigma);
            auto bias_error = entries > 1 ? sigma/sqrt(entries-1) : sigma;
            auto sigma_error = entries > 1 ? abs( bias/(entries-1) * 1./sqrt(entries-1) ) : abs(bias);
            auto resolution_error = entries > 1 ? 1./2 * sqrt(entries-1)* resolution/(entries-1) : 1./2 * resolution;
            
            bias_histogram.SetBinContent( bias_histogram.FindBin(link.center), bias );
            bias_histogram.SetBinError( bias_histogram.FindBin(link.center), bias_error );
            resolution_histogram.SetBinContent(resolution_histogram.FindBin(link.center), resolution);
            resolution_histogram.SetBinError(resolution_histogram.FindBin(link.center), resolution_error);
            sigma_histogram.SetBinContent(resolution_histogram.FindBin(link.center), sigma);
            sigma_histogram.SetBinError(resolution_histogram.FindBin(link.center), sigma_error);
        }
        
//        std::cout << "writing bias" << std::endl;
        bias_histogram.Write();
//        std::cout << "writing resolution" << std::endl;
        resolution_histogram.Write();
//        std::cout << "writing tree" << std::endl;
        sigma_histogram.Write();
        for( auto& link: link_c){
            if(link.tree_h->GetEntries() > 100 ) {link.tree_h->Write();}
        }
//        std::cout << "writing done" << std::endl;
    }
};


template<class Derived, class T, class CO>
struct momentum_resolution_output< Derived, histogram< reconstruction_base< angle_based, T>>, CO>{
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        auto link_c = data.value.link_c;
        
        std::vector<double> theta_c;
        std::sort(link_c.begin(), link_c.end(), [](auto const& l1_p, auto const& l2_p){ return l1_p.theta < l2_p.theta; });
        std::for_each(link_c.begin(), link_c.end(),
                     [&theta_c](auto const& link_p){
            if( std::none_of(theta_c.begin(), theta_c.end(), [&link_p](double theta_p){ return theta_p == link_p.theta; }) ){
                theta_c.push_back(link_p.theta);
            }
        }
                     );
        
        for( double theta : theta_c ){
            auto end_i = std::partition( link_c.begin(), link_c.end(), [&theta](auto const& link_p){ return link_p.theta == theta; }  );
            auto resolution_histogram = generate_histogram< momentum_resolution_tag, angle_based, CO >{}(theta);
        
            double resolution{0};
        
            for(auto link_i = link_c.begin(); link_i != end_i ; ++link_i){
                double value{0};
                int entries = link_i->tree_h->GetEntries();
                link_i->tree_h->SetBranchAddress("value", &value);
                if( entries < 100 ){continue;}
                for(auto i{0}; i < entries; ++i){
                    link_i->tree_h->GetEntry(i);
                    resolution += pow(value, 2);
                }
                resolution = entries > 1 ? sqrt(resolution/(entries-1)) : sqrt(resolution);
                auto resolution_error = entries > 1 ? 1./2 * sqrt(entries-1)* resolution/(entries-1) : 1./2 * resolution;
            
                resolution_histogram.SetBinContent(resolution_histogram.FindBin(link_i->phi), resolution);
                resolution_histogram.SetBinError(resolution_histogram.FindBin(link_i->phi), resolution_error);
            }
            
            resolution_histogram.Write();
            for(auto link_i = link_c.begin(); link_i != end_i ; ++link_i){
                if(link_i->tree_h->GetEntries() > 100 ) {link_i->tree_h->Write();}
            }
        }
    }
};

template<class Derived, class CO>
struct momentum_resolution_output< Derived, computation, CO>{
    value_and_error output() const {
        value_and_error result;
        auto const& data = static_cast<Derived const&>(*this).data();
        result.value = data.counter > 1 ? sqrt(data.residuals/(data.counter-1)) : data.residuals ;
        result.error = data.counter > 1 ? 1./2 * sqrt(data.counter-1)* result.value/(data.counter-1) : 1./2 * result.value;
        return result;
    }
};




//----------------------------------------------
//                  residuals

//way to separate X/Y ? just change output from single_value to custom
template<class Derived, class F> struct residuals_outcome{};
template<class Derived, class B>
struct residuals_outcome<Derived, histogram< B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        
        auto compute_position_x_l = [&parameters = reconstructed.parameters.x]( double z ){
            return parameters[3] * z * z * z + parameters[2] * z * z + parameters[1] * z + parameters[0];
        };
        auto compute_position_y_l = [&parameters = reconstructed.parameters.y]( double z ){
            return parameters[1] * z + parameters[0];
        };
        
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        auto transform_l = [&transformation_h]( auto cluster_p, auto transformation_p )
                                    {
                                        TVector3 position{ cluster_p.position.x, cluster_p.position.y, cluster_p.position.z};
                                        return (transformation_h->*transformation_p)(position);
                                    };
        
        
        auto const& cluster_c = reconstructed.get_clusters();
        for( auto const& cluster : cluster_c){
            
            TVector3 cluster_position;
            switch( cluster.opcode ) {
                case flag_set<details::vertex_tag>{}: {
                    cluster_position = transform_l( cluster, &TAGgeoTrafo::FromVTLocalToGlobal );
                    auto track_position_x = compute_position_x_l( cluster_position.Z() );
                    auto track_position_y = compute_position_y_l( cluster_position.Z() );
                    data.value_all_x.Fill( cluster_position.X() - track_position_x );
                    data.value_all_y.Fill( cluster_position.Y() - track_position_y );
                    data.value_vtx_x.Fill( cluster_position.X() - track_position_x );
                    data.value_vtx_y.Fill( cluster_position.Y() - track_position_y );
                    break;
                }
                case flag_set<details::it_tag>{}: {
                    cluster_position = transform_l( cluster, &TAGgeoTrafo::FromITLocalToGlobal );
                    auto track_position_x = compute_position_x_l( cluster_position.Z());
                    auto track_position_y = compute_position_y_l( cluster_position.Z());
                    data.value_all_x.Fill( cluster_position.X() - track_position_x );
                    data.value_all_y.Fill( cluster_position.Y() - track_position_y );
                    data.value_it_x.Fill( cluster_position.X() - track_position_x );
                    data.value_it_y.Fill( cluster_position.Y() - track_position_y );
                    break;
                }
                case flag_set<details::msd_tag, details::y_view_tag>{}: {
                    cluster_position = transform_l( cluster, &TAGgeoTrafo::FromMSDLocalToGlobal );
                    auto track_position_y = compute_position_y_l( cluster_position.Z());
                    data.value_all_y.Fill( cluster_position.Y() - track_position_y );
                    data.value_msd_y.Fill( cluster_position.Y() - track_position_y );
                    break;
                }
                case flag_set<details::msd_tag, details::x_view_tag>{}: {
                    auto track_position_x = compute_position_x_l( cluster_position.Z());
                    data.value_all_x.Fill( cluster_position.X() - track_position_x );
                    data.value_msd_x.Fill( cluster_position.X() - track_position_x );
                    break;
                }
                case flag_set<details::tof_tag>{}: {
                    cluster_position = transform_l( cluster, &TAGgeoTrafo::FromTWLocalToGlobal);
                    auto track_position_x = compute_position_x_l( cluster_position.Z());
                    auto track_position_y = compute_position_y_l( cluster_position.Z());
                    data.value_all_x.Fill( cluster_position.X() - track_position_x );
                    data.value_all_y.Fill( cluster_position.Y() - track_position_y );
                    data.value_tof_x.Fill( cluster_position.X() - track_position_x );
                    data.value_tof_y.Fill( cluster_position.Y() - track_position_y );
                    break;
                }
            }
        }
    }
};

template<class F, class CO> struct residuals_data{};
template< class B>
struct residuals_data<histogram< B>, no_requirement> {
    TH1D value_all_x{"residuals_mixed_all_x", ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_vtx_x{"residuals_mixed_vtx_x", ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_it_x{"residuals_mixed_it_x", ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_msd_x{"residuals_mixed_msd_x", ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_tof_x{"residuals_mixed_tof_x", ";cluster_position - track_position;Count", 500, -20, 20 };
    
    TH1D value_all_y{"residuals_mixed_all_y", ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_vtx_y{"residuals_mixed_vtx_y", ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_it_y{"residuals_mixed_it_y", ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_msd_y{"residuals_mixed_msd_y", ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_tof_y{"residuals_mixed_tof_y", ";cluster_position - track_position;Count", 500, -20, 20 };
    void clear(){}
};
template< class B, int C>
struct residuals_data<histogram< B>, isolate_charge<C>> {
    TH1D value_all_x{Form("residuals_charge%d_all_x", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_vtx_x{Form("residuals_charge%d_vtx_x", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_it_x{Form("residuals_charge%d_it_x", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_msd_x{Form("residuals_charge%d_msd_x", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_tof_x{Form("residuals_charge%d_tof_x", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    
    TH1D value_all_y{Form("residuals_charge%d_all_y", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_vtx_y{Form("residuals_charge%d_vtx_y", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_it_y{Form("residuals_charge%d_it_y", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_msd_y{Form("residuals_charge%d_msd_y", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    TH1D value_tof_y{Form("residuals_charge%d_tof_y", C), ";cluster_position - track_position;Count", 500, -20, 20 };
    void clear(){}
};


template<class Derived, class F, class CO> struct residuals_output{};
template<class Derived, class B, class CO>
struct residuals_output< Derived, histogram< B>, CO>{
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        data.value_all_x.Write();
        data.value_vtx_x.Write();
        data.value_it_x.Write();
        data.value_msd_x.Write();
        data.value_tof_x.Write();
        
        data.value_all_y.Write();
        data.value_vtx_y.Write();
        data.value_it_y.Write();
        data.value_msd_y.Write();
        data.value_tof_y.Write();
    }
};


//----------------------------------------------
//                  shearing_factor_distribution
template<class Derived, class F> struct shearing_factor_distribution_outcome{};
template<class Derived, class B>
struct shearing_factor_distribution_outcome<Derived, histogram< B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructed.shearing_factor );
    }
};

template<class F, class CO> struct shearing_factor_distribution_data{};
template< class B>
struct shearing_factor_distribution_data<histogram<B>, no_requirement> {
    TH1D value{"shearing_factor", ";;count", 500, 0, 5};
    void clear(){}
};

template< class B, int C>
struct shearing_factor_distribution_data<histogram< B>, isolate_charge<C>> {
    TH1D value{Form("shearing_factor_charge%d", C), ";;count", 500, 0, 5 };
    void clear(){}
};

//----------------------------------------------
//                  cluster_size_distribution
template<class Derived, class F> struct cluster_size_distribution_outcome{};
template<class Derived, class B>
struct cluster_size_distribution_outcome<Derived, histogram< B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructed.get_clusters().size() );
    }
};

template<class F, class CO> struct cluster_size_distribution_data{};
template< class B>
struct cluster_size_distribution_data<histogram< B>, no_requirement> {
    TH1D value{"cluster_size", ";;count", 16, 1, 17};
    void clear(){}
};

template<  class B, int C>
struct cluster_size_distribution_data<histogram<B>, isolate_charge<C>> {
    TH1D value{Form("cluster_size_charge%d", C), ";;count", 16, 1, 17 };
    void clear(){}
};


//----------------------------------------------
//                 splitted_mc_distribution
template<class Derived, class F> struct splitted_mc_distribution_outcome{};
template<class Derived, class B>
struct splitted_mc_distribution_outcome<Derived, histogram< B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructible.get_indices().size() );
    }
};

template<class F, class CO> struct splitted_mc_distribution_data{};
template< class B>
struct splitted_mc_distribution_data<histogram< B>, no_requirement> {
    TH1D value{"splitted_mc", ";;count", 5, 1, 6};
    void clear(){}
};

template< class B, int C>
struct splitted_mc_distribution_data<histogram< B>, isolate_charge<C>> {
    TH1D value{Form("splitted_mc_charge%d", C), ";;count", 5, 1, 6 };
    void clear(){}
};


//----------------------------------------------
//                  angle_distribution
template<class Derived, class F> struct angle_distribution_outcome{};
template<class Derived, class B>
struct angle_distribution_outcome<Derived, histogram< B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.theta.Fill( reconstructible.parameters.theta );
        data.phi.Fill( reconstructible.parameters.phi );
    }
};

template<class F, class CO> struct angle_distribution_data{};
template<class B>
struct angle_distribution_data<histogram<B>, no_requirement> {
    TH1D theta{"theta", ";#theta (rad);count", 200, 0, TMath::Pi()/2};
    TH1D phi{"phi", ";#phi (rad);count", 200, -TMath::Pi(), TMath::Pi()};
    void clear(){}
};

template<class B, int C>
struct angle_distribution_data<histogram<B>, isolate_charge<C>> {
    TH1D theta{Form("theta_charge%d", C),  ";#theta (rad);count", 200, 0, TMath::Pi()/2};
    TH1D phi{Form("phi_charge%d", C), ";#theta (phi);count", 200, -TMath::Pi(), TMath::Pi()};
    void clear(){}
};

template<class Derived, class F, class CO> struct angle_distribution_output{};
template<class Derived, class B, class CO>
struct angle_distribution_output< Derived, histogram< B>, CO> {
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        data.theta.Write();
        data.phi.Write();
    }
};


//----------------------------------------------
//                  arc_length_ratio
template<class Derived, class F> struct arc_length_ratio_outcome{};
template<class Derived, class B>
struct arc_length_ratio_outcome<Derived, histogram< B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( (reconstructible.parameters.length - reconstructed.length) / reconstructible.parameters.length );
        data.value_rec.Fill( reconstructed.length );
        data.value_mc.Fill(reconstructible.parameters.length);
    }
};

template<class F, class CO> struct arc_length_ratio_data{};
template< class B>
struct arc_length_ratio_data<histogram< B>, no_requirement> {
    TH1D value{"arc_length_ratio", ";;count", 100, -1, 1};
    TH1D value_mc{"arc_length_mc", ";;count", 300, 0, 150};
    TH1D value_rec{"arc_length_rec", ";;count", 300, 0, 150};
    void clear(){}
};

template<class B, int C>
struct arc_length_ratio_data<histogram<B>, isolate_charge<C>> {
    TH1D value{Form("arc_length_ratio%d", C), ";;count", 100, -1, 1 };
    TH1D value_mc{Form("arc_length_mc%d", C), ";;count", 300, 0, 150};
    TH1D value_rec{Form("arc_length_rec%d", C), ";;count", 300, 0, 150};
    void clear(){}
};


template<class Derived, class F, class CO> struct arc_length_ratio_output{};
template<class Derived, class B, class CO>
struct arc_length_ratio_output< Derived, histogram<B>, CO> {
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        data.value.Write();
        data.value_rec.Write();
        data.value_mc.Write();
    }
};

//----------------------------------------------
//                  tof_ratio
template<class Derived, class F> struct tof_ratio_outcome{};
template<class Derived, class B>
struct tof_ratio_outcome<Derived, histogram<B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( (reconstructible.parameters.tof*1e9 - reconstructed.tof) / (reconstructible.parameters.tof*1e9) );
        data.value_mc.Fill( reconstructible.parameters.tof*1e9);
        data.value_rec.Fill(reconstructed.tof);
//        data.value.Fill( reconstructible.parameters.tof);
    }
};

template<class F, class CO> struct tof_ratio_data{};
template<class B>
struct tof_ratio_data<histogram<B>, no_requirement> {
    TH1D value{"tof_ratio", ";;count", 100, -1, 1};
    TH1D value_mc{"tof_mc", ";;count", 100, 0, 20};
    TH1D value_rec{"tof_rec", ";;count", 100, 0, 20};
    void clear(){}
};

template<class B, int C>
struct tof_ratio_data<histogram<B>, isolate_charge<C>> {
    TH1D value{Form("tof_ratio%d", C), ";;count", 100, -1, 1 };
    TH1D value_mc{Form("tof_mc%d", C), ";;count", 100, 0, 20};
    TH1D value_rec{Form("tof_rec%d", C), ";;count", 100, 0, 20};
    void clear(){}
};

template<class Derived, class F, class CO> struct tof_ratio_output{};
template<class Derived, class B, class CO>
struct tof_ratio_output< Derived, histogram<B>, CO> {
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        data.value.Write();
        data.value_rec.Write();
        data.value_mc.Write();
    }
};



//----------------------------------------------
//                  beta_ratio
template<class Derived, class F> struct beta_ratio_outcome{};
template<class Derived, class B>
struct beta_ratio_outcome<Derived, histogram<B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( (reconstructible.parameters.beta- reconstructed.beta) / (reconstructible.parameters.beta) );
        data.value_mc.Fill( reconstructible.parameters.beta);
        data.value_rec.Fill(reconstructed.beta);
    }
};

template<class Derived>
struct beta_ratio_outcome<Derived, computation>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value += pow( (reconstructible.parameters.beta-reconstructed.beta) / (reconstructible.parameters.beta) , 2);
        ++data.counter;
    }
};

template<class F, class CO> struct beta_ratio_data{};
template<class B>
struct beta_ratio_data<histogram<B>, no_requirement> {
    TH1D value{"beta_ratio", ";;count", 100, -1, 1};
    TH1D value_mc{"beta_mc", ";;count", 100, 0, 1};
    TH1D value_rec{"beta_rec", ";;count", 100, 0, 1};
    void clear(){}
};

template<class B, int C>
struct beta_ratio_data<histogram<B>, isolate_charge<C>> {
    TH1D value{Form("beta_ratio%d", C), ";;count", 100, -1, 1 };
    TH1D value_mc{Form("beta_mc%d", C), ";;count", 100, 0, 1};
    TH1D value_rec{Form("beta_rec%d", C), ";;count", 100, 0, 1};
    void clear(){}
};

template<class CO>
struct beta_ratio_data<computation, CO> {
    double value{0};
    std::size_t counter{0};
    void clear(){ value=0; counter = 0; }
};

template<class Derived, class F, class CO> struct beta_ratio_output{};
template<class Derived, class B, class CO>
struct beta_ratio_output< Derived, histogram<B>, CO> {
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        data.value.Write();
        data.value_rec.Write();
        data.value_mc.Write();
    }
};

template<class Derived, class CO>
struct beta_ratio_output< Derived, computation, CO> {
    value_and_error output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        value_and_error result{};
        result.value = data.counter > 1 ? sqrt(data.value/(data.counter-1)) : data.value ;
        result.error = data.counter > 1 ? 1./2 * sqrt(data.counter-1)* result.value/(data.counter-1) : 1./2 * result.value;
        return result;
    }
};

//----------------------------------------------
//                determination_coefficient_distribution
template<class Derived, class F> struct determination_coefficient_distribution_outcome{};
template<class Derived, class B>
struct determination_coefficient_distribution_outcome<Derived, histogram<B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value_x.Fill( reconstructed.parameters.determination_coefficient_x );
        data.value_y.Fill( reconstructed.parameters.determination_coefficient_y );
        data.value_scan.Fill( reconstructed.determination_coefficient_scan );
    }
};

template<class F, class CO> struct determination_coefficient_distribution_data{};
template<class B>
struct determination_coefficient_distribution_data<histogram<B>, no_requirement> {
    TH1D value_x{"determination_coefficient_distribution_x", ";;count", 200, 0, 1};
    TH1D value_y{"determination_coefficient_distribution_y", ";;count", 200, 0, 1};
    TH1D value_scan{"determination_coefficient_distribution_scan", ";;count", 200, 0, 1};
    void clear(){}
};

template<class B, int C>
struct determination_coefficient_distribution_data<histogram<B>, isolate_charge<C>> {
    TH1D value_x{Form("determination_coefficient_distribution%d_x", C), ";;count", 200, 0, 1 };
    TH1D value_y{Form("determination_coefficient_distribution%d_y", C), ";;count", 200, 0, 1 };
    TH1D value_scan{Form("determination_coefficient_distribution%d_scan", C), ";;count", 200, 0, 1 };
    
    void clear(){}
};



template<class Derived, class F, class CO> struct determination_coefficient_distribution_output{};
template<class Derived, class B, class CO>
struct determination_coefficient_distribution_output< Derived, histogram<B>, CO> {
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        data.value_x.Write();
        data.value_y.Write();
        data.value_scan.Write();
    }
};




//----------------------------------------------
//                 producer

//template<template<class, class > class T, class F, class CO = no_requirement, class MO = no_requirement>
template< class F, class CO, class MO,
          template<class, class> class Data,
          template<class, class> class Predicate,
          template<class, class> class Outcome,
          template<class, class, class> class Output >
struct producer :
    Predicate<CO, MO>,
    Outcome< producer<F, CO, MO, Data, Predicate, Outcome, Output>, F >,
    Output< producer< F, CO, MO, Data, Predicate, Outcome, Output>, F, CO> {
    
    using is_computation_type = typename std::is_same<F, computation>::type;
        
    using Predicate<CO,MO>::predicate;
    using Outcome< producer<F, CO, MO, Data, Predicate, Outcome, Output>, F >::outcome;
    using Output< producer< F, CO, MO, Data, Predicate, Outcome, Output>, F, CO>::output;
        
    auto generate_aftereffect(){
        auto predicate_l = [this](reconstruction_module const& rm_p){ return predicate( rm_p ); };
        auto outcome_l = [this](reconstruction_module const& rm_p){ outcome( rm_p ); };
        return aftereffect::make_aftereffect(std::move(predicate_l), std::move(outcome_l));
    }

    auto& data() { return data_m; }
    auto const& data() const { return data_m; }
        
    void clear(){ data_m.clear(); }
private:
    Data<F, CO> data_m;
};

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using purity = producer< Format, ChargeOption, MatchOption,
                         purity_data,
                         successfull_reconstruction_predicate,
                         purity_outcome,
                         purity_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using efficiency = producer< Format, ChargeOption, MatchOption,
                         efficiency_data,
                         is_reconstructible_predicate,
                         efficiency_outcome,
                         efficiency_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using fake_distribution = producer< Format, ChargeOption, MatchOption,
                                    fake_distribution_data,
                                    fake_distribution_predicate,
                                    single_value_outcome,
                                    single_value_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using missed_distribution = producer< Format, ChargeOption, MatchOption,
                                        missed_distribution_data,
                                        missed_distribution_predicate,
                                        single_value_outcome,
                                        single_value_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using reconstructed_distribution = producer< Format, ChargeOption, MatchOption,
                                             reconstructed_distribution_data,
                                             successfull_reconstruction_predicate,
                                             single_value_outcome,
                                             single_value_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using reconstructible_distribution = producer< Format, ChargeOption, MatchOption,
                                             reconstructible_distribution_data,
                                             reconstructible_distribution_predicate,
                                             single_value_outcome,
                                             single_value_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using clone_distribution = producer< Format, ChargeOption, MatchOption,
                                    clone_distribution_data,
                                    successfull_reconstruction_predicate,
                                    clone_distribution_outcome,
                                    single_value_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using mass_distribution = producer< Format, ChargeOption, MatchOption,
                                             mass_distribution_data,
                                             successfull_reconstruction_predicate,
                                             mass_distribution_outcome,
                                             single_value_output >;


template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using mass_identification = producer< Format, ChargeOption, MatchOption,
                                      mass_identification_data,
                                      successfull_reconstruction_predicate,
                                      mass_identification_outcome,
                                      mass_identification_output >;


template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using momentum_resolution = producer< Format, ChargeOption, MatchOption,
                                    momentum_resolution_data,
                                    successfull_reconstruction_predicate,
                                    momentum_resolution_outcome,
                                    momentum_resolution_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using residuals = producer< Format, ChargeOption, MatchOption,
                            residuals_data,
                            successfull_reconstruction_predicate,
                            residuals_outcome,
                            residuals_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using shearing_factor_distribution = producer< Format, ChargeOption, MatchOption,
                                    shearing_factor_distribution_data,
                                    successfull_reconstruction_predicate,
                                    shearing_factor_distribution_outcome,
                                    single_value_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using cluster_size_distribution = producer< Format, ChargeOption, MatchOption,
                                    cluster_size_distribution_data,
                                    successfull_reconstruction_predicate,
                                    cluster_size_distribution_outcome,
                                    single_value_output >;
template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using splitted_mc_distribution = producer< Format, ChargeOption, MatchOption,
                                    splitted_mc_distribution_data,
                                    successfull_reconstruction_predicate,
                                    splitted_mc_distribution_outcome,
                                    single_value_output >;
template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using angle_distribution = producer< Format, ChargeOption, MatchOption,
                                    angle_distribution_data,
                                    successfull_reconstruction_predicate,
                                    angle_distribution_outcome,
                                    angle_distribution_output >;
template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using arc_length_ratio = producer< Format, ChargeOption, MatchOption,
                                    arc_length_ratio_data,
                                    successfull_reconstruction_predicate,
                                    arc_length_ratio_outcome,
                                    arc_length_ratio_output >;
template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using tof_ratio = producer< Format, ChargeOption, MatchOption,
                                    tof_ratio_data,
                                    successfull_reconstruction_predicate,
                                    tof_ratio_outcome,
                                    tof_ratio_output >;
template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using beta_ratio = producer< Format, ChargeOption, MatchOption,
                                    beta_ratio_data,
                                    successfull_reconstruction_predicate,
                                    beta_ratio_outcome,
                                    beta_ratio_output >;
template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using determination_coefficient_distribution = producer< Format, ChargeOption, MatchOption,
                                    determination_coefficient_distribution_data,
                                    successfull_reconstruction_predicate,
                                    determination_coefficient_distribution_outcome,
                                    determination_coefficient_distribution_output >;





namespace details{
template<bool ... Bs> struct bool_pack{};

template< class ... Ts>
struct all_of : std::is_same< bool_pack<true, bool(Ts::value)...>, bool_pack<bool(Ts::value)..., true> >::type {};
}//namespace details

} // namespace checker

template<class ... Producers>
struct TATOEchecker{
    void apply(std::vector<reconstruction_module> const& rm_pc){
        auto aftereffect_c = generate_aftereffects( std::make_index_sequence<sizeof...(Producers)>{} );
        for( auto const& module : rm_pc){
            aftereffect_c.evaluate( module );
        }
    }
    
    auto output() const { return output_impl( std::make_index_sequence<sizeof...(Producers)>{}); }
    
    //SFINAE based on producer type ? but then need to check that all are compatible
    void clear() { clear_impl( std::make_index_sequence<sizeof...(Producers)>{} ); }
    
private:
    template<std::size_t ... Indices>
    auto generate_aftereffects( std::index_sequence<Indices...>  ){
        return aftereffect::make_aftereffect_list( std::get<Indices>(producer_mc).generate_aftereffect()... );
    }
    
    template< std::size_t ... Indices, bool dummy = (sizeof...(Indices) == sizeof...(Producers)),
              typename std::enable_if_t< dummy && !checker::details::all_of< typename Producers::is_computation_type... >::value, std::nullptr_t> = nullptr>
    void output_impl( std::index_sequence<Indices...> ) const {
        int expander[] = {0, (std::get<Indices>(producer_mc).output(), 0)...};
    }
    
    template< std::size_t ... Indices, bool dummy = (sizeof...(Indices) == sizeof...(Producers)) ,
              typename std::enable_if_t< dummy && checker::details::all_of< typename Producers::is_computation_type... >::value, std::nullptr_t> = nullptr>
    std::vector<checker::value_and_error> output_impl( std::index_sequence<Indices...> ) const {
        std::vector<checker::value_and_error> result_c;
        int expander[] = {0, (result_c.push_back( std::get<Indices>(producer_mc).output() ), 0)...};
        return result_c;
    }
    
    template<std::size_t ... Indices>
    void clear_impl( std::index_sequence<Indices...> ) {
        int expander[] = {0, (std::get<Indices>(producer_mc).clear(), 0)...};
    }
    
private:
    std::tuple<Producers...> producer_mc;
};

template<class Producer>
struct TATOEchecker<Producer>{
    void apply(std::vector<reconstruction_module> const& rm_pc){
        auto aftereffect = producer_m.generate_aftereffect();
        for( auto const& module : rm_pc){ aftereffect( module ); }
    }
    
    auto output() const { return producer_m.output(); }
    
    //SFINAE based on producer type
    void clear() { producer_m.clear(); }
    
private:
    Producer producer_m;
};


struct computation_checker{
    struct eraser{
        virtual ~eraser() = default;
        virtual void apply( std::vector<reconstruction_module> const& ) = 0;
        virtual std::vector<checker::value_and_error> output() const =0;
        virtual void clear() = 0;
    };
    
    template<class T>
    struct holder : eraser{
        holder(T t_p) : t_m{ std::move(t_p) }{}
        
        void apply( std::vector<reconstruction_module> const& rm_pc ) override { t_m.apply( rm_pc ); }
        //SFINAE ON OuTPUT type : either vector or only one, but ease up listing have to be vector here because inheritance, cannot use auto
        std::vector<checker::value_and_error> output() const override{ return {t_m.output()}; }
        void clear() override { t_m.clear(); }
    private:
        T t_m;
    };
    
    template<class T>
    computation_checker(T t_p) : erased_checker_mh{ new holder<T>{ std::move(t_p) } }{}
    void apply( std::vector<reconstruction_module> const& rm_pc ){ erased_checker_mh->apply(rm_pc);}
    std::vector<checker::value_and_error> output() const { return erased_checker_mh->output(); }
    void clear() { erased_checker_mh->clear(); }
    
private:
    std::unique_ptr<eraser> erased_checker_mh;
};



struct histogram_checker{
    struct eraser{
        virtual ~eraser() = default;
        virtual void apply( std::vector<reconstruction_module> const& ) = 0;
        virtual void output() const =0;
    };
    
    template<class T>
    struct holder : eraser{
        holder(T t_p) : t_m{ std::move(t_p) }{}
        
        void apply( std::vector<reconstruction_module> const& rm_pc ) override { t_m.apply( rm_pc ); }
        void output() const override{ t_m.output(); }
    private:
        T t_m;
    };
    
    template<class T>
    histogram_checker(T t_p) : erased_checker_mh{ new holder<T>{ std::move(t_p) } }{}
    void apply( std::vector<reconstruction_module> const& rm_pc ){ erased_checker_mh->apply(rm_pc);}
    void output() const { erased_checker_mh->output(); }
    
private:
    std::unique_ptr<eraser> erased_checker_mh;
};


#endif /* TATOEchecker_h */
