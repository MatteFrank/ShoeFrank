
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

class TADIgeoField;

namespace details{

//template< class ... Ts> struct any_of : std::false_type {};
//template< class T> struct any_of<T> : T {};
//template< class T, class ... Ts>
//struct any_of<T, Ts...> : std::conditional< bool(T::value), T, any_of<Ts...>>::type {};

template<class ... Ts> struct cut_counter_impl{};
template<class ... Ts>
struct cut_counter_impl<double, Ts...>{
    constexpr void operator()(std::size_t& count_p) const {
        ++count_p;
        cut_counter_impl<Ts...>{}(count_p);
    }
};
template<std::size_t N, class ... Ts>
struct cut_counter_impl<std::array<double, N>, Ts...>{
    constexpr void operator()(std::size_t& count_p) const {
        count_p +=N;
        cut_counter_impl<Ts...>{}(count_p);
    }
};
template<> struct cut_counter_impl<double>{
    constexpr void operator()(std::size_t& count_p) const { ++count_p; }
};
template<std::size_t N>
struct cut_counter_impl< std::array<double, N> >{
    constexpr void operator()(std::size_t& count_p) const { count_p += N; }
};
template<> struct cut_counter_impl<>{
    constexpr void operator()(std::size_t&) const { }
};


template<class ... Ts>
struct cut_counter{
    constexpr std::size_t operator()() const {
        std::size_t count{0};
        cut_counter_impl<Ts...>{}(count);
        return count;
    }
};

template<class ... Ts>
struct cut_counter<std::tuple<Ts...>>{
    constexpr std::size_t operator()() const {
        std::size_t count{0};
        cut_counter_impl<Ts...>{}(count);
        return count;
    }
};

template<class T> struct cut_count_impl{};
template<class ... Ts>
struct cut_count_impl<std::tuple<Ts...>>{
    template<std::size_t ... Indices>
    constexpr std::size_t operator()( std::index_sequence<Indices...> ) const {
        return cut_counter<typename std::tuple_element<Indices, std::tuple<Ts...>>::type::cut_t...>{}();
    }
};

template< class T, std::size_t N>
constexpr std::size_t cut_count_up_to( ){
    return cut_count_impl<T>{}( std::make_index_sequence<N>{} );
}

template<std::size_t R>
struct range{
    static constexpr std::size_t value = R;
};

template<class R, class ... Ts>
struct configuration {
    using detector_tuple_t = std::tuple<Ts...>;
    static constexpr std::size_t detector_count = sizeof...(Ts);
    static constexpr std::size_t cut_count = cut_counter<typename Ts::cut_t...>{}();
    using cut_tuple_t = std::tuple< typename Ts::cut_t... >;
    static constexpr std::size_t range = R::value;
};

template<class C>
struct cut_holder{
    using tuple_t = typename C::cut_tuple_t;
    using detector_tuple_t = typename C::detector_tuple_t;
    
    constexpr void reset_cuts(){ cut_mc = saved_cut_mc; }
    constexpr void save_cuts(){ saved_cut_mc = cut_mc; }
    constexpr double* get_cut_handle( std::size_t index_p ){
        return get_cut_handle_impl(index_p, std::make_index_sequence< C::cut_count >{} );
    }
private:
    template<std::size_t ... Indices>
    constexpr double* get_cut_handle_impl(std::size_t index_p, std::index_sequence<Indices...>){
        double* result_h{nullptr};
        int expander[] =  { 0, ( index_p == Indices ? void(result_h = get_underlying_cut<Indices>( std::make_index_sequence<C::detector_count-1>{} ) ) : void() , 0)...};
        return result_h;
    }
    
    
    template< std::size_t Index,
    std::size_t ... Indices,
    typename std::enable_if_t< Index < cut_count_up_to<detector_tuple_t, 1>(  ) , std::nullptr_t> = nullptr >
    constexpr double* get_underlying_cut(std::index_sequence<Indices...>){
        return get_underlying_cut_impl<0>( Index );
    }
    template< std::size_t Index,
    std::size_t ... Indices,
    typename std::enable_if_t< Index >= cut_count_up_to<detector_tuple_t, C::detector_count -1>() , std::nullptr_t> = nullptr >
    constexpr double* get_underlying_cut( std::index_sequence<Indices...>){
        return get_underlying_cut_impl<C::detector_count -1>( Index - cut_count_up_to<detector_tuple_t, C::detector_count -1>()  );
    }
    template< std::size_t Index,
    std::size_t ... Indices,
    typename std::enable_if_t< (Index >= cut_count_up_to<detector_tuple_t, 1>() &&
                                Index <  cut_count_up_to<detector_tuple_t, C::detector_count-1>() ) , std::nullptr_t> = nullptr>
    constexpr double* get_underlying_cut( std::index_sequence<Indices...>){
        double* result_h{nullptr};
        int expander[] =  {0, ( cut_count_up_to<detector_tuple_t, Indices>() <= Index && Index < cut_count_up_to<detector_tuple_t, Indices+1>() ?
                               void(result_h = get_underlying_cut_impl<Indices>( Index - cut_count_up_to<detector_tuple_t, Indices>() ) ) :
                               void() , 0)...};
        return result_h;
    }
    
