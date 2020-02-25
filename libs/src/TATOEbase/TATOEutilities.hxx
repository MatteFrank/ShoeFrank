//
// Plot tot residual + Fit for FIRST data
//
/** TATOEutilities class
 
 \author A. Sécher
 */

//
//File      : TATOEutilities.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 10/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//


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
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"

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
        using data_type = TAVTbaseCluster; //to be noted
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



template<class DetectorProperties>
struct layer_generator
{
    using candidate = typename details::detector_traits<DetectorProperties>::candidate;
    
    struct layer{
        std::vector<candidate> candidate_c;
        const double depth;
        const std::pair<double, double> cut;
        
        std::vector<candidate>& get_candidates(){ return candidate_c; }
        std::vector<candidate> const & get_candidates() const { return candidate_c; }
        
        double cut_value_x() const { return cut.first; }
        double cut_value_y() const { return cut.second; }
        
        bool empty(){ return candidate_c.empty(); }
    };
    
    
    struct iterator{
        
        using value_type = layer;
        
        iterator(const DetectorProperties& detector_p, std::size_t index_p ) : detector_m{detector_p}, index_m{index_p} {}
        
        iterator& operator++(){ ++index_m ; return *this; }
        value_type operator*()
        {
            return {
                detector_m.generate_candidates(index_m),
                detector_m.layer_depth(index_m),
                std::make_pair( detector_m.cut_value_x(), detector_m.cut_value_y())
            };
        }
        friend bool operator!=(const iterator& lhs, const iterator& rhs){ return lhs.index_m != rhs.index_m; }
        
        
    private:
        const DetectorProperties& detector_m;
        std::size_t index_m;
    };

    
public:
    layer_generator( DetectorProperties const & detector_p) :
        detector_m{detector_p} {}
    iterator begin() const { return {detector_m, 0}; }
    iterator end() const { return {detector_m, detector_m.layer_count()}; }
    
private:
    DetectorProperties const & detector_m;
};


template<class DetectorProperties >
struct track_list
{
    using track = typename DetectorProperties::track;
    using candidate = typename DetectorProperties::candidate;
    
    struct pseudo_layer{
        candidate candidate_m;
        double depth;
        double optimal_cut;
        double minimal_cut;
        
        candidate& get_candidate(){ return candidate_m; }
        candidate const & get_candidate() const { return candidate_m; }
    };
    
    struct iterable_track{
        
        struct iterator{
            
            using value_type = pseudo_layer;
            
            iterator( const DetectorProperties& detector_p,
                     const iterable_track& track_p,
                     std::size_t index_p ) :
            detector_m{detector_p},
            track_m{track_p},
            index_m{index_p} {}
            
            iterator& operator++(){ ++index_m ; return *this; }
            value_type operator*()
            {
                return track_m.form_layer(index_m);
            }
            friend bool operator!=(const iterator& lhs, const iterator& rhs){ return lhs.index_m != rhs.index_m; }
            
            
        private:
            const DetectorProperties& detector_m;
            const iterable_track& track_m;
            std::size_t index_m;
        };
        
    public:
        
        iterable_track( DetectorProperties const & detector_p,
                        track const * track_ph ) :
            detector_m{ detector_p },
            track_mh{track_ph} {}
        iterator begin() { return iterator( detector_m, *this, 0 ); }
        iterator end()   { return iterator( detector_m, *this, track_mh->GetClustersN() ); }
        auto const * first_cluster() const { return track_mh->GetCluster(0); }
        std::size_t size() const { return track_mh->GetClustersN(); }
        
    private:
        pseudo_layer form_layer( std::size_t index_p ) const
        {
            //cluster are in inverse order
            std::size_t real_index = track_mh->GetClustersN() -1 -index_p;
            
            auto* cluster_h = track_mh->GetCluster( real_index );
            auto c = detector_m.generate_candidate( cluster_h );
            
            return pseudo_layer{
                std::move(c),
                detector_m.layer_depth( cluster_h->GetPlaneNumber() ),
                detector_m.optimal_cut_value(),
                detector_m.minimal_cut_value()
            };
        }
        
    private:
        DetectorProperties const & detector_m;
        track const * track_mh;
    };
    
    using iterator = typename std::vector<iterable_track>::iterator;
    
public:
    template<class Vertex>
    track_list( DetectorProperties const & detector_p,
                Vertex const * vertex_ph ) :
        detector_m{ detector_p },
        track_c{ form_tracks( vertex_ph) } {}
    iterator begin() { return track_c.begin(); }
    iterator end() { return track_c.end(); }
    
private:
    template<class Vertex>
    std::vector< iterable_track > form_tracks( Vertex const * vertex_ph ) const{
        std::vector< iterable_track > track_c;
        track_c.reserve( vertex_ph->GetTracksN() );
        for( auto i = 0 ; i < vertex_ph->GetTracksN() ; ++i ){
            track_c.emplace_back( detector_m, vertex_ph->GetTrack(i) );
        }
        return track_c;
    }
    
private:
    DetectorProperties const & detector_m;
    std::vector<iterable_track> track_c;
};

