//
//File      : utility_types.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 27/11/2019
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef utility_types_h
#define utility_types_h

namespace details {

    
    struct correction_tag{};
    struct estimation_tag{};
    struct null_tag{};
    
    struct adaptable_step_tag{};
    struct fixed_step_tag{};
    
    template<std::size_t Order>
    struct order_tag{ static constexpr std::size_t value = Order; };
    
    

    struct row_tag{};
    struct column_tag{};
    struct diagonal_tag{};
    
    template<class T>
    struct stepper_traits{};
    
    template<class T>
    struct filter_traits{};
    
    
    template< class T, class Tag, std::size_t Size > struct is_given_dimension_correct : std::false_type {};
    
    template< template<std::size_t, std::size_t> class Matrix,
              std::size_t NRows,
              std::size_t Size>
    struct is_given_dimension_correct< Matrix<NRows, Size>, column_tag, Size> : std::true_type {};
    
    template< template<std::size_t, std::size_t> class Matrix,
              std::size_t NCols,
              std::size_t Size>
    struct is_given_dimension_correct< Matrix<Size, NCols>, row_tag, Size> : std::true_type {};
    
    
    template<class T1, class T2, class Enabler> struct are_dimensions_coherent_impl : std::false_type {};
    
    template<class T1, class T2> struct are_dimensions_coherent : are_dimensions_coherent_impl<T1, T2, void> {};
    
    
    
    
    template<class T, std::size_t Size>
    struct lower_triangular_array{
        const std::array<T, Size> data_mc;
        const std::array<T, Size>& data() const { return data_mc; }
    };
    
    
    
    template<class State, class Enabler>
    struct is_state_coherent_impl : std::false_type{};
    
    template<class State>
    struct is_state_coherent_impl< State,
                                   std::enable_if_t< State::vector_dimension == State::covariance_dimension >   > : std::true_type{};
    
    template<class State>
    using is_state_coherent = is_state_coherent_impl<State, void>;
    
} // namespace details




#endif /* utility_types_h */
