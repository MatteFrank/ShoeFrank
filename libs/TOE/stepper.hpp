//
//File      : stepper.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 26/11/2019
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef stepper_hpp
#define stepper_hpp

#include <numeric>
#include <cmath>
#include <array>
#include <iostream>


#include "utility_types.hpp"
#include "state.hpp"
#include "expr.hpp"

namespace details{
    
    //will need some adaptation for matrices
    template<std::size_t NRows, template<std::size_t, std::size_t > class Matrix>
    inline double computation_error( Matrix<NRows, 1 > const& estimation_p, Matrix<NRows, 1 > const& correction_p)
    {
        Matrix<NRows, 1> difference = estimation_p - correction_p;
        // std::cout << "\nestimation:\n" << estimation_p << "correction: \n" << correction_p;
        // std::cout << "\ndifference:\n" << difference;
        auto temp = std::sqrt( std::inner_product(difference.data().begin(), difference.data().end(), difference.data().begin(), 0) );
        //std::cout << "\nupon calculation error is: " << temp << '\n';
        return (temp < 1e-15 ? 1e-15 : temp );
    }
    
    inline double computation_error(double estimation_p, double correction_p)
    {
        return std::abs(estimation_p - correction_p);
    }
    
    
    template< class T,
              class U,
              std::size_t NT,
              std::size_t NU,
              template<class, std::size_t> class ContainerT,
              template<class, std::size_t> class ContainerU >
    struct inner_product_impl{};
    
    template< class T,
              class U,
              std::size_t N,
              template<class, std::size_t> class ContainerT,
              template<class, std::size_t> class ContainerU >
    struct inner_product_impl<T, U, N, N, ContainerT, ContainerU>
    {
        T operator()( const ContainerT<T, N>& t_pc,
                      const ContainerU<U, N>& u_pc,
                      std::size_t index_p = N )
        {
            auto end = t_pc.begin();
            std::advance(end, index_p);
            return std::inner_product(t_pc.begin(), end , u_pc.begin(), T{});
        }
    };
    
    template< class T,
              class U,
              std::size_t NT,
              std::size_t NU,
              template<class, std::size_t> class ContainerT >
    struct inner_product_impl<T, U, NT, NU, ContainerT, lower_triangular_array>
    {
        T operator()( const ContainerT<T, NT>& t_pc,
                      const lower_triangular_array<U, NU>& lwt_pc,
                      std::size_t index_p = NT )
        {
            auto end = t_pc.begin();
            std::advance(end, index_p);
            
            std::size_t offset{0};
            while(index_p!=0){ offset += --index_p; }
            
            auto start = lwt_pc.data().begin();
            std::advance(start, offset);
            return std::inner_product(t_pc.begin(), end, start, T{});
        }
    };
    
    
    //maybe check tag on multiplicativity if T and U ?
    template< class T, class U, std::size_t NT, std::size_t NU,
              template<class, std::size_t> class ContainerT,
              template<class, std::size_t> class ContainerU >
    T inner_product( const ContainerT<T, NT>& t_pc,
                     const ContainerU<U, NU>& u_pc,
                     std::size_t index_p = NT)
    {
        return inner_product_impl<T, U, NT, NU, ContainerT, ContainerU>{}(t_pc, u_pc, index_p );
    }
    
    
    
    
    
    template<class OperatingType, std::size_t Dimension >
    struct operating_state_extension
    {
        double step;
        std::array<OperatingType, Dimension> evaluation;
    };
    
    template<class OperatingType, std::size_t Order, std::size_t Dimension>
    using extended_operating_state = aggregator< operating_state<OperatingType, Order>,
                                                 operating_state_extension<OperatingType, Dimension> >;
    
    
    template<class OperatingType, std::size_t Order, std::size_t Dimension>
    auto make_extended_operating_state( operating_state<OperatingType, Order> os_p,
                                        operating_state_extension<OperatingType, Dimension> ose_p )
         -> extended_operating_state<OperatingType, Order, Dimension>
    {
        return {std::move(os_p), std::move(ose_p)};
    }
    
} //namespace details





namespace details {
    
    
    // ------------------- evaluator ------------------------
    
    template<class Derived, std::size_t Order> struct evaluator{};
    
