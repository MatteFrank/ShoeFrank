
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

template<std::size_t N, std::size_t S >
struct local_scan_parameters{
    static constexpr std::size_t count = N;
    static constexpr std::size_t step_size = S;
};

template<std::size_t N>
constexpr std::size_t scan_iteration() {
    return N*2 + scan_iteration<N-1>();
}

template<>
constexpr std::size_t scan_iteration<1UL>() {
    return 2;
}
    
template<class P, class ... Ts>
struct configuration {
    using detector_tuple_t = std::tuple<Ts...>;
    static constexpr std::size_t detector_count = sizeof...(Ts);
    
    static constexpr std::size_t cut_count = cut_counter<typename Ts::cut_t...>{}();
    using cut_tuple_t = std::tuple< typename Ts::cut_t... >;
    
   // static constexpr std::size_t iteration_count = cut_count * (range+1) + scan_iteration<cut_count>() + 1 ;
};

template< class T > struct underlying_parameters{};
template< class P, class ... Ts>
struct underlying_parameters< configuration<P, Ts...> > {
    using type = P;
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
    template<class A>
    constexpr void apply(A * action_ph) const {
        apply_impl(action_ph, std::make_index_sequence<C::detector_count>{} );
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
    constexpr double* get_underlying_cut_impl( std::size_t ){ return &std::get<Index>(cut_mc); }
    template<std::size_t Index, typename std::enable_if_t< !std::is_same<typename std::tuple_element<Index, tuple_t>::type, double >::value, std::nullptr_t > = nullptr >
    constexpr double* get_underlying_cut_impl( std::size_t index_p ){ return &std::get<Index>(cut_mc)[index_p]; }
    
private:
    template<class A, std::size_t ... Indices>
    constexpr void apply_impl(A * action_ph, std::index_sequence<Indices...>) const {
        int expander[] =  { 0, ( action_ph->set_cuts( typename std::tuple_element<Indices, detector_tuple_t>::type{}, std::get<Indices>(cut_mc) ), void() , 0)... };
    }
    
    
    
private:
    template<std::size_t ... Indices>
    constexpr tuple_t retrieve_default( std::index_sequence<Indices...> ) const {
        tuple_t result;
        int expander[] = {0, (std::get<Indices>(result) = std::tuple_element<Indices, typename C::detector_tuple_t>::type::default_cut_value, 0)...};
        return result;
    }
    
public:
    void output() const { output_impl(std::make_index_sequence<C::detector_count>{}); }
private:
    template<std::size_t ... Indices>
    void output_impl( std::index_sequence<Indices...> ) const {
        std::cout << "\noutputing_current_cut_values: ";
        int expander[] = {0,  (output_element(std::get<Indices>(cut_mc)), 0)...};
        std::cout << '\n';
    }
    template<class T, typename std::enable_if_t< std::is_same<T, double>::value, std::nullptr_t> = nullptr>
    void output_element(T t_p) const { std::cout << t_p<< " "; }
    template<class T, typename std::enable_if_t< !std::is_same<T, double>::value, std::nullptr_t> = nullptr>
    void output_element(T t_pc) const {
        for( auto const& t : t_pc) { std::cout << t<< " " ; }
    }
private:
    tuple_t cut_mc{ retrieve_default( std::make_index_sequence<C::detector_count>{} ) };
//    tuple_t cut_mc;
//    tuple_t saved_cut_mc;
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
    constexpr void call() {
        puts(__PRETTY_FUNCTION__) ;
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};
        std::size_t correct_cluster_number{0};
        std::size_t recovered_cluster_number{0};\
        for(auto const & module : derived().result_m.module_c){
            reconstructed_number += module.reconstructed_number;
            reconstructible_number += module.reconstructible_number;
            correct_cluster_number += module.correct_cluster_number;
            recovered_cluster_number += module.recovered_cluster_number;
        }
        derived().baseline_m.efficiency = reconstructed_number * 1./reconstructible_number;
        derived().baseline_m.purity = correct_cluster_number * 1./recovered_cluster_number;
        
        std::cout << "new_baseline_efficiency: " << derived().baseline_m.efficiency << '\n';
        std::cout << "new_baseline_purity: " << derived().baseline_m.purity << '\n';
        std::cout << "reconstructed_number: " << reconstructed_number << '\n';
    }
    
private:
    constexpr Derived& derived(){ return static_cast<Derived&>(*this); }
};

