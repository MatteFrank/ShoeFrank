
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

template<std::size_t Index, class ... Ts>
struct configuration {
    using tuple_t = std::tuple<Ts...>;
    using detector_t = typename std::tuple_element< Index , tuple_t >::type;
    using cut_t = std::decay_t<decltype( detector_t::default_cut_value)>;
};
    
template<class C> struct action_factory{};
template< std::size_t I >
struct action_factory<configuration<I, vertex_tag, tof_tag>> {
    auto operator()(TADIgeoField* field_ph){
        using state_vector =  matrix<4,1> ;
        using state_covariance =  matrix<4, 4> ;
        using state = state_impl< state_vector, state_covariance  >;
        
        
        auto * clusterVTX_hc = static_cast<TAVTntuCluster*>( gTAGroot->FindDataDsc("vtClus")->Object() );
        auto * vertex_hc = static_cast<TAVTntuVertex*>( gTAGroot->FindDataDsc("vtVtx")->Object() );
        auto * geoVTX_h = static_cast<TAVTparGeo*>(gTAGroot->FindParaDsc(TAVTparGeo::GetDefParaName(), "TAVTparGeo")->Object() );
        
        auto * clusterTW_hc = static_cast<TATWntuPoint*>( gTAGroot->FindDataDsc("twPoint")->Object() );
        auto * geoTW_h = static_cast<TATWparGeo*>( gTAGroot->FindParaDsc(TATWparGeo::GetDefParaName(), "TATWparGeo")->Object() );
        
        
        auto list = start_list( detector_properties<vertex_tag>(vertex_hc,
                                                                clusterVTX_hc,
                                                                geoVTX_h) )
                        .add( detector_properties<tof_tag>(clusterTW_hc, geoTW_h) )
                        .finish();
        
        
        auto ode = make_ode< matrix<2,1>, 2>( model{ field_ph } );
        auto stepper = make_stepper<data_grkn56>( std::move(ode) );
        auto ukf = make_ukf<state>( std::move(stepper) );
        
        return new_TATOEactGlb(
                               std::move(ukf),
                               std::move(list),
                               nullptr,
                               static_cast<TAGparGeo*>( gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object() ),
                               true
                               );
    }
};

    
template<class C>
constexpr auto new_action(TADIgeoField* field_ph){ return action_factory<C>{}(field_ph); }

template<int LO, int UO>
struct range {
    constexpr static int lower_offset = LO;
    constexpr static int upper_offset = UO;
};


template<class C, class R>
struct procedure{
    using detector_t = typename C::detector_t;
    using cut_t = typename C::cut_t;
    
    constexpr procedure() {
        set_cuts();
    }
    
private:
    template< class T = cut_t,typename std::enable_if_t< !std::is_same<T, double>::value ,std::nullptr_t> = nullptr>
    constexpr void set_cuts(){
        for( auto i{0}; i < cut_mc.size(); ++i){ cut_mc[i] += R::lower_offset -1; }
    }
    
    template< class T = cut_t, typename std::enable_if_t< std::is_same<T, double>::value, std::nullptr_t> = nullptr>
    constexpr void set_cuts(){
        cut_mc += R::lower_offset - 1;
    }
    
public:
    
    template< class T = cut_t, typename std::enable_if_t< !std::is_same<T, double>::value ,std::nullptr_t> = nullptr>
    constexpr cut_t next_cut(){ return ++cut_mc; }
    
    template< class T = cut_t, typename std::enable_if_t< std::is_same<T, double>::value, std::nullptr_t> = nullptr>
    constexpr cut_t next_cut(){ return ++cut_mc; }
    
    template< class T = cut_t, typename std::enable_if_t< !std::is_same<T, double>::value, std::nullptr_t> = nullptr>
    constexpr bool increment(){
        return cut_mc[current_cut_m] < detector_t::default_cut_value[current_cut_m] + R::upper_offset ?
                        true :
                        (++current_cut_m < cut_mc.size()) ;
    }
    
    template< class T = cut_t, typename std::enable_if_t< std::is_same<T, double>::value, std::nullptr_t> = nullptr>
    constexpr bool increment(){
        return cut_mc < detector_t::default_cut_value + R::upper_offset;
    }
    
    constexpr auto* generate_action(TADIgeoField* field_ph){ return new_action<C>(field_ph) ; }
    
private:
    std::size_t current_cut_m{0};
    cut_t cut_mc{detector_t::default_cut_value };
};
}// namespace details