    template<class Derived>
    struct evaluator< Derived, 2>
    {
        using type = typename stepper_traits<Derived>::operating_type;
        using evaluation_t = std::array< type,
                                         stepper_traits<Derived>::evaluation_stage   >;
        
        
        public :
        evaluation_t compute_evaluation( operating_state<type, 2> os_p, double step_p ) const
        {
            evaluation_t evaluation_c{};
            
            for(std::size_t index{0} ; index < evaluation_c.size() ; ++index ){
                operating_state<type, 2> os;
                os.evaluation_point = os_p.evaluation_point + step_p * derived().data().global_delay[index];
                os.state(order_tag<0>{}) = os_p.state(order_tag<0>{}) +
                                           step_p * derived().data().global_delay[index] * os_p.state(order_tag<1>{}) +
                                           step_p * step_p * inner_product( evaluation_c,
                                                                            derived().data().partial_delay(order_tag<0>{}),
                                                                            index );
                os.state(order_tag<1>{}) = os_p.state(order_tag<1>{}) +
                                           step_p * inner_product( evaluation_c,
                                                                   derived().data().partial_delay(order_tag<1>{}),
                                                                   index );
                evaluation_c[index] = derived().ode( os );
            }
            
            return evaluation_c;
        }
        
    private:
        Derived& derived(){ return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
        
    };
    
    
    
    template<class Derived>
    struct evaluator< Derived, 1>
    {
        using type = typename stepper_traits<Derived>::operating_type;
        using evaluation_t = std::array< type,
                                         stepper_traits<Derived>::evaluation_stage   >;
        
        
        public :
        evaluation_t compute_evaluation( operating_state<type, 1> os_p, double step_p ) const
        {
            evaluation_t evaluation_c{};
            
            for(std::size_t index{0} ; index < evaluation_c.size() ; ++index ){
                operating_state<type, 1> os;
                os.evaluation_point = os_p.evaluation_point + step_p * derived().data().global_delay[index];
                os.state(order_tag<0>{}) = os_p.state(order_tag<0>{}) +
                                           step_p * inner_product( evaluation_c,
                                                                   derived().data().partial_delay(order_tag<0>{}),
                                                                   index );
                evaluation_c[index] = derived().ode( os );
            }
            
            return evaluation_c;
        }
        
    private:
        Derived& derived(){ return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
        
    };
    
    
    
    
    
    // -------------------------- solver -------------------------------

    template<class Derived, std::size_t Order, class SteppingPolicyTag> struct solver{};
    
    template<class Derived>
    struct solver<Derived, 2, details::adaptable_step_tag >
    {
        
        constexpr static std::size_t dimension = stepper_traits<Derived>::evaluation_stage;
        using type = typename stepper_traits<Derived>::operating_type;
        
        
    public:
        template<class PurposeTag>
        type compute_solution( const extended_operating_state<type, 2, dimension>& eos_p,
                               order_tag<0>,
                               PurposeTag   ) const
        {
            return eos_p.state(order_tag<0>{}) +
                   eos_p.step * eos_p.state(order_tag<1>{}) +
                   eos_p.step * eos_p.step * inner_product( eos_p.evaluation,
                                                            derived().data().weight( order_tag<0>{},
                                                                                     PurposeTag{} )      );
        }
        
        type compute_solution( const extended_operating_state<type, 2, dimension>& eos_p,
                               order_tag<1> ) const
        {
            return eos_p.state(order_tag<1>{}) +
                   eos_p.step * inner_product( eos_p.evaluation,
                                               derived().data().weight( order_tag<1>{} )   );
        }
        
        
    private:
        Derived& derived(){ return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
    };
    
    
    template<class Derived>
    struct solver<Derived, 2, details::fixed_step_tag>
    {
        constexpr static std::size_t dimension = stepper_traits<Derived>::evaluation_stage;
        using type = typename stepper_traits<Derived>::operating_type;
        
    public:
        type compute_solution( const extended_operating_state<type, 2, dimension>& eos_p,
                               order_tag<0>) const
        {
            return eos_p.state(order_tag<0>{}) +
                       eos_p.step * eos_p.state(order_tag<1>{}) +
                           eos_p.step * eos_p.step * inner_product( eos_p.evaluation,
                                                                    derived().data().weight( order_tag<0>{} )      );
        }
        
