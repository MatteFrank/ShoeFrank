//
//  LUPInverter.h
//  
//
//  Created by asecher on 27/02/2019.
//

#ifndef LUPinverter_h
#define LUPinverter_h

#include "matrix.hpp"

template<std::size_t NRows, std::size_t NCols>
class Matrix;


template<std::size_t N>
constexpr Matrix<N,N> make_identity_matrix();


template<std::size_t NCols>
class RowHandle{

private:
    std::array<double*, NCols> underlyingRow_mc;

public:
    template<std::size_t NRows>
    RowHandle(Matrix<NRows, NCols>& matrix_p, std::size_t rowIndex_p)
    {
        for(std::size_t i = 0 ; i < NCols ; ++i){
            underlyingRow_mc[i] = &matrix_p.get(rowIndex_p, i);
        }
    }

    
    RowHandle& substract(const Matrix<1, NCols>& rowMatrix_p)
    {
        return (*this).add(-1 * rowMatrix_p);
    }
    RowHandle& add(const Matrix<1, NCols>& rowMatrix_p)
    {
        for(std::size_t j = 0 ; j < NCols ; ++j){
            *underlyingRow_mc[j] += rowMatrix_p(0, j);
        }
        return *this;
    }
    
    template<typename T,
             class type_checker = std::enable_if_t<std::is_arithmetic<T>::value> >
    RowHandle& multiply(const T& value_p)
    {
        for(auto& value_h : underlyingRow_mc){ *value_h *= value_p; }
        return *this;
    }
    
    double& get(const std::size_t colIndex_p){ return underlyingRow_mc[colIndex_p]; }
};


//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__


class LUPInverter{
private:
    template<std::size_t N>
    void ComputeDiagonals(const Matrix<N, N>& aMatrix,
                          Matrix<N, N>& aLDMatrix,
                          Matrix<N, N>& aUDMatrix) const;
    
    template<std::size_t N>
    Matrix<N, N> ComputePermutationMatrix(const Matrix<N, N>& matrix_p) const ;
    
    template<std::size_t N>
    Matrix<N, 2*N> make_combination(const Matrix<N, N>& leftMatrix_p,
                                    const Matrix<N, N>& rightMatrix_p) const;
    
    template<std::size_t N>
    Matrix<N, N> ApplyLowerInversion(Matrix<N, N> lowerMatrix_p) const;
    
    template<std::size_t N>
    Matrix<N, N> ApplyUpperInversion(Matrix<N, N> upperMatrix_p) const;
    
    template<std::size_t N>
    void split(const Matrix<N, 2*N>& targetMatrix_p,
               Matrix<N, N>& leftMatrix_p,
               Matrix<N, N>& rightMatrix_p) const;

//
    
public:
    template< std::size_t N >
    Matrix<N, N> Invert(const Matrix<N
                        , N>& matrix_p) const;
    
    LUPInverter() = default;
};




//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__



template<std::size_t N>
void LUPInverter::ComputeDiagonals(const Matrix<N, N>& targetMatrix_p,
                                   Matrix<N, N>& lowerMatrix_p,
                                   Matrix<N, N>& upperMatrix_p) const
{
    
    auto sumLower_l = [&lowerMatrix_p, &upperMatrix_p](const std::size_t& rowIndex, const std::size_t& colIndex){
        double result = 0;
        for(std::size_t n = 0 ; n < colIndex ; ++n  ){ //difference is in the limit
            result += lowerMatrix_p(rowIndex, n) * upperMatrix_p(n, colIndex);
        }
        return result;
    };
    
    auto sumUpper_l = [&lowerMatrix_p, &upperMatrix_p](const std::size_t& rowIndex, const std::size_t& colIndex){
        double result = 0;
        for(std::size_t n = 0 ; n < rowIndex ; ++n  ){ //difference is in the limit
            result += lowerMatrix_p(rowIndex, n) * upperMatrix_p(n, colIndex);
        }
        return result;
    };
    
    
    for(std::size_t i = 0 ; i < N ; ++i){ upperMatrix_p.get(0, i) = targetMatrix_p(0, i); }
    for(std::size_t j = 1 ; j < N ; ++j)
        { lowerMatrix_p.get(j, 0) = targetMatrix_p(j, 0) / upperMatrix_p(0, 0); }
    
    for(std::size_t k = 1 ; k < N ; ++k)
    {
        for(std::size_t i = k ; i < N ; ++i)
                { upperMatrix_p.get(k, i) = targetMatrix_p(k, i) - sumUpper_l(k, i) ; }
        for(std::size_t j = k + 1 ; j < N ; ++j)
                { lowerMatrix_p.get(j, k) = ( targetMatrix_p(j, k) - sumLower_l(j, k) ) / upperMatrix_p(k, k); }
    }
    
}


//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

