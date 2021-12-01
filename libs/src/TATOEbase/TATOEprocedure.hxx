
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
#include <cmath>


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


namespace tag{
struct baseline{};
struct first_score{};
struct last_winner{};
}//namespace tag

template<class S>
struct baseline_procedure {
    template<class O>
    constexpr bool is_done(O const *) const { return is_done_m; }
    
    template<class O>
    constexpr void call( O* o_ph ) {
//        puts(__PRETTY_FUNCTION__) ;
        
        o_ph->baseline = S{}.compute_first_score( o_ph );
        std::cout << "baseline: " << o_ph->baseline << "\n";
    }
    
    template<class O>
    void setup_next_iteration(O* o_ph) { is_done_m = true; }
    template<class O> void setup_cuts(O* o_ph) { }
    template<class O> void finalise(O* o_ph){}
    template<class O> void setup(O* o_ph){};
    
private:
    bool is_done_m{false};
};


template<class S>
struct procedure_result {
    std::size_t cut_index;
    int modifier;
    S score;
};


struct baseline_scorer{
    struct score_holder{
        double value;
        double efficiency;
        double purity;
        friend std::ostream& operator<<(std::ostream& os_p, score_holder const& s_p){
            return os_p << "[efficiency, purity : score] -> [" << s_p.efficiency << ", " << s_p.purity << " : " << s_p.value << "]";
        }
    };
    template<class O>
    constexpr score_holder compute_score(O const* o_ph) const {
        auto reconstructible = o_ph->retrieve_reconstructible();
        auto reconstructed = o_ph->retrieve_reconstructed();
        auto efficiency = reconstructed.value / reconstructible.value;
        
        auto purity = o_ph->retrieve_purity();
        
        auto score = ( efficiency - o_ph->baseline.efficiency )/o_ph->baseline.efficiency +
        ( purity.value - o_ph->baseline.purity )/o_ph->baseline.purity;
        
        return score_holder{ std::move(score), std::move(efficiency), std::move(purity.value) };
    }
    template<class O>
    constexpr score_holder compute_first_score(O const* o_ph) const {
        auto reconstructible = o_ph->retrieve_reconstructible();
        auto reconstructed = o_ph->retrieve_reconstructed();
        auto efficiency = reconstructed.value / reconstructible.value;
        
        auto purity = o_ph->retrieve_purity();
        return score_holder{ 0, efficiency, purity.value};
    }
};

struct purity_scorer{
    struct score_holder{
        double value;
        double purity;
        friend std::ostream& operator<<(std::ostream& os_p, score_holder const& s_p){
            return os_p << "[purity : score] -> [" << s_p.purity << " : " << s_p.value << "]";
        }
    };
    template<class O>
    constexpr score_holder compute_score( O const* o_ph) const {
        auto purity = o_ph->retrieve_purity();
        auto score = ( purity.value - o_ph->baseline.purity )/o_ph->baseline.purity;
        return score_holder{ std::move(score), std::move(purity.value)};
    }
    template<class O>
    constexpr score_holder compute_first_score( O const* o_ph) const {
        return score_holder{ 0, o_ph->retrieve_purity().value };
    }
};

struct mass_scorer{
    struct score_holder{
        double value;
        double mass_yield;
        friend std::ostream& operator<<(std::ostream& os_p, score_holder const& s_p){
            return os_p << "[mass_yield : score] -> [" << s_p.mass_yield << " : " << s_p.value << "]";
        }
    };
    template<class O>
    constexpr score_holder compute_score( O const* o_ph) const {
        auto mass_yield = o_ph->retrieve_mass_yield();
        auto score = ( mass_yield.value - o_ph->baseline.mass_yield )/o_ph->baseline.mass_yield;
        return score_holder{ std::move(score), std::move(mass_yield.value)};
    }
    template<class O>
    constexpr score_holder compute_first_score( O const* o_ph) const {
        return score_holder{0, o_ph->retrieve_mass_yield().value};
    }
};


struct momentum_scorer{
    struct score_holder{
        double value;
        double residuals;
        friend std::ostream& operator<<(std::ostream& os_p, score_holder const& s_p){
            return os_p << "[momentum_residuals : score] -> [" << s_p.residuals << " : " << s_p.value << "]";
        }
    };
    template<class O>
    constexpr score_holder compute_score( O const* o_ph) const {
        auto residuals = o_ph->retrieve_momentum_residuals();
        auto score = -( residuals.value - o_ph->baseline.residuals )/o_ph->baseline.residuals;
        return score_holder{ std::move(score), std::move(residuals.value)};
    }
    template<class O>
    constexpr score_holder compute_first_score( O const* o_ph) const {
        return score_holder{0, o_ph->retrieve_momentum_residuals().value};
    }
};

