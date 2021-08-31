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
#include "matrix_new.hpp"
#include "expr.hpp"


#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"


#include "TAMSDntuCluster.hxx"
#include "TATWntuPoint.hxx"

#include "TAMSDntuPoint.hxx"


class TAVTntuCluster;
class TAVTcluster;
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAITntuCluster.hxx"
class TAITcluster;

#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"


#include "TADIgeoField.hxx"

#include "TAGntuGlbTrack.hxx"

//template<class ... Ps>
//struct TATOEcutter;

template<class Tag>
struct detector_properties {};

namespace details{
    struct vertex_tag{
        using vector_matrix =  matrix<2, 1>;
        using covariance_matrix =  matrix<2, 2> ;
        using measurement_matrix =  matrix<2,4> ;
        using data_type = TAVTbaseCluster; //to be noted
        using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
        using cut_t = double;
        constexpr static uint8_t shift = 3;
//        constexpr static double default_cut_value{15};
        constexpr static double default_cut_value{20};
    };
    struct it_tag{
        using vector_matrix =  matrix<2, 1>;
        using covariance_matrix =  matrix<2, 2> ;
        using measurement_matrix =  matrix<2,4> ;
        using data_type = TAITcluster;
        using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
        using cut_t = std::array<double, 2>;
        static constexpr uint8_t shift = 2;
        constexpr static std::array<double, 2> default_cut_value{20,20};
//        constexpr static std::array<double, 2> default_cut_value{38,42};
//        constexpr static std::array<double, 2> default_cut_value{33,38};

    };
    struct msd_tag{
        using vector_matrix =  matrix<1, 1> ;
        using covariance_matrix = matrix<1, 1> ;
        using measurement_matrix =  matrix<1,4> ;
        using data_type = TAMSDcluster;
        using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
        using cut_t = std::array<double, 3>;
        static constexpr uint8_t shift = 1;
        constexpr static std::array<double, 3> default_cut_value{20,20,20};
        
//        constexpr static std::array<double, 3> default_cut_value{35,16,27};
//        constexpr static std::array<double, 3> default_cut_value{13,18,23};

    };
struct ms2d_tag{
    using vector_matrix =  matrix<2, 1> ;
    using covariance_matrix = matrix<2, 2> ;
    using measurement_matrix =  matrix<2,4> ;
    using data_type = TAMSDpoint;
    using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
    using cut_t = double;
    static constexpr uint8_t shift = 4;

    constexpr static double default_cut_value{20};
};
    struct tof_tag{
        using vector_matrix =  matrix<2, 1>;
        using covariance_matrix =  matrix<2, 2> ;
        using measurement_matrix =  matrix<2,4> ;
        using data_type = TATWpoint;
        using candidate = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>;
        using cut_t = double;
        static constexpr uint8_t shift = 0;
        constexpr static double default_cut_value{3.2};
//        constexpr static double default_cut_value{3};
    };
    
    
    template<class T>
    struct detector_traits{};
    
    struct immutable_tag{};
    struct fleeting_tag{};
    struct normal_tag{};
    struct non_removable_tag{};
    
    struct all_mixed_tag{};
    struct all_separated_tag{};
    
    
    template<class C>
    class cut_holder;
} //namespace details


struct reconstruction_result{
    struct module {
        int charge;
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};
        std::size_t correct_cluster_number{0};
        std::size_t recovered_cluster_number{0};
        std::size_t total_cluster_number{0};
        std::size_t clone_number{0};
        std::size_t fake_number{0};
    };
    std::vector< module > module_c;
    
    void add( reconstruction_result const& result_p ){
        for( auto const& result : result_p.module_c ){
            auto module_i = std::find_if(
                              module_c.begin(),
                              module_c.end(),
                              [&result](auto& module_p){ return module_p.charge == result.charge; }
                                      );
            if( module_i == module_c.end() ){
                module_c.push_back( module{result.charge} );
                module_i = module_c.end() - 1;
            }
            
            module_i->reconstructed_number += result.reconstructed_number;
            module_i->reconstructible_number += result.reconstructible_number;
            module_i->correct_cluster_number += result.correct_cluster_number;
            module_i->recovered_cluster_number += result.recovered_cluster_number;
            module_i->total_cluster_number += result.total_cluster_number;
            module_i->clone_number += result.clone_number;
            module_i->fake_number += result.fake_number;
        }
    }
};


struct TATOEbaseAct {
    template<class C, class ... Ps>
    friend class TATOEoptimizer;
    
    template<class C>
    friend class details::cut_holder;
public:
    virtual void Action()  = 0;
    virtual void Output() = 0 ;
    virtual void RegisterHistograms() = 0;
    virtual ~TATOEbaseAct() = default;
    
private:
    virtual void reset_event_number() = 0;
    
