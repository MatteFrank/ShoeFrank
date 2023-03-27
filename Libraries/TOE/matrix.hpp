//
//  Matrix.hpp
//  
//
//  Created by asecher on 18/02/2019.
//

#ifndef Matrix_hpp
#define Matrix_hpp

#include <array>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "utility_types.hpp"

//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

template<std::size_t NRows, std::size_t NCols>
class Matrix{
    template< std::size_t RowIndex, std::size_t ColIndex, std::size_t N1, std::size_t N2 >
    friend constexpr const double& get( const Matrix<N1, N2>& matrix_p );
    
private:
    std::array<double, NRows * NCols> underlyingArray_mc;

public:
    //quick hack
    const std::array<double, NRows * NCols>& data() const { return underlyingArray_mc; }
    
    constexpr Matrix(std::initializer_list<double> initializerList_p)
    //precondition : intializerList_p.size() == NRows * NCols
    {
        auto filler{ underlyingArray_mc.begin() };
        for(auto&& value : initializerList_p){
            *(filler++) = value;
        }
    }
    constexpr Matrix(const std::array<double, NRows * NCols>& array_p) : underlyingArray_mc{ array_p } {}

    constexpr Matrix() : underlyingArray_mc{std::array<double, NRows * NCols>{}} {};
    //Matrix(const Matrix& other_p) = default;
    //Matrix(Matrix&& other_p) = default;
    //Matrix& operator=(const Matrix& other_p) =default;
    //Matrix& operator=(Matrix&& other_p) =default;
    
    
    constexpr Matrix& assign(const Matrix& otherMatrix_p) { underlyingArray_mc = otherMatrix_p.underlyingArray_mc; }
    constexpr Matrix& assign(Matrix&& otherMatrix_p) { underlyingArray_mc = std::move(otherMatrix_p.underlyingArray_mc); }
    
    
public:
    constexpr double operator()(std::size_t rowIndex_p, std::size_t colIndex_p) const
    {
        return underlyingArray_mc[ rowIndex_p * NCols + colIndex_p ];
    }
   
    double& get(std::size_t rowIndex_p, std::size_t colIndex_p)
    {
        return underlyingArray_mc[ rowIndex_p * NCols + colIndex_p ];
    }
    
    Matrix<1, NCols> row(std::size_t rowIndex_p) const
    {
        std::array<double, NCols> array{0};
        for(std::size_t j{0} ; j < NCols ; ++j){
            array[j] = (*this)(rowIndex_p, j);
        }
        return Matrix<1,NCols>{ array };
    }
    
    Matrix<NRows, 1> column(std::size_t index_p) const
    {
        std::array<double, NRows> array{0};
        for(std::size_t j{0} ; j < NRows ; ++j){
            array[j] = (*this)(j, index_p);
        }
        return Matrix<NRows, 1>{ array };
    }
    
    
    Matrix<NRows, NCols> swap_rows(std::size_t rowIndex1_p, std::size_t rowIndex2_p) const
    {
        auto matrix{ *this };
        for(std::size_t j = 0 ; j < NCols ; ++j){
            matrix.get(rowIndex1_p, j) = (*this)(rowIndex2_p, j);
            matrix.get(rowIndex2_p, j) = (*this)(rowIndex1_p, j);
        }
        return matrix;
    }

};


//as such, bad idea
//template<>
//struct Matrix<1,1>
//{
//    double data;
//    explicit operator double() { return data;}
//    explicit operator double() const { return data;}
//};


//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__
#include <iostream>
template< std::size_t NRows, std::size_t NCols>
std::ostream& operator<<(std::ostream& ostream_p, const Matrix<NRows, NCols>& matrix_p)
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

template< std::size_t RowIndex, std::size_t ColIndex, std::size_t NRows, std::size_t NCols >
constexpr double& get( Matrix<NRows, NCols>& matrix_p )
{
    return const_cast<double&>(
                    get<RowIndex, ColIndex>( static_cast<const Matrix<NRows, NCols>&>(matrix_p) )
                                    );
}


