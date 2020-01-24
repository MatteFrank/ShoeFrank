//
// Plot tot residual + Fit for FIRST data
//
/** TATOEutilities class
 
 \author A. Sécher
 */



#ifndef _TATOEutilities_HXX
#define _TATOEutilities_HXX

#include <numeric>
#include <cmath>
#include <iostream>

#include "state.hpp"
#include "matrix.hpp"


#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"


#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"



class TAVTntuCluster;
class TAVTcluster;
class TAVTntuVertex;
class TAVTvertex;

class TAITntuCluster;
class TAITcluster;

#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"


#include "TADIgeoField.hxx"




namespace details{
    struct vertex_tag{
        using vector_matrix =  Matrix<2, 1>;
        using covariance_matrix =  Matrix<2, 2> ;
        using measurement_matrix =  Matrix<2,4> ;
        using data_type = TAVTcluster;
        using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
    };
    struct it_tag{
        using vector_matrix =  Matrix<2, 1>;
        using covariance_matrix =  Matrix<2, 2> ;
        using measurement_matrix =  Matrix<2,4> ;
        using data_type = TAITcluster;
       using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
    };
    struct msd_tag{
        using vector_matrix =  Matrix<1, 1> ;
        using covariance_matrix = Matrix<1, 1> ;
        using measurement_matrix =  Matrix<1,4> ;
        using data_type = TAMSDcluster;
        using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
    };
    struct tof_tag{
        using vector_matrix =  Matrix<2, 1>;
        using covariance_matrix =  Matrix<2, 2> ;
        using measurement_matrix =  Matrix<2,4> ;
        using data_type = TATWpoint;
        using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
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
    using candidate = typename details::detector_traits<Derived>::candidate;
    
    struct layer{
        std::vector<candidate> candidate_c;
        double depth;
        double cut;
        
        std::vector<candidate>& get_candidates(){ return candidate_c; }
        std::vector<candidate> const & get_candidates() const { return candidate_c; }
    };
    
    
    
private:
    //change that to only ref to Derived ?
   // template<class T>
    struct iterator{
        
        using value_type = layer;
        
        iterator(const Derived& derived_p, std::size_t index_p ) : derived_m{derived_p}, index_m{index_p} {}
        
        iterator& operator++(){ ++index_m ; return *this; }
        value_type operator*()
        {
            return {
                derived_m.generate_candidates(index_m),
                derived_m.layer_depth(index_m),
                derived_m.cut_value()
            };
        }
        friend bool operator!=(const iterator& lhs, const iterator& rhs){ return lhs.index_m != rhs.index_m; }
        
        
    private:
        const Derived& derived_m;
        std::size_t index_m;
    };
    
    
    template< class T,
              typename std::enable_if_t< !std::is_same< typename details::detector_traits<T>::tag,
                                                        details::vertex_tag >::value,
                                          std::nullptr_t  > = nullptr         >
    auto make_begin_iterator(const T& t_p ) const -> iterator
    {
        return {t_p, 0};
    }
    
    template< class T,
              typename std::enable_if_t< std::is_same< typename details::detector_traits<T>::tag,
                                                        details::vertex_tag >::value,
                                         std::nullptr_t  > = nullptr               >
    auto make_begin_iterator(const T& t_p ) const -> iterator
    {
        return {t_p, 1};
    }
    
    
    