        type compute_solution( const extended_operating_state<type, 2, dimension>& eos_p,
                               order_tag<1> ) const
        {
            return eos_p.state(order_tag<1>{}) +
                      eos_p.step * inner_product( eos_p.evaluation,
                                                  derived().data().weight( order_tag<1>{} )   );
        }
        
        
    private:
        Derived& derived(){ return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
    };
    
    
    
    
    
    template<class Derived>
    struct solver<Derived, 1, details::adaptable_step_tag >
    {
        
        constexpr static std::size_t dimension = stepper_traits<Derived>::evaluation_stage;
        using type = typename stepper_traits<Derived>::operating_type;
        
        
    public:
        template<class PurposeTag>
        type compute_solution( const extended_operating_state<type, 1, dimension>& eos_p,
                               order_tag<0>,
                               PurposeTag   ) const
        {
            return eos_p.state(order_tag<0>{}) +
                   eos_p.step * inner_product( eos_p.evaluation,
                                               derived().data().weight( order_tag<0>{},
                                                                        PurposeTag{} )      );
        }

        
    private:
        Derived& derived(){ return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
    };
    
    
    
    
    
    //  ----------------- Stepping policy -------------------

    template<class Derived, std::size_t Order, class StepTag> struct stepping_policy_impl{};
    
    template<class Derived>
    struct stepping_policy_impl< Derived, 2, adaptable_step_tag >
    {
        
        static constexpr std::size_t dimension = stepper_traits<Derived>::evaluation_stage;
        using type = typename stepper_traits<Derived>::operating_type;
        static constexpr std::size_t degree = stepper_traits<Derived>::data::higher_degree + 1;

        

        class error_checker
        {
        public:
            
            explicit error_checker(double value_p) : value_m{ (value_p < 1e-15) ? 1e-15 : value_p }{}
            
            operator double() const { return value_m; }
            
        private:
            double value_m;
        };
        
        
    private:
        double tolerance_m{1e-5};
        
    public:
        void specify_tolerance(double tolerance_p){ tolerance_m = tolerance_p ;}
        
        
    public:
        auto step(operating_state<type, 2>&& os_p, double step_p) const
                -> std::pair< operating_state<type, 2>, double >
        {
//            std::cout << "Stepper::step " << step_p << '\n';
            
            bool isToleranceReached = false;
            auto eos = make_extended_operating_state( std::move(os_p),
                                                     operating_state_extension<type, dimension>{step_p, {}} );
            
            double error{0};
            
            while(!isToleranceReached){
                
                eos.evaluation = derived().compute_evaluation(eos, step_p);
                auto estimate = derived().compute_solution(eos, details::order_tag<0>{}, estimation_tag{});
                auto correction = derived().compute_solution(eos, details::order_tag<0>{}, correction_tag{});
//                std::cout << "Estimate: \n" << estimate << " - Correction: \n" << correction << '\n';
                auto local_error_estimate = details::computation_error( estimate, correction );
               
                
                isToleranceReached = local_error_estimate <= tolerance_m;
//                std::cout << "step_error: " << local_error_estimate << '\n';
                
                if(!isToleranceReached){
                    eos.step = optimize_step_length(eos.step, local_error_estimate) ;
                }
                else{
                    error = error_checker{ local_error_estimate };
//                    std::cout << "Step length:" << eos.step << ", error: "<< error <<'\n';
                    eos.state(order_tag<0>{}) = correction;
                }
                
            }
            
            eos.state( order_tag<1>{} ) = derived().compute_solution(eos, details::order_tag<1>{});

            eos.evaluation_point += step_p;
            return std::make_pair(std::move(eos), error);
        }
        
        operating_state<type, 2> force_step(operating_state<type, 2>&& os_p, double step_p) const
        {
            //std::cout << "stepper::force_step";
            auto eos = make_extended_operating_state( std::move(os_p),
                                                      operating_state_extension<type, dimension>{step_p, {}} );
            
            eos.evaluation = derived().compute_evaluation(eos, step_p);
            eos.state( order_tag<0>{} ) = derived().compute_solution(eos, details::order_tag<0>{}, correction_tag{});
            eos.state( order_tag<1>{} ) = derived().compute_solution(eos, details::order_tag<1>{});
            eos.evaluation_point += step_p;
            
            return  eos;
        }
        
