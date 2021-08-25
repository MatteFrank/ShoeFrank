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


template<class T> struct underlying{ using type = std::decay_t<T>; };


// ----------------- state_vector_impl ---------------------------


template<class Matrix> struct state_vector_impl {};
template< template<std::size_t, std::size_t> class Matrix, std::size_t Size >
struct state_vector_impl< Matrix<Size, 1> >
{
    static constexpr std::size_t vector_dimension = Size ;
    using type = Matrix<Size, 1>;
    
    type vector;
    
    
};


// ----------------- state_covariance_impl ---------------------------

template<class Matrix> struct state_covariance_impl {};
template< template<std::size_t, std::size_t> class Matrix, std::size_t Size >
struct state_covariance_impl< Matrix<Size, Size> >
{
    static constexpr std::size_t covariance_dimension = Size  ;
    using type = Matrix<Size, Size>;
    
    type covariance;
};

// ----------------- state_impl ---------------------------

struct evaluation_point
{
    double evaluation_point;
};


namespace details {
    template< class Vector, class Covariance >
    struct are_dimensions_coherent_impl< state_vector_impl<Vector>,
                                         state_covariance_impl<Covariance>,
                                         std::enable_if_t< state_vector_impl<Vector>::vector_dimension == state_covariance_impl<Covariance>::covariance_dimension >  > :
    std::true_type {};
} //namespace details



template<class Vector, class Covariance>
struct state_impl : aggregator< evaluation_point,
                                state_vector_impl<Vector>,
                                state_covariance_impl<Covariance>   >
{
    static_assert( details::are_dimensions_coherent< state_vector_impl<Vector>,
                                                     state_covariance_impl<Covariance> >::value , "cannot create state_impl if dimension of Vector and Covariance do not match"  );

    constexpr state_impl(evaluation_point&& ep_p, state_vector_impl<Vector>&& sv_p, state_covariance_impl<Covariance>&& sc_p ) :
        aggregator< evaluation_point,
                    state_vector_impl<Vector>,
                    state_covariance_impl<Covariance>   > { std::move(ep_p), std::move(sv_p), std::move(sc_p)  }
    {}
};




template< class Vector,
          class Covariance  >
struct underlying< state_impl<Vector, Covariance> >
{
    using vector = state_vector_impl<Vector>;
    using covariance = state_covariance_impl<Covariance>;
};


// ----------------- candidate_impl ---------------------------

template< class T > struct measurement_matrix_impl{};
template< template<std::size_t, std::size_t> class Matrix, std::size_t NRows, std::size_t NCols >
struct measurement_matrix_impl< Matrix< NRows, NCols > >
{
    static constexpr std::size_t column_dimension = NCols ;
    static constexpr std::size_t row_dimension = NRows ;
    using type = Matrix<NRows, NCols>;
    
    type measurement_matrix;
};



template<class T>
struct data_handle
{
    const T* data;
};


namespace details {
    template< class Vector, class Matrix >
    struct are_dimensions_coherent_impl< state_vector_impl<Vector>,
                                         measurement_matrix_impl<Matrix>,
                                         std::enable_if_t< state_vector_impl<Vector>::vector_dimension == measurement_matrix_impl<Matrix>::row_dimension >  > :
    std::true_type {};
} //namespace details


template< class Vector,
          class Covariance,
          class Matrix,
          class Data>
struct candidate_impl : aggregator< state_vector_impl<Vector>,
                                    state_covariance_impl<Covariance>,
                                    measurement_matrix_impl<Matrix>,
                                    data_handle<Data> >
{
    static_assert( details::are_dimensions_coherent< state_vector_impl<Vector>,
                                                     state_covariance_impl<Covariance> >::value , "cannot create candidate_impl if dimension of Vector and Covariance do not match"  );
    static_assert( details::are_dimensions_coherent< state_vector_impl<Vector>,
                                                     measurement_matrix_impl<Matrix> >::value , "cannot create candidate_impl if dimension of Vector and Matrix do not match"  );
    
    constexpr candidate_impl( state_vector_impl<Vector>&& sv_p, state_covariance_impl<Covariance>&& sc_p, measurement_matrix_impl<Matrix>&& mm_p, data_handle<Data>&& dh_p ) :
        aggregator< state_vector_impl<Vector>,
                    state_covariance_impl<Covariance>,
                    measurement_matrix_impl<Matrix>,
                    data_handle<Data> > { std::move(sv_p), std::move(sc_p), std::move(mm_p), std::move(dh_p)  }
    {}
};


