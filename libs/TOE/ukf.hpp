//
//File      : ukf.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 26/11/2019
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef ukf_hpp
#define ukf_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>

#include "utility_types.hpp"

namespace details{
    

    template<class Derived>
    struct sigma_handler
    {
        using stepper = typename filter_traits<Derived>::stepping_operator;
        using type = typename stepper_traits<stepper>::operating_type;
        static constexpr std::size_t order = stepper_traits<stepper>::order;
        static constexpr std::size_t sigma_dimension = 2 * filter_traits< Derived >::state::vector_dimension +1;
        
        
        struct weight{
            const double weight;
        };
        
        using state = typename filter_traits<Derived>::state;
        using state_vector = typename state::state_vector_impl;
        using matrix_vector = typename state_vector::type;
        using state_covariance = typename state::state_covariance_impl;
        using matrix_covariance = typename state_covariance::type;
        using weighted_state = aggregator<state_vector, weight>;
        
        
    private:
        const double weightMean_m = 0.3;
        const double weightSigma_m = (1 - weightMean_m) / (2 * filter_traits< Derived >::state::vector_dimension);
        const double factor_m = sqrt(sigma_dimension/ (1 - weightMean_m));
        
        
        
    private:
        template< std::size_t Index,
                  typename std::enable_if_t< (Index == 0 ), std::nullptr_t> = nullptr >
        operating_state<type, order> generate_single_sigma(const state& state_p)
        {
            return make_operating_state<order>(state_p);
        }
        
        template< std::size_t Index,
                  typename std::enable_if_t< (Index != 0 && Index <= state_vector::vector_dimension), std::nullptr_t > = nullptr >
        operating_state<type, order> generate_single_sigma(const state& state_p)
        {
            auto vector = state_p.vector + factor_m * make_cholesky_triangle(state_p.covariance).column(Index-1);
           // std::cout << "Positive sigma:" << Index << '\n' << vector;
            auto halves = split_half(std::move(vector), row_tag{});
            return { state_p.evaluation_point,
                     {std::move(halves.first), std::move(halves.second)}    };
        }
        
        template< std::size_t Index,
                  typename std::enable_if_t< (Index > state_vector::vector_dimension), std::nullptr_t > = nullptr >
        operating_state<type, order> generate_single_sigma(const state& state_p)
        {
            auto vector = state_p.vector - factor_m * make_cholesky_triangle(state_p.covariance).column(Index -1 -state_p.vector_dimension );
           // std::cout << "Negative sigma:" << Index << '\n' << vector;
            auto halves = split_half(std::move(vector), row_tag{});
            return { state_p.evaluation_point,
                     {std::move(halves.first), std::move(halves.second)}    };
        }
        
        template<std::size_t ... Indices>
        auto generate_sigma_points_impl(const state& state_p , std::index_sequence<Indices...>)
        {
            return std::vector<operating_state<type, order>>{ generate_single_sigma<Indices>(state_p)... };
        }
        
    public:
        std::vector<operating_state<type, order>> generate_sigma_points(const state& state_p)
        {
            return generate_sigma_points_impl(state_p, std::make_index_sequence<sigma_dimension>{});
        }
        
        
        
        
        

        
    private:
        template<std::size_t Index, typename std::enable_if_t< (Index == 0 ), std::nullptr_t> = nullptr >
        weighted_state generate_single_weighted_state(operating_state<type, order> os_p)
        {
            return {make_state_vector(std::move(os_p)), weight{weightMean_m} };
        }
        
        template<std::size_t Index, typename std::enable_if_t< (Index > 0 ), std::nullptr_t > = nullptr >
        weighted_state generate_single_weighted_state(operating_state<type, order> os_p)
        {
            return {make_state_vector(std::move(os_p)), weight{weightSigma_m} };
        }
        
