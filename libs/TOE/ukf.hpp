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
#include "expr.hpp"

template<class T>
class TD1;


namespace details{
    

    template<class Derived>
    struct sigma_handler
    {
        using stepper = typename filter_traits<Derived>::stepping_operator;
        using type = typename stepper_traits<stepper>::operating_type;
        static constexpr std::size_t order = stepper_traits<stepper>::order;
        static constexpr std::size_t sigma_dimension = 2 * filter_traits< Derived >::state::vector_dimension + 1;
        
        
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
        operating_state<type, order> generate_single_sigma(const state& state_p) const
        {
//            std::cout << "single_sigma::indice " << Index << '\n';
//            std::cout << "single_sigma::vector (" << state_p.vector(0,0) << ", " << state_p.vector(1,0) << ") -- (" << state_p.vector(2,0) << ", " << state_p.vector(3,0) << ")\n";
            return make_operating_state<order>(state_p);
        }
        
        template< std::size_t Index,
                  typename std::enable_if_t< (Index != 0 && Index <= state_vector::vector_dimension), std::nullptr_t > = nullptr >
        operating_state<type, order> generate_single_sigma(const state& state_p) const
        {
//           std::cout << "Positive sigma:" << Index << '\n';
//            std::cout << "single_sigma::starting_vector \n" << state_p.vector;
//            std::cout << "single_sigma::indice " << Index << '\n';
//            std::cout << "single_sigma::cholesky \n " << make_cholesky_triangle(state_p.covariance);
//            std::cout << "single_sigma::cholesky_factor \n " << factor_m * make_cholesky_triangle(state_p.covariance);
//            
            
            auto vector1 = expr::compute( state_p.vector + factor_m * column<Index-1>( form_cholesky_triangle(state_p.covariance)) );
//            auto vector2 = expr::compute( state_p.vector + factor_m * form_column( form_cholesky_triangle(state_p.covariance), Index-1 ) );
            
//            std::cout << "single_sigme::vector_calculation1 :\n" << vector1 ;
//            std::cout << "single_sigme::vector_calculation2 :\n" << vector2 ;
            
            auto halves1 = split_half( std::move(vector1) , row_tag{});
//            auto halves2 = split_half( std::move(vector2) , row_tag{});
            
//            std::cout << "single_sigma::vector (" << halves1.first(0,0) << ", " << halves1.first(1,0) << ") -- (" << halves1.second(0,0) << ", " << halves1.second(1,0) << ")\n";
//            std::cout << "single_sigma::vector (" << halves2.first(0,0) << ", " << halves2.first(1,0) << ") -- (" << halves2.second(0,0) << ", " << halves2.second(1,0) << ")\n";
            
            return { state_p.evaluation_point,
                     {std::move(halves1.first), std::move(halves1.second)}    };
        }
        
        template< std::size_t Index,
                  typename std::enable_if_t< (Index > state_vector::vector_dimension), std::nullptr_t > = nullptr >
        operating_state<type, order> generate_single_sigma(const state& state_p) const
        {
//            std::cout << "Negative sigma:" << Index << '\n';
//           std::cout << "single_sigma::starting_vector \n" << state_p.vector;
//            std::cout << "single_sigma::indice " << Index << '\n';
//            std::cout << "single_sigma::cholesky \n " << make_cholesky_triangle(state_p.covariance);
//            std::cout << "single_sigma::cholesky_factor \n " << factor_m * make_cholesky_triangle(state_p.covariance);
//            std::cout << Index -1 - state_vector::vector_dimension << " - " << Index-1 -state_p.vector_dimension << '\n';
//            auto vector1 = expr::compute( state_p.vector - factor_m * form_column( form_cholesky_triangle(state_p.covariance), Index -1 - state_vector::vector_dimension ) );
            auto vector2 = expr::compute( state_p.vector - factor_m * column<Index -1 - state_vector::vector_dimension>( form_cholesky_triangle(state_p.covariance) ) );
//
            
//            std::cout << "single_sigme::vector_calculation1 :\n" << vector1 ;
//            std::cout << "single_sigme::vector_calculation2 :\n" << vector2 ;
            
//            auto halves1 = split_half(  std::move(vector1) , row_tag{});
            auto halves2 = split_half(  std::move(vector2) , row_tag{});
            
//            std::cout << "single_sigma::vector (" << halves1.first(0,0) << ", " << halves1.first(1,0) << ") -- (" << halves1.second(0,0) << ", " << halves1.second(1,0) << ")\n";
//            std::cout << "single_sigma::vector (" << halves2.first(0,0) << ", " << halves2.first(1,0) << ") -- (" << halves2.second(0,0) << ", " << halves2.second(1,0) << ")\n";
            
            return { state_p.evaluation_point,
                     {std::move(halves2.first), std::move(halves2.second)}    };
        }
        
        template<std::size_t ... Indices>
        auto generate_sigma_points_impl(const state& state_p , std::index_sequence<Indices...>) const
        {
            return std::vector<operating_state<type, order>>{ generate_single_sigma<Indices>(state_p)... };
        }
        
