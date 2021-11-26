
/*
 
 \author A. Sécher
 */

//
//File      : TATOEdetector.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 28/07/21
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//


#ifndef _TATOEdetector_HXX
#define _TATOEdetector_HXX

#include <numeric>
#include <cmath>
#include <iostream>

#include "state.hpp"
#include "matrix_new.hpp"
//#include "expr.hpp"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

class TAVTntuCluster;
class TAVTcluster;
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTparGeo.hxx"

#include "TAITntuCluster.hxx"
#include "TAITparGeo.hxx"

#include "TAMSDntuCluster.hxx"
#include "TAMSDntuPoint.hxx"

#include "TATWntuPoint.hxx"


//#include "TAGntuGlbTrack.hxx"


template<class Tag>
struct detector_properties {};

namespace details{
template<class D> struct detector_traits{};

struct vertex_tag{
    using vector_matrix =  matrix<2, 1>;
    using covariance_matrix =  matrix<2, 2> ;
    using measurement_matrix =  matrix<2,4> ;
    using data_type = TAVTbaseCluster; //to be noted
    using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
    using cut_t = std::array<double, 2>;
    constexpr static uint8_t shift = 3;
    constexpr static double block_weight = 0.5;
    
//    constexpr static cut_t default_cut_value{15 ,15};
    constexpr static cut_t default_cut_value{25 ,8}; //16O200C2H4_effc
//    constexpr static cut_t default_cut_value{22 ,9}; //16O200C2H4_massc
};
    
struct it_tag{
    using vector_matrix =  matrix<2, 1>;
    using covariance_matrix =  matrix<2, 2> ;
    using measurement_matrix =  matrix<2,4> ;
    using data_type = TAITcluster;
    using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
    using cut_t = std::array<double, 4>;
    static constexpr uint8_t shift = 2;
    constexpr static double block_weight = 0.37;

//    constexpr static cut_t default_cut_value{15,15,15,15}; //default scan
    constexpr static cut_t default_cut_value{24,33,24,37}; //16O200C2H4_effc
//    constexpr static cut_t default_cut_value{23,30,21,31}; //16O200C2H4_massc
};

struct msd_tag{
    using vector_matrix =  matrix<1, 1> ;
    using covariance_matrix = matrix<1, 1> ;
    using measurement_matrix =  matrix<1,4> ;
    using data_type = TAMSDcluster;
    using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
    using cut_t = std::array<double, 6>;
    static constexpr uint8_t shift = 1;
    constexpr static double block_weight = 0.08;

    constexpr static cut_t default_cut_value{20,25,12,19,3,6};  //16O200C2H4_effc
//    constexpr static cut_t default_cut_value{15,15,15,15,15,15};
//    constexpr static cut_t default_cut_value{17,25,12,19,6,8}; //16O200C2H4_massc

};

struct ms2d_tag{
    using vector_matrix =  matrix<2, 1> ;
    using covariance_matrix = matrix<2, 2> ;
    using measurement_matrix =  matrix<2,4> ;
    using data_type = TAMSDpoint;
    using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
    using cut_t = double;
    static constexpr uint8_t shift = 4;
    constexpr static double block_weight = 0.5;
    constexpr static double default_cut_value{25};
};

struct tof_tag{
    using vector_matrix =  matrix<2, 1>;
    using covariance_matrix =  matrix<2, 2> ;
    using measurement_matrix =  matrix<2,4> ;
    using data_type = TATWpoint;
    using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
    using cut_t = std::array<double, 2 >;
    static constexpr uint8_t shift = 0;
    constexpr static double block_weight = 0.05;

//    constexpr static cut_t default_cut_value{4}; //efficiency+purity optimized 16O_200
    constexpr static cut_t default_cut_value{7,3}; //16O200C2H4_effc
//    constexpr static cut_t default_cut_value{11,2}; //16O200C2H4_massc
};
    
template<class Tag>
struct detector_traits<detector_properties<Tag>>
{
    using tag = Tag;
    using candidate = typename Tag::candidate;
    constexpr static double block_weight = Tag::block_weight;
};

} //namespace details



namespace orientation{
struct x{ static constexpr std::size_t index = 0; };
struct y{ static constexpr std::size_t index = 1; };
}//namespace orientation

template<class DetectorProperties>
struct layer_generator
{
    using candidate = typename details::detector_traits<DetectorProperties>::candidate;
    
