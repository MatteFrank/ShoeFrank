/*!
 \file TATOEdetector.hxx
 \brief   Declaration of TATOEdetector.
 
 \author A. Sécher
 */

//
//File      : TATOEdetector.hxx
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


template<class Tag>
struct detector_properties {};

namespace details{

/*!
 \struct cut
 \brief  chi2 cuts for each detector
*/
struct cut{
   //! operator +=
   //! \param[in] value_p value to add
    constexpr cut& operator+=( int value_p ){
        (value_p < 0) ?
              ((-value_p > static_cast<int>(value-1)) ? (value=1) : value+=value_p ) :
              value+=value_p;
        return *this;
    }
    //! operator size_t
    constexpr operator std::size_t () const { return value; }
    std::size_t value; ///< cut value
};

template<class D> struct detector_traits{};

/*!
 \struct vertex_tag
 \brief  Vertex informations
*/
struct vertex_tag{
    using vector_matrix      = matrix<2, 1>; ///< Initial slope vector
    using covariance_matrix  = matrix<2, 2>; ///< Covariance matrix
    using measurement_matrix = matrix<2, 4>; ///< Kalman measurement matrix
    using data_type          = TAVTbaseCluster; ///< Data type
    using candidate          = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>; ///< Candidate information array
    using cut_t              = std::array<cut, 2>; ///< cut container
    constexpr static uint8_t shift       = 3;      ///< shift
    constexpr static double block_weight = 0.5;    ///< block weight
   
    constexpr static cut_t default_cut_value{17,21}; ///< Default cuts for 16O200C2H4_basec
//    constexpr static cut_t default_cut_value{18,13}; //16O200C_basec
//    constexpr static cut_t default_cut_value{15 ,21}; //12C200C_basec
//    constexpr static cut_t default_cut_value{8, 9}; //16O400C_basec
};
  
/*!
 \struct it_tag
 \brief  Inner tracker informations
*/
struct it_tag{
    using vector_matrix      = matrix<2, 1>; ///< Initial slope vector
    using covariance_matrix  = matrix<2, 2>; ///< Covariance matrix
    using measurement_matrix = matrix<2, 4>; ///< Kalman measurement matrix
    using data_type          = TAITcluster;  ///< Data type
    using candidate          = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>; ///< Candidate information array
    using cut_t              = std::array<cut, 4>; ///< cut container
    static constexpr uint8_t shift       = 2;      ///< shift
    constexpr static double block_weight = 0.37;   ///< block weight
   
    constexpr static cut_t default_cut_value{59, 49, 63, 61}; ///< Default cuts for 16O200C2H4_basec
//    constexpr static cut_t default_cut_value{74, 42, 69, 67}; //16O200C_basec
//    constexpr static cut_t default_cut_value{73, 51, 77, 41}; //12C200C_basec
//    constexpr static cut_t default_cut_value{39,29,32, 29}; //16O400C_basec

};

/*!
 \struct msd_tag
 \brief  Micro strip detector informations
*/
struct msd_tag{
    using vector_matrix      = matrix<1, 1>; ///< Initial slope vector
    using covariance_matrix  = matrix<1, 1>; ///< Covariance matrix
    using measurement_matrix = matrix<1, 4>; ///< Kalman measurement matrix
    using data_type          = TAMSDcluster; ///< Data type
    using candidate          = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>; ///< Candidate information array
    using cut_t              = std::array<cut, 6>; ///< cut container
    static constexpr uint8_t shift       = 1;      ///< shift
    constexpr static double block_weight = 0.08;   ///< block weight
   
    constexpr static cut_t default_cut_value{6, 5, 3, 4, 2, 3}; ///< Default cuts for 16O200C2H4_basec
//    constexpr static cut_t default_cut_value{5,6,4,4,2,4};  //16O200C_basec
//    constexpr static cut_t default_cut_value{9,6,6,4,4,2};  //12C200C_basec
//    constexpr static cut_t default_cut_value{4, 4, 5, 2, 6, 3};  //16O400C_basec
};

/*!
 \struct ms2d_tag
 \brief  Micro strip detector 2D informations
*/
struct ms2d_tag{
    using vector_matrix      = matrix<2, 1>; ///< Initial slope vector
    using covariance_matrix  = matrix<2, 2>; ///< Covariance matrix
    using measurement_matrix = matrix<2, 4>; ///< Kalman measurement matrix
    using data_type          = TAMSDpoint;   ///< Data type
    using candidate          = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>; ///< Candidate information array
    using cut_t              = cut;             ///< cut container
    static constexpr uint8_t shift       = 4;   ///< shift
    constexpr static double block_weight = 0.5; ///< block weight
   
