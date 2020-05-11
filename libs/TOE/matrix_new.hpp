//
//File      : matrix_new.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 01/04/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef matrix_new_hpp
#define matrix_new_hpp

#include <array>
#include <iostream>
#include <cmath>

#include "utility_types.hpp"
#include "lup_inverter.hpp"

//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

template<std::size_t NRows, std::size_t NCols>
struct matrix{

    std::array<double, NRows * NCols> data_mc;
    
    std::array<double, NRows * NCols> const& data() const { return data_mc; }
    

    constexpr double operator()(std::size_t row_index_p, std::size_t col_index_p) const
    {
        return data_mc[ row_index_p * NCols + col_index_p ];
    }
    
    constexpr double& operator()(std::size_t row_index_p, std::size_t col_index_p)
    {
        return data_mc[ row_index_p * NCols + col_index_p ];
    }

};


//as such, bad idea
template<>
struct matrix<1,1>;


template< std::size_t NRows, std::size_t NCols>
std::ostream& operator<<(std::ostream& ostream_p, const matrix<NRows, NCols>& matrix_p)
{
    for(std::size_t i = 0 ; i < NRows ; ++i){
        for(std::size_t j = 0 ; j < NCols ; ++j){
            ostream_p << matrix_p(i,j) << " ";
        }
        ostream_p<< "\n";
    }
    return ostream_p;
}


//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

namespace details{
    
    //link to stack overflow answer
    //make it function template, and specify first argument first
    template<std::size_t N>
    constexpr double identity_filler(std::size_t index_p)
    {
        return (index_p % (N+1) == 0 ) ?  1. : 0.;
    }
    
    template<std::size_t NRows, std::size_t NCols>
    constexpr double empty_filler(std::size_t )
    {
        return 0.;
    }
    
    template< std::size_t NRows, std::size_t NCols, class Filler, std::size_t... Indices>
    constexpr auto make_matrix_impl( Filler f_p,
                                     std::index_sequence<Indices...>)
              -> matrix< NRows, NCols >
    {
        return {{ f_p(Indices)... }};
    }
    
    
    
    
}


template<std::size_t NRows, std::size_t NCols>
constexpr matrix<NRows, NCols> swap_rows(std::size_t row1_p, std::size_t row2_p, matrix<NRows, NCols> m_p)
{
    for(std::size_t j = 0 ; j < NCols ; ++j){
        auto temp = m_p(row1_p, j);
        m_p(row1_p, j) = m_p(row2_p, j);
        m_p(row2_p, j) = temp;
    }
    return std::move(m_p);
}



template<std::size_t N>
constexpr matrix<N, N> make_identity_matrix()
{
    return details::make_matrix_impl<N, N>(details::identity_filler<N>, std::make_index_sequence<N*N>{});
}

//default constructor
template<std::size_t NRows, std::size_t NCols>
constexpr matrix<NRows, NCols> make_empty_matrix()
{
    return details::make_matrix_impl<NRows, NCols>(details::empty_filler<NRows, NCols>, std::make_index_sequence<NRows*NCols>{});
}




//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__



template< std::size_t N >
constexpr matrix< N, N > form_inverse(matrix<N, N> const& m_p)
{
    return lup_inverter<N>{}.invert( m_p );
}
//



//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__


//maybe enforce symmetricity ?
template<std::size_t N>
matrix<N, N> form_cholesky_triangle(matrix<N, N> const& m_p)
{
    auto cholesky_triangle{ make_empty_matrix<N, N>() };

    auto diagonal_sum = [&cholesky_triangle]( std::size_t row_index_p ){
        double result{0};
        for(std::size_t j = 0 ; j < row_index_p ; ++j ){ result += pow( cholesky_triangle(row_index_p, j), 2 ) ; }
        return result;
    };

    auto lower_triangle_sum = [&cholesky_triangle]( std::size_t row_index_p, std::size_t column_index_p ){
        double result{0};
        for(std::size_t k = 0 ; k < column_index_p ; ++k )
                { result += cholesky_triangle( row_index_p, k ) * cholesky_triangle( column_index_p, k ) ; }
        return result;
    };


    for( std::size_t i = 0 ; i < N ; ++i ){
        for( std::size_t j = 0 ; j < i ; ++j  ){
            cholesky_triangle(i, j) = ( m_p(i, j) - lower_triangle_sum(i, j) ) / cholesky_triangle(j, j);
        }
        cholesky_triangle(i, i) = sqrt( m_p(i, i) - diagonal_sum(i) );
    }

    return cholesky_triangle;
}