    struct layer{
        std::vector<candidate> candidate_c;
        const double depth;
        std::array<double, 2> const cut_c;
        static constexpr double block_weight = details::detector_traits<DetectorProperties>::block_weight;
        
        std::vector<candidate>& get_candidates(){ return candidate_c; }
        std::vector<candidate> const & get_candidates() const { return candidate_c; }
        
        template<class T>
        double cut_value(T) const { return cut_c[T::index]; }
        
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
                {detector_m.get_cut_values(2*static_cast<int>(index_m/2)), detector_m.get_cut_values(2*static_cast<int>(index_m/2)+1) }
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


template<template<class> class D>
struct layer_generator<D<details::ms2d_tag>>
{
    using candidate = typename details::ms2d_tag::candidate;
    
    struct layer{
        std::vector<candidate> candidate_c;
        const double depth;
        const double cut;
        static constexpr double block_weight = details::ms2d_tag::block_weight;
        
        std::vector<candidate>& get_candidates(){ return candidate_c; }
        std::vector<candidate> const & get_candidates() const { return candidate_c; }
        
        template<class T>
        double cut_value(T) const { return cut; }
        
        bool empty(){ return candidate_c.empty(); }
    };
    
    
    struct iterator{
        
        using value_type = layer;
        
        iterator(const D<details::ms2d_tag>& detector_p, std::size_t index_p ) : detector_m{detector_p}, index_m{index_p} {}
        
        iterator& operator++(){ ++index_m ; return *this; }
        value_type operator*()
        {
            return {
                detector_m.generate_candidates(index_m),
                detector_m.layer_depth(index_m),
                detector_m.get_cut_value()
            };
        }
        friend bool operator!=(const iterator& lhs, const iterator& rhs){ return lhs.index_m != rhs.index_m; }
        
        
    private:
        const D<details::ms2d_tag>& detector_m;
        std::size_t index_m;
    };

    
public:
    layer_generator( D<details::ms2d_tag> const & detector_p) :
        detector_m{detector_p} {}
    iterator begin() const { return {detector_m, 1}; }
    iterator end() const { return {detector_m, detector_m.layer_count()}; }
    std::vector<candidate> generating_candidates(){ return detector_m.generate_candidates(0); }
    
private:
    D<details::ms2d_tag> const & detector_m;
};


template<class Detector >
struct track_list
{
    using track_type = typename Detector::track;
    using vertex_type = typename Detector::vertex;
    using candidate_type = typename Detector::candidate;
    
    struct pseudo_layer{
        candidate_type candidate_m;
        double depth;
        std::array<double,2> cut_c;
        
        candidate_type& get_candidate(){ return candidate_m; }
        candidate_type const & get_candidate() const { return candidate_m; }
        
        template<class T>
        double cut_value(T) const { return cut_c[T::index]; }
    };
    
    struct iterable_track{
        
        struct iterator{
            
            using value_type = pseudo_layer;
            
            iterator( const iterable_track& track_p,
                      std::size_t index_p ) :
            track_m{track_p},
            index_m{index_p} {}
            
            iterator& operator++(){ ++index_m ; return *this; }
            value_type operator*()
            {
                return track_m.form_layer(index_m);
            }
            friend bool operator!=(const iterator& lhs, const iterator& rhs){ return lhs.index_m != rhs.index_m; }
            
            
        private:
            const iterable_track& track_m;
            std::size_t index_m;
        };
        
    public:
        
        iterable_track( Detector const & detector_p,
                        vertex_type const * vertex_ph,
                        track_type const * track_ph ) :
            detector_m{ detector_p },
            vertex_mh{ vertex_ph },
            track_mh{track_ph} {}
        iterator begin() { return iterator( *this, 0 ); }
        iterator end()   { return iterator( *this, track_mh->GetClustersN() ); }
        auto const * first_cluster() const {
            return track_mh->GetCluster( track_mh->GetClustersN() -1 );
        }
        vertex_type const * vertex() const { return vertex_mh; }
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
                detector_m.layer_depth( cluster_h->GetSensorIdx() ),
                {detector_m.get_cut_value(0), detector_m.get_cut_value(1)}
            };
        }
        
    private:
        Detector const & detector_m;
        vertex_type const * vertex_mh;
        track_type const * track_mh;
    };
    
