//
// Plot tot residual + Fit for FIRST data
//
/** TATOEchecker class
 
 \author A. Sécher
 */

//
//File      : TATOEchecker.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 10/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//


#ifndef _TATOEchecker_HXX
#define _TATOEchecker_HXX


#include "TAMCntuEve.hxx"
#include "TAGparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGcluster.hxx"
#include "TATOEutilities.hxx"

class TATWpoint;

namespace details{
    struct should_pass_tag{};
    struct should_not_pass_tag{};
} // namespace details

template<class Action>
struct TATOEchecker{
    
    using candidate = typename std::decay_t<decltype( std::declval<typename Action::detector_list_t>().last() )>::candidate;
    using detector_list_t = typename Action::detector_list_t;
    using node_type = typename Action::node_type;
    
    struct particle{
        int charge;
        int mass;
        double momentum;
        int index{0};
        double track_slope_x{0};
        double track_slope_y{0};
    };
    
    struct track{
        std::vector<TAGcluster const *> data_ch;
        particle real_particle;
    };
    
private:
    TAMCntuEve* data_mhc;
   
    std::pair<double, double> target_limits_m;

    node_type const * current_node_mh = nullptr;
    
    std::size_t reconstructed_number_m{0};
    std::size_t reconstructible_number_m{0};
    std::size_t local_reconstructed_number_m{0};
    std::size_t local_reconstructible_number_m{0};
    
    std::vector< track > reconstructible_track_mc;
    
    Action& action_m;
    
public:
    TATOEchecker( TAGparGeo const * global_parameters_ph,
                  Action& action_p ) :
        data_mhc{ static_cast<TAMCntuEve*>( gTAGroot->FindDataDsc( "eveMc" )->Object() ) },
        target_limits_m{ retrieve_target_limits( global_parameters_ph ) },
        action_m{action_p} {}
    
private:
    std::pair<double, double> retrieve_target_limits( TAGparGeo const * global_parameters_ph ) const
    {
        auto position_z = global_parameters_ph->GetTargetPar().Position.Z();
        auto size = global_parameters_ph->GetTargetPar().Size.Z();
        
        return { position_z - size /2 , position_z + size /2  };
    }

    
public:
    
    void update_current_node( node_type const * current_node_ph ){ current_node_mh = current_node_ph; }
    
    void register_reconstructible_track(candidate const& candidate_p)
    {
        action_m.logger_m.template add_sub_header< details::immutable_tag >("reconstructible_track");
        
        auto find_index = [this, &candidate_p]( TAMCeveTrack const * mc_track_ph )
                          {
                              int index;
                              for( int i{0}; i < candidate_p.data->GetMcTracksN() ; ++i  ){
                                  auto id = candidate_p.data->GetMcTrackIdx(i);
                                  auto * mc_track_h = data_mhc->GetHit(id) ;
                                  if(  mc_track_h == mc_track_ph ){ index = id; }
                              }
                              return index;
                          };
        
        auto mc_track_ch = retrieve_tracks(candidate_p);
        auto iterator = std::find_if( mc_track_ch.begin(),
                                                     mc_track_ch.end(),
                                                     [this](TAMCeveTrack const * const track_h )
                                                     {
                                                         return check_origin( track_h  );
                                                     } );
        
        
        if( iterator != mc_track_ch.end() ){
            action_m.logger_m.add_sub_header("track_registered");
            
            auto reconstructible_track_h = *iterator;
            
            auto index = find_index( reconstructible_track_h );
            reconstructible_track_mc.push_back( form_track( index, action_m.list_m ) );
            
            action_m.logger_m << "mc_track_index: " << index << '\n';
            auto& real_particle = reconstructible_track_mc.back().real_particle;
            action_m.logger_m << "charge: " << real_particle.charge << '\n';
            action_m.logger_m << "mass: " << real_particle.mass << '\n';
            action_m.logger_m << "momentum: " << real_particle.momentum << '\n';
            action_m.logger_m << "track_slope_x: " << real_particle.track_slope_x << '\n';
            action_m.logger_m << "track_slope_y: " << real_particle.track_slope_y << '\n';
            
            ++reconstructible_number_m;
            ++local_reconstructible_number_m;
        }
    }
    
private:
    std::vector< TAMCeveTrack const * > retrieve_tracks(candidate const & candidate_p ) const {
        std::vector< TAMCeveTrack const * > track_ch;
        track_ch.reserve( candidate_p.data->GetMcTracksN() );
        
        for( int i{0}; i < candidate_p.data->GetMcTracksN() ; ++i  ){
            auto id = candidate_p.data->GetMcTrackIdx(i);
            track_ch.push_back( data_mhc->GetHit(id) );
        }
        
        return track_ch;
    }
    
    
    bool check_origin( TAMCeveTrack const * const track_ph ) const
    {
        return ( track_ph->GetInitPos().Z() >= target_limits_m.first ) &&
        ( track_ph->GetInitPos().Z() <= target_limits_m.second );
    }
    
    
    track form_track( int index_p,
                      detector_list_t const & list_p ) const
    {
        auto index_found = [this, &index_p](auto const & candidate_p)
                            {
                                bool is_index_found = false;
                                for( auto j{0} ; j < candidate_p.data->GetMcTracksN() ; ++j ){
                                    is_index_found = is_index_found ||
                                    ( candidate_p.data->GetMcTrackIdx(j) == index_p );
                                }
                                return is_index_found;
                            };
        
        
        std::vector<TAGcluster const *> data_ch;
        data_ch.reserve( 15 );
        
        list_p.apply_for_each(
                    [this, &data_ch, &index_found](auto const & detector_p)
                    {
                        for( std::size_t i{0} ; i < detector_p.layer_count() ; ++i  ){
                            auto candidate_c = detector_p.generate_candidates( i );
                                       
                            if( std::any_of( candidate_c.begin(), candidate_c.end() , index_found) ){
                                auto proper_candidate = *std::find_if( candidate_c.begin(), candidate_c.end(), index_found );
                                        
                                data_ch.push_back( proper_candidate.data );
                            }
                        }
                    }
                              );
        
        auto mc_track_h = data_mhc->GetHit(index_p);
        auto real_particle = particle{ mc_track_h->GetCharge(),
                                       static_cast<int>( mc_track_h->GetMass() * 1.1 ),
                                       mc_track_h->GetInitP().Mag() * 1000,
                                       index_p,
                                       mc_track_h->GetInitP().X()/mc_track_h->GetInitP().Z(),
                                       mc_track_h->GetInitP().Y()/mc_track_h->GetInitP().Z()
                                       };
        return track{ std::move(data_ch), std::move(real_particle) };
    }

public:
    
    
    template<class T>
    void register_reconstructed_track( std::vector< T > const & full_state_pc )
    {
        ++reconstructed_number_m ;
        ++local_reconstructed_number_m;
    }
    
