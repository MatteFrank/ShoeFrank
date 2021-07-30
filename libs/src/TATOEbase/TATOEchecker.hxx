//
//File      : TATOEchecker.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 28/07/2021
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef TATOEchecker_h
#define TATOEchecker_h

#include "aftereffect.hpp"

#include "TATOEutilities.hxx"

#include "TAGcluster.hxx"

#include "TH2D.h"

namespace checker {

struct value_and_error{
    double value;
    double error;
};

// -------------------------------------------
//                    Options
struct no_requirement{
    bool predicate(reconstruction_module<TAGcluster> const& rm_p){ return true; }
};

template<int C>
struct isolate_charge{
    static constexpr int charge = C;
    bool predicate(reconstruction_module<TAGcluster> const& rm_p){
        return rm_p.reconstructible_o.value().properties.charge == charge;
    }
};

struct well_matched{
    bool predicate(reconstruction_module<TAGcluster> const& rm_p){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto const& reconstructed = rm_p.reconstructed_o.value();
        return reconstructible.properties.charge == reconstructed.properties.charge &&
               reconstructible.properties.mass   == reconstructed.properties.mass;
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
    bool predicate( reconstruction_module<TAGcluster> const& rm_p ) const {
        return rm_p.reconstructible_o.has_value() && rm_p.reconstructed_o.has_value() &&
                CO{}.predicate(rm_p) &&
                MO{}.predicate(rm_p);
    }
};

//=====================================================
//                 type



//----------------------------------------------
//                  residuals
struct residuals{
    
};





//----------------------------------------------
//                  purity
struct purity_tag{};

template<class Derived, class F> struct purity_outcome{};
template<class Derived>
struct purity_outcome<Derived, histogram<per_nucleon, reconstructible_based>>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto & data = static_cast<Derived&>(*this).data();
        
        data.recovered_cluster_histogram.Fill(
                            reconstructible.properties.momentum/(1000*reconstructible.properties.mass),
                            reconstructed.get_clusters().size()
                                          );
        auto index_c = reconstructible.get_indices();
        for( auto const& cluster_h : reconstructed.get_clusters() ) {
            for( auto i{0}; i < cluster_h->GetMcTracksN() ; ++i ) {
                auto cluster_index = cluster_h->GetMcTrackIdx(i);
                if( std::any_of( index_c.begin(), index_c.end(),
                                 [&cluster_index]( int index_p ){ return index_p == cluster_index; } ) ){
                    data.correct_cluster_histogram.Fill( reconstructible.properties.momentum/(1000 * reconstructible.properties.mass) );
                    break;
                }
            }
        }
    }
};
template<class Derived>
struct purity_outcome<Derived, computation>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        
        auto & data = static_cast<Derived&>(*this).data();
        
        data.recovered_cluster_count += reconstructed.get_clusters().size();
        auto index_c = reconstructible.get_indices();
        for( auto const& cluster_h : reconstructed.get_clusters() ) {
            for( auto i{0}; i < cluster_h->GetMcTracksN() ; ++i ) {
                auto cluster_index = cluster_h->GetMcTrackIdx(i);
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
};
template<class CO>
struct purity_data< computation, CO >{
    std::size_t recovered_cluster_count{0};
    std::size_t correct_cluster_count{0};
};


//----------------------------------------------
//                  fake_distribution
template<class Derived, class F> struct fake_distribution_outcome{};
template<class Derived>
struct fake_distribution_outcome<Derived, computation>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        ++data.fake_count;
    }
};

template<class Derived, class F, class CO> struct fake_distribution_output{};
template< class Derived, class CO >
struct fake_distribution_output< Derived, computation, CO  >{
    value_and_error output() const {
        value_and_error result;
        auto const& data = static_cast<Derived const&>(*this).data();
        result.value = data.fake_count;
        result.error = sqrt(data.fake_count);
        return result;
    }
};

template<class F, class CO> struct fake_distribution_data{};
template<class CO> struct fake_distribution_data<computation, CO>{ std::size_t fake_count{0}; };

template<class CO, class MO>
struct fake_distribution_predicate {
    bool predicate( reconstruction_module<TAGcluster> const& rm_p ) const {
        return !rm_p.reconstructible_o.has_value() && rm_p.reconstructed_o.has_value() &&
                CO{}.predicate(rm_p) &&
                MO{}.predicate(rm_p);
    }
};

//----------------------------------------------
//                  reconstructed_distribution

