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

//----------------------------------------------
//                 Format

struct per_nucleon{};
struct absolute{};
struct reconstructible_based{};
struct reconstructed_based{};
template<class R = per_nucleon, class B = reconstructible_based>
struct histogram{};

struct computation{};



///
template<class T, class R, class CO>
struct generate_histogram{};


template<class CO, class MO>
struct successfull_reconstruction_predicate {
    bool predicate( reconstruction_module const& rm_p ) const {
        return rm_p.reconstructible_o.has_value() && rm_p.reconstructed_o.has_value() &&
                CO{}.predicate(rm_p) &&
                MO{}.predicate(rm_p);
    }
};


template<class Derived, class F> struct single_value_outcome{};
template<class Derived>
struct single_value_outcome<Derived, histogram<per_nucleon, reconstructible_based>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructible.properties.momentum/(1000*reconstructible.properties.nucleon_number) );
    }
};
template<class Derived>
struct single_value_outcome<Derived, histogram<per_nucleon, reconstructed_based>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructed.properties.momentum/(1000*reconstructed.properties.nucleon_number) );
    }
};
template<class Derived>
struct single_value_outcome<Derived, histogram<absolute, reconstructible_based>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructible.properties.momentum/1000 );
    }
};
template<class Derived>
struct single_value_outcome<Derived, histogram<absolute, reconstructed_based>>{
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

template<class Derived, class T, class B, class CO>
struct single_value_output< Derived, histogram<T, B>, CO>{
    void output() const { static_cast<Derived const&>(*this).data().value.Write(); }
};


//=====================================================
//                 type

//----------------------------------------------
//                  purity
struct purity_tag{};

template<class Derived, class F> struct purity_outcome{};
template<class Derived>
struct purity_outcome<Derived, histogram<per_nucleon, reconstructible_based>>{
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

template<class Derived, class T, class B, class CO>
struct purity_output< Derived, histogram<T, B>, CO>{
    void output() const {
        TH1D result{ generate_histogram< purity_tag, T, CO >{}() };
        
        auto const& data = static_cast<Derived const&>(*this).data();
        
        for(auto i{1}; i < result.GetNbinsX()+1 ; ++i){
            auto recovered = data.recovered_cluster_histogram.GetBinContent(i);
            auto correct = data.correct_cluster_histogram.GetBinContent(i);
            auto purity = recovered > 0 ? correct * 1./recovered : 0;
            auto error = recovered > 0 ? sqrt( purity* (1+ purity)/recovered) : 0;
            result.SetBinContent(i, purity);
            result.SetBinError(i, error );
        }
        result.Write();
    }
};

template<>
struct generate_histogram< purity_tag, per_nucleon, no_requirement >{
    TH1D operator()(){ return TH1D{"purity_mixed", ";p_{mc} (GeV/u); Count", 100, 0, 1.3 };  }
};

template< int C >
struct generate_histogram< purity_tag, per_nucleon, isolate_charge<C> >{
    TH1D operator()(){ return TH1D{ Form("purity_charge%d", C), ";p_{mc} (GeV/u); Count", 100, 0, 1.3 };  }
};


template<class F, class CO> struct purity_data{};
template<class B, class CO>
struct purity_data<histogram<per_nucleon, B>, CO>{
    TH1D recovered_cluster_histogram{"", "", 100, 0, 1.3 };
    TH1D correct_cluster_histogram{"", "", 100, 0, 1.3};
    
    void clear(){}
};
template<class CO>
struct purity_data< computation, CO >{
    std::size_t recovered_cluster_count{0};
    std::size_t correct_cluster_count{0};