    constexpr static cut_t default_cut_value{25}; ///< default cut values
};

/*!
 \struct tof_tag
 \brief Tof wall informations
*/
struct tof_tag{
    using vector_matrix      = matrix<2, 1>; ///< Initial slope vector
    using covariance_matrix  = matrix<2, 2>; ///< Covariance matrix
    using measurement_matrix = matrix<2, 4>; ///< Kalman measurement matrix
    using data_type          = TATWpoint;    ///< Data type
    using candidate          = candidate_impl< vector_matrix, covariance_matrix, measurement_matrix, data_type>; ///< Candidate information array
    using cut_t              = std::array<cut, 2 >; ///< cut container
    static constexpr uint8_t shift       = 0;       ///< shift
    constexpr static double block_weight = 0.05;    ///< block weight

    constexpr static cut_t default_cut_value{7,9}; ///< Default cuts for 16O200C2H4_basec
//    constexpr static cut_t default_cut_value{9,11}; //16O200C_basec
//    constexpr static cut_t default_cut_value{12,15};  //12C200C_basec
//    constexpr static cut_t default_cut_value{5,10}; //16O400C_basec
};
  
/*!
 \class detector_traits<detector_properties<Tag>>
 \tparam Tag detector
 \brief  Properties of detectors
 */
template<class Tag>
struct detector_traits<detector_properties<Tag>>
{
    using tag                            = Tag;                      ///< detector
    using candidate                      = typename Tag::candidate;  ///< candidate
    constexpr static double block_weight = Tag::block_weight;        ///< weight
};

} //namespace details

namespace orientation{
   /*!
    \struct x
    \brief orientation in x
    */
   struct x{
      static constexpr std::size_t index = 0; ///< index = 0
   };
   /*!
    \struct y
    \brief orientation in y
    \param[in] index = 1
    */
   struct y{
      static constexpr std::size_t index = 1; ///< index = 1
   };
}//namespace orientation

/*!
 \struct layer_generator
 \brief  Generates candidates for layer
 */
template<class DetectorProperties>
struct layer_generator
{
    using candidate = typename details::detector_traits<DetectorProperties>::candidate; ///< candidate
   
   /*!
    \struct layer
    \brief  layer structure
    */
    struct layer{
        std::vector<candidate>            candidate_c; ///< vector of candidate
        const double                      depth;       ///< depth of layer
        std::array<details::cut, 2> const cut_c;       ///< Cut array
        static constexpr double block_weight = details::detector_traits<DetectorProperties>::block_weight; ///< block weight
       
        //! Get candidates
        std::vector<candidate>&       get_candidates()       { return candidate_c;         }
        //! Get candidates const
        std::vector<candidate> const& get_candidates() const { return candidate_c;         }
        //! empty candidate list
        bool empty()                                         { return candidate_c.empty(); }

        template<class T>
        //! return cut value for a given index
        details::cut cut_value(T)                      const { return cut_c[T::index];     }
        
    };
   
   /*!
    \struct iterator
    \brief Layer iterator
    */
    struct iterator{
        
        using value_type = layer; ///< layer number
       
        //! Constructor
        //! \param[in] detector_p detector properties
        //! \param[in] index_p a given index
        iterator(const DetectorProperties& detector_p, std::size_t index_p )
         : detector_m{detector_p},
          index_m{index_p} {}
       
        //! Increment operator
        iterator& operator++(){ ++index_m ; return *this; }
       //! Multiplier operator
        value_type operator*()
        {
            return {
                detector_m.generate_candidates(index_m),
                detector_m.layer_depth(index_m),
                {detector_m.get_cut_values(2*static_cast<int>(index_m/2)), detector_m.get_cut_values(2*static_cast<int>(index_m/2)+1) }
            };
        }
        //! Operator not equal
        friend bool operator!=(const iterator& lhs, const iterator& rhs){ return lhs.index_m != rhs.index_m; }
       
