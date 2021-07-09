//
// Plot tot residual + Fit for FIRST data
//
/** TATOEchecker class
 
 \author A. Sécher
 */

//
//File      : TATOEchecker.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 10/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//


#ifndef _TATOEchecker_HXX
#define _TATOEchecker_HXX

#include "TH2.h"

#include "TAMCntuPart.hxx"
#include "TAGparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGcluster.hxx"
#include "TATOEutilities.hxx"


class TATWpoint;

    
namespace aftereffect {
    template<class T>
    struct optional{
        T t_m;
        T* t_mh;
        
        template< class T_ = T,
        typename std::enable_if_t< std::is_default_constructible<T_>::value, std::nullptr_t > = nullptr >
        explicit optional( T t_p  ) : t_m{ std::move(t_p) }, t_mh{ &t_m } {}
        
        template< class T_ = T,
        typename std::enable_if_t< std::is_default_constructible<T_>::value, std::nullptr_t > = nullptr >
        optional() : t_m{}, t_mh{nullptr} {}
        
        optional(optional const& rhs_p){
            if(rhs_p.has_value()){
                t_m = rhs_p.t_m;
                t_mh = &t_m;
            }
            else{
                t_m = T{};
                t_mh = nullptr;
            }
        }
        optional(optional&& rhs_p){
            if(rhs_p.has_value()){
                t_m = std::move(rhs_p.t_m);
                rhs_p.t_mh = nullptr;
                t_mh = &t_m;
            }
            else{
                t_m = T{};
                t_mh = nullptr;
            }
        }
        optional& operator=(optional const& rhs_p){
            if(rhs_p.has_value()){
                t_m = rhs_p.t_m;
                t_mh = &t_m;
            }
            else{
                t_m = T{};
                t_mh = nullptr;
            }
            return *this;
        }
        optional& operator=(optional&& rhs_p){
            if(rhs_p.has_value()){
                t_m = std::move( rhs_p.t_m );
                rhs_p.t_mh = nullptr;
                t_mh = &t_m;
            }
            else{
                t_m = T{};
                t_mh = nullptr;
            }
            return *this;
        }
        
        T const& value() const { return t_m; }
        T& value() { return t_m; }
        
        bool has_value() const { return t_mh != nullptr; }
    };

    
    
    template<class ... Ts>
    struct pack{};
    
    
    template<class F> struct callable_signature : callable_signature<decltype(&F::operator())> {};
    
    template<class T, class TReturn, class ...TArgs>
    struct callable_signature<TReturn(T::*)(TArgs...) const> {
        using return_type = TReturn;
        using argument_list = pack<TArgs...>;
    };
    
    
    template<class P, class O>
    struct aftereffect {
        P const predicate;
        O const outcome;
        
        using return_type = typename callable_signature<O>::return_type;
        
        template< class P_ = P,
                  class ArgumentCheck = std::enable_if_t< std::is_same< typename callable_signature<P_>::argument_list,
                                                                        typename callable_signature<O>::argument_list >::value  > >
        constexpr aftereffect(P predicate_p, O outcome_p) : predicate{ std::move(predicate_p) }, outcome{std::move(outcome_p)} {}
        
        template< class ... TArgs,
                  class O_ = O,
                  typename std::enable_if_t< !std::is_same< typename callable_signature<O_>::return_type, void>::value , std::nullptr_t> = nullptr >
        constexpr optional<return_type> operator()(TArgs&& ... args) const {
            if( predicate( std::forward<TArgs>(args)... ) ){ return optional<return_type>{ outcome( std::forward<TArgs>(args)... ) }; }
            return optional<return_type>{};
        }
        
        template< class ... TArgs,
                  class O_ = O,
                  typename std::enable_if_t< std::is_same< typename callable_signature<O_>::return_type, void>::value , std::nullptr_t> = nullptr >
        constexpr void operator()(TArgs&& ... args) const {
            if( predicate( std::forward<TArgs>(args)... ) ){ outcome( std::forward<TArgs>(args)... ); }
        }
        
    };
    
    
    template<class P, class O>
    constexpr auto make_aftereffect(P predicate_p, O outcome_p) -> aftereffect<P, O> {
        return {std::move(predicate_p), std::move(outcome_p)};
    }
    
    
    template<class Value, class ... As> struct first_of_impl{};
    
    template<class Value, class A, class ... As>
    struct first_of_impl<Value, A , As...>{
        static constexpr auto apply( Value const& value, A const& a, As const& ... as ) {
            auto current_result = a(value);
            if( current_result.has_value() ){  return current_result; }
            else{ return first_of_impl<Value, As...>::apply( value, as...  ); }
        }
    };
    
    template<class Value, class A>
    struct first_of_impl<Value, A >{
        static constexpr auto apply( Value const& value, A const& a ) { return a(value); }
    };

    template<class Value, class ... As>
    constexpr auto first_of( Value const& value, As const& ... as  ) {
        return first_of_impl<Value, As...>::apply( value, as... );
    }
    
    //doesn't work for empty containers
    template<class Iterator, class ... As>
    constexpr auto first_of( Iterator first, Iterator last, As const& ... as  ) {
        for (; first != last-1; ++first) {
            auto result_o = first_of( *first, as... );
            if( result_o.has_value() ){ return result_o; }
        }
        return first_of( *first, as...  );
    }
    
    template<class Value, class ... Fs> struct apply_impl{};
    template<class Value, class F, class ... Fs>
    struct apply_impl< Value, F, Fs...>{
        static constexpr void apply( Value const& value_p, F const f_p, Fs const& ...fs_p ){
            f_p(value_p);
            apply_impl<Value, Fs...>::apply(value_p, fs_p...);
        }
    };
    
    template<class Value, class F>
    struct apply_impl< Value, F>{
        static constexpr void apply( Value const& value_p, F const f_p ){ f_p(value_p); }
    };
    
    template<class Value>
    struct apply_impl< Value>{
        static constexpr void apply( Value const&  ){ ; }
    };