template<class Derived, class F> struct reconstructed_distribution_outcome{};
template<class Derived>
struct reconstructed_distribution_outcome<Derived, histogram<per_nucleon, reconstructible_based>>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.reconstructed_histogram.Fill(
                            reconstructible.properties.momentum/(1000*reconstructible.properties.mass)
                                          );
    }
};
template<class Derived>
struct reconstructed_distribution_outcome<Derived, histogram<per_nucleon, reconstructed_based>>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.reconstructed_histogram.Fill(
                            reconstructed.properties.momentum/(1000*reconstructed.properties.mass)
                                          );
    }
};
template<class Derived>
struct reconstructed_distribution_outcome<Derived, histogram<absolute, reconstructible_based>>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.reconstructed_histogram.Fill( reconstructible.properties.momentum/1000 );
    }
};
template<class Derived>
struct reconstructed_distribution_outcome<Derived, histogram<absolute, reconstructed_based>>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.reconstructed_histogram.Fill( reconstructed.properties.momentum/1000 );
    }
};

template<class Derived>
struct reconstructed_distribution_outcome<Derived, computation>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        ++data.reconstructed_count;
    }
};


template<class Derived, class F, class CO> struct reconstructed_distribution_output{};
template<class Derived, class CO >
struct reconstructed_distribution_output< Derived, computation, CO >{
    value_and_error output() const {
        value_and_error result;
        auto const& data = static_cast<Derived const&>(*this).data();
        result.value = data.reconstructed_count;
        result.error = sqrt(data.reconstructed_count);
        return result;
    }
};

template<class Derived, class T, class B, class CO>
struct reconstructed_distribution_output< Derived, histogram<T, B>, CO>{
    void output() const { static_cast<Derived const&>(*this).data().reconstructed_histogram.Write(); }
};

template<class F, class CO> struct reconstructed_distribution_data{};
template<>
struct reconstructed_distribution_data<histogram<per_nucleon, reconstructible_based>, no_requirement>{
    TH1D reconstructed_histogram{ "reconstructed_distribution_mixed_pmc/u", ";p_{mc} (GeV/u); Count", 100, 0, 1.3 };
};
template<>
struct reconstructed_distribution_data<histogram<absolute, reconstructible_based>, no_requirement>{
    TH1D reconstructed_histogram{ "reconstructed_distribution_mixed_pmc", ";p_{mc} (GeV); Count", 1000, 0, 9 };
};
template<int C>
struct reconstructed_distribution_data<histogram<per_nucleon, reconstructible_based>, isolate_charge<C>>{
    TH1D reconstructed_histogram{ Form("reconstructed_distribution_charge%d_pmc/u", C), ";p_{mc} (GeV/u); Count", 100, 0, 1.3 };
};
template<int C>
struct reconstructed_distribution_data<histogram<absolute, reconstructible_based>, isolate_charge<C>>{
    TH1D reconstructed_histogram{ Form("reconstructed_distribution_charge%d_pmc", C), ";p_{mc} (GeV); Count", 1000, 0, 9 };
};

template<>
struct reconstructed_distribution_data<histogram<per_nucleon, reconstructed_based>, no_requirement>{
    TH1D reconstructed_histogram{ "reconstructed_distribution_mixed_prec/u", ";p_{rec} (GeV/u); Count", 100, 0, 1.3 };
};
template<>
struct reconstructed_distribution_data<histogram<absolute, reconstructed_based>, no_requirement>{
    TH1D reconstructed_histogram{ "reconstructed_distribution_mixed_prec", ";p_{rec} (GeV); Count", 1000, 0, 9 };
};
template<int C>
struct reconstructed_distribution_data<histogram<per_nucleon, reconstructed_based>, isolate_charge<C>>{
    TH1D reconstructed_histogram{ Form("reconstructed_distribution_charge%d_prec/u", C), ";p_{rec} (GeV/u); Count", 100, 0, 1.3 };
};
template<int C>
struct reconstructed_distribution_data<histogram<absolute, reconstructed_based>, isolate_charge<C>>{
    TH1D reconstructed_histogram{ Form("reconstructed_distribution_charge%d_prec", C), ";p_{rec} (GeV); Count", 1000, 0, 9 };
};
template<class CO>
struct reconstructed_distribution_data< computation, CO >{
    std::size_t reconstructed_count{0};
};


//----------------------------------------------
//                  reconstructible_distribution

template<class Derived, class F> struct reconstructible_distribution_outcome{};
template<class Derived>
struct reconstructible_distribution_outcome<Derived, histogram<per_nucleon, reconstructible_based>>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.reconstructible_histogram.Fill(
                            reconstructible.properties.momentum/(1000*reconstructible.properties.mass)
                                          );
    }
};
template<class Derived>
struct reconstructible_distribution_outcome<Derived, histogram<absolute, reconstructible_based>>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.reconstructible_histogram.Fill( reconstructible.properties.momentum/1000 );
    }
};