template< std::size_t RowIndex, std::size_t ColIndex, std::size_t N1, std::size_t N2 >
constexpr const double& get(const Matrix<N1, N2>& matrix_p )
{
    return matrix_p.underlyingArray_mc[ RowIndex * N2 + ColIndex ];
}


//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

//default constructor
template<std::size_t NRows, std::size_t NCols>
constexpr Matrix<NRows, NCols> make_empty_matrix()
{
    return Matrix<NRows, NCols>{ std::array<double, NRows * NCols>{} };
}


namespace details{

    //link to stack overflow answer
    //make it function template, and specify first argument first
    template<std::size_t N>
    constexpr double identity_filler(std::size_t index_p)
    {  
        return (index_p % (N+1) == 0 ) ?  1. : 0.;
    }
    
    template<class Function, std::size_t... Indices>
    constexpr auto make_array_helper(Function f_p,
                                     std::index_sequence<Indices...>)
              -> std::array<double, sizeof...(Indices)>
    {
        return {{ f_p(Indices)... }};
    }
    
    //you gave in the wrong N idiot
    template<std::size_t N>
    constexpr std::array<double, N*N> make_identity_array()
    {
        return make_array_helper(identity_filler<N>, std::make_index_sequence<N*N>{});
    }
    
    
    //if you need several parameter pack, wrap them in a struct, a packer ? -> any_of
}

template<std::size_t N>
constexpr Matrix<N, N> make_identity_matrix()
{
    return Matrix<N, N>{ details::make_identity_array<N>() };
}

template< std::size_t N1, std::size_t N2 >
Matrix<N2, N1> make_transpose(const Matrix< N1, N2 >& matrix_p)
{
    auto matrix{ make_empty_matrix<N2, N1>() };
    
    for(std::size_t i = 0 ; i < N1 ; ++i){
        for(std::size_t j = 0 ; j < N2 ; ++j){
            matrix.get(j, i) = matrix_p(i, j);
        }
    }
    
    return matrix;
}

//maybe think about numerical operation and transpose

//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

template< std::size_t NRows, std::size_t NCols >
const Matrix< NRows, NCols> operator+(const Matrix< NRows, NCols>& leftMatrix_p, const Matrix< NRows, NCols>& rightMatrix_p)
{
    auto matrix{ make_empty_matrix<NRows, NCols>() };
    
    for(std::size_t i = 0 ; i < NRows ; ++i){
        for(std::size_t j = 0 ; j < NCols ; ++j){
            matrix.get(i,j) = leftMatrix_p(i, j) + rightMatrix_p(i, j);
        }
    }
    
    return matrix;
}


template< typename T,  std::size_t N1, std::size_t N2,
          class type_checker = std::enable_if_t<std::is_arithmetic<T>::value> >
const Matrix< N1, N2> operator*(const T& factor_p, const Matrix< N1, N2>& rightMatrix_p)
{
    auto matrix{ make_empty_matrix<N1, N2>() };
    
    for(std::size_t i = 0 ; i < N1 ; ++i){
        for(std::size_t j = 0 ; j < N2 ; ++j){
            matrix.get(i,j) = factor_p * rightMatrix_p(i, j);
        }
    }
    
    return matrix;
}


template< typename T,  std::size_t N1, std::size_t N2,
class type_checker = std::enable_if_t<std::is_arithmetic<T>::value> >
const Matrix< N1, N2> operator*(const Matrix< N1, N2>& rightMatrix_p, const T& factor_p)
{
    auto matrix{ make_empty_matrix<N1, N2>() };
    
    for(std::size_t i = 0 ; i < N1 ; ++i){
        for(std::size_t j = 0 ; j < N2 ; ++j){
            matrix.get(i,j) = factor_p * rightMatrix_p(i, j);
        }
    }
    
    return matrix;
}


