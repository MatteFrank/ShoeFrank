
//
//File      : TATOEprocedure.hxx
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 03/04/2021
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//


#ifndef _TATOEprocedure_HXX
#define _TATOEprocedure_HXX

#include <iostream>
#include <vector>
#include <algorithm>


template<class O> struct underlying_configuration;

namespace details{

template<std::size_t N, std::size_t S >
struct local_scan_parameters{
    static constexpr std::size_t count = N;
    static constexpr std::size_t step_size = S;
};

template<class ... Ps> struct first_of{};
template<class P, class ... Ps>
struct first_of<P, Ps...>{
    using type = P;
};

template<std::size_t Current, std::size_t Target, class ... Ts> struct find_among_impl{};
template<std::size_t Current, std::size_t Target, class T, class ... Ts>
struct find_among_impl<Current, Target, T, Ts...>{
    using type = typename find_among_impl<Current+1, Target, Ts...>::type;
};
template<std::size_t Found, class T, class ... Ts>
struct find_among_impl<Found, Found, T, Ts...>{
    using type = T;
};
template<std::size_t Index, class ... Ts>
using find_among = typename find_among_impl<0, Index, Ts...>::type;
    
}// namespace details


struct baseline_procedure {
    template<class O>
    constexpr bool is_done(O const *) const { return is_done_m; }
    
    template<class O>
    constexpr void call( O* o_ph ) {
        puts(__PRETTY_FUNCTION__) ;
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};
        std::size_t correct_cluster_number{0};
        std::size_t recovered_cluster_number{0};\
        for(auto const & module : o_ph->retrieve_results().module_c){
            reconstructed_number += module.reconstructed_number;
            reconstructible_number += module.reconstructible_number;
            correct_cluster_number += module.correct_cluster_number;
            recovered_cluster_number += module.recovered_cluster_number;
        }
        o_ph->baseline.efficiency = reconstructed_number * 1./reconstructible_number;
        o_ph->baseline.purity = correct_cluster_number * 1./recovered_cluster_number;
        
        std::cout << "new_baseline_efficiency: " << o_ph->baseline.efficiency << '\n';
        std::cout << "new_baseline_purity: " << o_ph->baseline.purity << '\n';
        std::cout << "reconstructed_number: " << reconstructed_number << '\n';
    }
    
    template<class O>
    void setup_next_iteration(O* o_ph) { is_done_m = true; }
    template<class O> void setup_cuts(O* o_ph) { }
    template<class O> void finalise(O* o_ph){}
    template<class O> void setup(O* o_ph){};
    
private:
    bool is_done_m{false};
};


// - first cut is not properly set
// - check ending
// - customize set to get rid of baseline if needed -> after doing local scan ?

struct procedure_result {
    std::size_t cut_index;
    int modifier;
    double score;
    double efficiency;
    double purity;
};


struct baseline_scorer{
    template<class O>
    constexpr double compute_score(double efficiency_p, double purity_p, O const* o_ph) const {
        return ( efficiency_p - o_ph->baseline.efficiency )/o_ph->baseline.efficiency +
               ( purity_p - o_ph->baseline.purity )/o_ph->baseline.purity;
    }
};

template<class Derived>
struct caller{
    template<class O>
    constexpr void call(O *o_ph){
        puts(__PRETTY_FUNCTION__) ;
        
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};
        std::size_t correct_cluster_number{0};
        std::size_t recovered_cluster_number{0};\
        for(auto const & module : o_ph->retrieve_results().module_c){
            reconstructed_number += module.reconstructed_number;
            reconstructible_number += module.reconstructible_number;
            correct_cluster_number += module.correct_cluster_number;
            recovered_cluster_number += module.recovered_cluster_number;
        }
        
        auto efficiency = reconstructed_number * 1./reconstructible_number;
        auto purity = correct_cluster_number * 1./recovered_cluster_number;
        
        auto& derived = static_cast<Derived&>(*this);
        auto score = derived.compute_score( efficiency, purity, o_ph);
                                                               
        std::cout << "efficiency: " << efficiency << '\n';
        std::cout << "purity: " << purity << '\n';
        
        derived.get_results().push_back(
                    procedure_result{
                            o_ph->cut_index,
                            o_ph->sign * o_ph->offset,
                            score,
                            efficiency,
                            purity
                                    }   );
    }
};

template<class Derived>
struct cut_setter{
    template<class O>
    constexpr void setup_cuts(O * o_ph){
        o_ph->reset_cuts();
        o_ph->set_selected_cut();
        o_ph->apply_cuts();
    }
};


template<class Derived>
struct global_setter{
    template<class O>
    constexpr void setup(O * o_ph){
        puts(__PRETTY_FUNCTION__);
        
        o_ph->set_selected_cut();
        o_ph->save_cuts();
        o_ph->apply_cuts();
        
        auto& derived = static_cast<Derived&>(*this);
        derived.get_iterator() = o_ph->get_available_cuts().cbegin();
        
        o_ph->sign = -1;
        o_ph->offset = 1;
        o_ph->cut_index = *derived.get_iterator();
    }
};

