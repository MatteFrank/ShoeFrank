//
//File      : grkn_data.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 26/11/2019
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef grkn_data_hpp
#define grkn_data_hpp

#include <array>
#include "utility_types.hpp"


struct data_grkn56{
public:
    static constexpr std::size_t order = 2;
    static constexpr std::size_t evaluation_stage = 9;
    using stepping_policy = details::adaptable_step_tag;
    static constexpr std::size_t higher_degree = 6;
    static constexpr std::size_t lower_degree = 5;
    
public:
    const std::array<double, evaluation_stage>  global_delay{
        0, 4./15,  2./5,  3./5,  9./10,  3./4,  2./7,    1.,    1.
    };
    
    
    using partial_delay_t = details::lower_triangular_array<double, 36>;
    
private:
    const partial_delay_t partialDelayDerivedState_mc{
        4./15,
        1./10,  3./10,
        3./20,      0,  9./20,
        9./40,      0,      0,         27./40,
        11./48,      0,      0,           5./8,        -5./48,
        27112./194481,      0,      0,   56450./64827, 80000./194481, -24544./21609,
        -26033./41796,      0,      0, -236575./38313, -14500./10449, 275936./45279, 228095./73788,
        7./81,      0,      0,              0,    -250./3483,      160./351,    2401./5590, 1./10
    };
    const partial_delay_t partialDelayState_mc{
        8./225,
        1./25,   1./25,
        9./160, 81./800,      9./400,
        81./640,       0,   729./3200,    81./1600,
        11283./88064,       0, 3159./88064, 7275./44032,         -33./688,
        6250./194481,       0,           0,           0,    -3400./194481,     1696./64827,
        -6706./45279,       0,           0,           0, 1047925./1946997, -147544./196209, 1615873./1874886,
        31./360,       0,           0,           0,                0,         64./585,       2401./7800, -1./300
    };
    
    
public:
    constexpr const partial_delay_t& partial_delay(details::order_tag<0>) const
    {return partialDelayState_mc;}
    constexpr const partial_delay_t& partial_delay(details::order_tag<1>) const
    {return partialDelayDerivedState_mc;}
    
    
    
    using weight_t = std::array<double, evaluation_stage>;
    
private:
    const weight_t weightDerivedState_mc{
        7./81, 0, 0, 0, -250./3483, 160./351, 2401./5590, 1./10, 0
    };
    const weight_t weightStateEstimation_mc{
        31./360, 0, 0, 0, 0, 64./588, 2401./7800, -1./300, 0
    };
    const weight_t weightStateCorrection_mc{
        31./360, 0, 0, 0, 0, 64./588, 2401./7800, 0, -1./300
    };
public:
    constexpr const weight_t& weight(details::order_tag<0>, details::estimation_tag) const
    { return weightStateEstimation_mc; }
    constexpr const weight_t& weight(details::order_tag<0>, details::correction_tag) const
    { return weightStateCorrection_mc; }
    constexpr const weight_t& weight(details::order_tag<1>) const
    { return weightDerivedState_mc; }
    
};



struct data_grkn4{
public:
    static constexpr std::size_t evaluation_stage = 4;
    static constexpr std::size_t order = 2;
    using stepping_policy = details::fixed_step_tag;
    
public:
    const std::array<double, evaluation_stage>  global_delay{
        0., 1./2, 1./2, 1.
    };

    
    using partial_delay_t =  details::lower_triangular_array<double, 6>;
    
private:
    const partial_delay_t partialDelayDerivedState_mc{
        1./2,
        0, 1./2,
        0,    0, 1.
    };
    const partial_delay_t partialDelayState_mc{
        0,
        1./2, 0,
        0, 1./2, 0
    };

    
public:
    constexpr const partial_delay_t& partial_delay(details::order_tag<0>) const
    { return partialDelayState_mc; }
    constexpr const partial_delay_t& partial_delay(details::order_tag<1>) const
    { return partialDelayDerivedState_mc; }
    
    
    
    
    
     using weight_t = std::array<double, evaluation_stage>;
    
    
private:
    const weight_t weightDerivedState_mc{
        1./6, 1./3, 1./3, 1./6
    };
    
    const weight_t weightState_mc{
        1./6, 1./6, 1./3, 0
    };
    
    
    
public:

    constexpr const weight_t& weight(details::order_tag<0>) const
    { return weightState_mc; }
    constexpr const weight_t& weight(details::order_tag<1>) const
    { return weightDerivedState_mc; }
    
};

#endif /* grkn_data_hpp */