    template<class Value, class ... Fs>
    constexpr void apply( Value const& value_p, Fs const&  ... fs ) {
        apply_impl<Value, Fs...>::apply(value_p, fs...);
    }
    
    
    
    template<class ...As>
    struct aftereffect_list{
        using tuple_type = typename std::tuple<As...>;
        
    private:
        tuple_type aftereffect_mc;
        
    public:
        aftereffect_list( As ... as_p ) : aftereffect_mc{ std::move(as_p)... } {}
        
        template< class Value >
        void evaluate( Value const& value_p) {
            evaluate_impl( value_p, std::make_index_sequence< sizeof...(As) >{} );
        }
        
    private:
        template<class Value, std::size_t ...Indices>
        void evaluate_impl( Value const& value_p, std::index_sequence<Indices...> ) {
            apply( value_p, std::get< Indices >(aftereffect_mc)...);
        }
        
    };
    
    template<class ...As>
    constexpr auto make_aftereffect_list( As&& ... as_p ) ->aftereffect_list<As...> {
        return {std::forward<As>(as_p)...};
    }
    
} // namespace aftereffect




//requires orthogonal aftereffects, and non-void return type from outcome



template<class Action>
struct TATOEchecker{
    
    using candidate = typename std::decay_t<decltype( std::declval<typename Action::detector_list_t>().last() )>::candidate;
    using detector_list_t = typename Action::detector_list_t;
    using node_type = typename Action::node_type;
    using data_type = typename Action::data_type;

    struct particle_type{
        double momentum;
        double track_slope_x{0};
        double track_slope_y{0};
        int charge;
        int mass;
    };
    
    
    struct reconstructible_track{
        std::vector<int> const & get_indices() const { return index_c; }
        std::vector<int>& get_indices() { return index_c; }
        std::vector<int> index_c;
        particle_type properties;
    };
    
    struct reconstructed_track{
        std::vector<data_type const*> cluster_c;
        std::vector<data_type const*> const& get_clusters() const { return cluster_c; }
        std::vector<data_type const*> & get_clusters() { return cluster_c; }
        particle_type properties;
        std::size_t clone_number{0};
    };
    
    struct reconstruction_module{
        data_type const* end_point_h;
        aftereffect::optional<reconstructible_track> reconstructible_o;
        aftereffect::optional<reconstructed_track> reconstructed_o;
        
        reconstruction_module( data_type const* end_point_ph,
                              aftereffect::optional<reconstructible_track> reconstructible_po,
                              aftereffect::optional<reconstructed_track> reconstructed_po) :
        end_point_h{end_point_ph},
        reconstructible_o{ std::move(reconstructible_po) },
        reconstructed_o{ std::move(reconstructed_po) } {}
    };
    
    struct computation_module{
        
        struct bundle {
            std::unique_ptr<TH1D> reconstructible_h{nullptr};
            std::unique_ptr<TH1D> reconstructed_h{nullptr};
            std::unique_ptr<TH1D> efficiency_h{nullptr};
            std::unique_ptr<TH2D> mass_identification_h{nullptr};
            //TH2D* charge_identification_h{nullptr};
            std::map<int, std::unique_ptr<TH1D>> momentum_resolution_c;
            std::unique_ptr<TH1D> momentum_difference_h{nullptr};
        } histogram_bundle;
        
        int charge;
        
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};
        std::size_t correct_cluster_number{0};
        std::size_t recovered_cluster_number{0};
        std::size_t total_cluster_number{0};
        std::size_t clone_number{0};
        std::size_t fake_number{0};
        
        computation_module(int charge_p) : charge{charge_p}
        {
            histogram_bundle.reconstructible_h.reset( new TH1D{ Form("reconstructible_charge%d", charge),
                                                                      ";Momentum(GeV/c);Count", 100, 0, 1.3 } );
            histogram_bundle.reconstructible_h->SetDirectory(nullptr);
            histogram_bundle.efficiency_h.reset( new TH1D{ Form("efficiency_charge%d", charge),
                ";Momentum(GeV/c);Count", 100, 0, 1.3 } );
            histogram_bundle.efficiency_h->SetDirectory(nullptr);
            histogram_bundle.reconstructed_h.reset( new TH1D{ Form("reconstructed_charge%d", charge),
                                                                    ";Momentum(GeV/c);Count", 100, 0, 1.3 } );
            histogram_bundle.reconstructed_h->SetDirectory(nullptr);
            histogram_bundle.mass_identification_h.reset( new TH2D{ Form("mass_identification_charge%d", charge),
                ";A_{reconstructed};A_{real};Count", 20, 0, 20, 20, 0, 20 } );
            histogram_bundle.mass_identification_h->SetDirectory(nullptr);
//            histogram_bundle.charge_identification_h = new TH2D{ Form("charge_identification_charge%d", charge),
//                ";Z_{reconstructed};Z_{real};Count", 20, 0, 20, 20, 0, 20 };
            histogram_bundle.momentum_difference_h.reset( new TH1D{ Form("momentum_difference_charge%d", charge),
                Form(";p_{mc} - p_{rec}(GeV/c);Count"), 500, -5, 5 } );
            histogram_bundle.momentum_difference_h->SetDirectory(nullptr);
        }
                                                                               
        
        TH1D const * get_reconstructed_histogram() const { return histogram_bundle.reconstructed_h.get(); }
        TH1D * get_reconstructed_histogram() { return histogram_bundle.reconstructed_h.get(); }
        
        TH1D const * get_reconstructible_histogram() const { return histogram_bundle.reconstructible_h.get(); }
        TH1D * get_reconstructible_histogram() { return histogram_bundle.reconstructible_h.get(); }
        
        TH1D const * get_efficiency_histogram() const { return histogram_bundle.efficiency_h.get(); }
        TH1D * get_efficiency_histogram() { return histogram_bundle.efficiency_h.get(); }
        
        TH1D const * get_momentum_difference_histogram() const { return histogram_bundle.momentum_difference_h.get(); }
        TH1D * get_momentum_difference_histogram() { return histogram_bundle.momentum_difference_h.get(); }
        