struct TATOEbaseCutter : TAGaction {
    TATOEbaseCutter(char const* name_p) :
        TAGaction(name_p, "TATOEcutter - Tool used to determine optimal cut values according to geometry for use in TOE") {}
    virtual void NextIteration() = 0;
    virtual Bool_t Action() = 0;
    virtual void Output() const  = 0 ;
    virtual ~TATOEbaseCutter() = default;
};
    
template<class ... Ps>
struct TATOEcutter : TATOEbaseCutter {
 
    
    struct iterator {
        using tuple_t = std::tuple< Ps... >;
        constexpr static std::size_t size = sizeof...(Ps);
        
        void increment() {
            find_procedure( std::make_index_sequence<size>{} );
        }
        
        constexpr iterator(TATOEcutter* c_ph, TADIgeoField* field_ph) : cutter_mh{c_ph}, field_mh{ field_ph} { reset_action<0>(); }
        
    private:
        template<std::size_t ... Indices>
        constexpr void find_procedure( std::index_sequence<Indices...> ){
            int expander[] = {0, ( ((current_procedure_m != Indices) ? void() : apply_increment<Indices>() ), void(), 0)...};
        }
        
        template<std::size_t Index>
        constexpr void apply_increment(){
            if( std::get<Index>(procedure_c).increment() ){
                using detector_t = typename std::tuple_element<Index, tuple_t>::type::detector_t;
                cutter_mh->action_mh->set_cuts( detector_t{}, std::get<Index>(procedure_c).next_cut() );
            }
            else{
                ++current_procedure_m;
                reset_action<Index+1>();
                // apply_increment<Index+1>(); -> no need, expander will be true for following expanded expression
            }
        }
        
    private:
        template<std::size_t Index, typename std::enable_if_t< (Index<size) , std::nullptr_t> = nullptr >
        constexpr void reset_action(){
            cutter_mh->action_mh.reset( std::get<Index>(procedure_c).generate_action(field_mh) );
        }
        
        template<std::size_t Index, typename std::enable_if_t< !(Index<size) , std::nullptr_t> = nullptr >
        constexpr void reset_action(){}
        
    private:
        std::size_t current_procedure_m{0};
        tuple_t procedure_c{};
        TADIgeoField* field_mh;
        TATOEcutter* cutter_mh;
    };
    
    friend class TATOEcutter<Ps...>::iterator;
    

    TATOEcutter( char const* name_p, TADIgeoField* field_ph ) :
        TATOEbaseCutter( name_p ),
        iterator_m{ this, field_ph } {}
    
    void NextIteration() override{
        iterator_m.increment();
    };
    
    Bool_t Action() override {
        action_mh->Action();
        result_mc.back().add( action_mh->retrieve_result() ) ;
//        action_mh->Output();
        return true;
    }
    
    void Output() const override{
        
        for( auto const& result : result_mc) {
            std::size_t reconstructed_number{0};
            std::size_t reconstructible_number{0};
            
            std::size_t correct_cluster_number{0};
            std::size_t recovered_cluster_number{0};
            
            std::size_t clone_number{0};
            
            for(auto const & module : result.module_c){
                reconstructed_number += module.reconstructed_number;
                reconstructible_number += module.reconstructible_number;
            
                correct_cluster_number += module.correct_cluster_number;
                recovered_cluster_number += module.recovered_cluster_number;
            
                clone_number += module.clone_number;
            }
        
            std::cout << "===== MIXED_RESULTS ====\n";
            std::cout << "----- efficiency -----\n";
            auto efficiency = reconstructed_number * 1./reconstructible_number;
            std::cout << "global_efficiency: " << efficiency * 100 << '\n';
            auto efficiency_error = sqrt(efficiency* (1+ efficiency)/reconstructible_number);
            std::cout << "global_efficiency_error: " << efficiency_error * 100<< '\n';
        
            std::cout << "----- purity -----\n";

            auto purity = correct_cluster_number * 1./recovered_cluster_number;
            std::cout << "global_purity: " << purity * 100 << '\n';
            auto purity_error = sqrt(purity* (1+purity)/recovered_cluster_number);
            std::cout << "global_purity_error: " << purity_error * 100<< '\n';

        }
    }


private:

    std::unique_ptr<TATOEbaseAct> action_mh;
    iterator iterator_m;
    std::vector<reconstruction_result> result_mc;
    
};

#endif
