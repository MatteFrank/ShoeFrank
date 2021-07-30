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
#include "aftereffect.hpp"
#include "matrix_new.hpp"
#include "TADIgeoField.hxx"



namespace details{
    struct immutable_tag{};
    struct fleeting_tag{};
    struct normal_tag{};
    struct non_removable_tag{};
    
    struct all_mixed_tag{};
    struct all_separated_tag{};
    
    
    template<class C>
    class cut_holder;

struct vertex_tag;
struct it_tag;
struct msd_tag;
struct tof_tag;
struct ms2d_tag;

} //namespace details


struct particle_properties{
    double momentum;
    int charge;
    int mass;
    double track_slope_x{0};
    double track_slope_y{0};
};

struct polynomial_fit_parameters{
    std::array<double, 4> x;
    std::array<double, 2> y;
};

struct reconstructible_track{
    std::vector<int> const & get_indices() const { return index_c; }
    std::vector<int>& get_indices() { return index_c; }
    std::vector<int> index_c;
    particle_properties properties;
};

template<class T>
struct reconstructed_track{
    std::vector<T const*> cluster_c;
    std::vector<T const*> const& get_clusters() const { return cluster_c; }
    std::vector<T const*> & get_clusters() { return cluster_c; }
    particle_properties properties;
    polynomial_fit_parameters parameters;
    std::size_t clone_number{0};
};

template<class T>
struct reconstruction_module{
    T const* end_point_h;
    aftereffect::optional<reconstructible_track> reconstructible_o;
    aftereffect::optional<reconstructed_track<T>> reconstructed_o;
    
    reconstruction_module( T const* end_point_ph,
                          aftereffect::optional<reconstructible_track> reconstructible_po,
                          aftereffect::optional<reconstructed_track<T>> reconstructed_po) :
    end_point_h{end_point_ph},
    reconstructible_o{ std::move(reconstructible_po) },
    reconstructed_o{ std::move(reconstructed_po) } {}
};



struct TAGcluster;

struct TATOEbaseAct {
    template<class C, class ... Ps>
    friend class TATOEoptimizer;
    
    template<class C>
    friend class details::cut_holder;
    
    using data_type = TAGcluster;
public:
    virtual void Action()  = 0;
    virtual void Output() =0;
    virtual ~TATOEbaseAct() = default;
    
private:
    virtual void reset_event_number() = 0;
    
    virtual void set_cuts( details::vertex_tag, double) = 0;
    virtual void set_cuts( details::it_tag, std::array<double, 2> const& ) = 0;
    virtual void set_cuts( details::msd_tag, std::array<double, 3> const& ) = 0;
    virtual void set_cuts( details::tof_tag, double) = 0;
    virtual void set_cuts( details::ms2d_tag, double) = 0;
    
    virtual std::vector<reconstruction_module<data_type>> const& retrieve_matched_results( ) const = 0;
};

//---------------------------------------------------------------


struct TATWpoint;
struct particle_hypothesis
{
    particle_properties properties;
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
    
    particle_hypothesis* particle_h = nullptr;
    static constexpr double conversion_factor = 0.000299792458; //[MeV/c . G^{-1} . cm^{-1} ]
    TADIgeoField const * field_mh; //G
    
    constexpr model(TADIgeoField const * field_ph) : field_mh{field_ph} {}
    
    
    auto operator()(const operating_state_t& os_p) const
    {
       return conversion_factor * particle_h->properties.charge/particle_h->properties.momentum * compute_R(os_p) * compute_change(os_p);
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











