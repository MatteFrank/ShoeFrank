

#ifndef _TATOEact_HXX
#define _TATOEact_HXX


#include <utility>
#include <iostream>

#include "detector_list.hpp"
#include "arborescence.hpp"

#include "TATOEutilities.hxx"
//______________________________________________________________________________
//



template<class T>
class TD;

struct TATOEbaseAct {
    virtual void Action()  = 0;
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
    
    using vector = typename underlying< typename details::filter_traits<UKF>::state >::vector::type ;
    using covariance = typename underlying< typename details::filter_traits<UKF>::state >::covariance::type ;
    using corrected_state = corrected_state_impl<vector, covariance>;
    
private:
    particle_properties particle_m{};
    UKF ukf_m;
    detector_list_t list_m;
    
public:
    
    TATOEactGlb(UKF&& ukf_p, detector_list_t&& list_p ) : ukf_m{std::move(ukf_p)}, list_m{std::move(list_p)} {
        ukf_m.call_stepper().ode.model().particle_h = &particle_m;
    }
    
    void Action() override {

        std::cout << " ---- ACTION --- \n";
        

        
        //form list of hypothesis
        //until here
        auto hypothesis_c = form_hypothesis();
        
        for(auto & hypothesis : hypothesis_c){
            particle_m = hypothesis;
            
            std::cout << " -- Particle -- \n";
            std::cout << "charge = " << particle_m.charge << '\n';
            std::cout << "mass = " << particle_m.mass << '\n';
            std::cout << "momentum = " << particle_m.momentum << '\n';
            std::cout << "track_slope = " << particle_m.track_slope << '\n';
            std::cout << "track_slope_error = " << particle_m.track_slope_error << '\n';
            
            reconstruct();
        }
        
        
        //form list of hypothesis
        
        //for each
        // - set particle_m
        // - reconstruct: either consume the arborescence and release global tracks -> better
        //
        //control tracks
        //fill
        
        
        //reconstruct
        // - create arborescence from first layer of vertex
        // - iterate over the list to retrieve each detector properties
        // - advance_reconstruction for each
        
        // advance_reconstruction
        // - retrieve handler for arborescence -> arborscence is a parameter
        // - propagate from the nodes
        // - retrieve the list of candidates, sort them according to the cut value
        // - correct the good ones and add to the arborescence
        
        
        
        
    };
    
private:
    