template<class Derived>
struct global_only_finaliser{
    template<class O>
    constexpr void finalise(O * o_ph) {
        puts(__PRETTY_FUNCTION__);
        auto& derived = static_cast<Derived&>(*this);
        
        std::sort(
            derived.get_results().begin(),
            derived.get_results().end(),
            [](auto const& v1_p, auto const& v2_p){ return v1_p.score > v2_p.score; }
                  );
        
        std::cout << "sorted_selection:\n";
        for(auto const& result : derived.get_results() ){
            std::cout << "[" << result.cut_index << " : " << result.modifier << "] -> " << result.score << '\n';
        }
        
        o_ph->reset_cuts();
        
        auto const& winner = derived.get_results().front();
        if( winner.score > 0 ){
            o_ph->sign = winner.modifier / o_ph->offset;
            o_ph->cut_index = winner.cut_index;
            o_ph->baseline.efficiency = winner.efficiency;
            o_ph->baseline.purity = winner.purity;
            
            auto available_cut_c = o_ph->generate_available_cuts();
            available_cut_c.erase( std::remove_if(
                                        available_cut_c.begin(),
                                        available_cut_c.end(),
                                        [&winner](auto const& value_p){ return value_p == winner.cut_index; }
                                                  ) );
            o_ph->get_available_cuts() = available_cut_c;
        }
        else{ o_ph->is_optimization_done() = true; }
    }
};

template<class Derived>
struct global_finaliser{
    template<class O>
    constexpr void finalise(O * o_ph) {
        puts(__PRETTY_FUNCTION__);
        auto& derived = static_cast<Derived&>(*this);
        
        std::sort(
            derived.get_results().begin(),
            derived.get_results().end(),
            [](auto const& v1_p, auto const& v2_p){ return v1_p.score > v2_p.score; }
                  );
        
        std::cout << "sorted_selection:\n";
        for(auto const& result : derived.get_results() ){
            std::cout << "[" << result.cut_index << " : " << result.modifier << "] -> " << result.score << '\n';
        }
        
        o_ph->reset_cuts();
        
        auto const& winner = derived.get_results().front();
        if( winner.score > 0 ){
            o_ph->sign = winner.modifier / o_ph->offset;
            o_ph->cut_index = winner.cut_index;
            o_ph->last_winner.efficiency = winner.efficiency;
            o_ph->last_winner.purity = winner.purity;
        }
        else{ o_ph->is_optimization_done() = true; }
    }
};



//CRTP can be used to customized procedures but must use templated template parameters
template< class S, template<class> class ... Ms >
struct global_scan_procedure_impl : S, Ms<global_scan_procedure_impl<S, Ms>>... {
    template<class O>
    constexpr bool is_done(O const * o_ph) const {
        return iterator_m == o_ph->get_available_cuts().cend() ;
    }
    
    template<class O>
    constexpr void setup_next_iteration(O * o_ph){
        puts(__PRETTY_FUNCTION__);
        if( o_ph->sign > 0 ){ o_ph->sign = -1; o_ph->cut_index = *(++iterator_m); }
        else { o_ph->sign = 1; }
    }
    
    std::vector<procedure_result>& get_results() { return result_mc; }
    std::vector<std::size_t>::const_iterator& get_iterator() { return iterator_m; }
private:
    std::vector<procedure_result> result_mc;
    std::vector<std::size_t>::const_iterator iterator_m{};
};


using global_scan_procedure_only = global_scan_procedure_impl<baseline_scorer, caller, cut_setter, global_setter, global_only_finaliser >;
using global_scan_procedure      = global_scan_procedure_impl<baseline_scorer, caller, cut_setter, global_setter, global_finaliser >;

//-----------------------------------------------------------------
//               local_scan_procedure



template<class Derived>
struct rough_scan_setter{
    template<class O>
    constexpr void setup(O * o_ph){
        puts(__PRETTY_FUNCTION__);
        
        auto& derived = static_cast<Derived&>(*this);
        auto score = derived.compute_score( o_ph->baseline.efficiency, o_ph->baseline.purity, o_ph );
        derived.get_results().push_back(
                    procedure_result{
                            o_ph->cut_index,
                            0,
                            score,
                            o_ph->baseline.efficiency,
                            o_ph->baseline.purity
                                    }   );
        
        
        derived.get_counter() = 1;

        o_ph->offset = Derived::step_size;
    }
};