//______________________________________________________________________________
//                              VTX



template<>
struct detector_properties< details::vertex_tag >
{
    using candidate = details::vertex_tag::candidate;
    using measurement_vector = underlying<candidate>::vector;
    using measurement_covariance = underlying<candidate>::covariance;
    using measurement_matrix = underlying<candidate>::measurement_matrix;
    using data_type = underlying<candidate>::data_type;
    
    using track = TAVTtrack;
    
private:
    
    const TAVTntuVertex* vertex_mhc;
    const TAVTntuCluster* cluster_mhc;
    const measurement_matrix matrix_m = {{ 1, 0, 0, 0,
                                           0, 1, 0, 0  }};
    const double optimal_cut_m;
    const double minimal_cut_m;
    constexpr static std::size_t layer{4};
    const std::array<double, layer> depth_mc;
    
    
public:
    //might go to intermediate struc holding the data ?
    detector_properties( TAVTntuVertex* vertex_phc,
                         TAVTntuCluster* cluster_phc,
                         TAVTparGeo* geo_ph,
                         double optimal_cut_p,
                         double minimal_cut_p ) :
        vertex_mhc{ vertex_phc },
        cluster_mhc{ cluster_phc },
        optimal_cut_m{optimal_cut_p},
        minimal_cut_m{minimal_cut_p},
        depth_mc{ retrieve_depth(geo_ph) } {}
    
    
private:
    template<std::size_t ... Indices>
    auto retrieve_depth_impl( TAVTparGeo* geo_ph,
                                         std::index_sequence<Indices...> ) const
            -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        return {{transformation_h->FromVTLocalToGlobal(geo_ph->GetSensorPosition(Indices)).Z()...}};
    }

            
    auto retrieve_depth( TAVTparGeo* geo_ph ) const
            -> std::array<double, layer>
    {
        return retrieve_depth_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
    
    
    
    
public:
    
    constexpr std::size_t layer_count() const { return layer; }
    constexpr double layer_depth( std::size_t index_p) const { return depth_mc[index_p]; }
    constexpr double minimal_cut_value() const { return minimal_cut_m; }
    constexpr double optimal_cut_value() const { return optimal_cut_m; }

    TAVTvertex const * retrieve_vertex( ) const;
    candidate generate_candidate( TAVTbaseCluster const * cluster_h  ) const ;
  
    track_list<detector_properties> get_track_list( TAVTvertex const * vertex_ph ) const
    {
        return { *this, vertex_ph };
    }
    
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
};


                                

//______________________________________________________________________________
//                      IT



template<>
struct detector_properties< details::it_tag >
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
    const std::pair<double, double> cut_m;
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
    detector_properties( TAITntuCluster* cluster_phc,
                         TAITparGeo* geo_ph,
                         std::pair<double, double> cut_p)  :
        cluster_mhc{cluster_phc},
        cut_m{cut_p},
        depth_mc{ retrieve_depth(geo_ph) }
    { }
    
    
private:
    template<std::size_t ... Indices>
    auto retrieve_depth_impl( TAITparGeo* geo_ph,
                                        std::index_sequence<Indices...> ) const
        -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        
        return {{transformation_h->FromITLocalToGlobal(geo_ph->GetSensorPosition(plane_mc[Indices][0])).Z()...}};
    }
    
    
    auto retrieve_depth( TAITparGeo* geo_ph ) const
        -> std::array<double, layer>
    {
        return retrieve_depth_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
    
    
public:
    
    constexpr std::size_t layer_count() const { return layer; }
    constexpr double layer_depth( std::size_t index_p) const { return depth_mc[index_p]; }
    constexpr double cut_value_x() const { return cut_m.first; }
    constexpr double cut_value_y() const { return cut_m.second; }
    
    layer_generator<detector_properties> form_layers() const
    {
        return {*this};
    }
    
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
        bool empty(){ return candidate_c.empty(); }
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
    
    detector_properties( TATWntuPoint* cluster_phc,
                         TATWparGeo* geo_ph,
                         double cut_p) :
        cluster_mhc{cluster_phc},
        cut_m{cut_p},
        depth_m{ retrieve_depth(geo_ph) } {}
    
    constexpr std::size_t layer_count() const { return 1; }
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
    
    
    std::vector<candidate> generate_candidates( std::size_t /**/) const
    {
        return generate_candidates();
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
   // double track_slope;
   // double track_slope_error;
    double light_ion_boost{1};
    
    std::vector<TATWpoint const * > end_point_ch;
    std::vector<TATWpoint const * > const & get_end_points() const { return end_point_ch; }
    std::vector<TATWpoint const * > & get_end_points() { return end_point_ch; }
};


struct model
{
    using operating_state_t = operating_state<Matrix<2,1>, 2>;
    
    particle_properties* particle_h = nullptr;
    static constexpr double conversion_factor = 0.000299792458; //[MeV/c . G^{-1} . cm^{-1} ]
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