    void clear() { recovered_cluster_count = correct_cluster_count = 0 ;  }
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
struct missed_distribution_data<histogram<absolute, reconstructible_based>, no_requirement>{
    TH1D value{ "missed_distribution_mixed_pmc", ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};

template<int C>
struct missed_distribution_data<histogram<absolute, reconstructible_based>, isolate_charge<C>>{
    TH1D value{ Form("missed_distribution_charge%d_pmc", C), ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};

//----------------------------------------------
//                  reconstructed_distribution

template<class F, class CO> struct reconstructed_distribution_data{};
template<>
struct reconstructed_distribution_data<histogram<per_nucleon, reconstructible_based>, no_requirement>{
    TH1D value{ "reconstructed_distribution_mixed_pmc/u", ";p_{mc} (GeV/u); Count", 50, 0, 1.3 };
    void clear(){}
};
template<>
struct reconstructed_distribution_data<histogram<absolute, reconstructible_based>, no_requirement>{
    TH1D value{ "reconstructed_distribution_mixed_pmc", ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};
template<int C>
struct reconstructed_distribution_data<histogram<per_nucleon, reconstructible_based>, isolate_charge<C>>{
    TH1D value{ Form("reconstructed_distribution_charge%d_pmc/u", C), ";p_{mc} (GeV/u); Count", 50, 0, 1.3 };
    void clear(){}
};
template<int C>
struct reconstructed_distribution_data<histogram<absolute, reconstructible_based>, isolate_charge<C>>{
    TH1D value{ Form("reconstructed_distribution_charge%d_pmc", C), ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};

template<>
struct reconstructed_distribution_data<histogram<per_nucleon, reconstructed_based>, no_requirement>{
    TH1D value{ "reconstructed_distribution_mixed_prec/u", ";p_{rec} (GeV/u); Count", 50, 0, 1.3 };
    void clear(){}
};
template<>
struct reconstructed_distribution_data<histogram<absolute, reconstructed_based>, no_requirement>{
    TH1D value{ "reconstructed_distribution_mixed_prec", ";p_{rec} (GeV); Count", 500, 0, 13 };
    void clear(){}
};
template<int C>
struct reconstructed_distribution_data<histogram<per_nucleon, reconstructed_based>, isolate_charge<C>>{
    TH1D value{ Form("reconstructed_distribution_charge%d_prec/u", C), ";p_{rec} (GeV/u); Count", 50, 0, 1.3 };
    void clear(){}
};
template<int C>
struct reconstructed_distribution_data<histogram<absolute, reconstructed_based>, isolate_charge<C>>{
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
struct reconstructible_distribution_data<histogram<per_nucleon, reconstructible_based>, no_requirement>{
    TH1D value{ "reconstructible_distribution_mixed_pmc/u", ";p_{mc} (GeV/u); Count", 50, 0, 1.3 };
    void clear(){}
};
template<>
struct reconstructible_distribution_data<histogram<absolute, reconstructible_based>, no_requirement>{
    TH1D value{ "reconstructible_distribution_mixed_pmc", ";p_{mc} (GeV); Count", 500, 0, 13 };
    void clear(){}
};
template<int C>
struct reconstructible_distribution_data<histogram<per_nucleon, reconstructible_based>, isolate_charge<C>>{
    TH1D value{ Form("reconstructible_distribution_charge%d_pmc/u", C), ";p_{mc} (GeV/u); Count", 50, 0, 1.3 };
    void clear(){}
};
template<int C>
struct reconstructible_distribution_data<histogram<absolute, reconstructible_based>, isolate_charge<C>>{
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


template<class F, class CO> struct clone_distribution_data{};
template<class CO> struct clone_distribution_data<computation, CO>{
    std::size_t counter{0};
    void clear(){ counter = 0 ; }
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

template<class Derived, class R, class B>
struct mass_identification_outcome<Derived, histogram<R, B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructible.properties.nucleon_number, reconstructed.properties.nucleon_number );
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

template<class F, class CO> struct mass_identification_data{};

template<class CO>
struct mass_identification_data<computation, CO>{
    std::size_t good_identification_counter{0};
    std::size_t total_counter{0};