namespace details {
    
    //row split -> definitively more generalizable : super specific here
    template<std::size_t NRows, std::size_t NCols>
    struct top_half_filler
    {
        constexpr top_half_filler( matrix<NRows, NCols> const& m_p ) : m{m_p} {}
        
        constexpr auto operator()( std::size_t index_p )
        {
            std::size_t row_index = index_p/NCols;
            std::size_t column_index = index_p%NCols;
            return m(row_index , column_index);
        }
        
    private:
        matrix<NRows, NCols> const& m;
    };
    
    template<std::size_t NRows, std::size_t NCols>
    struct bottom_half_filler
    {
        constexpr bottom_half_filler( matrix<NRows, NCols> const& m_p ) : m{m_p} {}
        
        constexpr auto operator()( std::size_t index_p )
        {
            std::size_t row_index = index_p/NCols + NRows/2;
            std::size_t column_index = index_p%NCols;
            return m( row_index, column_index);
        }
        
    private:
        matrix<NRows, NCols> const& m;
    };
    
    template<std::size_t NRows, std::size_t NCols>
    struct left_half_filler
    {
        constexpr left_half_filler( matrix<NRows, NCols> const& m_p ) : m{m_p} {}
        
        constexpr auto operator()( std::size_t index_p )
        {
            std::size_t row_index = index_p/(NCols/2);
            std::size_t column_index = index_p%(NCols/2);
            return m(row_index , column_index);
        }
        
    private:
        matrix<NRows, NCols> const& m;
    };
    
    template<std::size_t NRows, std::size_t NCols>
    struct right_half_filler
    {
        constexpr right_half_filler( matrix<NRows, NCols> const& m_p ) : m{m_p} {}
        
        constexpr auto operator()( std::size_t index_p )
        {
            std::size_t row_index = index_p/(NCols/2) ;
            std::size_t column_index = index_p%(NCols/2) + NCols/2 ;
            return m(row_index , column_index);
        }
        
    private:
        matrix<NRows, NCols> const& m;
    };
    
    
    template<std::size_t NRows, std::size_t NCols>
    struct row_filler
    {
        constexpr row_filler( matrix<NRows, NCols> const& m_p, std::size_t row_index_p ) : m{m_p}, row_index{row_index_p} {}
        
        constexpr auto operator()(std::size_t column_index_p)
        {
            return m(row_index, column_index_p);
        }
        
    private:
        matrix<NRows, NCols> const& m;
        std::size_t row_index;
    };
    
    
    template<std::size_t NRows, std::size_t NCols>
    struct column_filler
    {
        constexpr column_filler( matrix<NRows, NCols> const& m_p, std::size_t column_index_p ) : m{m_p}, column_index{column_index_p} {}
        
        constexpr auto operator()(std::size_t row_index_p)
        {
            return m(row_index_p, column_index);
        }
        
    private:
        matrix<NRows, NCols> const& m;
        std::size_t column_index;
    };
    
    template< std::size_t NRows1, std::size_t NRows2, std::size_t NCols>
    struct combine_row_filler
    {
        constexpr combine_row_filler( matrix<NRows1, NCols>  const& m1_p,
                                     matrix<NRows2, NCols>  const& m2_p ) : m1{m1_p}, m2{ m2_p} {}
        
        constexpr auto operator()( std::size_t index_p )
        {
            std::size_t column_index = index_p%(NCols);
            std::size_t row_index = index_p/(NCols);
            return (row_index < NRows1) ? m1(row_index, column_index) : m2(row_index - NRows1, column_index);
        }
    
    private:
        matrix<NRows1, NCols>  const& m1;
        matrix<NRows2, NCols>  const& m2;
    };
    
    
    template< std::size_t NRows, std::size_t NCols1, std::size_t NCols2 >
    struct combine_column_filler
    {
        constexpr combine_column_filler( matrix<NRows, NCols1> const& m1_p,
                                         matrix<NRows, NCols2> const& m2_p ) : m1{m1_p}, m2{m2_p} {}
    
        constexpr auto operator()(std::size_t index_p)
        {
            std::size_t column_index = index_p%(NCols1 + NCols2);
            std::size_t row_index = index_p/(NCols1 + NCols2);
            return ( column_index < NCols1 ) ?  m1(row_index, column_index) : m2(row_index, column_index - NCols1) ;
        }
        
    private:
        matrix<NRows, NCols1> const& m1;
        matrix<NRows, NCols2> const& m2;
    };