template< class Derived >
struct global_scan_procedure{
    struct selection{
        std::size_t cut_index;
        int modifier;
        double score;
        double efficiency;
        double purity;
    };
    
    constexpr void call() {
        puts(__PRETTY_FUNCTION__) ;
        
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};
        std::size_t correct_cluster_number{0};
        std::size_t recovered_cluster_number{0};\
        for(auto const & module : derived().result_m.module_c){
            reconstructed_number += module.reconstructed_number;
            reconstructible_number += module.reconstructible_number;
            correct_cluster_number += module.correct_cluster_number;
            recovered_cluster_number += module.recovered_cluster_number;
        }
        
        auto efficiency = reconstructed_number * 1./reconstructible_number;
        auto purity = correct_cluster_number * 1./recovered_cluster_number;
        
        auto score = ( efficiency - derived().baseline_m.efficiency )/derived().baseline_m.efficiency +
                     ( purity - derived().baseline_m.purity )/derived().baseline_m.purity;
                                                               
        std::cout << "efficiency: " << efficiency << " - " << ( efficiency - derived().baseline_m.efficiency )/derived().baseline_m.efficiency << '\n';
        std::cout << "purity: " << purity << " - " << ( purity - derived().baseline_m.purity )/derived().baseline_m.purity << '\n';
        
        selection_c.push_back( selection{
                                    derived().selected_cut_m,
                                    derived().modifier_m,
                                    score,
                                    efficiency,
                                    purity
                                         } );
    }
    
    constexpr bool select_cut() {
        std::sort( selection_c.begin(), selection_c.end(), [](auto const& s1_p, auto const& s2_p){ return s1_p.score > s2_p.score; } );
        
        std::cout << "sorted_selection:\n";
        for(auto const& selection : selection_c ){
            std::cout << "[" << selection.cut_index << " : " << selection.modifier << "] -> " << selection.score << '\n';
        }
        
        auto const& winner = selection_c.front();
        if( winner.score > 0 ){
            derived().selected_cut_m = winner.cut_index;
            derived().modifier_m = winner.modifier;
            
            derived().selected_m.efficiency = derived().baseline_m.efficiency; //from global to rough, baseline is zero-value
            derived().selected_m.purity = derived().baseline_m.purity;
            
            selection_c.clear();
            return true;
        }
        return false;
    }
    
private:
    constexpr Derived& derived(){ return static_cast<Derived&>(*this); }
private:
    std::vector<selection> selection_c;
};

template< class Derived >
struct local_scan_procedure{
    struct selection{
        int offset;
        double score;
        double efficiency;
        double purity;
    };
    
    constexpr void setup() {
        
        auto score = ( derived().selected_m.efficiency - derived().target_m.efficiency )/derived().target_m.efficiency +
                     ( derived().selected_m.purity - derived().target_m.purity )/derived().target_m.purity;
        std::cout << "score: " << score << " - ";
        std::cout << derived().selected_m.efficiency << " : " << derived().selected_m.purity << '\n';
        
        selection_c.push_back( selection{
            0,
            score,
            derived().selected_m.efficiency,
            derived().selected_m.purity
        } );
    }
    
    constexpr void call() {
        puts(__PRETTY_FUNCTION__) ;
        
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};
        std::size_t correct_cluster_number{0};
        std::size_t recovered_cluster_number{0};\
        for(auto const & module : derived().result_m.module_c){
            reconstructed_number += module.reconstructed_number;
            reconstructible_number += module.reconstructible_number;
            correct_cluster_number += module.correct_cluster_number;
            recovered_cluster_number += module.recovered_cluster_number;
        }
        auto efficiency = reconstructed_number * 1./reconstructible_number;
        auto purity = correct_cluster_number * 1./recovered_cluster_number;
        