    private:
        const DetectorProperties& detector_m; ///< Detector properties
        std::size_t               index_m;    ///< index array
    };

public:
    //! Constructor
    //! param[in] detector_p detector properties
    layer_generator( DetectorProperties const & detector_p) :
        detector_m{detector_p} {}
   
    //! begin iterator
    iterator begin() const { return {detector_m, 0}; }
    //! end iterator
    iterator end()   const { return {detector_m, detector_m.layer_count()}; }
    
private:
    DetectorProperties const & detector_m; ///< Detector properties
};

/*!
 \struct layer_generator<D<details::ms2d_tag>>
 \brief  Generates candidates in layer for MSD in 2D
 */
template<template<class> class D>
struct layer_generator<D<details::ms2d_tag>>
{
    using candidate = typename details::ms2d_tag::candidate; ///< candidate
   
    /*!
    \struct layer
    \brief  layer structure
    */
    struct layer{
        std::vector<candidate> candidate_c; ///< vector of candidates
        const double           depth;       ///< depth of layer
        const details::cut     cut;         ///< cut array
        static constexpr double block_weight = details::ms2d_tag::block_weight; ///< block weight
       
        //! Get candidates
        std::vector<candidate>&        get_candidates()       { return candidate_c;         }
        //! Get candidates const
        std::vector<candidate> const&  get_candidates() const { return candidate_c;         }
        //! empty candidate list
        bool empty()                                          { return candidate_c.empty(); }

        template<class T>
       //! return cut value
        details::cut cut_value(T)                       const { return cut;                 }
        
    };
   
    /*!
     \struct iterator
     \brief Layer iterator
    */
    struct iterator{
        
        using value_type = layer; ///< layer number
       
        //! Constructor
        //! \param[in] detector_p detector properties
        //! \param[in] index_p a given index
        iterator(const D<details::ms2d_tag>& detector_p, std::size_t index_p )
         : detector_m{detector_p},
           index_m{index_p} {}
       
        //! Increment operator
        iterator& operator++(){ ++index_m ; return *this; }
        //! Multiplier operator
        value_type operator*()
        {
            return {
                detector_m.generate_candidates(index_m),
                detector_m.layer_depth(index_m),
                detector_m.get_cut_value()
            };
        }
        //! Operator not equal
        friend bool operator!=(const iterator& lhs, const iterator& rhs){ return lhs.index_m != rhs.index_m; }
       
    private:
        const D<details::ms2d_tag>& detector_m; ///< Detector properties
        std::size_t                 index_m;    ///< index array
    };

public:
    //! Constructor
    //! param[in] detector_p detector properties
    layer_generator( D<details::ms2d_tag> const & detector_p) :
        detector_m{detector_p} {}
   
    //! begin iterator
    iterator               begin()          const   { return {detector_m, 1};                        }
    //! end iterator
    iterator               end()            const   { return {detector_m, detector_m.layer_count()}; }
    //! Get vector candidates
    std::vector<candidate> generating_candidates( ) { return detector_m.generate_candidates(0);      }
    
private:
    D<details::ms2d_tag> const & detector_m; ///< Detector properties
};


/*!
 \struct track_list
 \brief track list
 */
template<class Detector >
struct track_list
{
    using track_type     = typename Detector::track;     ///< track data
    using vertex_type    = typename Detector::vertex;    ///< vertex data
    using candidate_type = typename Detector::candidate; ///< candidate
   
   /*!
    \struct pseudo_layer
    \brief pseudo layer
    */
    struct pseudo_layer{
        candidate_type             candidate_m; ///< candidate
        double                     depth;       ///< depth of layer
        std::array<details::cut,2> cut_c;       ///< cut array
       
        //! Get candidates
        candidate_type&       get_candidate()       { return candidate_m; }
        //! Get candidates const
        candidate_type const& get_candidate() const { return candidate_m; }
        
        template<class T>
        //! return cut value
        details::cut          cut_value(T)    const { return cut_c[T::index]; }
    };
   
    /*!
    \struct iterable_track
    \brief Track iterator
    */
    struct iterable_track{
       
        /*!
        \struct iterator
        \brief Iterator
        */
        struct iterator{
            