    public:
        std::vector<operating_state<type, order>> generate_sigma_points(const state& state_p) const
        {
            return generate_sigma_points_impl(state_p, std::make_index_sequence<sigma_dimension>{});
        }
        
        
        
        
        

        
    private:
        template<std::size_t Index, typename std::enable_if_t< (Index == 0 ), std::nullptr_t> = nullptr >
        weighted_state generate_single_weighted_state(operating_state<type, order> os_p) const
        {
           // std::cout << "generate_single_weighted_state_mean::indice : " << Index << '\n';
          //  std::cout << "state : (" << os_p.state( details::order_tag<0>{} )(0,0) << ", " << os_p.state( details::order_tag<0>{} )(1,0) << ") + ("
           //                          << os_p.state( details::order_tag<1>{} )(0,0) << ", " << os_p.state( details::order_tag<1>{} )(1,0) << ")\n";
            return {make_state_vector(std::move(os_p)), weight{weightMean_m} };
        }
        
        template<std::size_t Index, typename std::enable_if_t< (Index > 0 ), std::nullptr_t > = nullptr >
        weighted_state generate_single_weighted_state(operating_state<type, order> os_p) const
        {
         //   std::cout << "generate_single_weighted_state::indice : " << Index << '\n';
          //  std::cout << "state : (" << os_p.state( details::order_tag<0>{} )(0,0) << ", " << os_p.state( details::order_tag<0>{} )(1,0) << ") + ("
        //    << os_p.state( details::order_tag<1>{} )(0,0) << ", " << os_p.state( details::order_tag<1>{} )(1,0) << ")\n";
            return {make_state_vector(std::move(os_p)), weight{weightSigma_m} };
        }
        
        template<std::size_t ... Indices>
        std::vector<weighted_state> generate_weighted_state(std::vector<operating_state<type, order>>&& os_pc , std::index_sequence<Indices...>) const
        {
            return std::vector<weighted_state>{ generate_single_weighted_state<Indices>(os_pc[Indices])... };
        }
        
        
    public:
        state generate_propagated_state(std::vector<operating_state<type, order>>&& os_pc) const
        {
//            std::cout << "sigma_dimensions : " << sigma_dimension << '\n';
            
            auto ws_c = generate_weighted_state(std::move(os_pc), std::make_index_sequence<sigma_dimension>{});


            state_vector sv = make_state_vector( std::accumulate( ws_c.begin(), ws_c.end(),
                                                                  matrix_vector{},
                                                                 [](matrix_vector v_p, const weighted_state& ws_p)
                                                                 -> matrix_vector
                                                                    { return std::move(v_p) + ws_p.vector * ws_p.weight; }    ) );
            

            state_covariance sc = make_state_covariance( std::accumulate( ws_c.begin(), ws_c.end(),
                                                                          matrix_covariance{},
                                                                          [&sv]( matrix_covariance c_p,
                                                                                 const weighted_state& ws_p  )
                                                                         -> matrix_covariance
                                                                         {return std::move(c_p) + ws_p.weight * (ws_p.vector - sv.vector) * transpose(ws_p.vector - sv.vector); }             ) );
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
                                double step_p  ) const
             -> std::vector< operating_state<type, order> >
        {
            for(auto& os : os_pc){
                os = derived().call_stepper().force_step( std::move(os), step_p);
            }
            return std::move(os_pc);
        }
        
        template<class Predicate>
        auto propagate_while( std::vector< operating_state<type, order> >&& os_pc,
                              Predicate&& predicate_p  )
             -> std::vector< operating_state<type, order> >
        {
           // std::cout << "propagate_while():\n";
            while( predicate_p( os_pc.front() ) ){
                os_pc = propagate_once(std::move(os_pc));
                //std::cout << "order1:\n" << os_pc.front().state(order_tag<1>{}) ;
            }
            return std::move(os_pc);
        }
        
        std::vector< operating_state<type, order> > propagate_once( std::vector< operating_state<type, order> >&& os_pc  )
        {
            double step = derived().step_length();
            const double evaluation_point = os_pc.front().evaluation_point;
            //std::cout << "propagate_once::pre-step :\n";
            const auto step_result = derived().call_stepper().step( std::move( os_pc.front() ), step );
            os_pc.front() = step_result.first;
            
            step = os_pc.front().evaluation_point - evaluation_point;
                
            for(auto iterator = os_pc.begin() + 1  ; iterator != os_pc.end()  ; ++iterator ){
                *iterator = derived().call_stepper().force_step( std::move(*iterator), step);
            }
            
//            std::cout << "propagate_once::step : " << step << '\n';
//            for(auto & sigma : os_pc){
//                std::cout << "sigma : (";
//                std::cout << sigma.state( details::order_tag<0>{} )(0,0) << ", " << sigma.state( details::order_tag<0>{} )(1,0) << ") -- (";
//                std::cout << sigma.state( details::order_tag<1>{} )(0,0) << ", " << sigma.state( details::order_tag<1>{} )(1,0) << ") -- ";
//                std::cout << sigma.evaluation_point << '\n';
//            }
//            std::cout << "propagate_once::error : " << std::setprecision(16) << step_result.second << '\n';
            
            //optimize next step
            auto new_step_length = derived().call_stepper().optimize_step_length(step, step_result.second);
            derived().step_length() = ( new_step_length > derived().max_step_length() ) ? derived().max_step_length() : new_step_length ;
               // std::cout << "propagate_once::next_step_length : " << derived().step_length() << '\n';
            
            return std::move(os_pc);
        }
        
