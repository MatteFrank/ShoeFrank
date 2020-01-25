

#ifndef _TATOEact_HXX
#define _TATOEact_HXX


#include <utility>
#include <iostream>

#include "detector_list.hpp"
#include "arborescence.hpp"

#include "TATOEutilities.hxx"
//______________________________________________________________________________
//

#include "TAMCntuEve.hxx"


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
    using state = state_impl<vector, covariance>;
    
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
            
            std::cout << std::scientific << " -- Particle -- \n";
            std::cout << "charge = " << particle_m.charge << '\n';
            std::cout << "mass = " << particle_m.mass << '\n';
            std::cout << "momentum = " << particle_m.momentum << '\n';
            std::cout << "track_slope = " << particle_m.track_slope << '\n';
            std::cout << "track_slope_error = " << particle_m.track_slope_error << '\n';
            
            if(particle_m.charge == 0){ continue; }
            
            reconstruct();
        }
        
        
        //form list of hypothesis
        
        //for each
        // - set particle_m
        // - reconstruct: either consume the arborescence and release global tracks -> better

     
        
        
        //reconstruct
        // - create arborescence from first layer of vertex
        // - iterate over the list to retrieve each detector properties
        // - advance_reconstruction for each
        //-- control tracks
           // -- fill
        
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
        
        auto candidate_c = tof.generate_candidates();
        std::vector<particle_properties> hypothesis_c;
        hypothesis_c.reserve(candidate_c.size());
        
        
        auto * data_hc = static_cast<TAMCntuEve*>( gTAGroot->FindDataDsc( "eveMc" )->Object() );
        
        
        for( const auto& candidate : candidate_c ) {
            auto endPosition = transformation_h->FromTWLocalToGlobal( candidate.data->GetPosition() );
            
            auto charge = candidate.data->GetChargeZ();
            auto mass = (charge > 1 ? charge*2 : charge );
            
            auto beam_energy = 200.; //MeV/u
            auto momentum = sqrt( (beam_energy * mass) * (beam_energy * mass) + 2 *  (beam_energy * mass) * (938 * mass)  );
            
            auto id = candidate.data->GetColumnHit()->GetMcTrackIdx(0);
            
            std::cout << "computed_momentum : " << momentum << '\n';
            std::cout << "initial_momentum : " << data_hc->GetHit(id)->GetInitP().Mag() * 1e3 << '\n';
//            std::cout << "final_momentum : " << data_hc->GetHit(id)->GetFinalP().Mag2() << '\n';
//             id = candidate.data->GetRowHit()->GetMcTrackIdx(0);
//            std::cout << "initial_momentum : " << data_hc->GetHit(id)->GetInitP().Mag2() << '\n';
//            std::cout << "final_momentum : " << data_hc->GetHit(id)->GetFinalP().Mag2() << '\n';
            
            momentum = data_hc->GetHit(id)->GetInitP().Mag() * 1000;
            
            
            //auto track_slope_x = ( endPosition.X() - vertexPosition.X() )/( endPosition.Z() - vertexPosition.Z() );
            auto lengthY = abs(endPosition.Y() - vertexPosition.Y());
            auto lengthZ = abs(endPosition.Z() - vertexPosition.Z());
            
            auto track_slope = lengthY/lengthZ;
            //deviation in x, use y twice.
            auto track_slope_error = track_slope * sqrt( (0.1/lengthY * 0.1/lengthY) + (1./lengthZ * 1./lengthZ) );
            
            hypothesis_c.push_back(
                particle_properties{ charge, mass, momentum, track_slope, track_slope_error, candidate.data }
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
                                                            covariance_matrix{{ pow(particle_m.track_slope_error, 2),                                   0,
                                                                                                                   0, pow(particle_m.track_slope_error, 2)  }} )
                                    },
                                    chisquared{0}
                                }
                                              );
                        }
                       );
        
        
        
    
        
        auto arborescence = make_arborescence( std::move(root_c) );

        
        list_m.apply_except_last( [this, &arborescence]( const auto& layer_pc )
                               {
                                   std::cout << " ---- DETECTOR --- \n";
                                   for( const auto& layer : layer_pc ){
                                       advance_reconstruction( arborescence, layer );
                                   }
                               } );
        
        auto tof = list_m.last();
        advance_reconstruction( arborescence, tof.form_layer() );
        
        
        auto& handler = arborescence.GetHandler();
        for(auto& node : handler){
            auto value_c = node.GetBranchValues();
            std::cout << " --- final_track --- " << '\n';
            for(auto& value : value_c){
                std::cout << "( " << value.vector(0,0) <<  ", " << value.vector(1,0) <<  " ) -- ( " <<value.vector(2,0) << ", " << value.vector(3,0) <<  " ) -- " << value.evaluation_point << " -- " <<  value.chisquared << '\n';
            }
            
            
        }
        
        
        
    }
    
    
    
    template< class T >
    std::vector<corrected_state> advance_reconstruction_impl( state s_p,
                                                              const T& layer_p )
    {
        auto sigma_c = ukf_m.generate_sigma_points( s_p );
        
        sigma_c = ukf_m.propagate_while(
                                        std::move(sigma_c),
                                        [this, &layer_p](const auto& os_p)
                                        { return os_p.evaluation_point + ukf_m.step_length() < layer_p.depth ; }
                                        );
        auto step = layer_p.depth - sigma_c.front().evaluation_point;
        sigma_c = ukf_m.force_propagation( std::move(sigma_c), step );
        auto ps = ukf_m.generate_propagated_state( std::move(sigma_c) );
        
        
        return confront(ps, layer_p);
    }
    
    
    template< class T, typename std::enable_if_t< !std::is_same< T,  detector_properties< details::tof_tag >::layer >::value,
                                                  std::nullptr_t  > = nullptr  >
    void advance_reconstruction( TAGTOEArborescence<corrected_state>& arborescence_p,
                                 const T& layer_p )
    {
        
        std::cout << " ---- ADVANCE_RECONSTRUCTION --- \n";
        
        
        auto& leaf_c = arborescence_p.GetHandler();
        
        
        for(auto& leaf : leaf_c){
            
            ukf_m.step_length() = 1e-3;
            
            auto s = make_state(leaf.GetValue());
            auto cs_c = advance_reconstruction_impl( s, layer_p );
            
            for( auto & cs : cs_c ){
                leaf.AddChild( std::move(cs) );
            }

        }
        
    }
    
    

    void advance_reconstruction( TAGTOEArborescence<corrected_state>& arborescence_p,
                                 const detector_properties<details::tof_tag>::layer& layer_p )
    {
        
        std::cout << " ---- FINALISE_RECONSTRUCTION --- \n";
        
        
        auto& leaf_c = arborescence_p.GetHandler();
        
        
        for(auto& leaf : leaf_c){
            
            ukf_m.step_length() = 1e-3;
            
            auto s = make_state(leaf.GetValue());
            auto cs_c = advance_reconstruction_impl( s, layer_p );
            
            if( cs_c.empty() ){ leaf.MarkAsInvalid(); }
            else{ leaf.AddChild( std::move(cs_c.front()) ); }
        }
        
    }
    
    
    
    
    template<class T, typename std::enable_if_t< !std::is_same< T,  detector_properties< details::tof_tag >::layer >::value,
                                                 std::nullptr_t  > = nullptr >
    std::vector<corrected_state> confront(const state& ps_p, const T& layer_p)
    {
        
        auto candidate_c = layer_p.get_candidates();
        
        std::vector<corrected_state> cs_c;
        cs_c.reserve( candidate_c.size() );
        
        using enriched_candidate = enriched_candidate_impl<typename decltype(candidate_c)::value_type>;
        std::vector< enriched_candidate > enriched_c;
        enriched_c.reserve( candidate_c.size() );
    
        
        std::for_each( candidate_c.begin(), candidate_c.end(),
                      [this, &ps_p, &enriched_c]( const auto& candidate_p )
                      {
                          enriched_c.push_back( make_enriched_candidate( candidate_p ,
                                                                        ukf_m.compute_chisquared(ps_p, candidate_p) ) );
                      }
                     );
    
        auto end = std::partition(
                                  enriched_c.begin(), enriched_c.end(),
                                  [this, &ps_p, &layer_p ]( const auto & ec_p )
                                  {
                                      auto error = ec_p.data->GetPosError();
                                      
                                      auto mps = split_half( ps_p.vector , details::row_tag{});
                                      mps.first.get(0,0) += layer_p.cut * error.X();
                                      mps.first.get(1,0) += layer_p.cut * error.Y();
                                      // TD<decltype(mps)>x;
                                      
                                      using ec = typename underlying<decltype(ec_p)>::type;
                                      using candidate = typename underlying<ec>::candidate;
                                      using vector = typename underlying<candidate>::vector;
                                      using covariance = typename underlying<candidate>::covariance;
                                      using measurement_matrix = typename underlying<candidate>::measurement_matrix;
                                      using data = typename underlying<candidate>::data_type;
                                      
                                      auto nc = candidate{
                                          vector{ std::move(mps.first) },
                                          covariance{ ec_p.covariance },
                                          measurement_matrix{ ec_p.measurement_matrix },
                                          data_handle<data>{ ec_p.data }
                                      } ;
                                      
                                      auto cut = ukf_m.compute_chisquared(ps_p, nc);
                                      
                                      return ec_p.chisquared < cut.chisquared;
                                  }
                                  );
        
        
        for(auto iterator = enriched_c.begin() ; iterator != end ; ++iterator ){
            auto state = ukf_m.correct_state( ps_p, *iterator ); //should be sliced properly
            auto cs = make_corrected_state( std::move(state),
                                           chisquared{std::move(iterator->chisquared)} );
            
            cs_c.push_back( std::move(cs) );
        }
    
    
        return cs_c;
    }
    
    
    
    
    std::vector<corrected_state> confront(const state& ps_p, const detector_properties<details::tof_tag>::layer& layer_p) //optionnal is more relevant here
    {
        using candidate = typename detector_properties< details::tof_tag >::candidate;
        using enriched_candidate = enriched_candidate_impl< candidate >;
        
        
        auto candidate_c = layer_p.get_candidates();
        auto proper_candidate = std::find_if( candidate_c.begin(), candidate_c.end(),
                                              [this](const candidate& c_p){ return c_p.data == particle_m.data;  } );
        enriched_candidate ec = make_enriched_candidate( *proper_candidate ,
                                                         ukf_m.compute_chisquared(ps_p, *proper_candidate) );
        
        std::vector<corrected_state> cs_c;
    
        auto selection = [this, &ps_p, &layer_p ]( const auto & ec_p )
                         {
                            auto error = ec_p.data->GetPosError();
            
                            auto mps = split_half( ps_p.vector , details::row_tag{});
                            mps.first.get(0,0) += layer_p.cut * error.X();
                            mps.first.get(1,0) += layer_p.cut * error.Y();
            
                             using ec = typename underlying<decltype(ec_p)>::type;
                             using candidate = typename underlying<ec>::candidate;
                             using vector = typename underlying<candidate>::vector;
                             using covariance = typename underlying<candidate>::covariance;
                             using measurement_matrix = typename underlying<candidate>::measurement_matrix;
                             using data = typename underlying<candidate>::data_type;
            
                             auto nc = candidate{
                                 vector{ std::move(mps.first) },
                                 covariance{ ec_p.covariance },
                                 measurement_matrix{ ec_p.measurement_matrix },
                                 data_handle<data>{ ec_p.data }
                             } ;
            
                             auto cut = ukf_m.compute_chisquared(ps_p, nc);
            
                             return ec_p.chisquared < cut.chisquared;
                         };
        
        if( selection(ec) ){
            auto state = ukf_m.correct_state( ps_p, ec ); //should be sliced properly
            auto cs = make_corrected_state( std::move(state),
                                            chisquared{std::move(ec.chisquared)} );
            cs_c.push_back( std::move(cs) );
        }
        
        return cs_c;
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