template< class Vector,
          class Covariance,
          class Matrix,
          class Data>
struct underlying< candidate_impl<Vector, Covariance, Matrix, Data> >
{
    using measurement_matrix = measurement_matrix_impl<Matrix>;
    using vector = state_vector_impl<Vector>;
    using covariance = state_covariance_impl<Covariance>;
    using data_type = Data;
};






// ----------------- enriched_candidate_impl ---------------------------

struct chisquared{
    double chisquared_predicted;
    double chisquared_corrected;
    double distance;
};


template< class Candidate>
using enriched_candidate_impl = aggregator< Candidate, chisquared >;

template< class Vector,
          class Covariance,
          class Matrix,
          class Data>
struct underlying< enriched_candidate_impl< candidate_impl<Vector, Covariance, Matrix, Data> > >
{
    using candidate =  candidate_impl<Vector, Covariance, Matrix, Data>;
    using measurement_matrix = measurement_matrix_impl<Matrix>;
    using vector = state_vector_impl<Vector>;
    using covariance = state_covariance_impl<Covariance>;
    using data_type = Data;
};


// ----------------- corrected_state_impl ---------------------------




template< class Vector,
          class Covariance>
using corrected_state_impl = aggregator< state_impl<Vector, Covariance> ,
                                         chisquared >;


template< class Vector,
class Covariance  >
struct underlying< corrected_state_impl<Vector, Covariance> >
{
    using state = state_impl<Vector, Covariance>;
};



// ----------------- used_in_full_state? ---------------------------
struct step_register
{
    double step_length;
};

// ----------------- makers ---------------------------

namespace details{
    
   
    
//    ----------------- sv ---------------------------
    
    //trick because matrix is not aggregate yet so need to create ini_list
    template<template<std::size_t, std::size_t> class Matrix, std::size_t N, std::size_t ... Indices>
    auto make_state_vector_impl( Matrix<N, 1> m_p, std::index_sequence<Indices...> ) -> state_vector_impl< Matrix<N,1> >
    {
        return { {std::move(m_p(Indices, 0))... }};
    }
    
    template< template<std::size_t, std::size_t> class Matrix,
              std::size_t N,
              std::size_t ... Indices>
    auto make_state_vector_impl( state_vector_impl< Matrix<N,1> > sv_p ,
                                 Matrix<N, 1> m_p,
                                 std::index_sequence<Indices...> )
        -> state_vector_impl< Matrix<2*N,1> >
    {
        return { { std::move( sv_p.vector(Indices, 0))... , std::move(m_p(Indices, 0))...  }};
    }
    
//    ----------------- sc ---------------------------
    
    template<template<std::size_t, std::size_t> class Matrix, std::size_t N, std::size_t ... Indices>
    auto make_state_covariance_impl( Matrix<N, N> m_p, std::index_sequence<Indices...> ) -> state_covariance_impl< Matrix<N,N> >
    {
        return {{ std::move( m_p.data_mc[Indices] )... }};
    }

    
    //    ----------------- other ---------------------------
    
    
    template<std::size_t Order, class Type> struct state_vector_maker{};
    
    template<template<std::size_t, std::size_t> class Matrix, std::size_t NRows>
    struct state_vector_maker<2, Matrix<NRows, 1>>
    {
        auto operator()(operating_state< Matrix<NRows, 1>, 2 > os_p)
        {
            return make_state_vector( form_combination( os_p.state(order_tag<0>{}),
                                               os_p.state(order_tag<1>{}),
                                               row_tag{}                     ) );
        }
    };
    
    
    
    
    template<std::size_t Order> struct operating_state_maker{};
    