            using value_type = pseudo_layer; ///< pseudo layer number
            //! constructor
            //! \param[in] track_p track iterator
            //! \param[in] index_p a given index
            iterator( const iterable_track& track_p, std::size_t index_p )
             : track_m{track_p},
              index_m{index_p} {}
            //! Increment operator
            iterator& operator++(){ ++index_m ; return *this; }
            //! Multiplier operator
            value_type operator*() { return track_m.form_layer(index_m); }
            //! Operator not equal
            friend bool operator!=(const iterator& lhs, const iterator& rhs){ return lhs.index_m != rhs.index_m; }
            
            
        private:
            const iterable_track& track_m; ///< iterable track
            std::size_t           index_m; ///< index
        };
        
    public:
        //! Constructor
        //! param[in] detector_p detector properties
        //! param[in] vertex_ph vertices data
        //! param[in] track_ph track data
        iterable_track( Detector const& detector_p,
                       vertex_type const* vertex_ph,
                       track_type const* track_ph )
        : detector_m{ detector_p },
          vertex_mh{ vertex_ph },
          track_mh{track_ph} {}
       
        //! begin iterator
        iterator           begin()                { return iterator( *this, starting_cluster_m );       }
        //! end iterator
        iterator           end()                  { return iterator( *this, track_mh->GetClustersN() ); }
       
        //! Get index skipped cluster
        void               skip_first_layer()     { starting_cluster_m = 1;          }
        //! Get first cluster from last plane
        auto const*        first_cluster()  const { return track_mh->GetCluster( track_mh->GetClustersN() -1 );}
        //! Get vertices data type
        vertex_type const* vertex()         const { return vertex_mh;                }
        //! Get size of cluster containers track
        std::size_t        size()           const { return track_mh->GetClustersN(); }
        //! Get track pointer
        track_type const*  get_underlying_track() { return track_mh;                 }
        
    private:
       //! return pseudo lay for a given index
        pseudo_layer form_layer( std::size_t index_p ) const {
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
        std::size_t        starting_cluster_m{0}; ///< Starting cluster index
        Detector const&    detector_m; ///< detector
        vertex_type const* vertex_mh;  ///< vertices data
        track_type const*  track_mh;   ///< track data
    };
    
    using iterator = typename std::vector<iterable_track>::iterator; ///< iterator
    
public:
   //! Constructor
   //! param[in] detector_p detector properties
   //! param[in] track_pc array of iterable tracks
    track_list( Detector const & detector_p,
                std::vector<iterable_track> track_pc )
    : detector_m{ detector_p },
      track_c{ std::move(track_pc) } {}
   
    //! begin iterator
    iterator begin() { return track_c.begin(); }
    //! end iterator
    iterator end()   { return track_c.end();   }
    
private:
    Detector const &            detector_m; ///< detector
    std::vector<iterable_track> track_c;    ///< array of iterable tracks
};

/*!
 \struct detector_properties< details::vertex_tag >
 \brief detector properties for VTX
*/
template<>
struct detector_properties< details::vertex_tag >
{
    using candidate              = details::vertex_tag::candidate;             ///< candidate
    using measurement_vector     = underlying<candidate>::vector;              ///< measurement slope vector
    using measurement_covariance = underlying<candidate>::covariance;          ///< covariance matrix
    using measurement_matrix     = underlying<candidate>::measurement_matrix;  ///< measurement matrix
    using data_type              = underlying<candidate>::data_type;           ///< data informations
    using track                  = TAVTtrack;                                  ///< vertex track
    using vertex                 = TAVTvertex;                                 ///< vertices
    
private:
    const TAVTntuVertex*             vertex_mhc;  ///< vertex container
    const TAVTntuCluster*            cluster_mhc; ///< cluster container
    const measurement_matrix         matrix_m = {{ 1, 0, 0, 0,
                                                   0, 1, 0, 0  }};///< initial measurements matrix
    details::vertex_tag::cut_t       cut_mc{details::vertex_tag::default_cut_value}; ///< cut value
    constexpr static std::size_t     layer{4}; ///< static layer number
    const std::array<double, layer>  depth_mc; ///< depth layer (in z)
   
public:
    //might go to intermediate struc holding the data ?
    //! constructor
    //! \param[in] vertex_phc vertices container
    //! \param[in] cluster_phc cluster container
    //! \param[in] geo_ph geo paramters
    detector_properties( TAVTntuVertex* vertex_phc,
                         TAVTntuCluster* cluster_phc,
                         TAVTparGeo* geo_ph )
    :   vertex_mhc{ vertex_phc },
        cluster_mhc{ cluster_phc },
        depth_mc{ retrieve_depth(geo_ph) } {}
   
private:
    template<std::size_t ... Indices>
    //! return local to global position for a given sensor
    //! \param[in] geo_ph geo parameters
    //! \param[in] Indices given indices
    auto retrieve_depth_impl( TAVTparGeo* geo_ph,
                                         std::index_sequence<Indices...> ) const
            -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        return {{transformation_h->FromVTLocalToGlobal(geo_ph->GetSensorPosition(Indices)).Z()...}};
    }
   
