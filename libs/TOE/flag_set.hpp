//
//File      : flag_set.hpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 02/10/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//

#ifndef flag_set_h
#define flag_set_h


template< class ... Ts >
struct flag_set{
public:
    constexpr operator uint8_t() const { return compute_value(); }
    
    
private:
    flag_set() = default;
    
    constexpr uint8_t compute_value() const {
        uint8_t result = 0;
        int expander[] = { 0, ( result |= 1UL << Ts::shift , void(), 0) ... };
        return result;
        // return 1;
    }
};



#endif /* flag_set_h */
