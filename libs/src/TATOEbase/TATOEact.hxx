

#ifndef _TATOEact_HXX
#define _TATOEact_HXX


#include <utility>
#include <iostream>

#include "detector_list.hpp"
#include "state.hpp"
#include "matrix.hpp"


#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAVTntuCluster.hxx"

/** TATOEact class, action interfacing TOE library
 
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


struct TATOEbaseAct {
    virtual void Action() const  = 0;
    virtual ~TATOEbaseAct() = default;
};


template<class UKF, class DetectorList>
struct TATOEactGlb {};


template<class UKF, class ... DetectorProperties>
struct TATOEactGlb< UKF, detector_list< details::finished_tag,
                                        DetectorProperties...  >   >
            : TATOEbaseAct
{
    using detector_list_t = detector_list< details::finished_tag, DetectorProperties...  >;
    
private:
    UKF ukf_m;
    detector_list_t list_m;
    
public:
    TATOEactGlb(UKF&& ukf_p, detector_list_t&& list_p ) : ukf_m{std::move(ukf_p)}, list_m{std::move(list_p)} {}
    
    void Action() const override { std::cout << "test\n"; };
};



template<class UKF, class DetectorList>
auto make_TATOEactGlb(UKF ukf_p, DetectorList list_p)
        ->TATOEactGlb<UKF, DetectorList>
{
    return {std::move(ukf_p), std::move(list_p)};
}



template<class UKF, class DetectorList>
auto make_new_TATOEactGlb(UKF ukf_p, DetectorList list_p)
        -> TATOEactGlb<UKF, DetectorList> *
{
    return new TATOEactGlb<UKF, DetectorList>{std::move(ukf_p), std::move(list_p)};
}


#endif