    template<std::size_t Index, typename std::enable_if_t< std::is_same<typename std::tuple_element<Index, tuple_t>::type, double >::value, std::nullptr_t > = nullptr >
    constexpr double* get_underlying_cut_impl( std::size_t ){return &std::get<Index>(cut_mc); }
    template<std::size_t Index, typename std::enable_if_t< !std::is_same<typename std::tuple_element<Index, tuple_t>::type, double >::value, std::nullptr_t > = nullptr >
    constexpr double* get_underlying_cut_impl( std::size_t index_p ){return &std::get<Index>(cut_mc)[index_p]; }
    
    
    template<std::size_t ... Indices>
    constexpr tuple_t retrieve_default( std::index_sequence<Indices...> ) const {
        tuple_t result;
        int expander[] = {0, (std::get<Indices>(result) = std::tuple_element<Indices, typename C::detector_tuple_t>::type::default_cut_value, 0)...};
        return result;
    }
private:
    tuple_t cut_mc{ retrieve_default( std::make_index_sequence<C::detector_count>{} ) };
    tuple_t saved_cut_mc{ cut_mc };
};



template<class C> struct action_factory{};
template<class R >
struct action_factory<configuration<R, vertex_tag, tof_tag>> {
    auto operator()(){
        using state_vector =  matrix<4,1> ;
        using state_covariance =  matrix<4, 4> ;
        using state = state_impl< state_vector, state_covariance  >;
        
        
        auto * cluster_vtx_hc = static_cast<TAVTntuCluster*>( gTAGroot->FindDataDsc("vtClus")->Object() );
        auto * vertex_hc = static_cast<TAVTntuVertex*>( gTAGroot->FindDataDsc("vtVtx")->Object() );
        auto * geo_vtx_h = static_cast<TAVTparGeo*>(gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() );
        
        auto * cluster_tw_hc = static_cast<TATWntuPoint*>( gTAGroot->FindDataDsc("twPoint")->Object() );
        auto * geo_tw_h = static_cast<TATWparGeo*>( gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object() );
        
        
        auto list = start_list( detector_properties<vertex_tag>(vertex_hc,
                                                                cluster_vtx_hc,
                                                                geo_vtx_h) )
                        .add( detector_properties<tof_tag>(cluster_tw_hc, geo_tw_h) )
                        .finish();
        
        auto * field_h = static_cast<TADIgeoField*>(gTAGroot->FindParaDsc(TADIgeoField::GetDefParaName())->Object());
        auto ode = make_ode< matrix<2,1>, 2>( model{ field_h } );
        auto stepper = make_stepper<data_grkn56>( std::move(ode) );
        auto ukf = make_ukf<state>( std::move(stepper) );
        
        return new_TATOEactGlb(
                               std::move(ukf),
                               std::move(list),
                               nullptr,
                               static_cast<TAGparGeo*>( gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object() ),
                               field_h,
                               true
                               );
    }
};
    
    
template< class R>
struct action_factory<configuration< R, vertex_tag, it_tag, tof_tag>> {
    auto operator()(){
        using state_vector =  matrix<4,1> ;
        using state_covariance =  matrix<4, 4> ;
        using state = state_impl< state_vector, state_covariance  >;
        
        
        auto * cluster_vtx_hc = static_cast<TAVTntuCluster*>( gTAGroot->FindDataDsc("vtClus")->Object() );
        auto * vertex_hc = static_cast<TAVTntuVertex*>( gTAGroot->FindDataDsc("vtVtx")->Object() );
        auto * geo_vtx_h = static_cast<TAVTparGeo*>(gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() );
        
        auto * cluster_it_hc = static_cast<TAITntuCluster*>(  gTAGroot->FindDataDsc("itClus")->Object() );
        auto * geo_it_h = static_cast<TAITparGeo*>( gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object() );
        
        auto * cluster_tw_hc = static_cast<TATWntuPoint*>( gTAGroot->FindDataDsc("twPoint")->Object() );
        auto * geo_tw_h = static_cast<TATWparGeo*>( gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object() );
        
        
        auto list = start_list( detector_properties<vertex_tag>(vertex_hc,
                                                                cluster_vtx_hc,
                                                                geo_vtx_h) )
                    .add( detector_properties<details::it_tag>(cluster_it_hc, geo_it_h) )
                    .add( detector_properties<tof_tag>(cluster_tw_hc, geo_tw_h) )
                    .finish();
        
        auto * field_h = static_cast<TADIgeoField*>(gTAGroot->FindParaDsc(TADIgeoField::GetDefParaName())->Object());
        auto ode = make_ode< matrix<2,1>, 2>( model{ field_h } );
        auto stepper = make_stepper<data_grkn56>( std::move(ode) );
        auto ukf = make_ukf<state>( std::move(stepper) );
        
        return new_TATOEactGlb(
                               std::move(ukf),
                               std::move(list),
                               nullptr,
                               static_cast<TAGparGeo*>( gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object() ),
                               field_h,
                               true
                               );
    }
};

template< class R >
struct action_factory<configuration<R, vertex_tag, it_tag, msd_tag, tof_tag>> {
    auto operator()(){
        using state_vector =  matrix<4,1> ;
        using state_covariance =  matrix<4, 4> ;
        using state = state_impl< state_vector, state_covariance  >;
        
        
        auto * cluster_vtx_hc = static_cast<TAVTntuCluster*>( gTAGroot->FindDataDsc("vtClus")->Object() );
        auto * vertex_hc = static_cast<TAVTntuVertex*>( gTAGroot->FindDataDsc("vtVtx")->Object() );
        auto * geo_vtx_h = static_cast<TAVTparGeo*>(gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() );
        
        auto * cluster_it_hc = static_cast<TAITntuCluster*>(  gTAGroot->FindDataDsc("itClus")->Object() );
        auto * geo_it_h = static_cast<TAITparGeo*>( gTAGroot->FindParaDsc(TAITparGeo::GetDefParaName(), "TAITparGeo")->Object() );
        
        auto * cluster_msd_hc = static_cast<TAMSDntuCluster*>(  gTAGroot->FindDataDsc("msdClus")->Object() );
        auto * geo_msd_h = static_cast<TAMSDparGeo*>( gTAGroot->FindParaDsc(TAMSDparGeo::GetDefParaName(), "TAMSDparGeo")->Object() );
        
        auto * cluster_tw_hc = static_cast<TATWntuPoint*>( gTAGroot->FindDataDsc("twPoint")->Object() );
        auto * geo_tw_h = static_cast<TATWparGeo*>( gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object() );
        
        
        auto list = start_list( detector_properties<vertex_tag>(vertex_hc,
                                                                cluster_vtx_hc,
                                                                geo_vtx_h) )
                        .add( detector_properties<details::it_tag>(cluster_it_hc, geo_it_h) )
                        .add( detector_properties<details::msd_tag>(cluster_msd_hc, geo_msd_h) )
                        .add( detector_properties<tof_tag>(cluster_tw_hc, geo_tw_h) )
                        .finish();
        
        auto * field_h = static_cast<TADIgeoField*>(gTAGroot->FindParaDsc(TADIgeoField::GetDefParaName())->Object());
        auto ode = make_ode< matrix<2,1>, 2>( model{ field_h } );
        auto stepper = make_stepper<data_grkn56>( std::move(ode) );
        auto ukf = make_ukf<state>( std::move(stepper) );
        
        return new_TATOEactGlb(
                               std::move(ukf),
                               std::move(list),
                               nullptr,
                               static_cast<TAGparGeo*>( gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object() ),
                               field_h,
                               true
                               );
    }
};
    
    
template<class C>
constexpr auto new_action(){ return action_factory<C>{}(); }



}// namespace details