        auto score = ( efficiency - derived().target_m.efficiency )/derived().target_m.efficiency +
                     ( purity - derived().target_m.purity )/derived().target_m.purity;
        
        std::cout << "efficiency: " << efficiency << " - " << ( efficiency - derived().target_m.efficiency )/derived().target_m.efficiency << '\n';
        std::cout << "purity: " << purity << " - " << ( purity - derived().target_m.purity )/derived().target_m.purity << '\n';
        
        selection_c.push_back( selection{
            derived().offset_m,
            score,
            efficiency,
            purity
        } );
    }
    
    constexpr void optimize_cut() {
        std::sort( selection_c.begin(), selection_c.end(), [](auto const& s1_p, auto const& s2_p){ return s1_p.score > s2_p.score; } );
        
        std::cout << "sorted_selection: "<< derived().selected_cut_m <<"\n";
        for(auto const& selection : selection_c ){
            std::cout << "[" << selection.offset << "] -> " << selection.score << '\n';
        }
        
        auto const& winner = selection_c.front();
        *derived().cut_mc.get_cut_handle( derived().selected_cut_m  ) += winner.offset;
        
        derived().selected_m.efficiency = winner.efficiency;
        derived().selected_m.purity = winner.purity;
        
        selection_c.clear();
    }
    
private:
    constexpr Derived& derived(){ return static_cast<Derived&>(*this); }
private:
    std::vector<selection> selection_c;
};

template< class Derived >
struct stop_procedure {
    constexpr void call() {}
};


struct TATOEbaseCutter : TAGaction {
    TATOEbaseCutter(char const* name_p) :
    TAGaction(name_p, "TATOEcutter - Tool used to determine optimal cut values according to geometry for use in TOE") {}
    virtual Bool_t Action() = 0;
    virtual ~TATOEbaseCutter() = default;
};