    template<>
    struct operating_state_maker<2>
    {
        template<class StateVector, class StateCovariance>
        auto operator()(state_impl<StateVector, StateCovariance> state_p)
        {
            auto halves = split_half( std::move(state_p.vector), details::row_tag{});
            return operating_state<decltype(halves.first), 2>{ std::move(state_p).evaluation_point,
                                                               { std::move(halves.first), std::move(halves.second)}     };
        }
    };
    
   

} //namespace details


// ----------------- os ---------------------------

template<std::size_t Order, class StateVector, class StateCovariance>
auto make_operating_state(state_impl<StateVector, StateCovariance> state_p)
{
    return details::operating_state_maker<Order>{}( std::move(state_p) );
}

// ----------------- sc ---------------------------


template<template <std::size_t, std::size_t> class Matrix, std::size_t N>
auto make_state_covariance( Matrix<N, N> m_p ) -> state_covariance_impl< Matrix<N,N> >
{
    return details::make_state_covariance_impl(std::move(m_p ), std::make_index_sequence<N*N>{});
}


template<template <std::size_t, std::size_t> class Matrix, std::size_t N>
auto make_state_covariance( state_covariance_impl< Matrix<N, N> > sc_p, Matrix<N, N> m_p ) -> state_covariance_impl< Matrix<2*N, 2*N> >
{
    return {combine_diagonals( std::move(sc_p.covariance), std::move(m_p) )};
}

// ----------------- sv ---------------------------

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


template<template <std::size_t, std::size_t> class Matrix, std::size_t N>
auto make_state_vector( state_vector_impl< Matrix<N,1> > sv_p,
                        Matrix<N, 1> m_p )
    -> state_vector_impl< Matrix<2*N,1> >
{
    return details::make_state_vector_impl( std::move(sv_p),
                                            std::move(m_p ),
                                            std::make_index_sequence<N>{});
}

// ----------------- cs ---------------------------

template<class Vector, class Covariance>
auto make_corrected_state( state_impl<Vector, Covariance> s_p, chisquared c_p ) -> corrected_state_impl<Vector, Covariance>
{
    using vector = typename underlying< state_impl<Vector, Covariance> >::vector;
    using covariance = typename underlying< state_impl<Vector, Covariance> >::covariance;
    
    return {
                {
                    evaluation_point{s_p.evaluation_point},
                    vector{std::move(s_p.vector)},
                    covariance{std::move(s_p.covariance)}
                },
                chisquared{c_p}
            };
}




// ----------------- s ---------------------------
template<class Vector, class Covariance>
auto make_state( corrected_state_impl<Vector, Covariance> s_p ) -> state_impl<Vector, Covariance>
{
    using vector = typename underlying< state_impl<Vector, Covariance> >::vector;
    using covariance = typename underlying< state_impl<Vector, Covariance> >::covariance;

    return {
        evaluation_point{s_p.evaluation_point},
        vector{ std::move(s_p.vector) },
        covariance{ std::move(s_p.covariance) }
    };
}



// ----------------- ec ---------------------------
template<class Vector, class Covariance, class Matrix, class Data>
auto make_enriched_candidate( candidate_impl<Vector, Covariance, Matrix, Data> candidate_p, chisquared c_p ) -> enriched_candidate_impl< candidate_impl<Vector, Covariance, Matrix, Data> >
{
    
    using vector = typename underlying< candidate_impl<Vector, Covariance, Matrix, Data> >::vector;
    using covariance = typename underlying< candidate_impl<Vector, Covariance, Matrix, Data> >::covariance;
    using measurement_matrix = typename underlying< candidate_impl<Vector, Covariance, Matrix, Data> >::measurement_matrix;
    
    return {
        {
            vector{std::move(candidate_p.vector)},
            covariance{std::move(candidate_p.covariance)},
            measurement_matrix{std::move(candidate_p.measurement_matrix)},
            data_handle<Data>{std::move(candidate_p.data)}
        },
        std::move(c_p)
    };
}



//template<std::size_t N, class T>
//auto make_state_vector( std::array<T, N> array_p )
//{
//    return state_vector{ details::make_state_vector_impl(std::move(array_p), std::make_index_sequence<N>{} ) };
//}



#endif /* state_hpp */