    template<class EnrichedCandidate>
    void check_validity( EnrichedCandidate const & ec_p,
                         double ec_chisquared_p,
                         double cutter_chisquared_p,
                         details::should_pass_tag )
    {
        if( abs( (cutter_chisquared_p - ec_chisquared_p)/cutter_chisquared_p ) > 10 ){ return; }
        
        action_m.logger_m.template add_sub_header<details::fleeting_tag>("check_validity");
        action_m.logger_m.add_sub_header("should_pass");
        
        action_m.logger_m.add_sub_header("current_hypothesis");
        action_m.logger_m << "charge: " << action_m.particle_m.charge << '\n';
        action_m.logger_m << "mass: " << action_m.particle_m.mass << '\n';
        action_m.logger_m << "momentum: " << action_m.particle_m.momentum << '\n';
        
        auto * root_h = current_node_mh->get_ancestor();
        action_m.logger_m << "track_slope_x: " << root_h->get_value().vector(2, 0) << '\n';
        action_m.logger_m << "track_slope_y: " << root_h->get_value().vector(3, 0) << '\n';
        
        
        action_m.logger_m.add_sub_header("cluster");
        action_m.logger_m << "mc_id: " ;
        std::vector<int> track_id_c;
        track_id_c.reserve( ec_p.data->GetMcTracksN() );
        for( int i{0} ; i < ec_p.data->GetMcTracksN() ; ++ i){
            track_id_c.push_back( ec_p.data->GetMcTrackIdx(i) );
            action_m.logger_m << ec_p.data->GetMcTrackIdx(i) << " ";
        }
        action_m.logger_m << '\n';
        
        bool went_through = ec_chisquared_p < cutter_chisquared_p;
        if( should_pass( track_id_c ) && !went_through ){
            action_m.logger_m.freeze();
        }
    }
    
    
private:
    bool should_pass( std::vector<int> const & track_id_pc )
    {
        auto predicate_id = [this, track_id_pc](track const & track_p )
                            {
                                return std::any_of( track_id_pc.begin(), track_id_pc.end(),
                                                     [&track_p](int id_p){ return track_p.real_particle.index == id_p;  } );
                            };
        auto predicate_particle = [this](track const & track_p )
                                  { return (track_p.real_particle.charge == action_m.particle_m.charge) &&
                                            (track_p.real_particle.mass == action_m.particle_m.mass);           };
        
        
        auto end_iterator = std::partition( reconstructible_track_mc.begin(), reconstructible_track_mc.end(), predicate_id);

        return std::any_of( reconstructible_track_mc.begin(), end_iterator, predicate_particle );
    }
    
//    template<class EnrichedCandidate>
//    void check_validity( EnrichedCandidate const & ec_p,
//                         particle_properties const & current_hypothesis_p,
//                         bool went_through_p,
//                         details::should_not_pass_tag )
//    {
//        logger_m.add_sub_header<1>("check_validity");
//        logger_m.add_sub_header<2>("should_not_pass");
//
//
//        if( !should_pass && went_through_p ){
//            logger_m.freeze();
//            action_logger_m.freeze();
//        }
//    }
    
public:
    template<class EnrichedCandidate>
    void check_validity( EnrichedCandidate const & ec_p,
                         bool went_through_p )
    {
       // check_validity(ec_p, went_through_p, details::should_pass_tag{} );
       // check_validity(ec_p, current_hypothesis_p, went_through_p, should_not_pass_tag{});
    }
    

    void compute_efficiency(){
        action_m.logger_m.add_root_header("EFFICIENCY");
        action_m.logger_m.template add_header<1, details::immutable_tag>("computation");
        action_m.logger_m << "reconstructible_tracks: " << local_reconstructible_number_m << '\n';
        action_m.logger_m << "local_efficiency: " << local_reconstructed_number_m * 100./local_reconstructible_number_m << '\n';
        action_m.logger_m << "global_efficiency: " << reconstructed_number_m * 100./reconstructible_number_m << '\n';

    }
    
    void reset_local_data(){
        local_reconstructed_number_m = 0;
        local_reconstructible_number_m = 0;
        reconstructible_track_mc.clear();
    }
    

};




#endif