template< std::size_t NRows, std::size_t NCols >
const Matrix< NRows, NCols> operator-(const Matrix< NRows, NCols>& leftMatrix_p, const Matrix< NRows, NCols>& rightMatrix_p)
{
    auto rightMatrix = -1 * rightMatrix_p;
    return leftMatrix_p + rightMatrix;
}


template<std::size_t N1, std::size_t N2, std::size_t N3 >
const Matrix<N1, N3> operator*(const Matrix< N1, N2>& leftMatrix_p,
                               const Matrix< N2, N3>& rightMatrix_p)
{
    auto matrix{ make_empty_matrix<N1, N3>() };
    
    for(std::size_t i = 0 ; i < N1 ; ++i){
        for(std::size_t j = 0 ; j < N3 ; ++j){
            for(std::size_t k = 0 ; k < N2 ; ++k){
                matrix.get(i,j) += leftMatrix_p(i, k) * rightMatrix_p(k, j);
            }
        }
    }
    
    return matrix;
}

//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

template<std::size_t NRows, std::size_t NCols>
constexpr bool operator==(const Matrix<NRows, NCols>& leftMatrix_p,
                const Matrix<NRows, NCols>& rightMatrix_p)
{
    auto areMatrixEquals{true};
    for(std::size_t i = 0 ; i < NRows ; ++i ){
        for(std::size_t j = 0 ; j < NCols ; ++j){
            areMatrixEquals *= ( leftMatrix_p(i,j) == rightMatrix_p(i,j) );
            if(!areMatrixEquals) break;
        }
        if(!areMatrixEquals) break;
    }
    return areMatrixEquals;
}
//

template<std::size_t NRows, std::size_t NCols>
constexpr bool operator!=(const Matrix<NRows, NCols>& leftMatrix_p,
                const Matrix<NRows, NCols>& rightMatrix_p)
{
    return !(leftMatrix_p == rightMatrix_p);
}


//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

#include "lup_inverter.hpp"

template< std::size_t N >
Matrix< N, N > make_inverse(const Matrix<N, N> & matrix_p)
{
    return LUPInverter{}.Invert(matrix_p);
}
//



//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__
#include <cmath>

//maybe enforce symmetricity ?
template<std::size_t N>
Matrix<N, N> make_cholesky_triangle(const Matrix<N, N>& matrix_p)
{
    auto choleskyTriangle{ make_empty_matrix<N, N>() };

    auto diagonalSum_l = [&choleskyTriangle]( std::size_t rowIndex_p ){
        double result{0};
        for(std::size_t j = 0 ; j < rowIndex_p ; ++j ){ result += pow( choleskyTriangle(rowIndex_p, j), 2 ) ; }
        return result;
    };

    auto lowerTriangleSum_l = [&choleskyTriangle]( std::size_t rowIndex_p, std::size_t columnIndex_p ){
        double result{0};
        for(std::size_t k = 0 ; k < columnIndex_p ; ++k )
                { result += choleskyTriangle( rowIndex_p, k ) * choleskyTriangle( columnIndex_p, k ) ; }
        return result;
    };


    for( std::size_t i = 0 ; i < N ; ++i ){
        for( std::size_t j = 0 ; j < i ; ++j  ){
            choleskyTriangle.get(i, j) = ( matrix_p(i, j) - lowerTriangleSum_l(i, j) ) / choleskyTriangle(j, j);
            //std::cout << "("<< i <<", "<< j << ") -> "<< choleskyTriangle(i, j) <<"\n";
        }
        choleskyTriangle.get(i, i) = sqrt( matrix_p(i, i) - diagonalSum_l(i) );
        //std::cout << "("<< i <<", "<< i << ") -> "<< choleskyTriangle(i, i) <<"\n";
    }

    return choleskyTriangle;
}