    std::vector<particle_properties> form_hypothesis()
    {
        
        std::cout << " ---- FORM_HYPOTHESIS --- \n";
        
        //probably own func
        auto vertex = list_m.first();
        auto tof = list_m.last();
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        auto vertexPosition = transformation_h->FromVTLocalToGlobal(vertex.vertex_handle()->GetVertexPosition());
        
        auto candidate_c = tof.generate_candidates(0);
        std::vector<particle_properties> hypothesis_c;
        hypothesis_c.reserve(candidate_c.size());
        
        for( const auto& candidate : candidate_c ) {
            auto endPosition = transformation_h->FromTWLocalToGlobal( candidate.data->GetPosition() );
            
            auto charge = candidate.data->GetChargeZ();
            auto mass = (charge == 1 ? charge : charge * 2);
            
            auto beam_energy = 200.; //MeV/u
            auto momentum = sqrt( (beam_energy * mass) * (beam_energy * mass) + 2 *  (beam_energy * mass) * (938 * mass)  );
            
            //auto track_slope_x = ( endPosition.X() - vertexPosition.X() )/( endPosition.Z() - vertexPosition.Z() );
            auto lengthY = endPosition.Y() - vertexPosition.Y();
            auto lengthZ = endPosition.Z() - vertexPosition.Z();
            
            auto track_slope = lengthY/lengthZ;
            //deviation in x, use y twice.
            auto track_slope_error = track_slope * sqrt( (0.1/lengthY * 0.1/lengthY) + (1./lengthZ * 1./lengthZ) );
            
            hypothesis_c.push_back(
                particle_properties{ charge, mass, momentum, track_slope, track_slope_error }
            );
        }
        
        return hypothesis_c;
    }
    
    
    void reconstruct()
    {
        std::cout << " ---- RECONSTRUCT --- \n";
        
        auto vertex = list_m.first();
        auto candidate_c = vertex.generate_candidates(0);
        
        
        std::vector< corrected_state > root_c;
        root_c.reserve( candidate_c.size() );
        std::for_each( candidate_c.begin(), candidate_c.end(),
                        [this, &vertex, &root_c]( auto candidate_p )
                        {
                            using state = typename underlying<corrected_state>::state;
                            
                            using candidate = decltype(candidate_p);
                            using vector = typename underlying<candidate>::vector;
                            using vector_matrix = typename vector::type;
                            using covariance = typename underlying<candidate>::covariance;
                            using covariance_matrix = typename covariance::type;
                            
                            
                            root_c.push_back(
                                corrected_state{
                                    state{
                                        evaluation_point{ vertex.layer_depth(0) },
                                        make_state_vector( vector{std::move(candidate_p.vector)},
                                                       vector_matrix{{  particle_m.track_slope, particle_m.track_slope }} ),
                                        make_state_covariance( covariance{ std::move(candidate_p.covariance) },
                                                           covariance_matrix{{ particle_m.track_slope_error,                            0,
                                                                                                          0, particle_m.track_slope_error  }} )
                                    },
                                    chisquared{0}
                                }
                                              );
                        }
                       );
        
        
        
    
        for( auto& root : root_c){
            std::cout << "root::vector \n" << root.vector;
            std::cout << "root::covariance \n" << root.covariance;
        }
        
        auto arborescence = make_arborescence( std::move(root_c) );

        
        list_m.apply_for_each( [this, &arborescence]( const auto& layer_pc )
                               {
                                   std::cout << " ---- DETECTOR --- \n";
                                   for( const auto& layer : layer_pc ){
                                       
                                       std::cout << "--- layer ---\n";
                                       
                                       advance_reconstruction( arborescence, layer );
                                   }
                               } );
    }
    
    
    
