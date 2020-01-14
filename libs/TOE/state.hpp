//
//File      : state.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 26/11/2019
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef state_hpp
#define state_hpp

#include <stdio.h>
#include <array>

#include "utility_types.hpp"



template<class OperatingType, std::size_t Order>
struct operating_state
{
    double evaluation_point;
    std::array<OperatingType, Order> state_c;
    
    template<class OrderTag, class Enabler = std::enable_if_t< (OrderTag::value < Order) > >
    OperatingType& state( OrderTag ){ return state_c[ OrderTag::value ]; }
    
    template<class OrderTag, class Enabler = std::enable_if_t< (OrderTag::value < Order) > >
    const OperatingType& state( OrderTag ) const { return state_c[ OrderTag::value ]; }
};



template<class ... Components>
struct aggregator : Components...
{
    aggregator(Components&&... components_p) : Components(std::move(components_p))... {}
};




template<class Matrix> struct state_vector_impl {};
template< template<std::size_t, std::size_t> class Matrix, std::size_t Size >
struct state_vector_impl< Matrix<Size, 1> >
{
    static constexpr std::size_t vector_dimension = Size ;
    using type = Matrix<Size, 1>;
    
    type vector;
};




template<class Matrix> struct state_covariance_impl {};
template< template<std::size_t, std::size_t> class Matrix, std::size_t Size >
struct state_covariance_impl< Matrix<Size, Size> >
{
    static constexpr std::size_t covariance_dimension = Size * Size ;
    using type = Matrix<Size, Size>;
    
    type covariance;
};



struct evaluation_point
{
    double evaluation_point;
};



template<class StateVector, class StateCovariance>
using state_impl = aggregator< evaluation_point,
                               StateVector,
                               StateCovariance   >;


template<class T>
struct data_handle
{
    const T* data;
};

template< class Vector,
          class Covariance,
          class Matrix,
          class Data>
using candidate_impl = aggregator< Vector,
                                   Covariance,
                                   Matrix,
                                   data_handle<Data> >;


struct chisquared{
    double chisquared;
};

template< class Vector,
          class Covariance>
using corrected_state_impl = aggregator< evaluation_point,
                                         Vector,
                                         Covariance,
                                         chisquared >;


namespace details{
    
    
    template<std::size_t Order> struct operating_state_maker{};
    
    template<>
    struct operating_state_maker<2>
    {
        template<class StateVector, class StateCovariance>
        auto operator()(state_impl<StateVector, StateCovariance> state_p)
        {
            auto halves = split_half( std::move(state_p.vector), details::row_tag{});
            return operating_state<decltype(halves.first), 2>{ {std::move(state_p).evaluation_point},
                                                               {std::move(halves.first), std::move(halves.second)}     };
        }
    };
    
    
    
    
    //trick because matrix is not aggregate yet so need to create ini_list
    template<template<std::size_t, std::size_t> class Matrix, std::size_t N, std::size_t ... Indices>
    auto make_state_vector_impl( Matrix<N, 1> m_p, std::index_sequence<Indices...> ) -> state_vector_impl< Matrix<N,1> >
    {
        return { {std::move(m_p(Indices, 0))... }};
    }
    
    template<template<std::size_t, std::size_t> class Matrix, std::size_t N, std::size_t ... Indices>
    auto make_state_covariance_impl( Matrix<N, N> m_p, std::index_sequence<Indices...> ) -> state_covariance_impl< Matrix<N,N> >
    {
        return {{ std::move( m_p.data()[Indices] )... }};
    }
    
    
    
    
    template<std::size_t Order, class Type> struct state_vector_maker{};
    
    template<template<std::size_t, std::size_t> class Matrix, std::size_t NRows>
    struct state_vector_maker<2, Matrix<NRows, 1>>
    {
        auto operator()(operating_state< Matrix<NRows, 1>, 2 > os_p)
        {
            return make_state_vector( combine( os_p.state(order_tag<0>{}),
                                               os_p.state(order_tag<1>{}),
                                               row_tag{}                     ) );
        }
    };
    
    
    
   

} //namespace details


template<std::size_t Order, class StateVector, class StateCovariance>
auto make_operating_state(state_impl<StateVector, StateCovariance> state_p)
{
    return details::operating_state_maker<Order>{}( std::move(state_p) );
}




template<template <std::size_t, std::size_t> class Matrix, std::size_t N>
auto make_state_covariance( Matrix<N, N> m_p ) -> state_covariance_impl< Matrix<N,N> >
{
    return details::make_state_covariance_impl(std::move(m_p ), std::make_index_sequence<N*N>{});
}



template<template <std::size_t, std::size_t> class Matrix, std::size_t N>
auto make_state_vector( Matrix<N, 1> m_p ) -> state_vector_impl< Matrix<N,1> >
{
    return details::make_state_vector_impl(std::move(m_p ), std::make_index_sequence<N>{});
}

template<class Type, std::size_t Order>
auto make_state_vector( operating_state<Type, Order> os_p )
{
    return details::state_vector_maker<Order, Type>{}( std::move(os_p) );
}



template<class Vector, class Covariance>
auto make_corrected_state( state_impl<Vector, Covariance> s_p, chisquared c_p ) -> corrected_state_impl<Vector, Covariance>
{
    return {
                std::move(s_p.evaluation_point),
                std::move(s_p.state_vector),
                std::move(s_p.state_covariance),
                std::move(c_p)
            };
}

//template<std::size_t N, class T>
//auto make_state_vector( std::array<T, N> array_p )
//{
//    return state_vector{ details::make_state_vector_impl(std::move(array_p), std::make_index_sequence<N>{} ) };
//}



#endif /* state_hpp */