        TH2D const * get_mass_identification_histogram() const { return histogram_bundle.mass_identification_h.get(); }
        TH2D * get_mass_identification_histogram() { return histogram_bundle.mass_identification_h.get(); }
//
//        TH2D const * get_charge_identification_histogram() const { return histogram_bundle.charge_identification_h; }
//        TH2D * get_charge_identification_histogram() { return histogram_bundle.charge_identification_h; }
        
        TH1D const * get_momentum_histogram( double momentum_p ) const {
            auto key = static_cast<int>( momentum_p/0.2 );
          //  std::cout << "get_momentum_histogram_const\n";
       //     std::cout << "momentum: " << momentum_p << '\n';/
        //    std::cout << "key: " << key << ']\n';
            auto histogram_i = histogram_bundle.momentum_resolution_c.find(key);
            return (histogram_bundle.momentum_resolution_c.find(key) != histogram_bundle.momentum_resolution_c.end() ) ?
                    histogram_i->get() : nullptr;
        }
        TH1D * get_momentum_histogram( double momentum_p ) {
            auto key = static_cast<int>( momentum_p/0.2 );
         //   std::cout << "get_momentum_histogram\n";
         //   std::cout << "momentum: " << momentum_p << '\n';
         //   std::cout << "key: " << key << '\n';
            auto histogram_i = histogram_bundle.momentum_resolution_c.find(key);
            if(histogram_bundle.momentum_resolution_c.find(key) != histogram_bundle.momentum_resolution_c.end() ) {
                return histogram_i->second.get();
            }
            histogram_bundle.momentum_resolution_c[key].reset( new TH1D{ Form("momentum_resolution_%d_charge%d", key, charge),
                Form("momentum_resolution_%.1fMeV/c_charge%d;p(GeV/c);p_{rec}(GeV/c)", (key+0.5)*0.2, charge), 300, 0, 15 } );
            histogram_bundle.momentum_resolution_c[key]->SetDirectory(nullptr);
            return histogram_bundle.momentum_resolution_c[key].get();
            
        }
        
    };
    
    struct candidate_indices {
        std::vector< int > index_c;
        std::vector<int> const & get_indices() const { return index_c; }
        std::vector<int>& get_indices() { return index_c; }
        void insert( int index_p ) {
            if( std::none_of(
                     index_c.begin(), index_c.end(),
                     [&index_p]( int current_index_p ){ return index_p == current_index_p; }
                            ) ){ index_c.push_back( index_p ); }
        }
    };
    
    
    
private:
    TAMCntuPart* data_mhc;
    std::pair<double, double> target_limits_m;
    Action& action_m;
    node_type const * current_node_mh = nullptr;
    
    TH1D* real_momentum_distribution_h{nullptr};
    TH1D* reconstructed_momentum_distribution_h{nullptr};

    std::vector< candidate_indices > candidate_index_mc;
    std::vector< reconstruction_module > reconstruction_module_mc;
    std::vector< computation_module > computation_module_mc;

    
public:
    TATOEchecker( TAGparGeo const * global_parameters_ph,
                  Action& action_p ) :
    data_mhc{ static_cast<TAMCntuPart*>( gTAGroot->FindDataDsc( TAMCntuPart::GetDefDataName() )->Object() ) },
        target_limits_m{ retrieve_target_limits( global_parameters_ph ) },
        action_m{action_p}
    {
        reconstruction_module_mc.reserve(20);
        computation_module_mc.reserve(20);
        real_momentum_distribution_h = new TH1D{ "real_momentum" ,
            ";Momentum(GeV/c);Count", 1000, 0, 9};
        reconstructed_momentum_distribution_h = new TH1D{ "reconstructed_momentum",
            ";Momentum(GeV/c);Count", 1000, 0, 9 };
    }
    
private:
    std::pair<double, double> retrieve_target_limits( TAGparGeo const * global_parameters_ph ) const
    {
        auto position_z = global_parameters_ph->GetTargetPar().Position.Z();
        auto size = global_parameters_ph->GetTargetPar().Size.Z();
        
        return { position_z - size /2 , position_z + size /2  };
    }

    