    private:
        Derived& derived() { return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
    };

    
    
    
    template<class Derived>
    struct kalman_filter{
        
        using state = typename filter_traits<Derived>::state;
        using state_vector = typename underlying<state>::vector;
        using state_covariance = typename underlying<state>::covariance;
        
        
        
    public:
        //template around candidate + enbale if goes deeper ?
        template< class Candidate,
                  class Enabler = std::enable_if_t< is_given_dimension_correct<
                                                             typename underlying<std::decay_t<Candidate>>::measurement_matrix::type,
                                                             column_tag,
                                                             state_vector::vector_dimension
                                                                          >::value
                                                  >
                >
        double compute_chisquared( const state& s_p,
                                       const Candidate& candidate_p ) const
        {
            auto residual_vector = expr::compute(candidate_p.vector - candidate_p.measurement_matrix * s_p.vector);
            
            auto residual_covariance = expr::compute(candidate_p.covariance + candidate_p.measurement_matrix * s_p.covariance * transpose(candidate_p.measurement_matrix));
            //std::cout << "---- state_covariance ----\n" << s_p.covariance;
            //std::cout << "---- candidate_covariance ----\n" << candidate_p.covariance;
//            std::cout << "---- residual_vector ----\n" << residual_vector;
//            std::cout << "---- residual_covariance ----\n" << residual_covariance;
            
//            std::cout << "---- final_chisquared: "<< (make_transpose(residual_vector) * residual_covariance * residual_vector)(0,0) <<" ----\n";
            return {expr::compute( transpose( residual_vector ) * form_inverse(std::move(residual_covariance)) * residual_vector)}; //not pretty
        }
        
        double compute_distance( state const& ps_p,
                                 state const& cs_p ) const
        {
            auto residual_vector = expr::compute(cs_p.vector - ps_p.vector);
            
            auto residual_covariance = expr::compute(cs_p.covariance + ps_p.covariance);
            //std::cout << "---- state_covariance ----\n" << s_p.covariance;
            //std::cout << "---- candidate_covariance ----\n" << candidate_p.covariance;
           // std::cout << "---- residual_covariance ----\n" << residual_covariance;
            
//            std::cout << "---- final_chisquared: "<< (make_transpose(residual_vector) * residual_covariance * residual_vector)(0,0) <<" ----\n";
            return {expr::compute( transpose( residual_vector ) * form_inverse(std::move(residual_covariance)) * residual_vector)}; //not pretty
        }
        
        
        
        
        //should probably check everything or nothing, but state to measurement dimensionality error is the most probable
        template< class Candidate,
                  class Enabler = std::enable_if_t< is_given_dimension_correct< typename underlying<std::decay_t<Candidate>>::measurement_matrix::type,
                                                                           column_tag,
                                                                           state_vector::vector_dimension >::value
                                                  >
                >
        state correct_state( state s_p,
                             Candidate&& candidate_p ) const
        {
            //residual propagated state

            
            auto residual_vector = expr::compute( candidate_p.vector - candidate_p.measurement_matrix * s_p.vector );
            auto residual_covariance = expr::compute( candidate_p.covariance + candidate_p.measurement_matrix * s_p.covariance * transpose(candidate_p.measurement_matrix) );
            auto gain = expr::compute( s_p.covariance * transpose(candidate_p.measurement_matrix) * form_inverse(residual_covariance) );
            
            //corrected state vector/covariance
            auto csv = make_state_vector( expr::compute( s_p.vector + gain * residual_vector ) );
            auto csc = make_state_covariance( expr::compute( s_p.covariance - gain * candidate_p.measurement_matrix * s_p.covariance ) ); //room for optimization
        
        
            return {
                     evaluation_point{s_p.evaluation_point} ,
                     std::move(csv),
                     std::move(csc),
                    };
        }
        

    };
    
    

    
    
    
    
}//namespace details
    

namespace details{
    

    
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
    using details::ukf_propagator< ukf >::propagate_while;
    using details::ukf_propagator< ukf >::propagate_once;
    using details::sigma_handler< ukf >::generate_propagated_state;
    using details::kalman_filter< ukf >::correct_state;
    
    
private:
    Stepper stepper_m;
    double step_m = 1e-3;
    const double maxStepLength_m{150};
    
public:
    const Stepper& call_stepper() const {return stepper_m;}
    Stepper& call_stepper() {return stepper_m;}
    double& step_length() {return step_m;}
    const double& step_length() const {return step_m;}
    double max_step_length() const {return maxStepLength_m;}
    
public:
    template< class Stepper_,
              class CoherenceChecker = std::enable_if_t< details::is_dimension_coherent< State, Stepper_>::value >   >
    constexpr ukf(Stepper_&& stepper_p) : stepper_m{std::move(stepper_p)} {}
    
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