        double optimize_step_length(double step_p, double local_error) const
        {
            return step_p * 0.9 * std::pow(tolerance_m/local_error, 1./degree);
        }
        
    private:
        Derived& derived(){ return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
    };
    
    
    
    template<class Derived>
    struct stepping_policy_impl< Derived, 2, details::fixed_step_tag >{
        
        static constexpr std::size_t dimension = stepper_traits<Derived>::evaluation_stage;
        using type = typename stepper_traits<Derived>::operating_type;
        

    public:
        
        auto step(operating_state<type, 2>&& os_p, double step_p) const
            -> std::pair< operating_state<type, 2>, double >
        {
            auto os = force_step( std::move(os_p), step_p );
            return {std::move(os), 0};
        }
        
        operating_state<type, 2> force_step(operating_state<type, 2>&& os_p, double step_p) const
        {
            //std::cout << "stepper::force_step";
            auto eos = make_extended_operating_state( std::move(os_p),
                                                      operating_state_extension<type, dimension>{step_p, {}} );
            
            eos.evaluation = derived().compute_evaluation(eos, step_p);
            eos.state( order_tag<0>{} ) = derived().compute_solution( eos, details::order_tag<0>{} );
            eos.state( order_tag<1>{} ) = derived().compute_solution( eos, details::order_tag<1>{} );
            eos.evaluation_point += step_p;
            
            return  eos;
        }
        
        void step()
        {
            auto evaluation_c = derived().compute_evaluation();
            update_state(evaluation_c);
        }
        
        
        //for compatibility reasons + no constexpr if
        double optimize_step_length(double /*step_p*/, double /*local_error*/) const
        {
            return 0;
        }

        
    private:
        Derived& derived(){ return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
    };
    
    
    
    
    
    template<class Derived>
    struct stepping_policy_impl< Derived, 1, adaptable_step_tag >
    {
        
        static constexpr std::size_t dimension = stepper_traits<Derived>::evaluation_stage;
        using type = typename stepper_traits<Derived>::operating_type;
        static constexpr std::size_t degree = stepper_traits<Derived>::data::higher_degree + 1;

        

        class error_checker
        {
        public:
            
            explicit error_checker(double value_p) : value_m{ (value_p < 1e-15) ? 1e-15 : value_p }{}
            
            operator double() const { return value_m; }
            
        private:
            double value_m;
        };
        
        
    private:
        double tolerance_m{1e-5};
        
    public:
        void specify_tolerance(double tolerance_p){ tolerance_m = tolerance_p ;}
        
        
    public:
        auto step(operating_state<type, 1>&& os_p, double step_p) const
                -> std::pair< operating_state<type, 1>, double >
        {
//            std::cout << "Stepper::step " << step_p << '\n';
            
            bool isToleranceReached = false;
            auto eos = make_extended_operating_state( std::move(os_p),
                                                     operating_state_extension<type, dimension>{step_p, {}} );
            
            double error{0};
            
            while(!isToleranceReached){
                
                eos.evaluation = derived().compute_evaluation(eos, step_p);
                auto estimate = derived().compute_solution(eos, details::order_tag<0>{}, estimation_tag{});
                auto correction = derived().compute_solution(eos, details::order_tag<0>{}, correction_tag{});
//                std::cout << "Estimate: \n" << estimate << " - Correction: \n" << correction << '\n';
                auto local_error_estimate = details::computation_error( estimate, correction );
               
                
                isToleranceReached = local_error_estimate <= tolerance_m;
//                std::cout << "step_error: " << local_error_estimate << '\n';
                
                if(!isToleranceReached){
                    eos.step = optimize_step_length(eos.step, local_error_estimate) ;
                }
                else{
                    error = error_checker{ local_error_estimate };
//                    std::cout << "Step length:" << eos.step << ", error: "<< error <<'\n';
                    eos.state(order_tag<0>{}) = correction;
                }
                
            }

            eos.evaluation_point += step_p;
            return std::make_pair(std::move(eos), error);
        }
        
