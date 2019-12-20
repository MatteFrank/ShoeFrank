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
    
    
    template<class T>
    struct stepper_traits{};
    
    template<class T>
    struct filter_traits{};
    
    
    template< class T, class Tag, std::size_t Size > struct has_correct_dimension : std::false_type {};
    
    template< template<std::size_t, std::size_t> class Matrix,
              std::size_t NRows,
              std::size_t Size>
    struct has_correct_dimension< Matrix<NRows, Size>, column_tag, Size> : std::true_type {};
    
    template< template<std::size_t, std::size_t> class Matrix,
              std::size_t NCols,
              std::size_t Size>
    struct has_correct_dimension< Matrix<Size, NCols>, row_tag, Size> : std::true_type {};
    
    
    
    template<class T, std::size_t Size>
    struct lower_triangular_array{
        const std::array<T, Size> data_mc;
        const std::array<T, Size>& data() const { return data_mc; }
    };
    
    
} // namespace details


//if extracted here candidate definition need to check ?
//nope -> kalman filter function
template< class T > struct measurement_matrix_impl{};
template< template<std::size_t, std::size_t> class Matrix, std::size_t NRows, std::size_t NCols >
struct measurement_matrix_impl< Matrix< NRows, NCols > >
{
    static constexpr std::size_t measurement_dimension = NRows ;
    using type = Matrix<NRows, NCols>;
    
    type measurement_matrix;
};


#endif /* utility_types_h */