    template<class T>
    auto make_end_iterator(const T& t_p) const -> iterator
    {
        return {t_p, derived().layer_count()};
    }
    
public:
    iterator begin() const {return make_begin_iterator( derived() );}
    iterator end() const {return make_end_iterator( derived() );;}
    
private:
    Derived& derived(){ return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
    
};


//______________________________________________________________________________
//                              VTX



template<>
struct detector_properties< details::vertex_tag > :
    range_generator< detector_properties< details::vertex_tag > >
{
    using candidate = details::vertex_tag::candidate;
    using measurement_vector = underlying<candidate>::vector;
    using measurement_covariance = underlying<candidate>::covariance;
    using measurement_matrix = underlying<candidate>::measurement_matrix;
    using data_type = underlying<candidate>::data_type;
    
    
private:
    
    const TAVTntuCluster* cluster_mhc;
    const TAVTntuVertex* vertex_mhc;
    const measurement_matrix matrix_m = {{ 1, 0, 0, 0,
                                           0, 1, 0, 0  }};
    const double cut_m;
    constexpr static std::size_t layer{4};
    const std::array<double, layer> depth_mc;
    
    
public:
    //might go to intermediate struc holding the data ?
    constexpr detector_properties( TAVTntuCluster* cluster_phc,
                                   TAVTntuVertex* vertex_phc,
                                   TAVTparGeo* geo_ph,
                                   double cut_p)  :
        cluster_mhc{cluster_phc},
        vertex_mhc{vertex_phc},
        cut_m{cut_p},
        depth_mc{ retrieve_depth(geo_ph) } {}
    
    
private:
    template<std::size_t ... Indices>
    constexpr auto retrieve_depth_impl( TAVTparGeo* geo_ph,
                                         std::index_sequence<Indices...> ) const
            -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        return {{transformation_h->FromVTLocalToGlobal(geo_ph->GetSensorPosition(Indices)).Z()...}};
    }

            
    constexpr auto retrieve_depth( TAVTparGeo* geo_ph ) const
            -> std::array<double, layer>
    {
        return retrieve_depth_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
    
    
public:
    
    constexpr std::size_t layer_count() const { return layer; }
    constexpr double layer_depth( std::size_t index_p) const { return depth_mc[index_p]; }
    constexpr double cut_value() const { return cut_m; }
    
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
    const TAVTvertex * vertex_handle() const ;
            
            

            
};

//______________________________________________________________________________
//                      IT



template<>
struct detector_properties< details::it_tag > :
    range_generator< detector_properties< details::it_tag > >
{
    using candidate = details::it_tag::candidate;
    using measurement_vector = underlying<candidate>::vector;
    using measurement_covariance = underlying<candidate>::covariance;
    using measurement_matrix = underlying<candidate>::measurement_matrix;
    using data_type = underlying<candidate>::data_type;
    
    
private:
    
    const TAITntuCluster* cluster_mhc;
    const measurement_matrix matrix_m = {{ 1, 0, 0, 0,
                                           0, 1, 0, 0  }};
    const double cut_m;
    constexpr static std::size_t layer{4};
    
    
    using sensor_container_t = std::array<std::size_t, 8>;
    const std::array<sensor_container_t, 4> plane_mc{
        sensor_container_t{  0,  1,  2,  3,  8,  9, 10, 11 },
        sensor_container_t{ 16, 17, 18, 19, 24, 25, 26, 27 },
        sensor_container_t{ 4,  5,  6,  7,  12, 13, 14, 15 },
        sensor_container_t{ 20, 21, 22, 23, 28, 29, 30, 31 }
           }; //moche
    
    const std::array<double, layer> depth_mc;
    
public:
    //might go to intermediate struc holding the data ?
    constexpr detector_properties( TAITntuCluster* cluster_phc,
                                   TAITparGeo* geo_ph,
                                   double cut_p)  :
    cluster_mhc{cluster_phc},
    cut_m{cut_p},
    depth_mc{ retrieve_depth(geo_ph) }
    { }
    
    
private:
    template<std::size_t ... Indices>
    constexpr auto retrieve_depth_impl( TAITparGeo* geo_ph,
                                        std::index_sequence<Indices...> ) const
        -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        
        return {{transformation_h->FromITLocalToGlobal(geo_ph->GetSensorPosition(plane_mc[Indices][0])).Z()...}};
    }
    
    
    constexpr auto retrieve_depth( TAITparGeo* geo_ph ) const
        -> std::array<double, layer>
    {
        return retrieve_depth_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
    
    
public:
    
    constexpr std::size_t layer_count() const { return layer; }
    constexpr double layer_depth( std::size_t index_p) const { return depth_mc[index_p]; }
    constexpr double cut_value() const { return cut_m; }
    
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
    
};

//______________________________________________________________________________
//                      TOF

template<>
struct detector_properties< details::tof_tag >
{
    using candidate = details::tof_tag::candidate;
    using measurement_vector = underlying<candidate>::vector;
    using measurement_covariance = underlying<candidate>::covariance;
    using measurement_matrix = underlying<candidate>::measurement_matrix;
    using data_type = underlying<candidate>::data_type;
    
    
    struct layer{
        std::vector<candidate> candidate_c;
        double depth;
        double cut;
        
        std::vector<candidate>& get_candidates(){ return candidate_c; }
        std::vector<candidate> const & get_candidates() const { return candidate_c; }
    };
    
    
    
    
private:
    
    const TATWntuPoint* cluster_mhc;
    const measurement_matrix matrix_m = {{ 1, 0, 0, 0,
                                           0, 1, 0, 0  }};
    const double cut_m;
    const double depth_m;

    
private:
    double retrieve_depth( TATWparGeo* geo_ph ) const
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        
        return transformation_h->FromTWLocalToGlobal(geo_ph->GetLayerPosition(1)).Z(); //1 ? 2 ? 0 ?
    }
    
    
public:
    
    constexpr detector_properties( TATWntuPoint* cluster_phc,
                                   TATWparGeo* geo_ph,
                                   double cut_p) :
        cluster_mhc{cluster_phc},
        cut_m{cut_p},
        depth_m{ retrieve_depth(geo_ph) } {}
    
    
    constexpr double layer_depth() const { return depth_m; }
    constexpr double cut_value() const { return cut_m; }
    