    //will only work for square matrices then -> easier to implement and the rest is not really needed
    template< std::size_t N1, std::size_t N2 >
    struct combine_diagonal_filler
    {
        constexpr combine_diagonal_filler( matrix<N1, N1> const& m1_p,
                                          matrix<N2, N2> const& m2_p ) : m1{m1_p}, m2{m2_p} {}
    
        constexpr auto operator()( std::size_t index_p )
        {
            std::size_t row = index_p / (N1+N2);
            std::size_t column = index_p % (N1+N2);
        
            return (row < N1) ?
                    ((column < N1) ? m1(row, column) : 0) :
                    ((column < N1) ? 0 : m2(row-N1, column-N1) );
        }
        
    private:
        matrix<N1, N1> const& m1;
        matrix<N2, N2> const& m2;
    };
    
}//namespace details


template<std::size_t NRows, std::size_t NCols>
constexpr auto split_half(matrix<NRows, NCols> matrix_p, details::row_tag)
{
    static_assert(NRows%2 == 0, "The matrix row dimension is not even, can't be splitten in half ");

    return std::make_pair( details::make_matrix_impl<NRows/2, NCols>( details::top_half_filler<NRows, NCols>(matrix_p),
                                                                      std::make_index_sequence<NRows/2 * NCols>{} ),
                           details::make_matrix_impl<NRows/2, NCols>( details::bottom_half_filler<NRows, NCols>(matrix_p),
                                                                      std::make_index_sequence<NRows/2 * NCols>{} )  );
}

template<std::size_t NRows, std::size_t NCols>
constexpr auto split_half(matrix<NRows, NCols> matrix_p, details::column_tag)
{
    static_assert(NCols%2 == 0, "The matrix column dimension is not even, can't be splitten in half ");
    
    return std::make_pair( details::make_matrix_impl<NRows, NCols/2>( details::left_half_filler<NRows, NCols>(matrix_p),
                                                                      std::make_index_sequence<NRows * NCols/2>{} ),
                           details::make_matrix_impl<NRows, NCols/2>( details::right_half_filler<NRows, NCols>(matrix_p),
                                                                      std::make_index_sequence<NRows * NCols/2>{} ) );
}


template<std::size_t NRows1, std::size_t NCols, std::size_t NRows2>
matrix<NRows1 + NRows2, NCols> form_combination( matrix<NRows1, NCols> const& m1_p,
                                                 matrix<NRows2, NCols> const& m2_p,
                                                 details::row_tag )
{
    return details::make_matrix_impl<NRows1 + NRows2, NCols>( details::combine_row_filler<NRows1, NRows2, NCols>( m1_p, m2_p),
                                                              std::make_index_sequence< (NRows1 + NRows2) * NCols>{} );
}


template<std::size_t NRows, std::size_t NCols1, std::size_t NCols2>
matrix<NRows, NCols1 + NCols2> form_combination( matrix<NRows, NCols1> const& m1_p,
                                                 matrix<NRows, NCols2> const& m2_p,
                                                 details::column_tag )
{
    return details::make_matrix_impl<NRows, NCols1+NCols2>( details::combine_column_filler<NRows, NCols1, NCols2 >( m1_p, m2_p ),
                                                            std::make_index_sequence< NRows * ( NCols1 + NCols2 ) >{} );
}


template<std::size_t N1,  std::size_t N2>
constexpr matrix<N1 + N2, N1 + N2> form_combination( matrix<N1, N1> const& m1_p,
                                                     matrix<N2, N2> const& m2_p,
                                                     details::diagonal_tag )
{
    return details::make_matrix_impl<N1+N2, N1+N2>( details::combine_diagonal_filler<N1, N2>(m1_p, m2_p),
                                                    std::make_index_sequence< (N1+N2)*(N1+N2) >{} );
}



template<std::size_t NRows, std::size_t NCols>
constexpr matrix<1, NCols> form_row( matrix<NRows, NCols> const& m_p, std::size_t row_index_p )
{
    return details::make_matrix_impl<1, NCols>( details::row_filler<NRows, NCols>(m_p, row_index_p) ,
                                                std::make_index_sequence<NCols>() );
}


template<std::size_t NRows, std::size_t NCols>
constexpr matrix<NRows, 1> form_column( matrix<NRows, NCols> const& m_p, std::size_t column_index_p )
{
    return details::make_matrix_impl<NRows, 1>( details::column_filler<NRows, NCols>(m_p, column_index_p) ,
                                                std::make_index_sequence<NRows>() );
}
#endif /* Matrix_hpp */