//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__
//template<std::size_t N >
//void LUPInverter::split(const Matrix<N, 2*N>& targetMatrix_p,
//                        Matrix<N, N>& leftMatrix_p,
//                        Matrix<N, N>& rightMatrix_p) const
//{
//    for(std::size_t i = 0 ; i < N ; ++i ){
//        for(std::size_t j = 0 ; j < N ; ++j){
//            leftMatrix_p.get(i, j) = targetMatrix_p(i, j);
//        }
//        for(std::size_t j = N ; j < 2 * N ; ++j){
//            rightMatrix_p.get(i , j - N) = targetMatrix_p(i, j);
//        }
//    }
//}
//


namespace details {
    
    //row split -> definitively more generalizable : super specific here
    template<std::size_t NRows, std::size_t ... Indices>
    constexpr auto get_top_half(Matrix<NRows, 1> matrix_p, std::index_sequence<Indices...> )
    {
        return Matrix<NRows/2, 1>{ matrix_p(Indices , 0)... };
    }
    
    template<std::size_t NRows,  std::size_t ... Indices>
    constexpr auto get_bottom_half(Matrix<NRows, 1> matrix_p, std::index_sequence<Indices...>  )
    {
        return Matrix<NRows/2, 1>{ matrix_p(Indices + NRows/2 , 0)... };
    }
    
    
    template<std::size_t NRows1, std::size_t NRows2, std::size_t ... Indices1, std::size_t ... Indices2>
    auto combine_row_impl( Matrix<NRows1, 1> m1_p,
                           std::index_sequence<Indices1...>,
                           Matrix<NRows2, 1> m2_p,
                           std::index_sequence<Indices2...> )
    {
        return Matrix<NRows1 + NRows2, 1>{m1_p(Indices1, 0)... , m2_p(Indices2, 0)... };
    }
    

    
    //will only work for square matrices then -> easier to implement and the rest is not really needed
    template< std::size_t N1, std::size_t N2>
    constexpr double diagonals_filler(std::size_t index_p, Matrix<N1, N1> m1_p, Matrix<N2, N2> m2_p)
    {
        std::size_t size = (N1 + N2) ;
        std::size_t row = index_p / size;
        std::size_t column = index_p % size;
        
        auto choose = [&m1_p, &m2_p](std::size_t index_p){ return (index_p < N1) ? m1_p(index_p, index_p) : m2_p(index_p-N1, index_p-N1); };
        
        return (row == column ) ? choose(row) : 0.;
    }
    
    template<std::size_t N1, std::size_t N2, std::size_t... Indices>
    constexpr auto combine_diagonals_impl( Matrix<N1, N1> m1_p, Matrix<N2, N2> m2_p,  std::index_sequence<Indices...>)
        -> std::array<double, sizeof...(Indices)>
    {
        return {{ diagonals_filler(Indices, m1_p, m2_p)... }};
    }
    
    
}//namespace details


template<std::size_t NRows, std::size_t NCols>
auto split_half(Matrix<NRows, NCols> matrix_p, details::row_tag)
{
    static_assert(NRows%2 == 0, "The matrix dimensions are not even, can't be splitten in half");

    return std::make_pair( details::get_top_half(std::move(matrix_p), std::make_index_sequence<NRows/2>{} ),
                           details::get_bottom_half(std::move(matrix_p), std::make_index_sequence<NRows/2>{})  );
}


template<std::size_t NRows1, std::size_t NRows2>
Matrix<NRows1 + NRows2, 1> combine( Matrix<NRows1, 1> m1_p,
                                    Matrix<NRows2, 1> m2_p,
                                    details::row_tag )
{
    return details::combine_row_impl(std::move(m1_p), std::make_index_sequence<NRows1>{}, std::move(m2_p), std::make_index_sequence<NRows2>{});
}



template<std::size_t N1,  std::size_t N2 >
constexpr Matrix<N1 + N2, N1 + N2> combine_diagonals( Matrix<N1, N1> m1_p,
                                                      Matrix<N2, N2> m2_p )
{
    return details::combine_diagonals_impl( std::move(m1_p), std::move(m2_p), std::make_index_sequence< (N1+N2)*(N1+N2) >{} );
}

#endif /* Matrix_hpp */