        template<std::size_t ... Indices>
        std::vector<weighted_state> generate_weighted_state(std::vector<operating_state<type, order>>&& os_pc , std::index_sequence<Indices...>)
        {
            return std::vector<weighted_state>{ generate_single_weighted_state<Indices>(os_pc[Indices])... };
        }
        
        
    public:
        state generate_propagated_state(std::vector<operating_state<type, order>>&& os_pc)
        {
            auto ws_c = generate_weighted_state(std::move(os_pc), std::make_index_sequence<sigma_dimension>{});
            auto debug_vector = [](decltype(state_vector::vector) v_p, weighted_state ws_p){
                std::cout << "Debug lambda:\nstate_vector : \n";
                std::cout << v_p ;
                std::cout << "\nweighted_state:\n";
                std::cout << ws_p.vector ;
                std::cout << " -> " << ws_p.weight << "\n\n";
            };
            state_vector sv = make_state_vector( std::accumulate( ws_c.begin(), ws_c.end(),
                                                                  matrix_vector{},
                                                                 [&debug_vector](matrix_vector v_p, const weighted_state& ws_p){ /*debug_vector(v_p, ws_p)*/; return std::move(v_p) + ws_p.vector * ws_p.weight; }    ) );
            
            auto debug_covariance = [&sv](matrix_covariance c_p, weighted_state ws_p){
                std::cout << "Debug lambda:\nstate_covariance : \n";
                std::cout << c_p ;
                std::cout << "state_vector: \n";
                std::cout << sv.vector ;
                std::cout << "weighted_state_vector: \n";
                std::cout << ws_p.vector;
                std::cout << "resulting vector used:\n";
                std::cout << (ws_p.vector - sv.vector);
                std::cout << "\nadded matrix ::\n";
                std::cout << (ws_p.vector - sv.vector) * make_transpose(ws_p.vector - sv.vector) ;
                std::cout << " -> " << ws_p.weight << "\n\n";
            };
            state_covariance sc = make_state_covariance( std::accumulate( ws_c.begin(), ws_c.end(),
                                                                          matrix_covariance{},
                                                                          [&sv, &debug_covariance]( matrix_covariance c_p,
                                                                                 const weighted_state& ws_p  )
                                                                         { /*debug_covariance(c_p, ws_p)*/ ;return std::move(c_p) + ws_p.weight * (ws_p.vector - sv.vector) * make_transpose(ws_p.vector - sv.vector); }             ) );
            return state{evaluation_point{os_pc.front().evaluation_point}, std::move(sv), std::move(sc)};
        }
    };
    

    
    template<class Derived>
    struct ukf_propagator
    {
        using stepper = typename filter_traits<Derived>::stepping_operator;
        using type = typename stepper_traits<stepper>::operating_type;
        static constexpr std::size_t order = stepper_traits<stepper>::order;
        
    public:
        //first time go past, will see in the future about going right after what we need
        //probably simple ref here -> but structure imply consommation but it imply a move too ..
        auto force_propagation( std::vector< operating_state<type, order> >&& os_pc,
                                double step_p  )
             -> std::vector< operating_state<type, order> >
        {
            for(auto& os : os_pc){
                os = derived().call_stepper().force_step( std::move(os), step_p);
            }
            return std::move(os_pc);
        }
        
        template<class Predicate>
        auto propagate_until( std::vector< operating_state<type, order> >&& os_pc,
                              Predicate&& predicate_p  )
             -> std::vector< operating_state<type, order> >
        {
            while( !predicate_p( os_pc.front() ) ){
                os_pc = propagate_once(std::move(os_pc));
                //std::cout << "order1:\n" << os_pc.front().state(order_tag<1>{}) ;
            }
            return std::move(os_pc);
        }
        
        std::vector< operating_state<type, order> > propagate_once( std::vector< operating_state<type, order> >&& os_pc  )
        {
            double step = derived().step_length();
            const double evaluation_point = os_pc.front().evaluation_point;
            const auto step_result = derived().call_stepper().step( std::move( os_pc.front() ), step );
            os_pc.front() = step_result.first;
            
            step = os_pc.front().evaluation_point - evaluation_point;
                
            for(auto iterator = os_pc.begin() + 1  ; iterator != os_pc.end()  ; ++iterator ){
                *iterator = derived().call_stepper().force_step( std::move(*iterator), step);
            }
            
            //optimize next step
            derived().step_length() = derived().call_stepper().optimize_step_length(step, step_result.second);
            
            return std::move(os_pc);
        }
        
    private:
        Derived& derived() { return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
    };

    
    
    
    template<class Derived>
    struct kalman_filter{
        
        using state = typename filter_traits<Derived>::state;
        using state_vector = typename state::state_vector_impl;
        using state_covariance = typename state::state_covariance_impl;
        
        struct chisquared{
            double chisquared;
        };
        
        template<class Vector, class Covariance, class T>
        using candidate = candidate_impl< Vector, Covariance, measurement_matrix_impl<T> >;

        
        
    public:
        template< class Vector,
                  class Covariance,
                  class T,
                  class Enabler = std::enable_if_t< has_correct_dimension<
                                                             T,
                                                             column_tag,
                                                             state_vector::vector_dimension
                                                                          >::value
                                                  >
                >
        chisquared compute_chisquared( const state& s_p,
                                       const candidate<Vector, Covariance, T>& candidate_p )
        {
            auto residual_vector = candidate_p.vector - candidate_p.measurement_matrix * s_p.vector;
            auto residual_covariance = candidate_p.covariance + candidate_p.measurement_matrix * s_p.covariance * make_transpose(candidate_p.measurement_matrix);
            
            return {(make_transpose(residual_vector) * residual_covariance * residual_vector)(0,0)}; //not pretty
        }
        
        
        
        
        