struct overall_scorer{
    struct score_holder{
        double value;
        double efficiency;
        double purity;
        double mass_yield;
        friend std::ostream& operator<<(std::ostream& os_p, score_holder const& s_p){
            return os_p << "[efficiency, purity, mass_yield : score] -> [" << s_p.efficiency << ", " << s_p.purity << ", " << s_p.mass_yield << " : " << s_p.value << "]";
        }
    };
    template<class O>
    constexpr score_holder compute_score(O const* o_ph) const {
        auto reconstructible = o_ph->retrieve_reconstructible();
        auto reconstructed = o_ph->retrieve_reconstructed();
        auto efficiency = reconstructed.value / reconstructible.value;
        
        auto purity = o_ph->retrieve_purity();
        auto mass_yield = o_ph->retrieve_mass_yield();
        
        auto score = pow( ( efficiency - o_ph->baseline.efficiency )/o_ph->baseline.efficiency, 3) +
                     pow( ( purity.value - o_ph->baseline.purity )/o_ph->baseline.purity, 3) +
                     ( mass_yield.value - o_ph->baseline.mass_yield )/o_ph->baseline.mass_yield  ;
        
        return score_holder{ std::move(score), std::move(efficiency), std::move(purity.value), std::move(mass_yield.value) };
    }
    template<class O>
    constexpr score_holder compute_first_score(O const* o_ph) const {
        auto reconstructible = o_ph->retrieve_reconstructible();
        auto reconstructed = o_ph->retrieve_reconstructed();
        auto efficiency = reconstructed.value / reconstructible.value;
        
        auto purity = o_ph->retrieve_purity();
        return score_holder{ 0, efficiency, purity.value, o_ph->retrieve_mass_yield().value};
    }
};