    void clear() { good_identification_counter = total_counter = 0 ;  }
};

template<class R, class B>
struct mass_identification_data<histogram<R, B>, no_requirement> {
    TH2D value{"mass_identification_mixed", ";A_{mc};A_{rec};Count", 20, 0, 20, 20, 0, 20  };
    void clear(){}
};

template<class R, class B, int C>
struct mass_identification_data<histogram<R, B>, isolate_charge<C>> {
    TH2D value{Form("mass_identification_charge%d", C), ";A_{mc};A_{rec};Count", 20, 0, 20, 20, 0, 20  };
    void clear(){}
};

//----------------------------------------------
//           mass_idistribution

template<class Derived, class F> struct mass_distribution_outcome{};
template<class Derived>
struct mass_distribution_outcome<Derived, histogram<absolute, reconstructible_based>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructible.properties.mass );
    }
};
template<class Derived>
struct mass_distribution_outcome<Derived, histogram<absolute, reconstructed_based>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructed.properties.mass/1000 );
    }
};


template<class F, class CO> struct mass_distribution_data{};
template<>
struct mass_distribution_data<histogram<absolute, reconstructible_based>, no_requirement>{
    TH1D value{ "mass_distribution_mixed_mc", ";mass (GeV/c^2); Count", 500, 0, 20 };
    void clear(){}
};
template<int C>
struct mass_distribution_data<histogram<absolute, reconstructible_based>, isolate_charge<C>>{
    TH1D value{ Form("mass_distribution_charge%d_mc", C), ";mass (GeV/c^2); Count", 500, 0, 20 };
    void clear(){}
};

template<>
struct mass_distribution_data<histogram<absolute, reconstructed_based>, no_requirement>{
    TH1D value{ "mass_distribution_mixed_rec", ";p_{rec} (GeV); Count", 500, 0, 20 };
    void clear(){}
};
template<int C>
struct mass_distribution_data<histogram<absolute, reconstructed_based>, isolate_charge<C>>{
    TH1D value{ Form("mass_distribution_charge%d_rec", C), ";p_{rec} (GeV); Count", 500, 0, 20 };
    void clear(){}
};


//----------------------------------------------
//           momentum_resolution
template<class Derived, class F> struct momentum_difference_outcome{};
template<class Derived, class B>
struct momentum_difference_outcome<Derived, histogram<absolute, B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        auto momentum_difference = (reconstructible.properties.momentum - reconstructed.properties.momentum)/1000;
        data.value.fill_with(reconstructible.properties.momentum/1000, momentum_difference);
    }
};


template<class F, class CO> struct momentum_difference_data{};
template<class B>
struct momentum_difference_data<histogram<absolute, B>, no_requirement> {
    struct link {
        std::unique_ptr<TTree> tree_h;
        double momentum;
    };
    static constexpr double bin_size = 0.2;
    struct holder{
        std::vector<link> link_c;
        holder() { link_c.reserve(100); }
        void fill_with( double momentum_p, double momentum_difference_p ){
            auto link_i = std::find_if(
                                link_c.begin(), link_c.end(),
                                [&momentum_p]( auto const& link_p ){
                                            return link_p.momentum-bin_size/2 <= momentum_p && momentum_p < link_p.momentum+bin_size/2;
                                                                    }
                                        );
            if( link_i != link_c.cend() ){
                link_i->tree_h->SetBranchAddress("value", &momentum_difference_p);
                link_i->tree_h->Fill();
            }
            else{
                auto momentum = (static_cast<int>( momentum_p/bin_size ) * bin_size + bin_size/2);
                link_c.push_back( link{std::make_unique<TTree>(Form("momentum_difference_%.2fGev/c_mixed", momentum), ""),
                                        momentum }  );
                link_c.back().tree_h->Branch("value", &momentum_difference_p);
                link_i->tree_h->Fill();
            }
        };
    };
    
    holder value;
    void clear(){}
};
template<class B, int C>
struct momentum_difference_data<histogram<absolute, B>, isolate_charge<C>> {
    struct link {
        std::unique_ptr<TTree> tree_h;
        double momentum;
    };
    static constexpr double bin_size = 0.2;
    struct holder{
        std::vector<link> link_c;
        holder() { link_c.reserve(100); }
        void fill_with( double momentum_p, double momentum_difference_p ){
            auto link_i = std::find_if(
                                link_c.begin(), link_c.end(),
                                [&momentum_p]( auto const& link_p ){
                                            return link_p.momentum-bin_size/2 <= momentum_p && momentum_p < link_p.momentum+bin_size/2;
                                                                    }
                                        );
            if( link_i != link_c.cend() ){
                link_i->tree_h->SetBranchAddress("value", &momentum_difference_p);
                link_i->tree_h->Fill();
            }
            else{
                auto momentum = (static_cast<int>( momentum_p/bin_size ) * bin_size + bin_size/2);
                link_c.push_back( link{ std::make_unique<TTree>(Form("momentum_difference_%.2fGev/c_charge%d", momentum, C), ""),
                                        momentum   } );
                link_c.back().tree_h->Branch("value", &momentum_difference_p);
                link_i->tree_h->Fill();
            }
        };
    };
    
    holder value;
    void clear(){}
};

template<class Derived, class F, class CO> struct momentum_difference_output{};
template<class Derived, class T, class B, class CO>
struct momentum_difference_output< Derived, histogram<T, B>, CO>{
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        for( auto& link: data.value.link_c){
            link.tree_h->Write();
        }
    }
};

//----------------------------------------------
//                  residuals