template< class Derived >
struct baseline_procedure{
    constexpr void call() { puts(__PRETTY_FUNCTION__) ;}
};

template< class Derived >
struct scan_procedure{
    constexpr void call() { puts(__PRETTY_FUNCTION__) ; static_cast<Derived&>(*this).selected_cut_m = 2;}
};

template< class Derived >
struct focus_procedure{
    constexpr void call() { puts(__PRETTY_FUNCTION__) ;}
};


struct TATOEbaseCutter : TAGaction {
    TATOEbaseCutter(char const* name_p) :
    TAGaction(name_p, "TATOEcutter - Tool used to determine optimal cut values according to geometry for use in TOE") {}
    virtual void NextIteration() = 0;
    virtual Bool_t Action() = 0;
    virtual void Output() const  = 0 ;
    virtual ~TATOEbaseCutter() = default;
};


template<class C>
struct TATOEcutter : TATOEbaseCutter,
    baseline_procedure< TATOEcutter<C> >,
    scan_procedure< TATOEcutter<C> >,
    focus_procedure< TATOEcutter<C> >
{
    using baseline_procedure< TATOEcutter >::call;
    friend class baseline_procedure< TATOEcutter >;
    using scan_procedure< TATOEcutter >::call;
    friend class scan_procedure< TATOEcutter >;
    using focus_procedure< TATOEcutter >::call;
    friend class focus_procedure< TATOEcutter >;
    
    struct iterator{
        virtual ~iterator() = default;
        virtual bool can_increment() = 0;
        virtual void increment_and_setup() = 0;
        virtual void switch_procedure() = 0;
    };
    
    struct baseline_iterator : iterator {
        baseline_iterator(TATOEcutter* c_ph) : c_mh{c_ph} {}
        bool can_increment() override { return !was_called_m; }
        void increment_and_setup() override { was_called_m = true; c_mh->call(); }
        void switch_procedure() override {
            c_mh->call_mhf = &scan_procedure<TATOEcutter>::call;
            c_mh->iterator_mh.reset( new scan_iterator{c_mh} );
        }
    private:
        bool was_called_m{false};
        TATOEcutter* c_mh;
    };
    
    struct scan_iterator : iterator {
        scan_iterator(TATOEcutter* c_ph) : c_mh{c_ph} {  }
        bool can_increment() override {
            return modifier_m < 0 || (++iterator_m)-- != c_mh->remaining_cut_mc.end() ;
        }
        void increment_and_setup() override {
            if( modifier_m > 0 ){ ++iterator_m; modifier_m = -1; }
            else { modifier_m += 2; }
            c_mh->cut_mc.reset_cuts();
            *c_mh->cut_mc.get_cut_handle( *iterator_m ) += modifier_m;
        }
        void switch_procedure() override {
            c_mh->cut_mc.reset_cuts();
            c_mh->call_mhf = &focus_procedure<TATOEcutter>::call;
            c_mh->iterator_mh.reset( new focus_iterator{c_mh} );
        }
    private:
        TATOEcutter* c_mh;
        std::vector<std::size_t>::iterator iterator_m{ c_mh->remaining_cut_mc.begin() };
        int modifier_m{-3};
    };
    
    struct focus_iterator : iterator {
        focus_iterator(TATOEcutter* c_ph) : c_mh{c_ph} {}
        bool can_increment() override { return counter_m < C::range + 1; }
        void increment_and_setup() override {
            c_mh->cut_mc.reset_cuts();
            *c_mh->cut_mc.get_cut_handle( c_mh->selected_cut_m ) += ++counter_m * c_mh->focus_modifier_m;
        }
        void switch_procedure() override {
            c_mh->cut_mc.save_cuts();
            c_mh->remaining_cut_mc.erase( std::remove_if(
                                                         c_mh->remaining_cut_mc.begin(),
                                                         c_mh->remaining_cut_mc.end(),
                                                         [this](auto const& value_p){ return value_p == c_mh->selected_cut_m; }
                                                         ) );
            c_mh->call_mhf = &baseline_procedure<TATOEcutter>::call;
            c_mh->iterator_mh.reset( new baseline_iterator{c_mh} );
        }
    private:
        TATOEcutter* c_mh;
        std::size_t counter_m{0};
    };
    friend class TATOEcutter::iterator;
    
    
    TATOEcutter(char const* name_p) :
        TATOEbaseCutter(name_p),
        action_mh{ details::new_action<C>() },
        iterator_mh{ new baseline_iterator{this} },
        call_mhf{&baseline_procedure<TATOEcutter>::call} {}
    
    void NextIteration() override{
        if( iterator_mh->can_increment() ) { puts(__PRETTY_FUNCTION__); std::cout << std::flush;  iterator_mh->increment_and_setup(); }
        else{ iterator_mh->switch_procedure(); iterator_mh->increment_and_setup(); }
    };
    
    Bool_t Action() override {
        
        std::cout << "current_cuts: ";
        for(auto i{0}; i < C::cut_count; ++i){ std::cout << *cut_mc.get_cut_handle(i) << " "; }
        std::cout << '\n';
        action_mh->Action();
        result_m = action_mh->retrieve_result();
        call();
        
        return true;
    }
    
    void Output() const override{
        
//        for( auto const& result : result_mc) {
//            std::size_t reconstructed_number{0};
//            std::size_t reconstructible_number{0};
//
//            std::size_t correct_cluster_number{0};
//            std::size_t recovered_cluster_number{0};
//
//            std::size_t clone_number{0};
//
//            for(auto const & module : result.module_c){
//                reconstructed_number += module.reconstructed_number;
//                reconstructible_number += module.reconstructible_number;
//
//                correct_cluster_number += module.correct_cluster_number;
//                recovered_cluster_number += module.recovered_cluster_number;
//
//                clone_number += module.clone_number;
//            }
//
//            std::cout << "===== MIXED_RESULTS ====\n";
//            std::cout << "----- efficiency -----\n";
//            auto efficiency = reconstructed_number * 1./reconstructible_number;
//            std::cout << "global_efficiency: " << efficiency * 100 << '\n';
//            auto efficiency_error = sqrt(efficiency* (1+ efficiency)/reconstructible_number);
//            std::cout << "global_efficiency_error: " << efficiency_error * 100<< '\n';
//
//            std::cout << "----- purity -----\n";
//
//            auto purity = correct_cluster_number * 1./recovered_cluster_number;
//            std::cout << "global_purity: " << purity * 100 << '\n';
//            auto purity_error = sqrt(purity* (1+purity)/recovered_cluster_number);
//            std::cout << "global_purity_error: " << purity_error * 100<< '\n';
//
//        }
    }
    
private:
    void call() { (this->*call_mhf)(); }
    
    std::vector<std::size_t> generate_remaining_cut() {
        std::vector<std::size_t> result_c;
        for( std::size_t i{0}; i < C::cut_count ; ++i ){ result_c.push_back(i); }
        return result_c;
    }
    
private:
    std::unique_ptr<TATOEbaseAct> action_mh;
    
    std::unique_ptr<iterator> iterator_mh;
    void (TATOEcutter::* call_mhf)();
    
    details::cut_holder<C> cut_mc;
    std::vector<std::size_t> remaining_cut_mc{ generate_remaining_cut() };
    std::size_t selected_cut_m;
    int focus_modifier_m{1};

    reconstruction_result result_m;
};



#endif