template<class Derived>
struct fine_scan_setter{
    template<class O>
    constexpr void setup(O * o_ph){
        puts(__PRETTY_FUNCTION__);
        
        o_ph->set_selected_cut();
        o_ph->save_cuts();
        o_ph->apply_cuts();
        
        auto& derived = static_cast<Derived&>(*this);
        auto score = derived.compute_score( o_ph->last_winner.efficiency, o_ph->last_winner.purity, o_ph );
        derived.get_results().push_back(
                    procedure_result{
                            o_ph->cut_index,
                            0,
                            score,
                            o_ph->last_winner.efficiency,
                            o_ph->last_winner.purity
                                    }   );
        
        
        derived.get_counter() = -static_cast<int>(Derived::count-1);

        o_ph->sign = 1 ;
        o_ph->offset = derived.get_counter() * Derived::step_size;
    }
};


template<class Derived>
struct rough_scan_finaliser{
    template<class O>
    constexpr void finalise(O * o_ph) {
        puts(__PRETTY_FUNCTION__);
        auto& derived = static_cast<Derived&>(*this);
        
        std::sort(
            derived.get_results().begin(),
            derived.get_results().end(),
            [](auto const& v1_p, auto const& v2_p){ return v1_p.score > v2_p.score; }
                  );
        
        std::cout << "sorted_selection:\n";
        for(auto const& result : derived.get_results() ){
            std::cout << "[" << result.cut_index << " : " << result.modifier << "] -> " << result.score << '\n';
        }
        
        o_ph->reset_cuts();
        
        auto const& winner = derived.get_results().front();
        o_ph->offset = winner.modifier / o_ph->sign;
        o_ph->last_winner.efficiency = winner.efficiency;
        o_ph->last_winner.purity = winner.purity;
    }
};


template<class Derived>
struct fine_scan_finaliser{
    template<class O>
    constexpr void finalise(O * o_ph) {
        puts(__PRETTY_FUNCTION__);
        auto& derived = static_cast<Derived&>(*this);
        
        std::sort(
            derived.get_results().begin(),
            derived.get_results().end(),
            [](auto const& v1_p, auto const& v2_p){ return v1_p.score > v2_p.score; }
                  );
        
        std::cout << "sorted_selection:\n";
        for(auto const& result : derived.get_results() ){
            std::cout << "[" << result.cut_index << " : " << result.modifier << "] -> " << result.score << '\n';
        }
        
        o_ph->reset_cuts();
        
        auto const& winner = derived.get_results().front();
        o_ph->baseline.efficiency = winner.efficiency;
        o_ph->baseline.purity = winner.purity;
        
        auto& available_cut_c = o_ph->get_available_cuts();
        available_cut_c.erase( std::remove_if(
                                    available_cut_c.begin(),
                                    available_cut_c.end(),
                                    [&winner](auto const& value_p){ return value_p == winner.cut_index; }
                                              ) );
        
        o_ph->offset = winner.modifier;
    }
};

template< class P, class S, template<class> class ... Ms >
struct local_scan_procedure_impl : S, Ms< local_scan_procedure_impl<P, S, Ms... > >... {
    static constexpr std::size_t count = P::count;
    static constexpr std::size_t step_size = P::step_size;
    
    template<class O>
    constexpr bool is_done(O const * o_ph) const {
        return counter_m == static_cast<int>(count) ;
    }
    
    template<class O>
    constexpr void setup_next_iteration(O * o_ph){
        o_ph->offset = ( (++counter_m != 0) ? counter_m : ++counter_m ) * step_size;
    }
    
    std::vector<procedure_result>& get_results() { return result_mc; }
    int& get_counter() { return counter_m; }
private:
    std::vector<procedure_result> result_mc;
    int counter_m;
};


template<class R>
using rough_scan_procedure = local_scan_procedure_impl<R, baseline_scorer, caller, cut_setter, rough_scan_setter, rough_scan_finaliser >;


template<class R>
using fine_scan_procedure = local_scan_procedure_impl<R, baseline_scorer, caller, cut_setter, fine_scan_setter, fine_scan_finaliser >;
//    constexpr void optimize_cut() {
//        std::sort( selection_c.begin(), selection_c.end(), [](auto const& s1_p, auto const& s2_p){ return s1_p.score > s2_p.score; } );
//        
//        std::cout << "sorted_selection: "<< derived().selected_cut_m <<"\n";
//        for(auto const& selection : selection_c ){
//            std::cout << "[" << selection.offset << "] -> " << selection.score << '\n';
//        }
//        
//        auto const& winner = selection_c.front();
//        *derived().cut_mc.get_cut_handle( derived().selected_cut_m  ) += winner.offset;
//        
//        derived().selected_m.efficiency = winner.efficiency;
//        derived().selected_m.purity = winner.purity;
//        
//        selection_c.clear();
//    }
//
//};

    
//    struct fine_scan_iterator {
//        fine_scan_iterator(TATOEcutter* c_ph) : c_mh{c_ph} {}
//        bool can_increment() { return counter_m < static_cast<int>(details::underlying_parameters<C>::type::step_size - 1 ); }
//        int counter_m{ - static_cast<int>(details::underlying_parameters<C>::type::step_size) };
//    };


#endif