public:
    
    void update_current_node( node_type const * current_node_ph ){ current_node_mh = current_node_ph; }
    
    void start_event() {
//        std::cout << "====start_event====\n";
        
        candidate_index_mc.clear();
        reconstruction_module_mc.clear();
        
        action_m.list_m.apply_for_each(
                                       [this](auto const & detector_p)
                                       {
                                           candidate_index_mc.push_back( candidate_indices{} );
                                           for( std::size_t i{0} ; i < detector_p.layer_count() ; ++i  ){
                                            
                                               auto candidate_c = detector_p.generate_candidates( i );
                                               for( auto const& candidate : candidate_c ) {
                                                   
                                                   for( int i{0}; i < candidate.data->GetMcTracksN() ; ++i  ){
                                                       auto id = candidate.data->GetMcTrackIdx(i);
                                                       candidate_index_mc.back().insert( id );
                                                   }
                                                   
                                               }
                                           }
                                       }
                                       );
        
//        std::cout << "generate_candidate:\n";
//        for( auto const& layer : candidate_index_mc ){
//            for( auto const& index : layer.get_indices() ){ std::cout << index << " "; }
//            std::cout << "\n";
//        }
    }
    
    void end_event(){
//        std::cout << "====end_event====\n";
        
        auto undiscerning_output =  aftereffect::make_aftereffect(
                    [](reconstruction_module const& /*module_p*/)
                    { return true; },
                    [this](reconstruction_module const& module_p)
                    {
                        action_m.logger_m.template add_sub_header< details::immutable_tag >("reconstruction_output");
                        if( module_p.reconstructible_o.has_value() ){
                           action_m.logger_m << "reconstructible:\n";
                            auto& reconstructible = module_p.reconstructible_o.value();
                            action_m.logger_m << "charge: " << reconstructible.properties.charge << '\n';
                            action_m.logger_m << "mass: " << reconstructible.properties.mass << '\n';
                            action_m.logger_m << "momentum: " << reconstructible.properties.momentum << '\n';
                            action_m.logger_m << "mc_track_index: " ;
                            for( auto index : reconstructible.get_indices() ){
                                action_m.logger_m << index << " ";
                            }
                            action_m.logger_m << '\n';
                        }
                        if( module_p.reconstructed_o.has_value() ){
                            action_m.logger_m << "reconstructed:\n";
                            auto& reconstructed = module_p.reconstructed_o.value();
                            action_m.logger_m << "charge: " << reconstructed.properties.charge << '\n';
                            action_m.logger_m << "mass: " << reconstructed.properties.mass << '\n';
                            action_m.logger_m << "momentum: " << reconstructed.properties.momentum << '\n';
                            action_m.logger_m << "mc_track_index: " ;
                            for( auto& cluster_h : reconstructed.get_clusters() ){
                                action_m.logger_m << "(" ;
                                for( auto i{0} ; i < cluster_h->GetMcTracksN() ; ++i ) {
                                    action_m.logger_m << cluster_h->GetMcTrackIdx(i) << " ";
                                }
                                action_m.logger_m << ")";
                            }
                            action_m.logger_m << "\n";
                        }
                    }
                                                                  );
        auto successful_reconstruction = aftereffect::make_aftereffect(
                    [](reconstruction_module const& module_p)
                    { return module_p.reconstructible_o.has_value() && module_p.reconstructed_o.has_value() ; }, // &&
                        //module_p.reconstructible_o.value().properties.charge == module_p.reconstructed_o.value().properties.charge; }, // &&
//                       module_p.reconstructible_o.value().properties.mass == module_p.reconstructed_o.value().properties.mass; },
                    [this](reconstruction_module const& module_p)
                    {
                        auto reconstructed = module_p.reconstructed_o.value();
                        auto reconstructible = module_p.reconstructible_o.value();
                        auto & computation_module = find_computation_module( reconstructible.properties.charge );
                        computation_module.reconstructed_number++;
                        computation_module.get_efficiency_histogram()->Fill( reconstructible.properties.momentum/(1000 * reconstructible.properties.mass)  );
                        computation_module.get_reconstructed_histogram()->Fill( reconstructed.properties.momentum/(1000 * reconstructed.properties.mass)  );
                                    
                        computation_module.clone_number += reconstructed.clone_number;
                        
                        computation_module.recovered_cluster_number += reconstructed.get_clusters().size();
                        auto index_c = reconstructible.get_indices();
                        for( auto const& cluster_h : reconstructed.get_clusters() ) {
                            for( auto i{0}; i < cluster_h->GetMcTracksN() ; ++i ) {
                                auto cluster_index = cluster_h->GetMcTrackIdx(i);
                                if( std::any_of( index_c.begin(), index_c.end(),
                                                 [&cluster_index]( int index_p ){ return index_p == cluster_index; } ) ){
                                    computation_module.correct_cluster_number++;
                                    break;
                                }
                            }
                        }
                        
                        real_momentum_distribution_h->Fill(reconstructible.properties.momentum/1000);
                        reconstructed_momentum_distribution_h->Fill(reconstructed.properties.momentum/1000);
                        
                    }
                                                                       );
        auto fake_reconstruction = aftereffect::make_aftereffect(
                    [](reconstruction_module const& module_p)
                    { return !module_p.reconstructible_o.has_value() && module_p.reconstructed_o.has_value(); },
                    [this](reconstruction_module const& module_p )
                    {
                        action_m.logger_m.add_sub_header("fake_reconstruction");
                        auto & computation_module = find_computation_module( module_p.reconstructed_o.value().properties.charge );
                        computation_module.fake_number++;
                        //action_m.logger_m.freeze_everything();
                    }
                                                                 );
        auto reconstruction_failure = aftereffect::make_aftereffect(
                    [](reconstruction_module const& module_p)
                    { return module_p.reconstructible_o.has_value() && !module_p.reconstructed_o.has_value(); },
                    [this](reconstruction_module const& /* module_p*/)
                    {
                        action_m.logger_m.add_sub_header("reconstruction_failure");
                        action_m.logger_m.freeze_everything();
                    }
                                                                    );
        
        auto reconstructible_registration = aftereffect::make_aftereffect(
                           [](reconstruction_module const& module_p)
                           { return module_p.reconstructible_o.has_value(); },
                           [this](reconstruction_module const& module_p)
                           {
                               auto reconstructible = module_p.reconstructible_o.value();
                               auto & computation_module = find_computation_module( reconstructible.properties.charge );
                               computation_module.reconstructible_number++;
                               computation_module.get_reconstructible_histogram()->Fill( reconstructible.properties.momentum/(1000 * reconstructible.properties.mass)  );
                            }
                                                                          );
        
        auto momentum_resolution = aftereffect::make_aftereffect(
                           [](reconstruction_module const& module_p)
                           { return module_p.reconstructible_o.has_value() && module_p.reconstructed_o.has_value() ;},
//                               module_p.reconstructible_o.value().properties.charge == module_p.reconstructed_o.value().properties.charge;}, // &&
//                                    module_p.reconstructible_o.value().properties.mass == module_p.reconstructed_o.value().properties.mass; },
                           [this](reconstruction_module const& module_p)
                           {
                               auto reconstructible = module_p.reconstructible_o.value();
                               auto reconstructed = module_p.reconstructed_o.value();
                               auto & computation_module = find_computation_module( reconstructible.properties.charge );
                               
                          //     auto * momentum_h = computation_module.get_momentum_histogram( reconstructible.properties.momentum/1000 );
                               
                           //    std::cout << "momentum_resolution\n";
                           //    if( momentum_h ){ momentum_h->Fill( reconstructed.properties.momentum/1000 ); }
                               
//                               computation_module.get_momentum_difference_histogram()->Fill( (reconstructible.properties.momentum/reconstructible.properties.mass -  reconstructed.properties.momentum/reconstructed.properties.mass)/1000 );
                               computation_module.get_momentum_difference_histogram()->Fill( (reconstructible.properties.momentum -  reconstructed.properties.momentum)/1000 );
                           }
                                                                          );
        
        
        auto mass_identification = aftereffect::make_aftereffect(
                           [](reconstruction_module const& module_p)
                           { return module_p.reconstructible_o.has_value() &&
                                    module_p.reconstructed_o.has_value() &&
                                    module_p.reconstructible_o.value().properties.charge == module_p.reconstructed_o.value().properties.charge; },
                           [this](reconstruction_module const& module_p)
                           {
                               auto reconstructible = module_p.reconstructible_o.value();
                               auto reconstructed = module_p.reconstructed_o.value();
                               auto & computation_module = find_computation_module( reconstructible.properties.charge );
                               auto * mass_id_h = computation_module.get_mass_identification_histogram( );
                               mass_id_h->Fill( reconstructed.properties.mass, reconstructible.properties.mass );
                           }
                                                                          );
        
        
        auto aftereffect_c = aftereffect::make_aftereffect_list(
//                                    std::move(undiscerning_output),
                                    std::move(reconstructible_registration),
                                    std::move(successful_reconstruction),
                                    std::move(fake_reconstruction),
                                   // std::move(reconstruction_failure),
                                    std::move( mass_identification ),
                                    std::move(momentum_resolution)
                                                                );
                    
        for( auto const& module : reconstruction_module_mc ){ aftereffect_c.evaluate( module ); }
        

//        action_m.logger_m.output();
    };
    
    double retrieve_momentum( candidate const& candidate_p ) const {
        for( auto const& module : reconstruction_module_mc ) {
            if( module.end_point_h == candidate_p.data && module.reconstructible_o.has_value() ){ return module.reconstructible_o.value().properties.momentum; }
        }
        return 0;
    }
    
    
    int retrieve_mass( candidate const& candidate_p ) const {
        for( auto const& module : reconstruction_module_mc ) {
            if( module.end_point_h == candidate_p.data && module.reconstructible_o.has_value() ){ return module.reconstructible_o.value().properties.mass; }
        }
        return 0;
    }
    
    int retrieve_charge( candidate const& candidate_p ) const {
        for( auto const& module : reconstruction_module_mc ) {
            if( module.end_point_h == candidate_p.data && module.reconstructible_o.has_value() ){ return module.reconstructible_o.value().properties.charge; }
        }
        return 0;
    }
    
    void submit_reconstructible_track(candidate const& candidate_p)
    {
        auto output_reconstructible = [this]( auto const& reconstructible_p )
                                    {
                                        action_m.logger_m << "charge: " << reconstructible_p.properties.charge << '\n';
                                        action_m.logger_m << "mass: " << reconstructible_p.properties.mass << '\n';
                                        action_m.logger_m << "momentum: " << reconstructible_p.properties.momentum << '\n';
                                        action_m.logger_m << "track_slope_x: " << reconstructible_p.properties.track_slope_x << '\n';
                                        action_m.logger_m << "track_slope_y: " << reconstructible_p.properties.track_slope_y << '\n';
                                        action_m.logger_m << "mc_track_index: " ;
                                        for( auto index : reconstructible_p.get_indices() ){
                                            action_m.logger_m << index << " ";
                                        }
                                        action_m.logger_m << '\n';
                                    };
        
        
        auto find_indices = [this]( candidate const& candidate_p )
                            {
                                std::vector<int> index_c;
                                index_c.reserve( candidate_p.data->GetMcTracksN() );
                                for( int i{0}; i < candidate_p.data->GetMcTracksN() ; ++i  ){
                                    auto id = candidate_p.data->GetMcTrackIdx(i);
                                    index_c.emplace_back( id );
                                }
                                return index_c;
                            };
        
        
        
        auto check_reconstructibility = [this]( std::vector<int> const& index_pc )
        {
            bool is_reconstructible = true;
            
            auto index_found = [&index_pc](candidate_indices const& candidate_index_pc)
            {
                for( auto candidate_index : candidate_index_pc.get_indices() ){
                    if( std::any_of( index_pc.begin(),
                                     index_pc.end(),
                                    [&candidate_index](int index_p){ return candidate_index == index_p; } ) )
                    {
                        return true;
                    }
                }
                return false;
            };
            
            for( auto const& candidate_index_c : candidate_index_mc ){
                is_reconstructible = index_found( candidate_index_c );
                if( !is_reconstructible ){ break; }
            }
                
            return is_reconstructible;
        };
        
        
        
        auto origin_aftereffect = aftereffect::make_aftereffect(
                            [this]( int index_p )
                            {
                                auto const * track_h = data_mhc->GetTrack(index_p);
            
                                return track_h->GetCharge() > 0 ?
                                    ( track_h->GetInitPos().Z() >= target_limits_m.first ) &&
                                    ( track_h->GetInitPos().Z() <= target_limits_m.second )    :
                                    false;
                                return false;
                            },
                            [](int index_p){ return std::vector<int>{index_p}; }
                                                            );
        
        
        auto scattered_aftereffect = aftereffect::make_aftereffect(
                                [this]( int index_p )
                                {
                                    auto const * track_h = data_mhc->GetTrack(index_p);
                                    auto const * mother_track_h = track_h->GetCharge() > 0 ?
                                                        data_mhc->GetTrack( track_h->GetMotherID() ) :
                                                        nullptr;
                                    if( mother_track_h &&
                                        ( mother_track_h->GetCharge() == track_h->GetCharge() ) &&
                                        ( mother_track_h->GetMass() == track_h->GetMass() ) &&
                                        ( mother_track_h->GetInitPos().Z() >= target_limits_m.first ) &&
                                        ( mother_track_h->GetInitPos().Z() <= target_limits_m.second ) ){
                                        return true;
                                    }
                                    return false;
                                },
                                [this](int index_p)
                                {
                                    std::vector<int> index_c{index_p};
                                    auto const * track_h = data_mhc->GetTrack(index_p);
                                    index_c.push_back( track_h->GetMotherID() );
                                    return index_c;
                                }
                                                               );
        
       
        auto check_charge_reconstruction = [this]( candidate const& candidate_p, std::vector<int> const& index_pc )
        {
            auto real_charge = data_mhc->GetTrack(index_pc[0])->GetCharge();
            //std::cout << "real_charge: " << real_charge << '\n';
            auto reconstructed_charge = static_cast<TATWpoint const *>( candidate_p.data )->GetChargeZ();
            //std::cout << "reconstructed_charge: " << reconstructed_charge << '\n';
            return real_charge == reconstructed_charge;
        };
        
        action_m.logger_m.template add_sub_header< details::immutable_tag >("submit_reconstructible_track");
        
        auto track_index_c = find_indices( candidate_p );

        if( !track_index_c.empty() ){
            auto result_o = aftereffect::first_of( track_index_c.begin(),
                                                   track_index_c.end(),
                                                   origin_aftereffect,
                                                   scattered_aftereffect );
        
            
            if( result_o.has_value() &&
                check_reconstructibility( result_o.value() ) ) { //&&
                //check_charge_reconstruction(candidate_p, result_o.value()) ){
                
                action_m.logger_m.add_sub_header("track_registered");
                auto particle = form_particle(result_o.value());
                auto reconstructible = reconstructible_track{result_o.value(), std::move(particle)};
                output_reconstructible( reconstructible );
                register_reconstructible_track( candidate_p.data, std::move(reconstructible) );
            }
        }
    }
    
