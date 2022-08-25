/*!
 \file TATOEutilities.hxx
 \brief Declaration of TATOEutilities
 
  \author A. Sécher
*/

//
//File      : TATOEutilities.hxx
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 10/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSiS, Strasbourg, France
//


#ifndef _TATOEutilities_HXX
#define _TATOEutilities_HXX

#include <numeric>
#include <cmath>
#include <iostream>


#include "state.hpp"
#include "aftereffect.hpp"
#include "matrix_new.hpp"
#include "expr.hpp"

#include "TATOEdetector.hxx"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TADIgeoField.hxx"

namespace details{
    struct immutable_tag{};
    struct fleeting_tag{};
    struct normal_tag{};
    struct non_removable_tag{};
    
    struct all_mixed_tag{};
    struct all_separated_tag{};
    
   /*!
    \class cut_holder
    \brief cur holder
    */
    template<class C>
    class cut_holder;

    /*!
     \struct x_view_tag
     \brief view orientation in x
    */
    struct x_view_tag{
       static constexpr uint8_t shift = 7; ///< shift = 7
    };

    /*!
     \struct y_view_tag
     \brief view orientation in y
    */
    struct y_view_tag{
       static constexpr uint8_t shift = 6; ///< shift = 6
    };

} //namespace details

/*!
 \struct particle_properties
 \brief particle properties
 */
struct particle_properties{
    double momentum;         ///< momentum
    int    charge;           ///< atomic charge
    int    nucleon_number;   ///< number of nucleon
    double mass;             ///< mass
    double track_slope_x{0}; ///< track slope in x
    double track_slope_y{0}; ///< track slope in y
};

/*!
 \struct polynomial_fit_parameters
 \brief polynomial fit parameters for trajectory
 */
struct polynomial_fit_parameters{
    std::array<double, 4> x; ///< coefficient for x
    double                determination_coefficient_x; ///< I don't know in x
    std::array<double, 2> y;  ///< coefficient for y
    double                determination_coefficient_y; ///< I don't know in x
};

/*!
 \struct track_slope_parameters
 \brief track slope parameters
 */
struct track_slope_parameters{
    double x;       ///< position in x
    double y;       ///< position in x
    double delta_x; ///< delta position in x
    double delta_y; ///< delta position in x
};

/*!
 \struct additional_parameters
 \brief additional parameters for particle/track
 */
struct additional_parameters{
    double theta;   ///< theta angle
    double phi;     ///< phi angle
    double length;  ///< length of track
    double tof;     ///< time of flight of particle
    double beta;    ///< beta of particle
};

/*!
 \struct vertex_position
 \brief vertex position
 */
struct vertex_position{
   double x; ///< position in x
   double y; ///< position in y
};

/*!
 \struct reconstructible_track
 \brief reconstructible track
 */
struct reconstructible_track{
    std::vector<int>      index_c;    ///< index vector
    particle_properties   properties; ///< particle properties
    additional_parameters parameters; ///< additional parameter
   
    //! get vector indices
    std::vector<int> const& get_indices() const { return index_c; }
    //! get vector indices const
    std::vector<int>&       get_indices()       { return index_c; }
};

/*!
 \struct cluster
 \brief cluster parameters
 */
struct cluster{
   
   /*!
    \struct vector
    \brief vector parameters
    */
    struct vector{
       double x; ///< position in x
       double y; ///< position in y
       double z; ///< position in z
    };
   
    vector           position;    ///< position of cluster
    std::vector<int> mc_index_c;  ///< indices vector
    chisquared       chi2;        ///< chi2
    uint8_t          opcode;      ///< option code
};

/*!
 \struct reconstructed_track
 \brief reconstructed track parameters
 */
struct reconstructed_track{
    std::vector<cluster>      cluster_c;        ///< cluster vector
    particle_properties       properties;       ///< particle properties
    double                    shearing_factor;  ///< shearing factor
    polynomial_fit_parameters parameters;       ///< polynomial fit parameters of the track length
    std::size_t               clone_number{0};  ///< clone number
    double                    length;           ///< length of track
    double                    tof;              ///< time of flight of the particle
    double                    beta;             ///< beta of the particle
    double                    determination_coefficient_scan; ///< and I don't know
   
    //! get cluster vector
    std::vector<cluster> const& get_clusters() const { return cluster_c; }
    //! get cluster vector const
    std::vector<cluster>&       get_clusters()       { return cluster_c; }
};

/*!
 \struct reconstruction_module
 \brief reconstruction module parameters
 */
struct reconstruction_module{
    aftereffect::optional<reconstructible_track> reconstructible_o; ///< reconstructible track
    aftereffect::optional<reconstructed_track>   reconstructed_o;   ///< reconstructed track
   
   //! constructor
   //! \param[in] reconstructible_po reconstructible track
   //! \param[in] reconstructed_po reconstructed track
    reconstruction_module( aftereffect::optional<reconstructible_track> reconstructible_po,
                           aftereffect::optional<reconstructed_track>   reconstructed_po)
    : reconstructible_o{ std::move(reconstructible_po) },
      reconstructed_o{ std::move(reconstructed_po) } {}
};


struct TAGcluster;
struct computation_checker;

/*!
 \struct TATOEbaseAct
 \brief base class of TOE action
 */
struct TATOEbaseAct {
    template<class C, class S, template<class> class ... Ps>
    friend class TATOEoptimizer;
    
    template<class C>
    friend class details::cut_holder;
    
