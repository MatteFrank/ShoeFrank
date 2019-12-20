//
// Plot tot residual + Fit for FIRST data
//

#ifndef _TATOEutilities_HXX
#define _TATOEutilities_HXX

#include <numeric>
#include <cmath>

#include "state.hpp"
#include "matrix.hpp"


#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAVTntuCluster.hxx"

/** TATOEutilities class
 
 \author A. Sécher
 */

#include <iostream>

namespace details{
    struct vertex_tag{
        using measurement_vector = state_vector_impl< Matrix<2, 1> >;
        using measurement_covariance = state_covariance_impl< Matrix<2, 2> >;
        using measurement_matrix = measurement_matrix_impl< Matrix<2,4> >;
        using candidate = candidate_impl<measurement_vector, measurement_covariance, measurement_matrix>;
    };
    struct msd_tag{
        using measurement_vector = state_vector_impl< Matrix<2, 1> >;
        using measurement_covariance = state_covariance_impl< Matrix<2, 2> >;
        using measurement_matrix = measurement_matrix_impl< Matrix<2,4> >;
        using candidate = candidate_impl<measurement_vector, measurement_covariance, measurement_matrix>;
    };
    struct it_tag{
        using measurement_vector = state_vector_impl< Matrix<2, 1> >;
        using measurement_covariance = state_covariance_impl< Matrix<2, 2> >;
        using measurement_matrix = measurement_matrix_impl< Matrix<2,4> >;
        using candidate = candidate_impl<measurement_vector, measurement_covariance, measurement_matrix>;
    };
    struct tof_tag{
        using measurement_vector = state_vector_impl< Matrix<2, 1> >;
        using measurement_covariance = state_covariance_impl< Matrix<2, 2> >;
        using measurement_matrix = measurement_matrix_impl< Matrix<2,4> >;
        using candidate = candidate_impl<measurement_vector, measurement_covariance, measurement_matrix>;
    };
    
    
    template<class T>
    struct detector_traits{};
    
    
    
} //namespace details



template<class Tag>
struct detector_properties {};



namespace details{
    template<class Tag>
    struct detector_traits<detector_properties<Tag>>
    {
        using tag = Tag;
        using candidate = typename Tag::candidate;
    };
} //namespace details



template<class Derived>
struct range_generator{
    using value_type = typename details::detector_traits<Derived>::candidate;
    
private:
    template<class T>
    struct iterator{
        
        iterator(const T& t_p, std::size_t index_p ) : t_m{t_p}, index_m{index_p} {}
        
        
        iterator& operator++(){ ++index_m ; return *this; }
        value_type operator*(){ return t_m.generate_candidate(index_m) ; }
        friend bool operator!=(const iterator& lhs, const iterator& rhs){ return lhs.index_m != rhs.index_m; }
        
        
    private:
        T& t_m;
        std::size_t index_m;
    };
    
    
    template< class T,
    typename std::enable_if_t< !std::is_same< typename details::detector_traits<T>::tag,
    details::vertex_tag>::value,
    std::nullptr_t  > = nullptr         >
    auto make_begin_iterator(T& t_p ) -> iterator<T>
    {
        return {t_p, 0};
    }
    
    template< class T,
    typename std::enable_if_t< std::is_same< typename details::detector_traits<T>::tag,
    details::vertex_tag>::value,
    std::nullptr_t  > = nullptr               >
    auto make_begin_iterator(T& t_p ) -> iterator<T>
    {
        return {t_p, 1};
    }
    
    
    
    template<class T>
    auto make_end_iterator(T& t_p) -> iterator<T>
    {
        return {t_p, derived().layer_count()};
    }
    
public:
    iterator<Derived> begin() const {return make_begin_iterator( derived() );}
    iterator<Derived> end() const {return make_end_iterator( derived() );;}
    
private:
    Derived& derived(){ static_cast<Derived&>(*this); }
    const Derived& derived() const { static_cast<const Derived&>(*this); }
    
};




template<>
struct detector_properties< details::vertex_tag > : range_generator< detector_properties< details::vertex_tag > >
{
    using measurement_vector = details::vertex_tag::measurement_vector;
    using measurement_covariance = details::vertex_tag::measurement_covariance;
    using measurement_matrix = details::vertex_tag::measurement_matrix;
    using candidate = details::vertex_tag::candidate;
    
    
private:
    
    const TAVTntuCluster* cluster_mhc;
    const measurement_matrix matrix_m = {{ 1, 0, 0, 0,
        0, 1, 0, 0  }};
    const double cut_m;
    const std::size_t layer_m;
    
public:
    
    detector_properties( TAVTntuCluster* cluster_phc, double cut_p, std::size_t layer_p = 4)
    :  cluster_mhc{cluster_phc} , cut_m{cut_p}, layer_m{ layer_p } {}
    
    
    std::vector<candidate> generate_candidate(std::size_t index_p) const
    {
        std::vector<candidate> candidate_c;
        std::size_t entries = cluster_mhc->GetClustersN(index_p);
        candidate_c.reserve( entries );
        
        for(std::size_t i{0}; i < entries ; ++i)
        {
            auto transformation = static_cast<TAGgeoTrafo&>( * gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
            
            auto cluster = cluster_mhc->GetCluster(index_p, i);
            auto position =  transformation.FromVTLocalToGlobal(cluster->GetPositionG());
            auto error = cluster->GetPosError();
            
            candidate_c.emplace_back( measurement_vector{{ position.X(), position.Y() }},
                                     measurement_covariance{{ error.X(),         0,
                0, error.Y()    }},
                                     measurement_matrix{matrix_m} );
        }
        
        return candidate_c;
    }
};


//______________________________________________________________________________
//

struct particle_properties
{
    int charge;
    double momentum;
};


struct model
{
    using operating_state_t = operating_state<Matrix<2,1>, 2>;
    
    particle_properties* particle_h;
    
    //auto operator()(const operating_state_t& os_p)
    Matrix<2,1> operator()(const int& os_p) const
    {
        std::cout << particle_h->charge <<  "  " << particle_h->momentum << '\n';
        return {};
        //return particle_h->charge/particle_h->momentum * compute_R(os_p) * compute_change(os_p);
    }
    
private:
    double compute_R(const operating_state_t& os_p)
    {
        return sqrt( os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(0,0) +
                     os_p.state(details::order_tag<1>{})(1,0) * os_p.state(details::order_tag<1>{})(1,0) +
                     1 );
    }
    
    Matrix<2,1> compute_change(const operating_state_t& os_p)
    {
        return {compute_change_x(os_p), compute_change_y(os_p)};
    }
    
    double compute_change_x(const operating_state_t& os_p)
    {
        return os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(1,0) /* * Bx*/ -
               (1 + os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(0,0))/* * By*/ +
               os_p.state(details::order_tag<1>{})(1,0)/* * Bz*/ ;
    }
    
    double compute_change_y(const operating_state_t& os_p)
    {
        return (1 + os_p.state(details::order_tag<1>{})(1,0) * os_p.state(details::order_tag<1>{})(1,0) /* * Bx*/ -
                os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(1,0))/* * By*/ +
                os_p.state(details::order_tag<1>{})(0,0)/* * Bz*/ ;
    }
    
    
};


#endif