private:
    particle_type form_particle( std::vector<int> const& index_pc ) const
    {
        auto mc_track_h = index_pc.size() > 1 ? data_mhc->GetTrack(index_pc[1]) : data_mhc->GetTrack(index_pc[0]) ;
        //if size > 1, then the particle has been scattered, therefore need to retrieve mother parameters
        return particle_type{
            mc_track_h->GetInitP().Mag() * 1000,
            mc_track_h->GetInitP().X()/mc_track_h->GetInitP().Z(),
            mc_track_h->GetInitP().Y()/mc_track_h->GetInitP().Z(),
            mc_track_h->GetCharge(),
            static_cast<int>( mc_track_h->GetMass() * 1.1 ) //bad hack to get number of nucleons
        };
    }
    
    void register_reconstructible_track(data_type const* end_point_ph, reconstructible_track&& reconstructible_p)
    {
        reconstruction_module_mc.push_back(
                                           reconstruction_module{
                                               end_point_ph,
                                               aftereffect::optional<reconstructible_track>{ std::move(reconstructible_p) },
                                               aftereffect::optional<reconstructed_track>{ }
                                           }
                                           );
    }
    
    

    
    computation_module& find_computation_module(int charge_p)
    {
        auto module_i = std::find_if( computation_module_mc.begin(), computation_module_mc.end(),
                                      [&charge_p](computation_module const & module_p){ return charge_p == module_p.charge; });
        if( module_i == computation_module_mc.end() ){
            computation_module_mc.push_back( computation_module{charge_p} ) ;
            return computation_module_mc.back();
        }
        return *module_i;
    }
    
    