template<class Derived>
struct reconstructible_distribution_outcome<Derived, computation>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        ++data.reconstructible_count;
    }
};


template<class Derived, class F, class CO> struct reconstructible_distribution_output{};
template<class Derived, class CO >
struct reconstructible_distribution_output< Derived, computation, CO >{
    value_and_error output() const {
        value_and_error result;
        auto const& data = static_cast<Derived const&>(*this).data();
        result.value = data.reconstructible_count;
        result.error = sqrt(data.reconstructible_count);
        return result;
    }
};

template<class Derived, class T, class B, class CO>
struct reconstructible_distribution_output< Derived, histogram<T, B>, CO>{
    void output() const { static_cast<Derived const&>(*this).data().reconstructible_histogram.Write(); }
};

template<class F, class CO> struct reconstructible_distribution_data{};
template<>
struct reconstructible_distribution_data<histogram<per_nucleon, reconstructible_based>, no_requirement>{
    TH1D reconstructible_histogram{ "reconstructible_distribution_mixed_pmc/u", ";p_{mc} (GeV/u); Count", 100, 0, 1.3 };
};
template<>
struct reconstructible_distribution_data<histogram<absolute, reconstructible_based>, no_requirement>{
    TH1D reconstructible_histogram{ "reconstructible_distribution_mixed_pmc", ";p_{mc} (GeV); Count", 1000, 0, 9 };
};
template<int C>
struct reconstructible_distribution_data<histogram<per_nucleon, reconstructible_based>, isolate_charge<C>>{
    TH1D reconstructible_histogram{ Form("reconstructible_distribution_charge%d_pmc/u", C), ";p_{mc} (GeV/u); Count", 100, 0, 1.3 };
};
template<int C>
struct reconstructible_distribution_data<histogram<absolute, reconstructible_based>, isolate_charge<C>>{
    TH1D reconstructible_histogram{ Form("reconstructible_distribution_charge%d_pmc", C), ";p_{mc} (GeV); Count", 1000, 0, 9 };
};


template<class CO>
struct reconstructible_distribution_data< computation, CO >{
    std::size_t reconstructible_count{0};
};

template<class CO, class MO>
struct reconstructible_distribution_predicate {
    bool predicate( reconstruction_module<TAGcluster> const& rm_p ) const {
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
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.clone_count+=reconstructed.clone_number;
    }
};

template<class Derived, class F, class CO> struct clone_distribution_output{};
template< class Derived, class CO >
struct clone_distribution_output< Derived, computation, CO  >{
    value_and_error output() const {
        value_and_error result;
        auto const& data = static_cast<Derived const&>(*this).data();
        result.value = data.clone_count;
        result.error = sqrt(data.clone_count);
        return result;
    }
};

template<class F, class CO> struct clone_distribution_data{};
template<class CO> struct clone_distribution_data<computation, CO>{ std::size_t clone_count{0}; };


//----------------------------------------------
//           mass_identification
template<class Derived, class F> struct mass_identification_outcome{};
template<class Derived, class R, class B>
struct mass_identification_outcome<Derived, histogram<R, B>>{
    void outcome( reconstruction_module<TAGcluster> const& rm_p ){
        auto const& reconstructed = rm_p.reconstructed_o.value();
        auto const& reconstructible = rm_p.reconstructible_o.value();
        auto & data = static_cast<Derived&>(*this).data();
        data.value.Fill( reconstructible.properties.mass, reconstructed.properties.mass );
    }
};

template<class Derived, class F, class CO> struct mass_identification_output{};
template<class Derived, class R, class B, class CO>
struct mass_identification_output<Derived, histogram<R, B>, CO>{
    void output() const { static_cast<Derived const&>(*this).data().value.Write(); }
};

template<class F, class CO> struct mass_identification_data{};
template<class R, class B>
struct mass_identification_data<histogram<R, B>, no_requirement> {
    TH2D value{"mass_identification_mixed", ";A_{mc};A_{rec};Count", 20, 0, 20, 20, 0, 20  };
};

template<class R, class B, int C>
struct mass_identification_data<histogram<R, B>, isolate_charge<C>> {
    TH2D value{Form("mass_identification_charge%d", C), ";A_{mc};A_{rec};Count", 20, 0, 20, 20, 0, 20  };
};

//----------------------------------------------
//           momentum_resolution
struct momentum_resolution{
    
};