    //! return depth array
    //! \param[in] geo_ph geo parameters
    auto retrieve_depth( TAVTparGeo* geo_ph ) const
            -> std::array<double, layer>
    {
        return retrieve_depth_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
   
public:
    //! return layer number
    constexpr std::size_t  layer_count()                      const { return layer;             }
    //! return layer depth
    constexpr double       layer_depth( std::size_t index_p)  const { return depth_mc[index_p]; }
    //! Get cut value
    //! \param[in] index_p: index position of the cut
    constexpr details::cut get_cut_value(std::size_t index_p) const { return cut_mc[index_p];   }
    // Generates candidate from cluster
    candidate generate_candidate( TAVTbaseCluster const * cluster_h  ) const ;
  
    //! Get track list
    track_list<detector_properties> get_track_list( ) const
    {
        auto vertex_c = retrieve_vertices();
        return { *this, form_tracks( std::move(vertex_c) ) };
    }
    // Generates candidates
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
   
    //! Set cuts
    //! \param[in] cut_pc a given cut
    constexpr void set_cuts( details::vertex_tag::cut_t&&  cut_pc ) { cut_mc = std::move(cut_pc);      }
    //! Set cuts from vector
    //! \param[in] cut_pc a given vector cut
              void set_cuts( std::vector<std::size_t> cut_pc )      { cut_mc = {cut_pc[0], cut_pc[1]}; }

private:
    // Retrieves vertices
    std::vector< TAVTvertex const *> retrieve_vertices( ) const;
    // Retrieves vector of track list
    std::vector< track_list< detector_properties >::iterable_track >
        form_tracks( std::vector< TAVTvertex const * > vertex_pc ) const ;

};

//______________________________________________________________________________
//                      IT
/*!
 \struct detector_properties< details::it_tag >
 \brief detector properties for ITR
 */
template<>
struct detector_properties< details::it_tag >
{
    using candidate              = details::it_tag::candidate;                ///< candidate
    using measurement_vector     = underlying<candidate>::vector;             ///< measurement slope vector
    using measurement_covariance = underlying<candidate>::covariance;         ///< covariance matrix
    using measurement_matrix     = underlying<candidate>::measurement_matrix; ///< measurement matrix
    using data_type              = underlying<candidate>::data_type;          ///< data informations
    
    
private:
    const TAITntuCluster*        cluster_mhc; ///< cluster container
    const measurement_matrix     matrix_m = {{ 1, 0, 0, 0,
                                               0, 1, 0, 0  }}; ///< initial measurements matrix
    details::it_tag::cut_t       cut_mc{details::it_tag::default_cut_value};  ///< cut value
    constexpr static std::size_t layer{4}; ///< static layer number
    
    using sensor_container_t = std::array<std::size_t, 8>;
    std::array<sensor_container_t, 4> plane_mc{
       sensor_container_t{  0,  1,  2,  3,  8,  9, 10, 11 },
       sensor_container_t{ 16, 17, 18, 19, 24, 25, 26, 27 },
       sensor_container_t{  4,  5,  6,  7, 12, 13, 14, 15 },
       sensor_container_t{ 20, 21, 22, 23, 28, 29, 30, 31 }
    }; ///< Default values for sensor id per layer
    
    const std::array<double, layer> depth_mc; ///< depth layer (in z)
    
public:
    //might go to intermediate struc holding the data ?
    //! constructor
    //! \param[in] cluster_phc cluster container
    //! \param[in] geo_ph geo paramters
    detector_properties( TAITntuCluster* cluster_phc,
                         TAITparGeo* geo_ph )
     : cluster_mhc{cluster_phc},
       depth_mc{ retrieve_depth(geo_ph) }
    { set_plane_mc(geo_ph); }
   
