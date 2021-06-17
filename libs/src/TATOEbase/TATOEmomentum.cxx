///////////////////////////////////////////////////////////
//                                                        //
// Class Description of TATOEmomentum                       //
//                                                        //
////////////////////////////////////////////////////////////

//
//File      : TATOEmomentum.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 12/05/2021
//Framework : PhD thesis, CNRS/IPHC/DRS/DeSis, Strasbourg, France
//


#include "TATOEmomentum.hxx"


#include "TATOEutilities.hxx"
#include "stepper.hpp"
#include "grkn_data.hpp"

#include "TADIgeoField.hxx"


TVector3 GetMomentumAtZ( TAGtrack* track_ph, double z ) {
    auto compute_y_l = [&track_ph]( double z ){
        auto parameters = track_ph->GetParameters();
        return parameters.parameter_y[1] * z + parameters.parameter_y[0];  };
    auto compute_x_l = [&track_ph]( double z ){
        auto parameters = track_ph->GetParameters();
        return parameters.parameter_x[3] * z * z * z +
               parameters.parameter_x[2] * z * z +
               parameters.parameter_x[1] * z +
               parameters.parameter_x[0];
    };
    
    //------------ "ode" ----------------
    auto step = 1e-3;
    
    auto * field_h = static_cast<TADIgeoField*>(gTAGroot->FindParaDsc(TADIgeoField::GetDefParaName())->Object());
    auto ode = make_ode< matrix<3,1>, 1>(
                    [&step, &track_ph, &compute_x_l, &compute_y_l, &field_h](operating_state<matrix<3,1>, 1> const& os_p){
                            double const z = os_p.evaluation_point;
                            double const dx_dz = os_p.state( details::order_tag<0>{} )(0,0)/os_p.state( details::order_tag<0>{} )(2,0);
                            double const dy_dz = os_p.state( details::order_tag<0>{} )(1,0)/os_p.state( details::order_tag<0>{} )(2,0);
                                             
                            double const R = sqrt( dx_dz*dx_dz + dy_dz*dy_dz + 1 );
                                             
                            TVector3 reco_position{
                                        compute_x_l(z),
                                        compute_y_l(z),
                                        z
                                                    };

                            auto const field = field_h->GetField(reco_position);
                                             
                            auto const p_x = os_p.state( details::order_tag<0>{} )( 0, 0 );
                            auto const p_y = os_p.state( details::order_tag<0>{} )( 1, 0 );
                            auto const p_z = os_p.state( details::order_tag<0>{} )( 2, 0 );
                                             
                            auto p = sqrt( p_x * p_x + p_y * p_y + p_z * p_z);
                                             
                            auto const dpx_dz = 0.3e-3 * R * track_ph->GetCharge()/p * (p_y*field.Z() - p_z * field.Y());
                            auto const dpy_dz = 0.3e-3 * R * track_ph->GetCharge()/p * (p_z*field.X() - p_x * field.Z());
                            auto const dpz_dz = 0.3e-3 * R * track_ph->GetCharge()/p * (p_x*field.Y() - p_y * field.X());
                                             
                            return matrix<3, 1>{ dpx_dz, dpy_dz, dpz_dz };
                                                                                                                }
                                         );

    auto stepper = make_stepper<data_rkf45>( std::move(ode) );
//    stepper.specify_tolerance(1e-8);

    auto * starting_point_h = track_ph->GetCorrPoint( 0 );
    
    auto os = operating_state<matrix<3,1>, 1>{
            starting_point_h->GetPositionG().Z(),
            {
                starting_point_h->GetMomentum().X() * 1000,
                starting_point_h->GetMomentum().Y() * 1000,
                starting_point_h->GetMomentum().Z() * 1000
            }
                                             };
    
    while( os.evaluation_point + step < z ){
        auto step_result = stepper.step( std::move(os), step );
        if( step_result.second != 0 ){
            auto new_step_length = stepper.optimize_step_length(step, step_result.second);
            step = ( new_step_length > 1e-1 ) ?
                            1e-1 :
                            (new_step_length < 1e-3) ? 1e-3 : new_step_length;
        }
        os = std::move(step_result.first);
    }
    step = z - os.evaluation_point;
    
    os = stepper.force_step( std::move(os), step );
    
    return TVector3{
        os.state(details::order_tag<0>{})(0,0) / 1000,
        os.state(details::order_tag<0>{})(1,0) / 1000,
        os.state(details::order_tag<0>{})(2,0) / 1000
    };
}