    using data_type = TAGcluster; ///< global cluster data
   
public:
    //! virtual Action
    virtual void Action()   = 0;
    //! virtual Output
    virtual void Output()   = 0;
    //! virtual dectructor
    virtual ~TATOEbaseAct() = default;
    //! virtual get computation checker
    virtual std::vector<computation_checker>& get_computation_checker() = 0;
   
private:
     //! virtual reset event number
    virtual void reset_event_number() = 0;
    //! virtual set cut for vertex
    virtual void set_cuts( details::vertex_tag, details::vertex_tag::cut_t const&) = 0;
    //! virtual set cut for inner tracker
    virtual void set_cuts( details::it_tag, details::it_tag::cut_t const& ) = 0;
    //! virtual set cut for micro strip detector
    virtual void set_cuts( details::msd_tag, details::msd_tag::cut_t const& ) = 0;
    //! virtual set cut for tof wall
    virtual void set_cuts( details::tof_tag, details::tof_tag::cut_t const&) = 0;
    //! virtual set cut for micro strip detector in 2D
    virtual void set_cuts( details::ms2d_tag, details::ms2d_tag::cut_t const&) = 0;
   
    //! virtual set cut for vertex
    virtual void set_cuts( details::vertex_tag, std::vector<std::size_t> const& ) = 0;
    //! virtual set cut for inner tracker
    virtual void set_cuts( details::it_tag, std::vector<std::size_t> const& ) = 0;
    //! virtual set cut for micro strip detector
    virtual void set_cuts( details::msd_tag, std::vector<std::size_t> const& ) = 0;
    //! virtual set cut for tof wall
    virtual void set_cuts( details::tof_tag, std::vector<std::size_t> const&) = 0;
    //! virtual set cut for micro strip detector in 2D
    virtual void set_cuts( details::ms2d_tag, std::vector<std::size_t> const&) = 0;
   
    //! virtual retrieve matched results(
    virtual std::vector<reconstruction_module> const& retrieve_matched_results( ) const = 0;
};

//---------------------------------------------------------------


struct TATWpoint;
/*!
 \struct particle_hypothesis
 \brief particle properties hypothesis
 */
struct particle_hypothesis
{
    particle_properties             properties;         ///< particle properties
    double                          light_ion_boost{1}; ///< ion boost
    std::vector<TATWpoint const * > end_point_ch;       ///< end point vector in TW
   
    //! get end point vector in TW
    std::vector<TATWpoint const * > const& get_end_points() const { return end_point_ch; }
    //! end point vector in TW const
    std::vector<TATWpoint const * >&       get_end_points()       { return end_point_ch; }
};


/*!
 \struct model
 \brief model of propagation
 */
struct model
{
    using operating_state_t = operating_state<matrix<2,1>, 2>; ///< state matrix
   
   /*!
    \struct field_interpolation
    \brief field interpolation
    */
    struct field_interpolation{
        double x; ///< position in x
        double y; ///< position in y
        double z; ///< position in z
    };
    
    particle_hypothesis*    particle_h = nullptr; ///< particle hypotheis
    TADIgeoField const*     field_mh;             ///< field map
    static constexpr double conversion_factor = 0.000299792458; ///< [MeV/c . G^{-1} . cm^{-1} ]
   
    //! constructor
    //! \param[in] field_ph field map
    constexpr model(TADIgeoField const* field_ph)
     : field_mh{field_ph} {}
    
   
    //! return something ?
    //! \param[in] os_p operating state
    auto operator()(const operating_state_t& os_p) const
    {
       return conversion_factor * particle_h->properties.charge/particle_h->properties.momentum * compute_R(os_p) * compute_change(os_p);
    }
    
private:
    //! return R
    //! \param[in] os_p operating state
    double compute_R(const operating_state_t& os_p) const
    {
        return sqrt( os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(0,0) +
                     os_p.state(details::order_tag<1>{})(1,0) * os_p.state(details::order_tag<1>{})(1,0) +
                     1 );
    }
   
    //! compute change
    //! \param[in] os_p operating state
    matrix<2,1> compute_change(const operating_state_t& os_p) const
    {
        TVector3 position{
            os_p.state( details::order_tag<0>{} )(0,0),
            os_p.state( details::order_tag<0>{} )(1,0),
            os_p.evaluation_point
        };
        auto const temp_field = field_mh->GetField(position);
        auto const field      = field_interpolation{ temp_field.X(), temp_field.Y(), temp_field.Z() };
        
        return {compute_change_x(os_p, field), compute_change_y(os_p, field)};
    }
   
    //! compute change in x
    //! \param[in] os_p operating state
    //! \param[in] field_p field interpolation
    double compute_change_x(const operating_state_t& os_p, field_interpolation const& field_p) const
    {
        return os_p.state(details::order_tag<1>{})(0,0) *
                   os_p.state(details::order_tag<1>{})(1,0) *
                        field_p.x -
               ( 1 + os_p.state(details::order_tag<1>{})(0,0) * os_p.state(details::order_tag<1>{})(0,0) ) *
                        field_p.y +
               os_p.state(details::order_tag<1>{})(1,0) * field_p.z ;
    }
   
   //! compute change in y
   //! \param[in] os_p operating state
   //! \param[in] field_p field interpolation
    double compute_change_y(const operating_state_t& os_p, field_interpolation const& field_p) const
    {
        return (1 + os_p.state(details::order_tag<1>{})(1,0) *
                    os_p.state(details::order_tag<1>{})(1,0) ) *
                        field_p.x -
                os_p.state(details::order_tag<1>{})(0,0) *
                    os_p.state(details::order_tag<1>{})(1,0) *
                        field_p.y -
                os_p.state(details::order_tag<1>{})(0,0) * field_p.z ;
    }
};

#endif











