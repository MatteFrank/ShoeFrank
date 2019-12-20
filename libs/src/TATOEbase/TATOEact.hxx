

#ifndef _TATOEact_HXX
#define _TATOEact_HXX


#include <utility>
#include <iostream>

#include "detector_list.hpp"


#include "TATOEutilities.hxx"
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
    particle_properties particle_m{2, 5.6};
    UKF ukf_m;
    detector_list_t list_m;
    
public:
    TATOEactGlb(UKF&& ukf_p, detector_list_t&& list_p ) : ukf_m{std::move(ukf_p)}, list_m{std::move(list_p)} {
        ukf_m.call_stepper().ode.model().particle_h = &particle_m;
    }
    
    void Action() const override { std::cout << "test\n"; std::cout << ukf_m.call_stepper().ode( 4  ) << '\n'; };
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











