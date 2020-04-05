//
//File      : ukf.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 01/04/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef LUPinverter_h
#define LUPinverter_h

#include "expr.hpp"
#include "matrix_new.hpp"

//template<std::size_t NRows, std::size_t NCols>
//struct matrix;

template<std::size_t N>
constexpr matrix<N,N> make_identity_matrix();

template<std::size_t NRows, std::size_t NCols>
constexpr matrix<NRows, NCols> make_empty_matrix();

template<std::size_t NRows, std::size_t NCols>
class row_modifier{

public:
    constexpr row_modifier(matrix<NRows, NCols>& m_p, std::size_t index_p) : m{m_p}, row_index{index_p} {}
    
    constexpr row_modifier& substract(matrix<1, NCols> const& row_p)
    {
        for(std::size_t j = 0 ; j < NCols ; ++j){
            m(row_index, j) -= row_p(0, j);
        }
        return *this;
    }
    constexpr row_modifier& add(matrix<1, NCols> const& row_p)
    {
        for(std::size_t j = 0 ; j < NCols ; ++j){
            m(row_index, j) += row_p(0, j);
        }
        return *this;
    }
    
    template< typename T,
              class type_checker = std::enable_if_t<std::is_arithmetic<T>::value> >
    constexpr row_modifier& multiply(const T& value_p)
    {
        for(std::size_t j = 0 ; j < NCols ; ++j){
            m(row_index, j) *= value_p;
        }
        return *this;
    }

private:
    matrix<NRows, NCols>& m;
    std::size_t const row_index;
};


//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

template<std::size_t N>
struct lup_inverter{
    
    struct diagonals{
        matrix<N, N> lower;
        matrix<N, N> upper;
    };
    
public:
    constexpr matrix<N, N> invert(matrix<N, N> const& m_p) const
    {
        auto permutation{ form_permutation_matrix(m_p) };
        
        auto diagonals = form_diagonals( permutation * m_p );
        
        auto inverse_lower = apply_lower_inversion( std::move( diagonals.lower ) );
        auto inverse_upper = apply_upper_inversion( std::move( diagonals.upper ) );
        
        return inverse_upper * inverse_lower * permutation;
    }
    
    
private:
    constexpr matrix<N, N> form_permutation_matrix(matrix<N, N> const& m_p) const
    {
        auto make_permutation_matrix = [&m_p]()
        {
            for( std::size_t i = 1 ; i < N; ++i ){
                if( m_p(i,0) != 0 ){ return swap_rows( 0, i, make_identity_matrix<N>() ); }
            }
            return make_empty_matrix<N, N>();
        };
    
        return m_p(0, 0) != 0 ? make_identity_matrix<N>() : make_permutation_matrix() ;
    }

    
    constexpr diagonals form_diagonals( matrix<N, N> const& target_p ) const
    {
        auto d = diagonals{ make_identity_matrix<N>(), make_identity_matrix<N>() };
        
        auto sum_lower = [&d](std::size_t row_index_p, std::size_t col_index_p){
            double result = 0;
            for(std::size_t n = 0 ; n < col_index_p ; ++n  ){ //difference is in the limit
                result += d.lower(row_index_p, n) * d.upper(n, col_index_p);
            }
            return result;
        };
        
        auto sum_upper = [&d](std::size_t row_index_p, std::size_t col_index_p){
            double result = 0;
            for(std::size_t n = 0 ; n < row_index_p ; ++n  ){ //difference is in the limit
                result += d.lower(row_index_p, n) * d.upper(n, col_index_p);
            }
            return result;
        };
        
        for(std::size_t i = 0 ; i < N ; ++i){ d.upper(0, i) = target_p(0, i); }
        for(std::size_t j = 1 ; j < N ; ++j){ d.lower(j, 0) = target_p(j, 0) / d.upper(0, 0); }
        
        for(std::size_t k = 1 ; k < N ; ++k)
        {
            for(std::size_t i = k ; i < N ; ++i) { d.upper(k, i) = target_p(k, i) - sum_upper(k, i) ; }
            for(std::size_t j = k + 1 ; j < N ; ++j) { d.lower(j, k) = ( target_p(j, k) - sum_lower(j, k) ) / d.upper(k, k); }
        }
        
        return d;
    }
    

    constexpr matrix<N, N> apply_lower_inversion( matrix<N, N>&& lower_p) const
    {
        auto combined = form_combination( lower_p, make_identity_matrix<N>(), details::column_tag{} );
        
        for(std::size_t i = 1 ; i < N ; ++i){
            auto rm = row_modifier<N, 2*N>{ combined, i };
            for(std::size_t k = 0 ; k < i ; ++k){
                rm.substract( combined(i,k) * form_row( combined, k ) );
            }
        }
        
        return split_half(combined, details::column_tag{}).second;
    }
    

    constexpr matrix<N, N> apply_upper_inversion( matrix<N, N>&& upper_p) const
    {
        auto combined = form_combination( upper_p, make_identity_matrix<N>(), details::column_tag{} );
        
        for(int i = N - 1 ; i >= 0 ; --i){
            auto i_ = static_cast<std::size_t>(i);
            auto rm = row_modifier<N, 2*N>{ combined, i_ };
            for(int k = N - 1 ; k > i ; --k){
                auto k_ = static_cast<std::size_t>(k);
                rm.substract( combined(i,k) * form_row( combined, k ) );
            }
            rm.multiply( 1./combined(i_,i_) );
        }
        
        return split_half(combined, details::column_tag{}).second ;
    }

};




template<>
struct matrix<1, 1>
{
    double data;
    operator double() { return data;}
    operator double() const { return data;}
    
    constexpr double  operator()(std::size_t, std::size_t) const { return data; }
    constexpr double& operator()(std::size_t, std::size_t)       { return data; }
};


template<>
struct lup_inverter<1>
{
    constexpr auto invert( matrix<1, 1> const& m_p ) const { return matrix<1, 1>{ 1./m_p }; }
};



#endif /* LUPInverter_h */