//way to separate X/Y ? just change output from single_value to custom
template<class Derived, class F> struct residuals_outcome{};
template<class Derived, class R, class B>
struct residuals_outcome<Derived, histogram<R, B>>{
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
template< class R, class B>
struct residuals_data<histogram<R, B>, no_requirement> {
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
template<class R, class B, int C>
struct residuals_data<histogram<R, B>, isolate_charge<C>> {
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
template<class Derived, class T, class B, class CO>
struct residuals_output< Derived, histogram<T, B>, CO>{
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
//                  chisquared_distribution
template<class Derived, class F> struct cluster_chisquared_distribution_outcome{};
template<class Derived, class R, class B>
struct cluster_chisquared_distribution_outcome<Derived, histogram<R, B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        for( auto const& cluster : reconstructed.get_clusters() ){
            data.value_predicted.Fill( cluster.chi2.prediction );
            data.value_corrected.Fill( cluster.chi2.correction );
            data.value_distance.Fill( cluster.chi2.distance );
        }
    }
};

template<class Derived, class F, class CO> struct cluster_chisquared_distribution_output{};
template<class Derived, class T, class B, class CO>
struct cluster_chisquared_distribution_output< Derived, histogram<T, B>, CO> {
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        data.value_predicted.Write();
        data.value_corrected.Write();
        data.value_distance.Write();
    }
};

template<class F, class CO> struct cluster_chisquared_distribution_data{};
template< class R, class B>
struct cluster_chisquared_distribution_data<histogram<R, B>, no_requirement> {
    TH1D value_predicted{"cluster_chisquared_predicted", ";;Count", 500, 0, 40 };
    TH1D value_corrected{"cluster_chisquared_corrected", ";;Count", 500, 0, 40 };
    TH1D value_distance{"distance", ";;Count", 500, 0, 40 };
//    TH1D value_ratio{"ratio", ";;Count", 500, 0, 40};

    void clear(){}
};

template< class R, class B, int C>
struct cluster_chisquared_distribution_data<histogram<R, B>, isolate_charge<C>> {
    TH1D value_predicted{Form("cluster_chisquared_predicted_charge%d", C), ";;Count", 500, 0, 40 };
    TH1D value_corrected{Form("cluster_chisquared_corrected_charge%d", C), ";;Count", 500, 0, 40 };
    TH1D value_distance{Form("distance_charge%d", C), ";;Count", 500, 0, 40 };
//    TH1D value_ratio{"ratio", ";;Count", 500, 0, 40};
    
    void clear(){}
};

template<class Derived, class F> struct track_chisquared_distribution_outcome{};
template<class Derived, class R, class B>
struct track_chisquared_distribution_outcome<Derived, histogram<R, B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        double corrected{0.}, predicted{0.}, distance{0.};
//        for( auto const& cluster : reconstructed.get_clusters() ){
        for( auto cluster_i = reconstructed.get_clusters().begin() +1 ; cluster_i != reconstructed.get_clusters().end() ; ++cluster_i){
            corrected += cluster_i->chi2.correction;
            predicted += cluster_i->chi2.prediction;
            distance += cluster_i->chi2.distance;
        }
        corrected /= reconstructed.get_clusters().size() - 1;
        predicted /= reconstructed.get_clusters().size() - 1;
        distance /= reconstructed.get_clusters().size() - 1;
        data.value_predicted.Fill( predicted );
        data.value_corrected.Fill( corrected );
        data.value_distance.Fill( distance );
    }
};

template<class Derived, class F, class CO> struct track_chisquared_distribution_output{};
template<class Derived, class T, class B, class CO>
struct track_chisquared_distribution_output< Derived, histogram<T, B>, CO> {
    void output() const {
        auto& data = static_cast<Derived const&>(*this).data();
        data.value_predicted.Write();
        data.value_corrected.Write();
        data.value_distance.Write();
    }
};

template<class F, class CO> struct track_chisquared_distribution_data{};
template< class R, class B>
struct track_chisquared_distribution_data<histogram<R, B>, no_requirement> {
    TH1D value_predicted{"track_chisquared_predicted", ";;Count", 500, 0, 40 };
    TH1D value_corrected{"track_chisquared_corrected", ";;Count", 500, 0, 40 };
    TH1D value_distance{"distance", ";;Count", 500, 0, 40 };
    TH1D value_ratio{"ratio", ";;Count", 500, 0, 40};

    void clear(){}
};

template< class R, class B, int C>
struct track_chisquared_distribution_data<histogram<R, B>, isolate_charge<C>> {
    TH1D value_predicted{Form("track_chisquared_predicted_charge%d", C), ";;Count", 500, 0, 40 };
    TH1D value_corrected{Form("track_chisquared_corrected_charge%d", C), ";;Count", 500, 0, 40 };
    TH1D value_distance{Form("distance_charge%d", C), ";;Count", 500, 0, 40 };
    TH1D value_ratio{"ratio", ";;Count", 500, 0, 40};
    
    void clear(){}
};



