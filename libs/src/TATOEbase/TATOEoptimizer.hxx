
//
//File      : TATOEoptimizer.hxx
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 03/04/2021
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//


#ifndef _TATOEoptimizer_HXX
#define _TATOEoptimizer_HXX

#include "detector_list.hpp"
#include "grkn_data.hpp"
#include "stepper.hpp"
#include "ukf.hpp"

#include "TATOEutilities.hxx"
#include "TATOEprocedure.hxx"
#include "TATOEact.hxx"

#include "TAGaction.hxx"
#include "TAGparGeo.hxx"
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
 
template<class ... Ts>
struct configuration {
    using detector_tuple_t = std::tuple<Ts...>;
    static constexpr std::size_t detector_count = sizeof...(Ts);
    
    static constexpr std::size_t cut_count = cut_counter<typename Ts::cut_t...>{}();
    using cut_tuple_t = std::tuple< typename Ts::cut_t... >;
    
   // static constexpr std::size_t iteration_count = cut_count * (range+1) + scan_iteration<cut_count>() + 1 ;
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
template< >
struct action_factory<configuration< vertex_tag, tof_tag>> {
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
    
    
template<>
struct action_factory<configuration< vertex_tag, it_tag, tof_tag>> {
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

template< >
struct action_factory<configuration<vertex_tag, it_tag, msd_tag, tof_tag>> {
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

struct TATOEbaseOptimizer : TAGaction {
    TATOEbaseOptimizer(char const* name_p) :
    TAGaction(name_p, "TATOEoptimizer - Tool used to determine optimal cut values according to geometry for use in TOE") {}
    virtual Bool_t Action() = 0;
    virtual ~TATOEbaseOptimizer() = default;
};

template<class O> struct underlying_configuration{};

template<class C, class ... Ps>
struct TATOEoptimizer : TATOEbaseOptimizer{
    struct targeted_values{
        double efficiency;
        double purity;
    };
    
    struct procedure_eraser{
        virtual ~procedure_eraser() = default;
        virtual void call(TATOEoptimizer* o_ph) = 0;
        virtual bool is_done(TATOEoptimizer const * o_ph) const = 0;
        virtual void setup_next_iteration(TATOEoptimizer* o_ph) = 0;
        virtual void setup_cuts(TATOEoptimizer* o_ph) = 0;
        virtual void finalise(TATOEoptimizer* o_ph) =0;
        virtual void setup(TATOEoptimizer* o_ph) =0;
    };

    template<class T>
    struct holder : procedure_eraser {
        holder(T t_p): t_m{ std::move(t_p) } {}
        holder() = default;
        void call(TATOEoptimizer* o_ph) override { t_m.call(o_ph); }
        bool is_done(TATOEoptimizer const * o_ph) const override { return t_m.is_done(o_ph); }
        void setup_next_iteration(TATOEoptimizer* o_ph) override { t_m.setup_next_iteration(o_ph); }
        void setup_cuts(TATOEoptimizer* o_ph) override { t_m.setup_cuts(o_ph); }
        virtual void finalise(TATOEoptimizer* o_ph) override { t_m.finalise(o_ph); }
        virtual void setup(TATOEoptimizer* o_ph) override { t_m.setup(o_ph); }
        T t_m;
    };
    
    
    TATOEoptimizer(char const* name_p):
        TATOEbaseOptimizer(name_p),
        action_mh{ details::new_action<C>() },
        erased_procedure_mh{ new holder< baseline_procedure > {}}
    {}
        
    void call() { erased_procedure_mh->call(this); }
    bool& is_optimization_done() { return is_optimization_done_m; }
    bool is_procedure_done() const { return erased_procedure_mh->is_done( this ); }
    void finalise_procedure() { erased_procedure_mh->finalise(this); }
    void switch_procedure(){
        puts(__PRETTY_FUNCTION__);
        if( current_procedure_m > sizeof...(Ps)-1 ){ current_procedure_m = 0; }
        switch_procedure_impl( std::make_index_sequence<sizeof...(Ps)>{} );
        ++current_procedure_m;
    }
    void setup_procedure(){ erased_procedure_mh->setup(this); }
    void setup_next_iteration(){ erased_procedure_mh->setup_next_iteration(this); }
    void setup_cuts(){ erased_procedure_mh->setup_cuts(this); }

    
    Bool_t Action() override {
        action_mh->Action();
        return true;
    }
    
    void apply_cuts(){ cut_mc.apply( action_mh.get() ); }
    void save_cuts() { cut_mc.save_cuts(); }
    void output_cuts() const { cut_mc.output(); }
    void reset_cuts() { cut_mc.reset_cuts(); }
    void set_selected_cut(){ *cut_mc.get_cut_handle( cut_index ) += offset * sign; }
    
    std::vector<std::size_t> generate_available_cuts() {
        std::vector<std::size_t> result_c;
        for( std::size_t i{0}; i < C::cut_count ; ++i ){ result_c.push_back(i); }
        return result_c;
    }
    
    auto const& retrieve_results() { return action_mh->retrieve_matched_results(); }
    
private:
    template<std::size_t ... Indices>
    void switch_procedure_impl(std::index_sequence<Indices...>) {
        int expander[] = {0, ( (current_procedure_m != Indices) ? void() : switch_procedure_to<Indices>() , 0)...};
    }
    template<std::size_t Index>
    void switch_procedure_to() {
        erased_procedure_mh.reset( new holder< typename details::find_among<Index, Ps...> >{} );
    }

private:
    std::unique_ptr<TATOEbaseAct> action_mh;
    std::unique_ptr<procedure_eraser> erased_procedure_mh;
    details::cut_holder<C> cut_mc{};
    
public:
    int sign{0};
    std::size_t cut_index{0};
    int offset{1};
    targeted_values baseline;
    targeted_values last_winner;
    std::vector<std::size_t>& get_available_cuts() { return available_cut_mc; }
    std::vector<std::size_t> const & get_available_cuts() const { return available_cut_mc; }
private:
    std::vector<std::size_t> available_cut_mc{ generate_available_cuts() };
    std::size_t current_procedure_m{0};
    bool is_optimization_done_m{false};
};

template<class C, class ... Ps>
struct underlying_configuration< TATOEoptimizer<C, Ps...> >{
    using type = C;
};

template<class C, class ... Ps>
auto new_optimizer(char const* name_p ) {
    return new TATOEoptimizer<C, Ps...>( name_p );
}



#endif