public:
    template<class T>
    void submit_reconstructed_track( T const& track_p )
    {
        action_m.logger_m.template add_sub_header< details::immutable_tag >("reconstructed_track");
        
        //output_current_hypothesis();
        
        auto const& state_c = track_p.get_clusters();
        using full_state = typename std::decay_t<decltype(state_c)>::value_type ;
        std::vector<full_state> full_state_c{ state_c.begin()+ 1 , state_c.end() }; //remove vertex
        
        std::vector< data_type const* > cluster_c;
        cluster_c.reserve( full_state_c.size() );
        for( auto const& full_state : full_state_c ){
            cluster_c.emplace_back( full_state.data );
        }
        
        auto particle = particle_type{ track_p.momentum,
                                       0,//incompatible for now
                                       0, //incompatible for now
                                       track_p.particle.charge,
                                       track_p.particle.mass };
        
        action_m.logger_m << "charge: " << particle.charge << '\n';
        action_m.logger_m << "mass: " << particle.mass << '\n';
        action_m.logger_m << "momentum: " << particle.momentum << '\n';
        
        auto reconstructed = reconstructed_track{ cluster_c, std::move(particle), track_p.clone };
        register_reconstructed_track( cluster_c.back(), std::move(reconstructed) );


    }
    
private:
    void register_reconstructed_track(data_type const* end_point_ph, reconstructed_track&& reconstructed_p)
    {
        auto module_i = std::find_if( reconstruction_module_mc.begin(),
                                     reconstruction_module_mc.end(),
                                     [&end_point_ph](reconstruction_module const& module_p){ return module_p.end_point_h == end_point_ph;  } );
        if( module_i != reconstruction_module_mc.end() ){
            module_i->reconstructed_o = aftereffect::optional<reconstructed_track>{ std::move(reconstructed_p) };
            
            action_m.logger_m << "corresponding_mc_id: ";
            auto index_c = module_i->reconstructible_o.value().get_indices();
            for( auto index : index_c ){ action_m.logger_m << index << " "; }
            action_m.logger_m << "\n";
        }
        else{
            reconstruction_module_mc.emplace_back(
                                                  end_point_ph,
                                                  aftereffect::optional<reconstructible_track>{  },
                                                  aftereffect::optional<reconstructed_track>{ std::move(reconstructed_p) }
                                                  );
        }
    }

public:
    void output_current_hypothesis()
    {
        action_m.logger_m.add_sub_header("current_hypothesis");
        action_m.logger_m << "charge: " << action_m.particle_m.charge << '\n';
        action_m.logger_m << "mass: " << action_m.particle_m.mass << '\n';
        action_m.logger_m << "momentum: " << action_m.particle_m.momentum << '\n';
        
     //   auto * root_h = current_node_mh->get_ancestor();
     //   action_m.logger_m << "track_slope_x: " << root_h->get_value().vector(2, 0) << '\n';
    //    action_m.logger_m << "track_slope_y: " << root_h->get_value().vector(3, 0) << '\n';
    }
    