    using iterator = typename std::vector<iterable_track>::iterator;
    
public:
    track_list( Detector const & detector_p,
                std::vector<iterable_track> track_pc ) :
        detector_m{ detector_p },
        track_c{ std::move(track_pc) } {}
    iterator begin() { return track_c.begin(); }
    iterator end() { return track_c.end(); }
    
private:
    Detector const & detector_m;
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
    using vertex = TAVTvertex;
    
private:
    
    const TAVTntuVertex* vertex_mhc;
    const TAVTntuCluster* cluster_mhc;
    const measurement_matrix matrix_m = {{ 1, 0, 0, 0,
                                           0, 1, 0, 0  }};
    details::vertex_tag::cut_t cut_mc{details::vertex_tag::default_cut_value};
    constexpr static std::size_t layer{4};
    const std::array<double, layer> depth_mc;
    
    
public:
    //might go to intermediate struc holding the data ?
    detector_properties( TAVTntuVertex* vertex_phc,
                         TAVTntuCluster* cluster_phc,
                         TAVTparGeo* geo_ph ) :
        vertex_mhc{ vertex_phc },
        cluster_mhc{ cluster_phc },
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
    
    constexpr double get_cut_value(std::size_t index_p) const { return cut_mc[index_p]; }

    
    candidate generate_candidate( TAVTbaseCluster const * cluster_h  ) const ;
  
    track_list<detector_properties> get_track_list( ) const
    {
        auto vertex_c = retrieve_vertices();
        return { *this, form_tracks( std::move(vertex_c) ) };
    }
    
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
    
    constexpr void set_cuts( details::vertex_tag::cut_t&&  cut_pc ){ cut_mc = std::move(cut_pc); }
    
private:
    std::vector< TAVTvertex const *> retrieve_vertices( ) const;
    
    std::vector< track_list< detector_properties >::iterable_track >
        form_tracks( std::vector< TAVTvertex const * > vertex_pc ) const ;

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
    details::it_tag::cut_t cut_mc{details::it_tag::default_cut_value};
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
                         TAITparGeo* geo_ph )  :
        cluster_mhc{cluster_phc},
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
    constexpr double get_cut_values( std::size_t index_p ) const { return cut_mc[index_p]; }
    
    constexpr void set_cuts( details::it_tag::cut_t&& cut_pc  ){ cut_mc = std::move(cut_pc); }
    
    layer_generator<detector_properties> form_layers() const
    {
        return {*this};
    }
    
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
};





//______________________________________________________________________________
//                      MSD



template<>
struct detector_properties< details::msd_tag >
{
    using candidate = details::msd_tag::candidate;
    using measurement_vector = underlying<candidate>::vector;
    using measurement_covariance = underlying<candidate>::covariance;
    using measurement_matrix = underlying<candidate>::measurement_matrix;
    using data_type = underlying<candidate>::data_type;
    
    
private:
    
    const TAMSDntuCluster* cluster_mhc;
    std::array<measurement_matrix, 2> const matrix_mc{
                measurement_matrix{ 1, 0, 0, 0 },
                measurement_matrix{ 0, 1, 0, 0 }
                                                      };
    
    details::msd_tag::cut_t cut_mc{details::msd_tag::default_cut_value};
    constexpr static std::size_t layer{6};
    