   //! Set sensor id per plane
   //! \param[in] geo_ph geo paramters
   void set_plane_mc(TAITparGeo* geo_ph) {
      sensor_container_t array[4];
      for (Int_t i = 0; i < geo_ph->GetSubLayersN(); ++i) {
         std::size_t* p = geo_ph->GetSensorsPerLayer(i);
         std::copy(p, p+8, array[i].begin());
      }
      plane_mc = {array[0], array[1], array[2], array[3]};
   }
   
private:
    template<std::size_t ... Indices>
    //! return local to global position for a given sensor
    //! \param[in] geo_ph geo parameters
    //! \param[in] Indices given indices
    auto retrieve_depth_impl( TAITparGeo* geo_ph, std::index_sequence<Indices...> ) const
        -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        
        return {{transformation_h->FromITLocalToGlobal(geo_ph->GetSensorPosition(plane_mc[Indices][0])).Z()...}};
    }
   
    //! return depth array
    //! \param[in] geo_ph geo parameters
    auto retrieve_depth( TAITparGeo* geo_ph ) const
        -> std::array<double, layer>
    {
        return retrieve_depth_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
   
public:
    //! return layer
    constexpr std::size_t  layer_count()                         const { return layer;               }
    //! return layer depth for a given index
    //! \param[in] index_p a given index
    constexpr double       layer_depth( std::size_t index_p)     const { return depth_mc[index_p];   }
    //! Get cut value
    //! \param[in] index_p: index position of the cut
    constexpr details::cut get_cut_values( std::size_t index_p ) const { return cut_mc[index_p];     }
   
    //! Set cuts
    //! \param[in] cut_pc a given cut
    constexpr void set_cuts( details::it_tag::cut_t&& cut_pc )         { cut_mc = std::move(cut_pc); }
    //! Set cuts from vector
    //! \param[in] cut_pc a given vector cut
              void set_cuts( std::vector<std::size_t> cut_pc )         { cut_mc = { cut_pc[0], cut_pc[1], cut_pc[2], cut_pc[3]}; }

   //! return layer generator
    layer_generator<detector_properties> form_layers()           const { return {*this};             }
   
    // Generates candidates
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
};

//______________________________________________________________________________
//                      MSD
/*!
 \struct detector_properties< details::msd_tag >
 \brief detector properties for MSD
 */
template<>
struct detector_properties< details::msd_tag >
{
    using candidate              = details::msd_tag::candidate;               ///< candidate
    using measurement_vector     = underlying<candidate>::vector;             ///< measurement slope vector
    using measurement_covariance = underlying<candidate>::covariance;         ///< covariance matrix
    using measurement_matrix     = underlying<candidate>::measurement_matrix; ///< measurement matrix
    using data_type              = underlying<candidate>::data_type;          ///< data informations
   
private:
    const TAMSDntuCluster*                  cluster_mhc; ///< cluster container
    std::array<measurement_matrix, 2> const matrix_mc{
                                                      measurement_matrix{ 1, 0, 0, 0 },
                                                      measurement_matrix{ 0, 1, 0, 0 }
                                                      };///< initial measurements matrix
    details::msd_tag::cut_t                 cut_mc{details::msd_tag::default_cut_value}; ///< cut value
    constexpr static std::size_t            layer{6}; ///< static layer number
    const std::array<std::size_t, layer>    view_mc;  ///< view number
    const std::array<double, layer>         depth_mc; ///< depth layer (in z)
    
public:
    //might go to intermediate struc holding the data ?
    //! constructor
    //! \param[in] cluster_phc cluster container
    //! \param[in] geo_ph geo paramters
    detector_properties( TAMSDntuCluster* cluster_phc,
                         TAMSDparGeo* geo_ph )
    : cluster_mhc{cluster_phc},
      view_mc{ retrieve_view(geo_ph) },
      depth_mc{ retrieve_depth(geo_ph) }{ }
   
private:
    template<std::size_t ... Indices>
    //! return local to global position for a given sensor
    //! \param[in] geo_ph geo parameters
    //! \param[in] Indices given indices
    auto retrieve_depth_impl( TAMSDparGeo* geo_ph,
                             std::index_sequence<Indices...> ) const
    -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        
        return {transformation_h->FromMSDLocalToGlobal(geo_ph->GetSensorPosition(Indices)).Z()...};
    }
    