//----------------------------------------------
//                 Outputer

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
        auto predicate_l = [this](reconstruction_module<TAGcluster> const& rm_p){ return predicate( rm_p ); };
        auto outcome_l = [this](reconstruction_module<TAGcluster> const& rm_p){ outcome( rm_p ); };
        return aftereffect::make_aftereffect(std::move(predicate_l), std::move(outcome_l));
    }

    auto& data() { return data_m; }
    auto const& data() const { return data_m; }
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
                                    fake_distribution_outcome,
                                    fake_distribution_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using reconstructed_distribution = producer< Format, ChargeOption, MatchOption,
                                             reconstructed_distribution_data,
                                             successfull_reconstruction_predicate,
                                             reconstructed_distribution_outcome,
                                             reconstructed_distribution_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using reconstructible_distribution = producer< Format, ChargeOption, MatchOption,
                                             reconstructible_distribution_data,
                                             reconstructible_distribution_predicate,
                                             reconstructible_distribution_outcome,
                                             reconstructible_distribution_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using clone_distribution = producer< Format, ChargeOption, MatchOption,
                                    clone_distribution_data,
                                    successfull_reconstruction_predicate,
                                    clone_distribution_outcome,
                                    clone_distribution_output >;

template<class Format, class ChargeOption = no_requirement, class MatchOption = no_requirement>
using mass_identification = producer< Format, ChargeOption, MatchOption,
                                    mass_identification_data,
                                    successfull_reconstruction_predicate,
                                    mass_identification_outcome,
                                    mass_identification_output >;


} // namespace checker

template<class ... Producers>
struct TATOEchecker{
    void apply(std::vector<reconstruction_module<TAGcluster>> const& rm_pc){
        auto aftereffect_c = generate_aftereffects( std::make_index_sequence<sizeof...(Producers)>{} );
        for( auto const& module : rm_pc){
            aftereffect_c.evaluate( module );
        }
    }
    
    void output() const { output_impl( std::make_index_sequence<sizeof...(Producers)>{}); }
    
private:
    template<std::size_t ... Indices>
    auto generate_aftereffects( std::index_sequence<Indices...>  ){
        return aftereffect::make_aftereffect_list( std::get<Indices>(producer_mc).generate_aftereffect()... );
    }
    
    template<std::size_t ... Indices>
    void output_impl( std::index_sequence<Indices...> ) const {
        int expander[] = {0, (std::get<Indices>(producer_mc).output(), 0)...};
    }
    
private:
    std::tuple<Producers...> producer_mc;
};

template<class Producer>
struct TATOEchecker<Producer>{
    void apply(std::vector<reconstruction_module<TAGcluster>> const& rm_pc){
        auto aftereffect = producer_m.generate_aftereffect();
        for( auto const& module : rm_pc){ aftereffect( module ); }
    }
    
    auto output() const { return producer_m.output(); }
    
private:
    Producer producer_m;
};


struct computation_checker{
    struct eraser{
        virtual ~eraser() = default;
        virtual void apply( std::vector<reconstruction_module<TAGcluster>> const& ) = 0;
        virtual checker::value_and_error output() const =0;
    };
    
    template<class T>
    struct holder : eraser{
        holder(T t_p) : t_m{ std::move(t_p) }{}
        
        void apply( std::vector<reconstruction_module<TAGcluster>> const& rm_pc ) override { t_m.apply( rm_pc ); }
        checker::value_and_error output() const override{ return t_m.output(); }
    private:
        T t_m;
    };
    
    template<class T>
    computation_checker(T t_p) : erased_checker_mh{ new holder<T>{ std::move(t_p) } }{}
    void apply( std::vector<reconstruction_module<TAGcluster>> const& rm_pc ){ erased_checker_mh->apply(rm_pc);}
    checker::value_and_error output() const { return erased_checker_mh->output(); }
    
private:
    std::unique_ptr<eraser> erased_checker_mh;
};



struct histogram_checker{
    struct eraser{
        virtual ~eraser() = default;
        virtual void apply( std::vector<reconstruction_module<TAGcluster>> const& ) = 0;
        virtual void output() const =0;
    };
    
    template<class T>
    struct holder : eraser{
        holder(T t_p) : t_m{ std::move(t_p) }{}
        
        void apply( std::vector<reconstruction_module<TAGcluster>> const& rm_pc ) override { t_m.apply( rm_pc ); }
        void output() const override{ t_m.output(); }
    private:
        T t_m;
    };
    
    template<class T>
    histogram_checker(T t_p) : erased_checker_mh{ new holder<T>{ std::move(t_p) } }{}
    void apply( std::vector<reconstruction_module<TAGcluster>> const& rm_pc ){ erased_checker_mh->apply(rm_pc);}
    void output() const { erased_checker_mh->output(); }
    
private:
    std::unique_ptr<eraser> erased_checker_mh;
};


#endif /* TATOEchecker_h */