public:
    reconstruction_result retrieve_results() {
        reconstruction_result result;
        
        for( auto const& module : computation_module_mc ){
            result.module_c.push_back( reconstruction_result::module{ module.charge } );
            auto& result_module = result.module_c.back();
            result_module.reconstructed_number = module.reconstructed_number;
            result_module.reconstructible_number = module.reconstructible_number;
            result_module.correct_cluster_number = module.correct_cluster_number;
            result_module.recovered_cluster_number = module.recovered_cluster_number;
            result_module.total_cluster_number = module.total_cluster_number;
            result_module.clone_number = module.clone_number;
            result_module.fake_number = module.fake_number;
        }
        
        computation_module_mc.clear();
        
        return result;
    }
    
    
    void compute_results( details::all_mixed_tag ) const{
        
        //        action_m.logger_m.freeze_everything();
        action_m.logger_m.add_root_header("RESULTS");
        action_m.logger_m.template add_header<1, details::immutable_tag>("mixed");
        
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};

        std::size_t correct_cluster_number{0};
        std::size_t recovered_cluster_number{0};
       
//        std::size_t total_cluster_number{0};
//        std::size_t local_total_cluster_number{0};
        std::size_t fake_number{0};
        
        std::size_t clone_number{0};
        
        for(auto const & module : computation_module_mc){
            reconstructed_number += module.reconstructed_number;
            reconstructible_number += module.reconstructible_number;
            
            correct_cluster_number += module.correct_cluster_number;
            recovered_cluster_number += module.recovered_cluster_number;
//            total_cluster_number += module.total_cluster_number;
            
            fake_number += module.fake_number;
            clone_number += module.clone_number;
        }
        //look at mass efficiency reconstruction ? because of the plage im momentum of protons !
        action_m.logger_m.template add_header<1, details::immutable_tag>("efficiency");
        auto efficiency = reconstructed_number * 1./reconstructible_number;
        action_m.logger_m << "global_efficiency: " << efficiency * 100 << '\n';
        auto efficiency_error = sqrt(efficiency* (1+ efficiency)/reconstructible_number);
        action_m.logger_m << "global_efficiency_error: " << efficiency_error * 100<< '\n';

        action_m.logger_m.template add_header<1, details::immutable_tag>("purity");
        auto purity = correct_cluster_number * 1./recovered_cluster_number;
        action_m.logger_m << "global_purity: " << purity * 100 << '\n';
        auto purity_error = sqrt(purity* (1+purity)/recovered_cluster_number);
        action_m.logger_m << "global_purity_error: " << purity_error * 100<< '\n';

//        action_m.logger_m.template add_header<1, details::immutable_tag>("coverage");
//        double coverage = recovered_cluster_number * 1./total_cluster_number;
//        action_m.logger_m << "global_coverage: " << coverage * 100 << '\n';
//        action_m.logger_m << "global_coverage_error: " << sqrt(coverage* (1+coverage)/total_cluster_number) * 100<< '\n';

        action_m.logger_m.template add_header<1, details::immutable_tag>("fake_yield");
        auto fake_yield = fake_number * 1./reconstructed_number;
        action_m.logger_m << "fake_yield: " << fake_yield * 100 << '\n';
        auto fake_yield_error = sqrt( fake_yield * (1+ fake_yield)/reconstructed_number);
        action_m.logger_m << "fake_yield_error: " << fake_yield_error * 100<< '\n';

        action_m.logger_m.template add_header<1, details::immutable_tag>("multiplicity");
        auto multiplicity = clone_number * 1./reconstructed_number;
        action_m.logger_m << "global_clone_ratio: " << multiplicity << '\n';
        auto multiplicity_error = sqrt(multiplicity * (1+multiplicity)/reconstructed_number);
        action_m.logger_m << "global_clone_ratio_error: " << multiplicity_error << '\n';
        
    }
    
    void compute_results( details::all_separated_tag ){
        //        action_m.logger_m.freeze_everything();
        action_m.logger_m.add_root_header("RESULTS");
        action_m.logger_m.template add_header<1, details::immutable_tag>("separated");
        for(auto const & module : computation_module_mc){
            action_m.logger_m << "charge: " << module.charge << '\n';
            compute_efficiency( module );
            compute_purity( module );
        }
    }
    
    void register_histograms( details::all_separated_tag )
    {
        
//        action_m.reconstructed_track_mhc->AddHistogram(real_momentum_distribution_h);
//        action_m.reconstructed_track_mhc->AddHistogram(reconstructed_momentum_distribution_h);
//
        for(auto const & module : computation_module_mc){
            TH1D* efficiency_histogram_h = const_cast<TH1D*>( module.get_efficiency_histogram() );

            TH1D const * reconstructible_h = module.get_reconstructible_histogram() ;
            TH2D const* mass_identification_h = module.get_mass_identification_histogram();
//            TH2D const* charge_identification_h = module.get_charge_identification_histogram();
    
            for(auto i{0} ; i < reconstructible_h->GetNbinsX() ; ++i)
            {
                if(reconstructible_h->GetBinContent(i) != 0){
                    auto reconstructed = efficiency_histogram_h->GetBinContent(i);
                    auto reconstructible = reconstructible_h->GetBinContent(i);
                    auto efficiency = reconstructed * 1./reconstructible;
                    efficiency_histogram_h->SetBinContent( i, efficiency );
                    
                    auto error = sqrt( efficiency * ( 1 + efficiency) / reconstructible );
                    efficiency_histogram_h->SetBinError( i, error );
                }
                else {
                    efficiency_histogram_h->SetBinContent(i, 0 );
                    efficiency_histogram_h->SetBinError( i, 0);
                }
            }
    

//            action_m.reconstructed_track_mhc->AddHistogram( const_cast<TH1D*>(reconstructible_h) );
//            action_m.reconstructed_track_mhc->AddHistogram( efficiency_histogram_h );
//
//            action_m.reconstructed_track_mhc->AddHistogram(const_cast<TH1D*>( module.get_momentum_difference_histogram()));
//            action_m.reconstructed_track_mhc->AddHistogram(const_cast<TH1D*>( module.get_reconstructed_histogram()));
//
//            action_m.reconstructed_track_mhc->AddHistogram( const_cast<TH2D*>(mass_identification_h) );
//            action_m.reconstructed_track_mhc->AddHistogram( const_cast<TH2D*>(charge_identification_h) );
            
            
            TH1D* momentum_histogram_h = new TH1D{ Form("momentum_charge%d", module.charge),
                ";Momentum (Gev/c/n);#frac{#sigma_p}{p}", 75, 0, 15 };
            for( auto& pair : module.histogram_bundle.momentum_resolution_c  ){
//                action_m.reconstructed_track_mhc->AddHistogram( pair.second);
                
                auto momentum = (pair.first+0.5)*0.2;
                auto rms = abs( pair.second->GetMean() - momentum ) ;
                auto bin_index = momentum_histogram_h->FindBin(momentum);
                if(pair.second->GetEntries()>50){ momentum_histogram_h->SetBinContent( bin_index, rms/momentum ); }
            }
//            action_m.reconstructed_track_mhc->AddHistogram( momentum_histogram_h );
            
            
        }
    }
    