template<class C>
struct TATOEcutter : TATOEbaseCutter,
    baseline_procedure< TATOEcutter<C> >,
    global_scan_procedure< TATOEcutter<C> >,
    local_scan_procedure< TATOEcutter<C> >,
    stop_procedure< TATOEcutter<C> >
{
    using baseline_procedure< TATOEcutter >::call;
    friend class baseline_procedure< TATOEcutter >;
    using global_scan_procedure< TATOEcutter >::call;
    using global_scan_procedure< TATOEcutter >::select_cut;
    friend class global_scan_procedure< TATOEcutter >;
    using local_scan_procedure< TATOEcutter >::call;
    using local_scan_procedure< TATOEcutter >::setup;
    using local_scan_procedure< TATOEcutter >::optimize_cut;
    friend class local_scan_procedure< TATOEcutter >;
    using stop_procedure< TATOEcutter >::call;
    
    struct iterator{
        struct eraser{
            virtual ~eraser() = default;
            virtual bool can_increment() = 0;
            virtual void increment_and_setup() = 0;
            virtual void switch_procedure(iterator& i_p) = 0;
            virtual bool is_done() const  =0;
        };

        template<class T>
        struct holder : eraser {
            constexpr holder() = default;
            constexpr holder(T t) : t_m{ std::move(t)} {}
            virtual bool can_increment() override{ return t_m.can_increment(); }
            virtual void increment_and_setup()  override{ t_m.increment_and_setup(); }
            virtual void switch_procedure(iterator& i_p) override{ t_m.switch_procedure(i_p); }
            virtual bool is_done() const override { return t_m.is_done(); }
            T t_m;
        };
        
        constexpr iterator( iterator&& ) = default;
        template<class T>
        constexpr iterator(TATOEcutter * c_h, T t) : c_mh{c_h}, erased_mh{ new holder<T>{ std::move(t) } } {}
        template<class T>
        constexpr iterator& operator=(T t){
            erased_mh.reset( new holder<T>{ std::move(t) } );
            return *this;
        }
        
        struct empty_result{};
        constexpr empty_result operator*(){
            if( !erased_mh->can_increment() ) { erased_mh->switch_procedure(*this); }
            erased_mh->increment_and_setup();
            c_mh->get_cut_holder().output();
            c_mh->apply_cuts();
            return {};
        }
        friend bool operator!=(const iterator& lhs, const iterator& /*rhs*/){ return !lhs.erased_mh->is_done(); }
        constexpr iterator& operator++(){
            c_mh->retrieve_results();
            c_mh->call();
            std::cout << "remaining_cuts: ";
            for(auto const& remaining_cut : c_mh->remaining_cut_mc){ std::cout << remaining_cut << " "; }
            std::cout << "\nselected_cut: " << c_mh->selected_cut_m << "\n";
            return *this;
        }
        
        bool can_increment() { return erased_mh->can_increment(); }
        void increment_and_setup() { erased_mh->increment_and_setup(); }
        void switch_procedure(iterator& i_p) { erased_mh->switch_procedure(i_p); }
        
    private:
        TATOEcutter * c_mh;
        std::unique_ptr<eraser> erased_mh;
    };
    
    struct stop_iterator {
        bool can_increment(){ return false; }
        void increment_and_setup() { }
        void switch_procedure(iterator& ){ }
        
        bool is_done() const { return true; }
    };
    struct baseline_iterator {
        baseline_iterator(TATOEcutter* c_ph) : c_mh{c_ph} {}
        bool can_increment() { return !was_called_m; }
        void increment_and_setup() { was_called_m = true; }
        void switch_procedure( iterator& i_p ) {
            c_mh->call_mhf = &global_scan_procedure<TATOEcutter>::call;
            c_mh->modifier_m = -3;
            i_p = global_scan_iterator{c_mh};
        }
        bool is_done() const { return false; }
    private:
        bool was_called_m{false};
        TATOEcutter* c_mh;
    };
    
    struct global_scan_iterator {
        global_scan_iterator(TATOEcutter* c_ph) : c_mh{c_ph} { c_mh->selected_cut_m = *iterator_m; }
        bool can_increment() {
            std::cout << "negative_modifier: " << std::boolalpha << (c_mh->modifier_m < 0) << '\n';
            std::cout << "no_remaining_cuts: " << std::boolalpha << ((++iterator_m)-- != c_mh->remaining_cut_mc.end()) << '\n';
            return c_mh->modifier_m < 0 || (++iterator_m)-- != c_mh->remaining_cut_mc.end() ;
        }
        void increment_and_setup() {
            if( c_mh->modifier_m > 0 ){ c_mh->modifier_m = -1; c_mh->selected_cut_m = *(++iterator_m); }
            else { c_mh->modifier_m += 2;  }
            c_mh->cut_mc.reset_cuts();
            *c_mh->cut_mc.get_cut_handle( *iterator_m ) += c_mh->modifier_m;
        }
        void switch_procedure(iterator& i_p) {
            c_mh->cut_mc.reset_cuts();
            if( c_mh->select_cut() ) {
                c_mh->call_mhf = &local_scan_procedure<TATOEcutter>::call;
                c_mh->setup();
                i_p = rough_scan_iterator{c_mh};
            }
            else {
                c_mh->call_mhf = &stop_procedure<TATOEcutter>::call;
                i_p = stop_iterator{};
            }
        }
        bool is_done() const { return false; }
    private:
        TATOEcutter* c_mh;
        std::vector<std::size_t>::iterator iterator_m{ c_mh->remaining_cut_mc.begin() };
    };
    
    struct rough_scan_iterator {
        rough_scan_iterator(TATOEcutter* c_ph) : c_mh{c_ph} {}
        bool can_increment() { return counter_m < details::underlying_parameters<C>::type::count ; }
        void increment_and_setup() {
            c_mh->cut_mc.reset_cuts();
            c_mh->offset_m = ++counter_m * c_mh->modifier_m * details::underlying_parameters<C>::type::step_size;
            *c_mh->cut_mc.get_cut_handle( c_mh->selected_cut_m ) += c_mh->offset_m;
        }
        void switch_procedure(iterator& i_p) {
            c_mh->cut_mc.reset_cuts();
            c_mh->optimize_cut();
            c_mh->cut_mc.save_cuts();

            c_mh->setup();
            c_mh->call_mhf = &local_scan_procedure<TATOEcutter>::call;
            i_p = fine_scan_iterator{c_mh};
        }
        bool is_done() const { return false; }
    private:
        TATOEcutter* c_mh;
        int counter_m{0};
    };
    
    struct fine_scan_iterator {
        fine_scan_iterator(TATOEcutter* c_ph) : c_mh{c_ph} {}
        bool can_increment() { return counter_m < static_cast<int>(details::underlying_parameters<C>::type::step_size - 1 ); }
        void increment_and_setup() {
            c_mh->cut_mc.reset_cuts();
            ++counter_m != 0 ? 0 : ++counter_m;
            c_mh->offset_m = counter_m;
            *c_mh->cut_mc.get_cut_handle( c_mh->selected_cut_m ) += c_mh->offset_m;
        }
        void switch_procedure(iterator& i_p) {
            c_mh->cut_mc.reset_cuts();
            c_mh->optimize_cut();
            c_mh->cut_mc.save_cuts();
            
            c_mh->remaining_cut_mc.erase( std::remove_if(
                                                c_mh->remaining_cut_mc.begin(),
                                                c_mh->remaining_cut_mc.end(),
                                                [this](auto const& value_p){ return value_p == c_mh->selected_cut_m; }
                                                         ) );
            c_mh->call_mhf = &baseline_procedure<TATOEcutter>::call;
            i_p = baseline_iterator{c_mh};
        }
        bool is_done() const { return false; }
    private:
        TATOEcutter* c_mh;
        int counter_m{ - static_cast<int>(details::underlying_parameters<C>::type::step_size) };
    };
    
    friend class TATOEcutter::iterator;
    friend class TATOEcutter::baseline_iterator;
    friend class TATOEcutter::global_scan_iterator;
    friend class TATOEcutter::rough_scan_iterator;
    friend class TATOEcutter::fine_scan_iterator;
    
    struct targeted_values {
        double efficiency;
        double purity;
    };
    
    TATOEcutter(char const* name_p) :
        TATOEbaseCutter(name_p),
        action_mh{ details::new_action<C>() },
    call_mhf{&baseline_procedure<TATOEcutter>::call} { cut_mc.output(); }
    
    Bool_t Action() override {
        action_mh->Action();
        return true;
    }
    
    iterator begin(){ return iterator{ this, baseline_iterator{this} };}
    iterator end(){ return iterator{ this, stop_iterator{} }; }
    
    details::cut_holder<C>& get_cut_holder() {return cut_mc;}
    void apply_cuts(){ cut_mc.apply( action_mh.get() ); }
    void retrieve_results() { result_m = action_mh->retrieve_results(); }
    
private:
    void call() { (this->*call_mhf)(); }
    
    std::vector<std::size_t> generate_remaining_cut() {
        std::vector<std::size_t> result_c;
        for( std::size_t i{0}; i < C::cut_count ; ++i ){ result_c.push_back(i); }
        return result_c;
    }
    
private:
    std::unique_ptr<TATOEbaseAct> action_mh;
    
    void (TATOEcutter::* call_mhf)();
    
    details::cut_holder<C> cut_mc{};
    std::vector<std::size_t> remaining_cut_mc{ generate_remaining_cut() };
    std::size_t selected_cut_m;
    int modifier_m{-3};
    int offset_m;

    reconstruction_result result_m;
    targeted_values baseline_m;
    targeted_values target_m{0.92, 0.97};
    targeted_values selected_m;
};



#endif