//----------------------------------------------
//                  shearing_factor_distribution
template<class Derived, class F> struct shearing_factor_distribution_outcome{};
template<class Derived, class R, class B>
struct shearing_factor_distribution_outcome<Derived, histogram<R, B>>{
    void outcome( reconstruction_module const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructed.shearing_factor );
    }
};

template<class F, class CO> struct shearing_factor_distribution_data{};
template< class R, class B>
struct shearing_factor_distribution_data<histogram<R, B>, no_requirement> {
    TH1D value{"shearing_factor", ";;count", 500, 0, 5};
    void clear(){}
};

template< class R, class B, int C>
struct shearing_factor_distribution_data<histogram<R, B>, isolate_charge<C>> {
    TH1D value{Form("shearing_factor_charge%d", C), ";;count", 500, 0, 5 };
    void clear(){}
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


template<class Format, class ChargeOption, class MatchOption>
struct mass_identification_impl{};

template<class R, class B, class ChargeOption, class MatchOption>
struct mass_identification_impl< histogram<R,B>, ChargeOption, MatchOption > : producer< histogram<R,B>, ChargeOption, MatchOption,
mass_identification_data,
successfull_reconstruction_predicate,
mass_identification_outcome,
single_value_output > {};

template< class ChargeOption, class MatchOption>
struct mass_identification_impl<computation, ChargeOption, MatchOption > : producer< computation, ChargeOption, MatchOption,
mass_identification_data,
successfull_reconstruction_predicate,
mass_identification_outcome,
mass_identification_output > {};

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using mass_identification = mass_identification_impl<Format, ChargeOption, MatchOption>;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using momentum_difference = producer< Format, ChargeOption, MatchOption,
                                    momentum_difference_data,
                                    successfull_reconstruction_predicate,
                                    momentum_difference_outcome,
                                    momentum_difference_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using residuals = producer< Format, ChargeOption, MatchOption,
                            residuals_data,
                            successfull_reconstruction_predicate,
                            residuals_outcome,
                            residuals_output >;

//chisquare_distribution
template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using cluster_chisquared_distribution = producer< Format, ChargeOption, MatchOption,
                                         cluster_chisquared_distribution_data,
                                         successfull_reconstruction_predicate,
                                         cluster_chisquared_distribution_outcome,
                                         cluster_chisquared_distribution_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using track_chisquared_distribution = producer< Format, ChargeOption, MatchOption,
                                         track_chisquared_distribution_data,
                                         successfull_reconstruction_predicate,
                                         track_chisquared_distribution_outcome,
                                         track_chisquared_distribution_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using shearing_factor_distribution = producer< Format, ChargeOption, MatchOption,
                                    shearing_factor_distribution_data,
                                    successfull_reconstruction_predicate,
                                    shearing_factor_distribution_outcome,
                                    single_value_output >;

} // namespace checker

template<class ... Producers>
struct TATOEchecker{
    void apply(std::vector<reconstruction_module> const& rm_pc){
        auto aftereffect_c = generate_aftereffects( std::make_index_sequence<sizeof...(Producers)>{} );
        for( auto const& module : rm_pc){
            aftereffect_c.evaluate( module );
        }
    }
    
    void output() const { output_impl( std::make_index_sequence<sizeof...(Producers)>{}); }
    
    //SFINAE based on producer type ? but then need to check that all are compatible
    void clear() { clear_impl( std::make_index_sequence<sizeof...(Producers)>{} ); }
    
private:
    template<std::size_t ... Indices>
    auto generate_aftereffects( std::index_sequence<Indices...>  ){
        return aftereffect::make_aftereffect_list( std::get<Indices>(producer_mc).generate_aftereffect()... );
    }
    
    template<std::size_t ... Indices>
    void output_impl( std::index_sequence<Indices...> ) const {
        int expander[] = {0, (std::get<Indices>(producer_mc).output(), 0)...};
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
        virtual checker::value_and_error output() const =0;
        virtual void clear() = 0;
    };
    
    template<class T>
    struct holder : eraser{
        holder(T t_p) : t_m{ std::move(t_p) }{}
        
        void apply( std::vector<reconstruction_module> const& rm_pc ) override { t_m.apply( rm_pc ); }
        checker::value_and_error output() const override{ return t_m.output(); }
        void clear() override { t_m.clear(); }
    private:
        T t_m;
    };
    
    template<class T>
    computation_checker(T t_p) : erased_checker_mh{ new holder<T>{ std::move(t_p) } }{}
    void apply( std::vector<reconstruction_module> const& rm_pc ){ erased_checker_mh->apply(rm_pc);}
    checker::value_and_error output() const { return erased_checker_mh->output(); }
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