        operating_state<type, 1> force_step(operating_state<type, 1>&& os_p, double step_p) const
        {
            //std::cout << "stepper::force_step";
            auto eos = make_extended_operating_state( std::move(os_p),
                                                      operating_state_extension<type, dimension>{step_p, {}} );
            
            eos.evaluation = derived().compute_evaluation(eos, step_p);
            eos.state( order_tag<0>{} ) = derived().compute_solution(eos, details::order_tag<0>{}, correction_tag{});
            eos.evaluation_point += step_p;
            
            return  eos;
        }
        
        double optimize_step_length(double step_p, double local_error) const
        {
            return step_p * 0.9 * std::pow(tolerance_m/local_error, 1./degree);
        }
        
    private:
        Derived& derived(){ return static_cast<Derived&>(*this); }
        const Derived& derived() const { return static_cast<const Derived&>(*this); }
    };
    
    
    
    
    
    
    
} //namespace details


template<class Derived>
struct worker : details::evaluator< Derived,
                                    details::stepper_traits<Derived>::order >,
                details::solver< Derived,
                                 details::stepper_traits<Derived>::order,
                                 typename details::stepper_traits<Derived>::stepping_policy >   {};



template<class Derived>
struct stepping_policy : details::stepping_policy_impl< Derived,
                                                        details::stepper_traits<Derived>::order,
                                                        typename details::stepper_traits<Derived>::stepping_policy >  {};



namespace details{
    template<class Callback, class Data, class Enabler>
    struct is_order_coherent_impl : std::false_type {};
    
    template<class Callback, class Data>
    struct is_order_coherent_impl< Callback, Data,
                                   std::enable_if_t<Callback::order == Data::order>  >
                                 : std::true_type {};
    
    template<class Callback, class Data>
    using is_order_coherent = is_order_coherent_impl<Callback, Data, void>;
}    //namespace details







template< class Callable, class Data >
struct stepper : public stepping_policy< stepper<Callable, Data> >,
                        worker< stepper<Callable, Data> >
{
    using stepping_policy<stepper>::step;
    using stepping_policy<stepper>::force_step;
    
public:
    Callable ode;
    
private:
    const Data data_mc;
public:
    const Data& data() const {return data_mc;}
    
public:
    template< class Callable_ = Callable,
              class MoveAllower = std::enable_if_t< !std::is_same<std::decay_t<Callable_>, stepper>::value >,
              class OrderChecker = std::enable_if_t< details::is_order_coherent<Callable_, Data>::value >>
    constexpr stepper(Callable&& f_p) : ode{ std::forward<Callable>(f_p) } {}
};



namespace details{
    
    template<class T> struct underlying_size{};
    template<class Type, std::size_t Size>
    struct underlying_size< std::array<Type, Size> >
    {
        static constexpr std::size_t value = Size;
    };
    
    template<>
    struct underlying_size< double >
    {
        static constexpr std::size_t value = 1;
    };
    
    template<template<std::size_t, std::size_t > class Matrix, std::size_t NRows, std::size_t NCols>
    struct underlying_size< Matrix<NRows, NCols> >
    {
        static constexpr std::size_t value = NRows * NCols;
    };
    
    template<class Callback, class Data>
    struct stepper_traits< stepper<Callback, Data> >
    {
        static constexpr std::size_t evaluation_stage = Data::evaluation_stage;
        using data = Data;
        static constexpr std::size_t order = Data::order;
        
        using stepping_policy = typename Data::stepping_policy;
        
        using operating_type = typename Callback::operating_type;
        static constexpr std::size_t operating_dimension = underlying_size<operating_type>::value;
    };
    
}

template<class Data, class Callable>
constexpr auto make_stepper(Callable c_p) -> stepper<Callable, Data>
{
    return {std::move(c_p)};
}







template<class OperatingType, std::size_t Order, class Callable>
struct ode{
    using operating_type = OperatingType;
    static constexpr std::size_t order = Order;
    
private:
    Callable c_m;
    
public:
    constexpr ode(Callable&& c_p) : c_m{std::forward<Callable>(c_p)} {}
    
    template< class T >
    constexpr operating_type operator()(T&& t_p) const
    {
        return c_m(std::forward<T>(t_p));
    }
    
    Callable& model()
    {
        return c_m;
    }
};




template<class OperatingType, std::size_t Order, class Callable>
auto make_ode(Callable&& c_p) -> ode<OperatingType, Order, Callable>
{
    return {std::forward<Callable>(c_p)};
}

#endif /* stepper_hpp */