    const std::array<std::size_t, layer> view_mc;
    const std::array<double, layer> depth_mc;
    
public:
    //might go to intermediate struc holding the data ?
    detector_properties( TAMSDntuCluster* cluster_phc,
                         TAMSDparGeo* geo_ph )  :
    cluster_mhc{cluster_phc},
    view_mc{ retrieve_view(geo_ph) },
    depth_mc{ retrieve_depth(geo_ph) }
    { }
    
    
private:
    template<std::size_t ... Indices>
    auto retrieve_depth_impl( TAMSDparGeo* geo_ph,
                             std::index_sequence<Indices...> ) const
    -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        
        return {transformation_h->FromMSDLocalToGlobal(geo_ph->GetSensorPosition(Indices)).Z()...};
    }
    
    
    auto retrieve_depth( TAMSDparGeo* geo_ph ) const
    -> std::array<double, layer>
    {
        return retrieve_depth_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
                                                        
    template<std::size_t ... Indices>
    auto retrieve_view_impl( TAMSDparGeo* geo_ph,
                              std::index_sequence<Indices...> ) const
    -> std::array<std::size_t, layer>
    {
        return { static_cast<std::size_t>(geo_ph->GetSensorPar(Indices).TypeIdx)...};
    }
                                                        
    auto retrieve_view( TAMSDparGeo* geo_ph ) const
    -> std::array<std::size_t, layer>
    {
        return retrieve_view_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
    
    
public:
    
    constexpr std::size_t layer_count() const { return layer; }
    constexpr double layer_depth( std::size_t index_p ) const { return depth_mc[index_p]; }
    constexpr double get_cut_values( std::size_t index_p ) const { return cut_mc[index_p]; }
    
    layer_generator<detector_properties> form_layers() const
    {
        return {*this};
    }
    
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
    
    constexpr void set_cuts( details::msd_tag::cut_t&& cut_pc  ){ cut_mc = std::move(cut_pc); }
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
        std::array<double, 2> cut_c;
        
        std::vector<candidate>& get_candidates(){ return candidate_c; }
        std::vector<candidate> const & get_candidates() const { return candidate_c; }
        bool empty(){ return candidate_c.empty(); }
        
        template<class T>
        double cut_value(T) const { return cut_c[T::index]; }
    };
    
private:
    
    const TATWntuPoint* cluster_mhc;
    const measurement_matrix matrix_m = {{ 1, 0, 0, 0,
                                           0, 1, 0, 0  }};
    details::tof_tag::cut_t cut_mc{details::tof_tag::default_cut_value};
    const double depth_m;

    
private:
    double retrieve_depth( TATWparGeo* geo_ph ) const
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        auto offset = (geo_ph->GetLayerPosition(0).Z() - geo_ph->GetLayerPosition(1).Z())/2;

        return transformation_h->FromTWLocalToGlobal(geo_ph->GetLayerPosition(1)).Z() + offset; //1 ? 2 ? 0 ?
    }
    
    
public:
    
    detector_properties( TATWntuPoint* cluster_phc,
                         TATWparGeo* geo_ph) :
        cluster_mhc{cluster_phc},
        depth_m{ retrieve_depth(geo_ph) } {}
    
    constexpr std::size_t layer_count() const { return 1; }
    constexpr double layer_depth() const { return depth_m; }
    constexpr double get_cut_value(std::size_t index_p) const { return cut_mc[index_p]; }
    
    //remove with sfinae inside of range generator ?
    std::vector<candidate> generate_candidates() const
    {
        std::vector<candidate> candidate_c;
        std::size_t entries = cluster_mhc->GetPointsN();
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
            {get_cut_value(0), get_cut_value(1)}
        };
    }
    
    constexpr void set_cuts( details::tof_tag::cut_t&& cut_pc ){ cut_mc = std::move(cut_pc); }
};

//______________________________________________________________________________
//                   MS2D


template<>
struct detector_properties< details::ms2d_tag >
{
    using candidate = details::ms2d_tag::candidate;
    using measurement_vector = underlying<candidate>::vector;
    using measurement_covariance = underlying<candidate>::covariance;
    using measurement_matrix = underlying<candidate>::measurement_matrix;
    using data_type = underlying<candidate>::data_type;
    
    
private:
    
    const TAMSDntuPoint* cluster_mhc;
    const measurement_matrix matrix_m = {{ 1, 0, 0, 0,
                                           0, 1, 0, 0  }};
    double cut_m{details::ms2d_tag::default_cut_value};
    constexpr static std::size_t layer{3};
    
    const std::array<double, layer> depth_mc;
    
public:
    //might go to intermediate struc holding the data ?
    detector_properties( TAMSDntuPoint* cluster_phc,
                         TAMSDparGeo* geo_ph )  :
        cluster_mhc{cluster_phc},
        depth_mc{ retrieve_depth(geo_ph) }
    {}
    
    
private:
    template<std::size_t ... Indices>
    auto retrieve_depth_impl( TAMSDparGeo* geo_ph,
                             std::index_sequence<Indices...> ) const
    -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        return {transformation_h->FromMSDLocalToGlobal(geo_ph->GetSensorPosition(Indices)).Z()...};
    }

    auto retrieve_depth( TAMSDparGeo* geo_ph ) const
    -> std::array<double, layer>
    {
        return retrieve_depth_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
    
public:
    
    constexpr std::size_t layer_count() const { return layer; }
    constexpr double layer_depth( std::size_t index_p) const { return depth_mc[index_p]; }
    constexpr double get_cut_value() const { return cut_m; }
    
    constexpr void set_cuts( double cut_p  ){ cut_m = std::move(cut_p); }
    
    layer_generator<detector_properties> form_layers() const
    {
        return {*this};
    }
    
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
};


#endif