    //! return depth array
    //! \param[in] geo_ph geo parameters
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
   
    //! return view array
    //! \param[in] geo_ph geo parameters
    auto retrieve_view( TAMSDparGeo* geo_ph ) const
    -> std::array<std::size_t, layer>
    {
        return retrieve_view_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
   
public:
    //! return layer number
    constexpr std::size_t  layer_count()                         const { return layer;               }
    //! return layer depth for a given plane
    //! \param[in] index_p a given index plane
    constexpr double       layer_depth( std::size_t index_p )    const { return depth_mc[index_p];   }
    //! return cut for a given index
    //! \param[in] index_p a given index plane
    constexpr details::cut get_cut_values( std::size_t index_p ) const { return cut_mc[index_p];     }
   
    //! Set cuts
    //! \param[in] cut_pc a given cut
    constexpr void set_cuts( details::msd_tag::cut_t&& cut_pc )        { cut_mc = std::move(cut_pc); }
    //! Set cuts from vector
    //! \param[in] cut_pc a given vector cut
              void set_cuts( std::vector<std::size_t>  cut_pc )        { cut_mc = {cut_pc[0], cut_pc[1], cut_pc[2], cut_pc[3], cut_pc[4], cut_pc[5]}; }
   
   //! return layer generator
   layer_generator<detector_properties> form_layers()            const { return {*this};             }
   
   // Generates candidates
   std::vector<candidate> generate_candidates(std::size_t index_p) const ;
};


//______________________________________________________________________________
//                      TOF
/*!
 \struct detector_properties< details::tof_tag >
 \brief detector properties for TW
 */
template<>
struct detector_properties< details::tof_tag >
{
    using candidate              = details::tof_tag::candidate;               ///< candidate
    using measurement_vector     = underlying<candidate>::vector;             ///< measurement slope vector
    using measurement_covariance = underlying<candidate>::covariance;         ///< covariance matrix
    using measurement_matrix     = underlying<candidate>::measurement_matrix; ///< measurement matrix
    using data_type              = underlying<candidate>::data_type;          ///< data informations
    
   /*!
    \struct layer
    \brief layers for TW
    */
    struct layer{
        std::vector<candidate>      candidate_c; ///< vector of candidates
        double                      depth;       ///< depth of layer
        std::array<details::cut, 2> cut_c;       ///< cut array
       
        //! Get candidates
        std::vector<candidate>&        get_candidates()       { return candidate_c;         }
        //! Get candidates const
        std::vector<candidate> const & get_candidates() const { return candidate_c;         }
        //! empty candidate list
        bool                           empty()                { return candidate_c.empty(); }
        
        template<class T>
        //! return cut value for a given index
        details::cut cut_value(T)                       const { return cut_c[T::index];     }
    };
    
private:
    const TATWntuPoint*      cluster_mhc; ///< cluster container
    const measurement_matrix matrix_m = {{ 1, 0, 0, 0,
                                           0, 1, 0, 0  }}; ///< initial measurements matrix
    details::tof_tag::cut_t  cut_mc{details::tof_tag::default_cut_value}; ///< cut value
    const double             depth_m;///< depth layer (in z)

    
private:
   //! return depth array
   //! \param[in] geo_ph geo parameters
    double retrieve_depth( TATWparGeo* geo_ph ) const
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        auto offset = (geo_ph->GetLayerPosition(0).Z() - geo_ph->GetLayerPosition(1).Z())/2;

        return transformation_h->FromTWLocalToGlobal(geo_ph->GetLayerPosition(1)).Z() + offset; //1 ? 2 ? 0 ?
    }
   
public:
    //! constructor
    //! \param[in] cluster_phc points container
    //! \param[in] geo_ph geo paramters
    detector_properties( TATWntuPoint* cluster_phc,
                         TATWparGeo* geo_ph)
    :   cluster_mhc{cluster_phc},
        depth_m{ retrieve_depth(geo_ph) } {}
   
    //! return layer number
    constexpr std::size_t  layer_count()                      const { return 1;               }
    //! return layer depth
    constexpr double       layer_depth()                      const { return depth_m;         }
    //! Get cut value
    //! \param[in] index_p: index position of the cut
    constexpr details::cut get_cut_value(std::size_t index_p) const { return cut_mc[index_p]; }
   