    virtual void set_cuts( details::vertex_tag, double) = 0;
    virtual void set_cuts( details::it_tag, std::array<double, 2> const& ) = 0;
    virtual void set_cuts( details::msd_tag, std::array<double, 3> const& ) = 0;
    virtual void set_cuts( details::tof_tag, double) = 0;
    virtual void set_cuts( details::ms2d_tag, double) = 0;
    
    virtual reconstruction_result retrieve_results( ) = 0;
};



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
        const double cut;
        
        std::vector<candidate>& get_candidates(){ return candidate_c; }
        std::vector<candidate> const & get_candidates() const { return candidate_c; }
        
        double cut_value() const { return cut; }
        
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
                detector_m.get_cut_values(index_m/2)
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
        
        std::vector<candidate>& get_candidates(){ return candidate_c; }
        std::vector<candidate> const & get_candidates() const { return candidate_c; }
        
        double cut_value() const { return cut; }
        
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
        double cut;
        
        candidate_type& get_candidate(){ return candidate_m; }
        candidate_type const & get_candidate() const { return candidate_m; }
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
                detector_m.minimal_cut_value()
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
    double minimal_cut_m{details::vertex_tag::default_cut_value};
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
    constexpr double minimal_cut_value() const { return minimal_cut_m; }

    
    candidate generate_candidate( TAVTbaseCluster const * cluster_h  ) const ;
  
    track_list<detector_properties> get_track_list( ) const
    {
        auto vertex_c = retrieve_vertices();
        return { *this, form_tracks( std::move(vertex_c) ) };
    }
    
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
    
    constexpr void set_cuts( double&& cut_p ){ minimal_cut_m = std::move(cut_p); }
    
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
    std::array<double, 2> cut_mc{details::it_tag::default_cut_value};
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
    
    constexpr void set_cuts( std::array<double, 2>&& cut_pc  ){ cut_mc = std::move(cut_pc); }
    
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
    
    std::array<double, 3> cut_mc{details::msd_tag::default_cut_value};
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
    
    constexpr void set_cuts( std::array<double, 3>&& cut_pc  ){ cut_mc = std::move(cut_pc); }
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
    double cut_m{details::tof_tag::default_cut_value};
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
    constexpr double cut_value() const { return cut_m; }
    
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
            cut_value()
        };
    }
    
    constexpr void set_cuts( double&& cut_p ){ cut_m = std::move(cut_p); }
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
    using operating_state_t = operating_state<matrix<2,1>, 2>;
    
    struct field_interpolation{
        double x;
        double y;
        double z;
    };
    
    particle_properties* particle_h = nullptr;
    static constexpr double conversion_factor = 0.000299792458; //[MeV/c . G^{-1} . cm^{-1} ]
    TADIgeoField const * field_mh; //G
    
    constexpr model(TADIgeoField const * field_ph) : field_mh{field_ph} {}
    
    
    auto operator()(const operating_state_t& os_p) const
    {
       return conversion_factor * particle_h->charge/particle_h->momentum * compute_R(os_p) * compute_change(os_p);
    }
    
private:
    double compute_R(const operating_state_t& os_p) const
    {
        return sqrt( os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(0,0) +
                     os_p.state(details::order_tag<1>{})(1,0) * os_p.state(details::order_tag<1>{})(1,0) +
                     1 );
    }
    
    matrix<2,1> compute_change(const operating_state_t& os_p) const
    {
        TVector3 position{
            os_p.state( details::order_tag<0>{} )(0,0),
            os_p.state( details::order_tag<0>{} )(1,0),
            os_p.evaluation_point
        };
        auto const temp_field = field_mh->GetField(position);
        auto const field = field_interpolation{ temp_field.X(), temp_field.Y(), temp_field.Z() };
        
        return {compute_change_x(os_p, field), compute_change_y(os_p, field)};
    }
    
    double compute_change_x(const operating_state_t& os_p, field_interpolation const& field_p) const
    {
        return os_p.state(details::order_tag<1>{})(0,0) *
                   os_p.state(details::order_tag<1>{})(1,0) *
                        field_p.x -
               ( 1 + os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(0,0) ) *
                        field_p.y +
               os_p.state(details::order_tag<1>{})(1,0) * field_p.z ;
    }
    
    double compute_change_y(const operating_state_t& os_p, field_interpolation const& field_p) const
    {
        return (1 + os_p.state(details::order_tag<1>{})(1,0) *
                    os_p.state(details::order_tag<1>{})(1,0) ) *
                        field_p.x -
                os_p.state(details::order_tag<1>{})(0,0) *
                    os_p.state(details::order_tag<1>{})(1,0) *
                        field_p.y +
                os_p.state(details::order_tag<1>{})(0,0) * field_p.z ;
    }
    
    
};




#endif