        //should probably check everything or nothing, but state to measurement dimensionality error is the most probable
        template< class MeasurementVector, class MeasurementCovariance, class T,
                  class Enabler = std::enable_if_t< has_correct_dimension<T, column_tag, state_vector::vector_dimension>::value >  >
        state correct_state( state s_p,
                             candidate<MeasurementVector, MeasurementCovariance, T> candidate_p )
        {
            //residual propagated state
            auto residual_vector = candidate_p.vector - candidate_p.measurement_matrix * s_p.vector;
            auto residual_covariance = candidate_p.covariance + candidate_p.measurement_matrix * s_p.covariance * make_transpose(candidate_p.measurement_matrix);
            auto gain = s_p.covariance * make_transpose(candidate_p.measurement_matrix) * make_inverse(residual_covariance);
            
            //corrected state vector/covariance
            auto csv = make_state_vector( s_p.vector + gain * residual_vector );
            auto csc = make_state_covariance( s_p.covariance - gain * candidate_p.measurement_matrix * s_p.covariance ); //room for optimization
        
        
            return { evaluation_point{s_p.evaluation_point} ,
                     std::move(csv),
                     std::move(csc) };
        }
        

    };
    
    

    
    
    
    
}//namespace details
    

template<class StateVector, class StateCovariance, class EvaluationPoint, class Chisquared>
state_impl<StateVector, StateCovariance> make_state(aggregator<EvaluationPoint, StateVector, StateCovariance, Chisquared> cs_p)
{
    return { evaluation_point{cs_p.evaluation_point},
             make_state_vector(cs_p.vector),
             make_state_covariance(cs_p.covariance)  };
}


namespace details{
    
    template<class State, class Enabler>
    struct is_state_coherent_impl : std::false_type{};
    
    template<class State>
    struct is_state_coherent_impl< State,
                                   std::enable_if_t< State::vector_dimension *
                                                     State::vector_dimension ==
                                                     State::covariance_dimension >   >
                                  : std::true_type{};
    
    template<class State>
    using is_state_coherent = is_state_coherent_impl<State, void>;
    
    
    
    template<class State, class Stepper, class Enabler>
    struct is_dimension_coherent_impl : std::false_type {};
    
    
    //
    template<class State, class Stepper>
    struct is_dimension_coherent_impl< State, Stepper,
                                        std::enable_if_t< State::vector_dimension ==
                                                          stepper_traits<Stepper>::operating_dimension *
                                                          stepper_traits<Stepper>::order > >
                                      : std::true_type {};
    
    
    template<class State, class Stepper>
    using is_dimension_coherent = is_dimension_coherent_impl<State, Stepper, void>;

} // namespace details



template<class State, class Stepper>
struct ukf : public details::sigma_handler< ukf<State, Stepper> >,
                    details::ukf_propagator< ukf<State, Stepper> >,
                    details::kalman_filter< ukf<State, Stepper> >

{
    using details::sigma_handler< ukf >::generate_sigma_points;
    using details::ukf_propagator< ukf >::propagate_until;
    using details::ukf_propagator< ukf >::propagate_once;
    using details::sigma_handler< ukf >::generate_propagated_state;
    using details::kalman_filter< ukf >::correct_state;
    
    
private:
    Stepper stepper_m;
    double step_m = 1e-3;
    
public:
    const Stepper& call_stepper() const {return stepper_m;}
    Stepper& call_stepper() {return stepper_m;}
    double& step_length() {return step_m;}
    const double& step_length() const {return step_m;}
    
public:
    template< class Stepper_,
              class StateChecker = std::enable_if_t< details::is_state_coherent< State >::value >,
              class CoherenceChecker = std::enable_if_t< details::is_dimension_coherent< State, Stepper_>::value >   >
    ukf(Stepper_&& stepper_p) : stepper_m{std::move(stepper_p)} {}
    
};



namespace details{
    
    template<class State, class Stepper>
    struct filter_traits< ukf<State, Stepper> >
    {
        using state = State;
        using stepping_operator = Stepper;
    };
    
}//namespace details


//user should choose between consumption or copy
template<class State, class Stepper>
constexpr auto make_ukf(Stepper stepper_p) -> ukf<State, Stepper>
{
    return { std::move(stepper_p) };
}


#endif /* ukf_hpp */