    //remove with sfinae inside of range generator ?
    std::vector<candidate> generate_candidates() const
    {
        std::vector<candidate> candidate_c;
        std::size_t entries = cluster_mhc->GetPointN();
        candidate_c.reserve( entries );
        
//        std::cout << "detector_properties<details::tof_tag>::generate_candidate : " << entries << '\n';
        
        for(std::size_t i{0}; i < entries ; ++i)
        {
            auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
            
            auto cluster_h = cluster_mhc->GetPoint(i);
            auto position =  transformation_h->FromTWLocalToGlobal(cluster_h->GetPosition());
            auto error = cluster_h->GetPosError();
            

            
            candidate_c.emplace_back( measurement_vector{{ position.X(), position.Y() }},
                                      measurement_covariance{{ pow(error.X(), 2),         0,
                                                                      0, pow( error.Y(), 2)   }},
                                      measurement_matrix{matrix_m},
                                      data_handle<data_type>{cluster_h} );

        }
        
        return candidate_c;
    }
    
    
    layer form_layer() const
    {
        return {
            generate_candidates(),
            layer_depth(),
            cut_value()
        };
    }
    


};

//______________________________________________________________________________
//

struct particle_properties
{
    int charge;
    int mass;
    double momentum;
    double track_slope;
    double track_slope_error;
    TATWpoint const * data;
};


struct model
{
    using operating_state_t = operating_state<Matrix<2,1>, 2>;
    
    particle_properties* particle_h = nullptr;
    static constexpr double conversion_factor = 0.000299792458;
    TADIgeoField* field_mh;
    
    constexpr model(TADIgeoField* field_ph) : field_mh{field_ph} {}
    
    
    auto operator()(const operating_state_t& os_p) const
   // Matrix<2,1> operator()(const int& os_p) const
    {
      //  std::cout << "------ LET'S GOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO --------\n";
        //return {};
       return conversion_factor * particle_h->charge/particle_h->momentum * compute_R(os_p) * compute_change(os_p);
    }
    
private:
    double compute_R(const operating_state_t& os_p) const
    {
        return sqrt( os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(0,0) +
                     os_p.state(details::order_tag<1>{})(1,0) * os_p.state(details::order_tag<1>{})(1,0) +
                     1 );
    }
    
    Matrix<2,1> compute_change(const operating_state_t& os_p) const
    {
        return {compute_change_x(os_p), compute_change_y(os_p)};
    }
    
    double compute_change_x(const operating_state_t& os_p) const
    {
        return os_p.state(details::order_tag<1>{})(0,0) *
                   os_p.state(details::order_tag<1>{})(1,0) *
                        field_x(os_p) -
               ( 1 + os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(0,0) ) *
                        field_y(os_p) +
               os_p.state(details::order_tag<1>{})(1,0) * field_z(os_p) ;
    }
    
    double compute_change_y(const operating_state_t& os_p) const
    {
        return (1 + os_p.state(details::order_tag<1>{})(1,0) *
                    os_p.state(details::order_tag<1>{})(1,0) ) *
                        field_x(os_p) -
                os_p.state(details::order_tag<1>{})(0,0) *
                    os_p.state(details::order_tag<1>{})(1,0) *
                        field_y(os_p) +
                os_p.state(details::order_tag<1>{})(0,0) * field_z(os_p) ;
    }
    
    double field_x(const operating_state_t& os_p) const
    {
        TVector3 position{
            os_p.state( details::order_tag<0>{} )(0,0),
            os_p.state( details::order_tag<0>{} )(1,0),
            os_p.evaluation_point
        };
        
//        auto value = field_mh->GetField(position).X();
//        if(value != 0){
//            std::cout << "field_x:: z " << os_p.evaluation_point << '\n';
//            std::cout << "field_x:: value " << value << '\n';
//        }
        return field_mh->GetField(position).X();
    }
    
    double field_y(const operating_state_t& os_p) const
    {
        TVector3 position{
            os_p.state( details::order_tag<0>{} )(0,0),
            os_p.state( details::order_tag<0>{} )(1,0),
            os_p.evaluation_point
        };
        
//        auto value = field_mh->GetField(position).Y();
//        if(value != 0){
//            std::cout << "field_y:: z " << os_p.evaluation_point << '\n';
//            std::cout << "field_y:: value " << value << '\n';
//        }
        return field_mh->GetField(position).Y();
    }
    
    double field_z(const operating_state_t& os_p) const
    {
        TVector3 position{
            os_p.state( details::order_tag<0>{} )(0,0),
            os_p.state( details::order_tag<0>{} )(1,0),
            os_p.evaluation_point
        };
//
//        auto value = field_mh->GetField(position).Z();
//        if(value != 0){
//            std::cout << "field_z:: z " << os_p.evaluation_point << '\n';
//            std::cout << "field_z:: value " << value << '\n';
//        }
        return field_mh->GetField(position).Z();
    }
    
};


#endif