   //! Set cuts
   //! \param[in] cut_pc a given cut
   constexpr void set_cuts( details::tof_tag::cut_t&& cut_pc )      { cut_mc = std::move(cut_pc);      }
   //! Set cuts from vector
   //! \param[in] cut_pc a given vector cut
             void set_cuts( std::vector<std::size_t>  cut_pc )      { cut_mc = {cut_pc[0], cut_pc[1]}; }

   
    //remove with sfinae inside of range generator ?
    //! Generates candidates
    std::vector<candidate> generate_candidates() const
    {
        std::vector<candidate> candidate_c;
        std::size_t entries = cluster_mhc->GetPointsN();
        candidate_c.reserve( entries );
       
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
   
    //! Generates candidates
    std::vector<candidate> generate_candidates( std::size_t /**/) const {
        return generate_candidates();
    }
   
    //! return layer
    layer form_layer() const {
        return {
            generate_candidates(),
            layer_depth(),
            {get_cut_value(0), get_cut_value(1)}
        };
    }
};

//______________________________________________________________________________
//                   MS2D
/*!
 \struct detector_properties< details::ms2d_tag >
 \brief detector properties for MSD 2D
 */
template<>
struct detector_properties< details::ms2d_tag >
{
    using candidate              = details::ms2d_tag::candidate;              ///< candidate
    using measurement_vector     = underlying<candidate>::vector;             ///< measurement slope vector
    using measurement_covariance = underlying<candidate>::covariance;         ///< covariance matrix
    using measurement_matrix     = underlying<candidate>::measurement_matrix; ///< measurement matrix
    using data_type              = underlying<candidate>::data_type;          ///< data informations
    
    
private:
    const TAMSDntuPoint*         cluster_mhc; ///< cluster container
    const measurement_matrix     matrix_m = {{ 1, 0, 0, 0,
                                               0, 1, 0, 0  }}; ///< initial measurements matrix
    details::cut                 cut_m{details::ms2d_tag::default_cut_value}; //< cut value
    constexpr static std::size_t layer{3}; ///< static layer number
    
    const std::array<double, layer> depth_mc;
    
public:
    //might go to intermediate struc holding the data ?
    //! constructor
    //! \param[in] cluster_phc cluster container
    //! \param[in] geo_ph geo paramters
    detector_properties( TAMSDntuPoint* cluster_phc,
                         TAMSDparGeo* geo_ph )
    :   cluster_mhc{cluster_phc},
        depth_mc{ retrieve_depth(geo_ph) } {}
    
private:
    template<std::size_t ... Indices>
    //! return local to global position for a given sensor
    //! \param[in] geo_ph geo parameters
    //! \param[in] Indices given indices
    auto retrieve_depth_impl( TAMSDparGeo* geo_ph,
                             std::index_sequence<Indices...> ) const
    -> std::array<double, layer>
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        return {transformation_h->FromMSDLocalToGlobal(geo_ph->GetSensorPosition(Indices)).Z()...};
    }
   
    //! return depth array
    //! \param[in] geo_ph geo parameters
    auto retrieve_depth( TAMSDparGeo* geo_ph ) const
    -> std::array<double, layer>
    {
        return retrieve_depth_impl( geo_ph, std::make_index_sequence<layer>{} );
    }
    
public:
    //! return layer number
    constexpr std::size_t  layer_count()                     const { return layer;             }
    //! return layer depth for a given index
    //! \param[in] index_p a given index
    constexpr double       layer_depth( std::size_t index_p) const { return depth_mc[index_p]; }
    //! Get cut value
    constexpr details::cut get_cut_value()                   const { return cut_m;             }
   
    //! Set cuts
    //! \param[in] cut_p a given cut
    constexpr void set_cuts( details::cut cut_p  )                 { cut_m = std::move(cut_p); }
    //! Set cuts from vector
    //! \param[in] cut_p a given vector cut
              void set_cuts( std::vector<std::size_t> cut_p )      { cut_m = {cut_p[0]};       }

    //! return layer generator
    layer_generator<detector_properties> form_layers()       const { return {*this};           }
   
    // Generates candidates
    std::vector<candidate> generate_candidates(std::size_t index_p) const ;
};


#endif