template<class Derived>
struct caller{
    template<class O>
    constexpr void call(O *o_ph){
        puts(__PRETTY_FUNCTION__) ;
        
        auto reconstructible = o_ph->retrieve_reconstructible();
        auto reconstructed = o_ph->retrieve_reconstructed();
        auto efficiency = reconstructed.value / reconstructible.value;
        
        auto purity = o_ph->retrieve_purity();
        
        auto& derived = static_cast<Derived&>(*this);
        auto score = derived.compute_score( o_ph); //should be only o_ph right ? -> want to store efficiency and purity thouuugh
        std::cout << "score: " << score << '\n';
        
        derived.get_results().push_back(
                    procedure_result< typename Derived::score >{
                            o_ph->cut_index,
                            o_ph->sign * o_ph->offset,
                            std::move(score)
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
//        puts(__PRETTY_FUNCTION__);
        auto& derived = static_cast<Derived&>(*this);
        
        std::sort(
            derived.get_results().begin(),
            derived.get_results().end(),
            [](auto const& v1_p, auto const& v2_p){ return v1_p.score.value > v2_p.score.value; }
                  );
        
        std::cout << "sorted_selection:\n";
        for(auto const& result : derived.get_results() ){
            std::cout << "[" << result.cut_index << " : " << result.modifier << "] -> " << result.score << '\n';
        }
        
        o_ph->reset_cuts();
        
        auto const& winner = derived.get_results().front();
        if( winner.score.value > 0 ){
            o_ph->sign = winner.modifier / o_ph->offset;
            o_ph->cut_index = winner.cut_index;
            o_ph->baseline = winner.score;
            
//            auto available_cut_c = o_ph->generate_available_cuts();
//            available_cut_c.erase( std::remove_if(
//                                        available_cut_c.begin(),
//                                        available_cut_c.end(),
//                                        [&winner](auto const& value_p){ return value_p == winner.cut_index; }
//                                                  ) );
//            o_ph->get_available_cuts() = available_cut_c;
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
            [](auto const& v1_p, auto const& v2_p){ return v1_p.score.value > v2_p.score.value; }
                  );
        
        std::cout << "sorted_selection:\n";
        for(auto const& result : derived.get_results() ){
            std::cout << "[" << result.cut_index << " : " << result.modifier << "] -> " << result.score << '\n';
        }
        
        o_ph->reset_cuts();
        
        auto const& winner = derived.get_results().front();
        if( winner.score.value > 0 ){
            o_ph->sign = winner.modifier / o_ph->offset;
            o_ph->cut_index = winner.cut_index;
//            o_ph->last_winner.efficiency = winner.efficiency;
//            o_ph->last_winner.purity = winner.purity;
            o_ph->last_winner = winner.score;
        }
        else{ o_ph->is_optimization_done() = true; }
    }
};



//CRTP can be used to customized procedures but must use templated template parameters
template< class S, template<class> class ... Ms >
struct global_scan_procedure_impl : S, Ms<global_scan_procedure_impl<S, Ms>>... {
    using score = typename S::score_holder;
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
    
    std::vector<procedure_result<score>>& get_results() { return result_mc; }
    std::vector<std::size_t>::const_iterator& get_iterator() { return iterator_m; }
private:
    std::vector<procedure_result<score>> result_mc;
    std::vector<std::size_t>::const_iterator iterator_m{};
};


template<class S>
using global_scan_procedure_only = global_scan_procedure_impl<S, caller, cut_setter, global_setter, global_only_finaliser >;
template<class S>
using global_scan_procedure      = global_scan_procedure_impl<S, caller, cut_setter, global_setter, global_finaliser >;

//-----------------------------------------------------------------
//               local_scan_procedure



template<class Derived>
struct rough_scan_setter{
    template<class O>
    constexpr void setup(O * o_ph){
        puts(__PRETTY_FUNCTION__);
        
        using score_t = typename Derived::score;
        auto& derived = static_cast<Derived&>(*this);
//        auto score = derived.compute_score( o_ph->baseline.efficiency, o_ph->baseline.purity, o_ph );
//        auto score = derived.compute_score( o_ph );
        auto score = o_ph->baseline;
        score.value = 0;
        derived.get_results().push_back(
                    procedure_result<score_t>{
                            o_ph->cut_index,
                            0,
                            std::move(score)
//                            o_ph->baseline.efficiency,
//                            o_ph->baseline.purity
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
//        auto score = derived.compute_score<tag::last_winner>( o_ph );
        derived.get_results().push_back(
                    procedure_result<typename Derived::score>{
                            o_ph->cut_index,
                            0,
                            o_ph->last_winner
//                            o_ph->last_winner.efficiency,
//                            o_ph->last_winner.purity
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
            [](auto const& v1_p, auto const& v2_p){ return v1_p.score.value > v2_p.score.value; }
                  );
        
        std::cout << "sorted_selection:\n";
        for(auto const& result : derived.get_results() ){
            std::cout << "[" << result.cut_index << " : " << result.modifier << "] -> " << result.score << '\n';
        }
        
        o_ph->reset_cuts();
        
        auto const& winner = derived.get_results().front();
        o_ph->offset = winner.modifier / o_ph->sign;
        o_ph->last_winner = winner.score;
//        o_ph->last_winner.efficiency = winner.efficiency;
//        o_ph->last_winner.purity = winner.purity;
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
            [](auto const& v1_p, auto const& v2_p){ return v1_p.score.value > v2_p.score.value; }
                  );
        
        std::cout << "sorted_selection:\n";
        for(auto const& result : derived.get_results() ){
            std::cout << "[" << result.cut_index << " : " << result.modifier << "] -> " << result.score << '\n';
        }
        
        o_ph->reset_cuts();
        
        auto const& winner = derived.get_results().front();
//        o_ph->baseline.efficiency = winner.efficiency;
//        o_ph->baseline.purity = winner.purity;
        o_ph->baseline = winner.score;
        
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
    using score = typename S::score_holder;
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
    
    std::vector<procedure_result<score>>& get_results() { return result_mc; }
    int& get_counter() { return counter_m; }
private:
    std::vector<procedure_result<score>> result_mc;
    int counter_m;
};


template<class S>
using rough_scan_procedure = local_scan_procedure_impl<details::local_scan_parameters<3,3>, S, caller, cut_setter, rough_scan_setter, rough_scan_finaliser >;



template<class S>
using fine_scan_procedure = local_scan_procedure_impl<details::local_scan_parameters<3,1>, S, caller, cut_setter, fine_scan_setter, fine_scan_finaliser >;


#endif