    template<class T>
    void advance_reconstruction( TAGTOEArborescence<corrected_state>& arborescence_p,
                                 const T& layer_p )
    {
        
        std::cout << " ---- ADVANCE_RECONSTRUCTION --- \n";
        
        
        auto& leaf_c = arborescence_p.GetHandler();
        
        
        std::cout << " leaf_c::size : " << leaf_c.size() << '\n';
        
        for(auto& leaf : leaf_c){
            
            ukf_m.step_length() = 1e-3;
            
            auto s = make_state(leaf.GetValue());
            std::cout << "starting_state : " <<  s.vector(0,0) << ", " << s.vector(1,0) << ", " << s.evaluation_point << '\n';
            
            auto sigma_c = ukf_m.generate_sigma_points( make_state(leaf.GetValue()) );
            
            for(auto & sigma : sigma_c){
                std::cout << "sigma : (";
                std::cout << sigma.state( details::order_tag<0>{} )(0,0) << ", " << sigma.state( details::order_tag<0>{} )(1,0) << ") -- (";
                std::cout << sigma.state( details::order_tag<1>{} )(0,0) << ", " << sigma.state( details::order_tag<1>{} )(1,0) << ") -- ";
                std::cout << sigma.evaluation_point << '\n';
            }
//
            sigma_c = ukf_m.propagate_while(
                                             std::move(sigma_c),
                                             [this, &layer_p](const auto& os_p)
                                             { return os_p.evaluation_point + ukf_m.step_length() < layer_p.depth ; }
                                            );
            
            std::cout << "after_while_propagation:\n";
            
            for(auto & sigma : sigma_c){
                std::cout << "sigma : (";
                std::cout << sigma.state( details::order_tag<0>{} )(0,0) << ", " << sigma.state( details::order_tag<0>{} )(1,0) << ") -- (";
                std::cout << sigma.state( details::order_tag<1>{} )(0,0) << ", " << sigma.state( details::order_tag<1>{} )(1,0) << ") -- ";
                std::cout << sigma.evaluation_point << '\n';
            }
            
            auto step = layer_p.depth - sigma_c.front().evaluation_point;
            std::cout << "layer::depth : " << layer_p.depth << '\n';
            std::cout << "force_propagation::step_length : " << step << '\n';
            std::cout << "ukf::step_length : " << ukf_m.step_length() << '\n';

            
            sigma_c = ukf_m.force_propagation( std::move(sigma_c), step );

            for(auto & sigma : sigma_c){
                std::cout << "sigma : (";
                std::cout << sigma.state( details::order_tag<0>{} )(0,0) << ", " << sigma.state( details::order_tag<0>{} )(1,0) << ") -- (";
                std::cout << sigma.state( details::order_tag<1>{} )(0,0) << ", " << sigma.state( details::order_tag<1>{} )(1,0) << ") -- ";
                std::cout << sigma.evaluation_point << '\n';
            }

            
            auto ps = ukf_m.generate_propagated_state( std::move(sigma_c) );
            
            std::cout << "propagated_state::vector : (" <<  ps.vector(0,0) << ", " << ps.vector(1,0) << ") -- (" <<  ps.vector(2,0) << ", " << ps.vector(3,0) << ") --  " << ps.evaluation_point << "\n";
//            std::cout << "propagated_state::covariance : \n" <<  ps.covariance;
            
            auto candidate_c = layer_p.get_candidates();
            using enriched_candidate = enriched_candidate_impl<typename decltype(candidate_c)::value_type>;
            std::vector< enriched_candidate > enriched_c;
            enriched_c.reserve( candidate_c.size() );
            
            std::cout << "for_each \n";
            
            std::for_each( candidate_c.begin(), candidate_c.end(),
                            [this, &ps, &enriched_c]( const auto& candidate_p )
                            {
                                enriched_c.push_back( make_enriched_candidate( candidate_p ,
                                                                               ukf_m.compute_chisquared(ps, candidate_p) ) );
                                
                                std::cout << "candidate::vector : (" <<  candidate_p.vector(0,0) << ", " << candidate_p.vector(1,0) << ")\n";
                               // std::cout << "candidate::covariance : \n " <<  candidate_p.covariance ;
                                std::cout << "chisquared : " <<  enriched_c.back().chisquared  << '\n';
                            }
                          );
//
            
            std::cout << "sort \n";
            
            std::sort( enriched_c.begin(), enriched_c.end(),
                        [](const enriched_candidate& ec1_p, const enriched_candidate& ec2_p )
                        {
                            return ec1_p.chisquared < ec2_p.chisquared;
                        }
                      );

            auto end = enriched_c.begin() == enriched_c.end() ? enriched_c.end() : enriched_c.begin() + 1 ;
            
//            auto end = std::partition(
//                                enriched_c.begin(), enriched_c.end(),
//                                [ &enriched_c ]( const auto& ec_p )
//                                {
//                                    return ec_p.chisquared/enriched_c.back().chisquared < 0.1;
//                                }
//                                    );
////
//            auto end = std::partition(
//                                enriched_c.begin(), enriched_c.end(),
//                                [ &layer_p ]( const auto& ec_p )
//                                {
//                                    return ec_p.chisquared < layer_p.cut;
//                                }
//                                      );
            
            
            std::cout << "correction \n";
            
            
            for(auto iterator = enriched_c.begin() ; iterator != end ; ++iterator ){
                auto state = ukf_m.correct_state( ps, *iterator ); //should be sliced properly
                auto cs = make_corrected_state( std::move(state),
                                                chisquared{std::move(iterator->chisquared)} );
                std::cout << "good_candidates : (" <<  iterator->vector(0,0) << ", " << iterator->vector(1,0) << ", " << layer_p.depth << ")\n";
                std::cout << "corrected_state : (" <<  cs.vector(0,0) << ", " << cs.vector(1,0)  << ") -- (" <<  cs.vector(2,0) << ", " << cs.vector(3,0) << ") --  " << cs.evaluation_point << "\n";
                
                leaf.AddChild(std::move(cs));
            }
            
            
       //     if( end == enriched_c.begin() ){ leaf.MarkAsInvalid(); }
            
            std::cout << "-- end_leaf --\n";
            
        }
        
    }
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