//combine two square matrix of the same size, column-wise
template<std::size_t N>
Matrix<N, 2*N> LUPInverter::make_combination(const Matrix<N, N>& leftMatrix_p,
                                             const Matrix<N, N>& rightMatrix_p) const
{
    auto matrix{ make_empty_matrix<N, 2*N>() };
    
    for(std::size_t i = 0 ; i < N ; ++i ){
        for(std::size_t j = 0 ; j < N ; ++j){
            matrix.get(i, j) = leftMatrix_p(i, j);
            matrix.get(i, j + N) = rightMatrix_p(i, j);
        }
    }

    return matrix;
}

//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__


template< std::size_t N >
Matrix<N, N> LUPInverter::ApplyLowerInversion(Matrix<N, N> lowerMatrix_p) const
{
    auto combinatedMatrix = make_combination(lowerMatrix_p,
                                             make_identity_matrix<N>());
    
    for(std::size_t i = 1 ; i < N ; ++i){
        RowHandle<2*N> rowHandle{ combinatedMatrix, i };
        for(std::size_t k = 0 ; k < i ; ++k){
            rowHandle.substract( combinatedMatrix(i,k) * combinatedMatrix.row(k) )  ;
        }
    }
    
    
    auto identityMatrix{ make_empty_matrix<N, N>() };
    auto resultingMatrix{ make_empty_matrix<N, N>() };
    split(combinatedMatrix, identityMatrix, resultingMatrix);

    if(identityMatrix != make_identity_matrix<N>()){
        throw std::logic_error("Inversion of lower matrix failed.");
    }
    
    return resultingMatrix;
}



template< std::size_t N>
Matrix<N, N> LUPInverter::ApplyUpperInversion(Matrix<N, N> upperMatrix_p) const
{
    
    auto combinatedMatrix = make_combination(upperMatrix_p,
                                             make_identity_matrix<N>());
    
    
    for(int i = N - 1 ; i >= 0 ; --i){
        auto i_ = static_cast<std::size_t>(i);
        RowHandle<2*N> rowHandle{ combinatedMatrix, i_ };
        for(int k = N - 1 ; k > i ; --k){
            auto k_ = static_cast<std::size_t>(k);
            rowHandle.substract( combinatedMatrix(i_,k_) * combinatedMatrix.row(k_) )  ;
        }
        rowHandle.multiply( 1./combinatedMatrix(i_,i_) );
    }
    
    auto identityMatrix{ make_empty_matrix<N, N>() };
    auto resultingMatrix{ make_empty_matrix<N, N>() };
    split(combinatedMatrix, identityMatrix, resultingMatrix);

    
 //   std::cout << "Upper matrix inversion: \n" << upperMatrix_p << "\n\n"<< combinatedMatrix << "\n\n" << identityMatrix << "\n\n" << resultingMatrix;
   // std::cout << "Identity matrix<N==1>: \n" << make_identity_matrix<N>();
    if(identityMatrix != make_identity_matrix<N>()){
       // std::cout << identityMatrix;
        throw std::logic_error("Inversion of upper matrix failed.");
    }
    
    return resultingMatrix;
}


//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

template<std::size_t N >
void LUPInverter::split(const Matrix<N, 2*N>& targetMatrix_p,
                        Matrix<N, N>& leftMatrix_p,
                        Matrix<N, N>& rightMatrix_p) const
{
    for(std::size_t i = 0 ; i < N ; ++i ){
        for(std::size_t j = 0 ; j < N ; ++j){
            leftMatrix_p.get(i, j) = targetMatrix_p(i, j);
        }
        for(std::size_t j = N ; j < 2 * N ; ++j){
            rightMatrix_p.get(i , j - N) = targetMatrix_p(i, j);
        }
    }
}

//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

template<std::size_t N>
Matrix<N, N> LUPInverter::ComputePermutationMatrix(const Matrix<N, N>& matrix_p) const
//precondition: matrix_p(i, 0) != 0, for at least one i
{
    auto permutationMatrix{ make_identity_matrix<N>() };

    for(std::size_t row{1} ; row < N ; ++row){
        if( (permutationMatrix * matrix_p)(0, 0) != 0 ){ break; }
        permutationMatrix = make_identity_matrix<N>();
        permutationMatrix.swap_rows(0, row);
    }
    
    return permutationMatrix;
}



//__ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo____ooo000OOOOOO000ooo__

template< std::size_t N>
inline Matrix< N, N > LUPInverter::Invert(const Matrix<N, N>& matrix_p) const
{
    auto permutationMatrix{ ComputePermutationMatrix(matrix_p) };
    auto targetMatrix = permutationMatrix * matrix_p;
    
    auto lowerMatrix{ make_identity_matrix<N>() };
    auto upperMatrix{ make_identity_matrix<N>() };
    ComputeDiagonals(targetMatrix, lowerMatrix, upperMatrix);
    
    auto invertedLowerMatrix = ApplyLowerInversion( lowerMatrix );
    auto invertedUpperMatrix = ApplyUpperInversion( upperMatrix );
    
    return invertedUpperMatrix * invertedLowerMatrix * permutationMatrix;
}


template<>
inline Matrix<1,1> LUPInverter::Invert(const Matrix<1, 1>& matrix_p) const
{
    return Matrix<1,1>{1./matrix_p(0,0)};
}



#endif /* LUPInverter_h */