private:
    void compute_efficiency( computation_module const & module_p ){
        auto efficiency = module_p.reconstructed_number * 1./module_p.reconstructible_number;
        action_m.logger_m << "global_efficiency: " << efficiency * 100 << '\n';
        auto efficiency_error = sqrt( efficiency* (1+ efficiency)/module_p.reconstructible_number);
        action_m.logger_m << "global_efficiency_error: " << efficiency_error * 100<< '\n';
    }
    
    
    void compute_purity( computation_module const & module_p ){
        auto purity = module_p.correct_cluster_number * 1./module_p.recovered_cluster_number;
        action_m.logger_m << "global_purity: " << purity * 100 << '\n';
        auto purity_error = sqrt( purity* (1+purity)/module_p.recovered_cluster_number);
        action_m.logger_m << "global_purity_error: " << purity_error * 100<< '\n';
    }
    

};


template<class Action>
struct empty_checker{
    using candidate = typename std::decay_t<decltype( std::declval<typename Action::detector_list_t>().last() )>::candidate;
    using track = typename Action::track;
    using node_type = typename Action::node_type;
    void update_current_node( node_type const *  ){}
    void submit_reconstructed_track(track const& ){}
    void submit_reconstructible_track(candidate const& ){}
    void end_event(){}
    void start_event(){}
    void output_current_hypothesis(){}
    void compute_results( details::all_mixed_tag )const { }
    void compute_results( details::all_separated_tag ){}
    void register_histograms( details::all_separated_tag ){}
    reconstruction_result retrieve_results() const{ return {}; }
};

template<class Action>
struct checker{
    using candidate = typename std::decay_t<decltype( std::declval<typename Action::detector_list_t>().last() )>::candidate;
    using track = typename Action::track;
    using node_type = typename Action::node_type;

    struct eraser{
        virtual ~eraser() = default;
        virtual void update_current_node( node_type const * current_node_ph ) = 0;
        virtual void submit_reconstructed_track(track const& track_p) = 0;
        virtual void submit_reconstructible_track(candidate const& candidate_p) = 0;
        virtual void end_event() = 0;
        virtual void start_event() = 0;
        virtual void output_current_hypothesis() = 0;
        virtual void compute_results( details::all_mixed_tag ) const = 0;
        virtual void compute_results( details::all_separated_tag ) = 0;
        virtual void register_histograms( details::all_separated_tag ) = 0;
        virtual reconstruction_result retrieve_results() = 0;
    };

    template<class T>
    struct holder : eraser {
        constexpr holder() = default;
        constexpr holder(T t) : t_m{ std::move(t)} {}
        void update_current_node( node_type const * current_node_ph ) override { t_m.update_current_node( current_node_ph ); }
        void submit_reconstructed_track(track const& track_p) override{ t_m.submit_reconstructed_track( track_p); }
        void submit_reconstructible_track(candidate const& candidate_p) override{ t_m.submit_reconstructible_track( candidate_p); }
        void end_event() override{ t_m.end_event(); }
        void start_event() override{ t_m.start_event();}
        void output_current_hypothesis() override{ t_m.output_current_hypothesis(); }
        void compute_results( details::all_mixed_tag )const override{ return t_m.compute_results( details::all_mixed_tag{} );}
        void compute_results( details::all_separated_tag ) override{ t_m.compute_results( details::all_separated_tag{} ); }
        void register_histograms( details::all_separated_tag ) override{ t_m.register_histograms( details::all_separated_tag{}); }
        reconstruction_result retrieve_results() override{ return t_m.retrieve_results(); }
        T t_m;
    };

    constexpr checker() = default;
    template<class T>
    constexpr checker(T t) : erased_m{ new holder<T>{ std::move(t) } } {}
    template<class T>
    constexpr checker& operator=(T t){ 
        erased_m.reset( new holder<T>{ std::move(t) } ); 
        return *this;
    }

    void update_current_node( node_type const * current_node_ph )  { erased_m->update_current_node( current_node_ph ); }
    void submit_reconstructed_track(track const& track_p) { erased_m->submit_reconstructed_track( track_p); }
    void submit_reconstructible_track(candidate const& candidate_p) { erased_m->submit_reconstructible_track( candidate_p); }
    void end_event() { erased_m->end_event(); }
    void start_event() { erased_m->start_event();}
    void output_current_hypothesis() { erased_m->output_current_hypothesis(); }
    void compute_results( details::all_mixed_tag ) const { return erased_m->compute_results( details::all_mixed_tag{} );}
    void compute_results( details::all_separated_tag ) { erased_m->compute_results( details::all_separated_tag{} ); }
    void register_histograms( details::all_separated_tag ) { erased_m->register_histograms( details::all_separated_tag{}); }
    reconstruction_result retrieve_results() { return erased_m->retrieve_results(); }

    private:
    std::unique_ptr<eraser> erased_m;
}; 


#endif
