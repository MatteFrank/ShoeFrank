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
    
    
    
    template<std::size_t Charge>
    struct charge_tag {};
    
    struct all_mixed_tag{};
    struct all_separated_tag{};
} // namespace details

template<class Action>
struct TATOEchecker{
    
    using candidate = typename std::decay_t<decltype( std::declval<typename Action::detector_list_t>().last() )>::candidate;
    using detector_list_t = typename Action::detector_list_t;
    using node_type = typename Action::node_type;
    
    struct particle{
        std::vector<int> const & get_indices() const { return index_c; }
        std::vector<int>& get_indices() { return index_c; }
        
        std::vector<int> index_c;
        double momentum;
        double track_slope_x{0};
        double track_slope_y{0};
        int charge;
        int mass;
    };
    
    struct track{
        std::vector<TAGcluster const *> data_ch;
        particle real_particle;
    };
    
    struct computation_module{
        
        struct histogram_bundle {
            TH1D* reconstructible_h{nullptr};
            TH1D* reconstructed_h{nullptr};
        } efficiency_histogram_bundle;
        
        int charge;
        
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};
        std::size_t local_reconstructed_number{0};
        std::size_t local_reconstructible_number{0};
        
        std::size_t correct_cluster_number{0};
        std::size_t total_cluster_number{0};
        std::size_t local_correct_cluster_number{0};
        std::size_t local_total_cluster_number{0};
                                                                               
        
        computation_module(int charge_p, double /*beam_energy_p*/) : charge{charge_p}
        {
            efficiency_histogram_bundle.reconstructible_h = new TH1D{ Form("reconstructible_charge%d", charge),
                                                                      ";Momentum(Gev/c);Count", 50, 0, 1.3 };
            efficiency_histogram_bundle.reconstructed_h = new TH1D{ Form("reconstructed_charge%d", charge),
                                                                    ";Momentum(Gev/c);Count", 50, 0, 1.3 };
        }
                                                                               
        
        TH1D const * get_reconstructed_histogram() const { return efficiency_histogram_bundle.reconstructed_h; }
        TH1D * get_reconstructed_histogram() { return efficiency_histogram_bundle.reconstructed_h; }
        
        TH1D const * get_reconstructible_histogram() const { return efficiency_histogram_bundle.reconstructible_h; }
        TH1D * get_reconstructible_histogram() { return efficiency_histogram_bundle.reconstructible_h; }
        
        void reset_local()
        {
            local_correct_cluster_number = 0;
            local_total_cluster_number = 0;
            local_reconstructible_number = 0;
            local_reconstructed_number = 0;
        }
    };
    
private:
    TAMCntuEve* data_mhc;
   
    std::pair<double, double> target_limits_m;

    node_type const * current_node_mh = nullptr;
    
    std::vector< particle > reconstructible_track_mc;
    std::vector< computation_module > module_c;
    std::size_t fake_number_m{0};
    std::size_t local_fake_number_m{0};
    
    Action& action_m;
    
public:
    TATOEchecker( TAGparGeo const * global_parameters_ph,
                  Action& action_p ) :
        data_mhc{ static_cast<TAMCntuEve*>( gTAGroot->FindDataDsc( "eveMc" )->Object() ) },
        target_limits_m{ retrieve_target_limits( global_parameters_ph ) },
        action_m{action_p}
    {
        module_c.reserve(20);
    }
    
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
        
        auto output_real_particle = [this]( particle& real_particle_p )
                                    {
                                        action_m.logger_m << "charge: " << real_particle_p.charge << '\n';
                                        action_m.logger_m << "mass: " << real_particle_p.mass << '\n';
                                        action_m.logger_m << "momentum: " << real_particle_p.momentum << '\n';
                                        action_m.logger_m << "track_slope_x: " << real_particle_p.track_slope_x << '\n';
                                        action_m.logger_m << "track_slope_y: " << real_particle_p.track_slope_y << '\n';
                                        action_m.logger_m << "mc_track_index: " ;
                                        for( auto index : real_particle_p.get_indices() ){
                                            action_m.logger_m << index << " ";
                                        }
                                        action_m.logger_m << '\n';
                                    };
        
        
        auto find_indices = [this]( candidate const& candidate_p )
                            {
                                std::vector<int> index_c;
                                index_c.reserve( candidate_p.data->GetMcTracksN() );
                                for( int i{0}; i < candidate_p.data->GetMcTracksN() ; ++i  ){
                                    auto id = candidate_p.data->GetMcTrackIdx(i);
                                    index_c.emplace_back( id );
                                }
                                return index_c;
                            };
        
        
        
        auto find_and_register = [this, &output_real_particle]( std::vector<int> track_index_pc,
                                                                auto predicate_p,
                                                                auto filler_p )
                                 {
                                     auto found_i = std::find_if( track_index_pc.begin(),
                                                                  track_index_pc.end(),
                                                                  predicate_p );
                                     
                                     if( found_i != track_index_pc.end() ){
                                         action_m.logger_m.add_sub_header("track_registered");
                                         
                                         std::vector<int> index_c{ filler_p( *found_i) };
                                         reconstructible_track_mc.push_back( form_particle( index_c ) );
                                         //reconstructible_track_mc.push_back( form_track( index_c, action_m.list_m ) );
                                         
                                         auto & particle = reconstructible_track_mc.back();
                                         output_real_particle( particle );
                                         
                                         auto & module = find_module( particle.charge );
                                         ++module.reconstructible_number;
                                         ++module.local_reconstructible_number;
                                         module.get_reconstructible_histogram()->Fill( particle.momentum/(1000*particle.mass) );
//                                         std::cout << "value: " << particle.momentum/(1000*particle.mass) << std::endl;
                                         
                                         return true;
                                     }
                                     
                                     return false;
                                 };
        
        
        
        auto check_origin = [this]( int index_p )
                            {
                                auto const * track_h = data_mhc->GetHit(index_p);
            
                                return track_h->GetCharge() > 0 ?
                                    ( track_h->GetInitPos().Z() >= target_limits_m.first ) &&
                                    ( track_h->GetInitPos().Z() <= target_limits_m.second )    :
                                    false;
                            };
        
        
        auto check_scattering = [this, &check_origin]( int index_p )
                                {
                                    auto const * track_h = data_mhc->GetHit(index_p);
                                    auto const * mother_track_h = track_h->GetCharge() > 0 ?
                                                        data_mhc->GetHit( track_h->GetMotherID() ) :
                                                        nullptr;
                                    if( mother_track_h ){
                                        return (  (mother_track_h->GetCharge() == track_h->GetCharge() ) &&
                                                  (mother_track_h->GetMass() == track_h->GetMass() )         ) ?
                                               check_origin( track_h->GetMotherID() ) : false ;
                                    }
                                    return false;
                                };
        
        
        action_m.logger_m.template add_sub_header< details::immutable_tag >("reconstructible_track");
        
        auto track_index_c = find_indices( candidate_p );
        

        find_and_register( track_index_c,
                           check_origin,
                           [](int index_p){ return std::vector<int>{index_p}; } ) ||
        find_and_register( track_index_c,
                           check_scattering,
                           [this](int index_p)
                           {
                               std::vector<int> index_c{index_p};
                               auto const * track_h = data_mhc->GetHit(index_p);
                               index_c.push_back( track_h->GetMotherID() );
                               return index_c;
                           } );
    }
    
private:
    
    
    track form_track( std::vector<int> index_pc,
                      detector_list_t const & list_p ) const
    {
        auto index_found = [this, &index_pc](auto const & candidate_p)
                            {
                                bool is_index_found = false;
                                for( auto j{0} ; j < candidate_p.data->GetMcTracksN() ; ++j ){
                                    is_index_found = is_index_found ||
                                                     ( std::any_of( index_pc.begin(), index_pc.end(),
                                                                    [&candidate_p, &j](int index_p)
                                                                    { return index_p == candidate_p.data->GetMcTrackIdx(j); } ) );
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
                            
                            auto candidate_i = std::find_if( candidate_c.begin(), candidate_c.end(), index_found );
                            if( candidate_i != candidate_c.end() ){ data_ch.push_back( candidate_i->data ); }
                        }
                    }
                              );
        
        
        auto mc_track_h = index_pc.size() > 1 ? data_mhc->GetHit(index_pc[1]) : data_mhc->GetHit(index_pc[0]) ;
        //if size > 1, then the particle has been scattered, therefore need to retrieve mother parameters
        auto real_particle = particle{
                                index_pc,
                                mc_track_h->GetInitP().Mag() * 1000,
                                mc_track_h->GetInitP().X()/mc_track_h->GetInitP().Z(),
                                mc_track_h->GetInitP().Y()/mc_track_h->GetInitP().Z(),
                                mc_track_h->GetCharge(),
                                static_cast<int>( mc_track_h->GetMass() * 1.1 ) //bad hack to get number of nucleons
                                       };
        return track{ std::move(data_ch), std::move(real_particle) };
    }
    
    particle form_particle( std::vector<int> index_pc ) const
    {
        auto mc_track_h = index_pc.size() > 1 ? data_mhc->GetHit(index_pc[1]) : data_mhc->GetHit(index_pc[0]) ;
        //if size > 1, then the particle has been scattered, therefore need to retrieve mother parameters
        return particle{
            index_pc,
            mc_track_h->GetInitP().Mag() * 1000,
            mc_track_h->GetInitP().X()/mc_track_h->GetInitP().Z(),
            mc_track_h->GetInitP().Y()/mc_track_h->GetInitP().Z(),
            mc_track_h->GetCharge(),
            static_cast<int>( mc_track_h->GetMass() * 1.1 ) //bad hack to get number of nucleons
        };
    }

    
    computation_module& find_module(int charge_p)
    {
        auto module_i = std::find_if( module_c.begin(), module_c.end(),
                                      [&charge_p](computation_module const & module_p){ return charge_p == module_p.charge; });
        if( module_i == module_c.end() ){
            module_c.push_back( computation_module{charge_p, action_m.beam_energy_m} ) ;
            return module_c.back();
        }
        return *module_i;
    }
    
    
public:
    
    
    template<class T>
    void register_reconstructed_track( std::vector< T > const & full_state_pc )
    {
        action_m.logger_m.template add_sub_header< details::immutable_tag >("reconstructed_track");
        
//        output_current_hypothesis();
        
        std::vector< T > full_state_c{ full_state_pc.begin()+ 1 , full_state_pc.end() }; //remove vertex
        
        auto retrieve_track_ids = [this]( TAGcluster const * data_ph ) -> std::vector<int>
                                  {
                                      std::vector<int> track_id_c;
                                      track_id_c.reserve( data_ph->GetMcTracksN() );
                                      for( int i{0} ; i < data_ph->GetMcTracksN() ; ++ i){
                                          track_id_c.push_back( data_ph->GetMcTrackIdx(i) );
                                      }
                                      return track_id_c;
                                  };
        
        auto retrieve_reconstructible = [this](std::vector<int> const& id_pc)
                                        {
                                            for( auto const & id : id_pc){
                                                auto reconstructible_i =
                                                        std::find_if( reconstructible_track_mc.begin(),
                                                                      reconstructible_track_mc.end(),
                                                                      [&id](particle const & particle_p)
                                                                      {
                                                                          auto const & reconstructible_id_c = particle_p.get_indices();
                                                                          return std::any_of( reconstructible_id_c.begin(),
                                                                                              reconstructible_id_c.end(),
                                                                                              [&id](int id_p){ return id_p == id; } );
                                                                      });
                                                if( reconstructible_i != reconstructible_track_mc.end() ){ return reconstructible_i; }
                                            }
                                            return reconstructible_track_mc.end();
                                        };
        
        auto const * end_point_h = full_state_c.back().data;
        auto const& id_c = retrieve_track_ids(end_point_h);
        
        auto reconstructible_i = retrieve_reconstructible( id_c );
        
        if( reconstructible_i !=  reconstructible_track_mc.end() ){
            
            auto & module = find_module( reconstructible_i->charge );
            ++module.reconstructed_number ;
            ++module.local_reconstructed_number;
            module.get_reconstructed_histogram()->Fill( reconstructible_i->momentum/(1000 * reconstructible_i->mass)  );
            
            action_m.logger_m << "targeted_track_id: ";
            auto const & reconstructible_id_c = reconstructible_i->get_indices();
            for(auto const& id : reconstructible_id_c){ action_m.logger_m << id << " "; }
            action_m.logger_m << '\n';
            
            module.total_cluster_number += full_state_c.size();
            module.local_total_cluster_number += full_state_c.size();
            
            
            for( auto const & full_state : full_state_c ){
                action_m.logger_m << "current_track_id: ";
                auto current_track_id_c = retrieve_track_ids( full_state.data );
                for(auto const& id : current_track_id_c){ action_m.logger_m << id << " "; }
                action_m.logger_m << '\n';
                
                if( std::any_of( reconstructible_id_c.begin(),
                                 reconstructible_id_c.end(),
                                 [&current_track_id_c](int id_p)
                                 {
                                    return std::any_of( current_track_id_c.begin(),
                                                        current_track_id_c.end(),
                                                        [&id_p](int current_id_p){ return current_id_p == id_p; } );
                                 }   ) )
                {
                    ++module.correct_cluster_number;
                    ++module.local_correct_cluster_number;
                }
            }
            
            
        }
        else{
            ++fake_number_m;
            ++local_fake_number_m;
            action_m.logger_m.add_sub_header("fake_track");
            for( auto const & full_state : full_state_c ){
                action_m.logger_m << "current_track_id: ";
                auto current_track_id_c = retrieve_track_ids( full_state.data );
                for(auto const& id : current_track_id_c){ action_m.logger_m << id << " "; }
                action_m.logger_m << '\n';
            }
        }
        
    }
    
    
public:
    
    template<class EnrichedCandidate>
    void check_validity( EnrichedCandidate const & ec_p,
                         double ec_chisquared_p,
                         double cutter_chisquared_p,
                         details::should_pass_tag )
    {
        bool went_through = ec_chisquared_p < cutter_chisquared_p; //this should be tested before ...
        if( went_through ){ return ; }
        if( abs( (cutter_chisquared_p - ec_chisquared_p)/cutter_chisquared_p ) > 10 ){ return; }
        
        action_m.logger_m.template add_sub_header<details::fleeting_tag>("check_validity");
        action_m.logger_m.add_sub_header("should_pass");
        
        output_current_hypothesis();
        
        action_m.logger_m.add_sub_header("cluster");
        action_m.logger_m << "mc_id: ";
        std::vector<int> track_id_c;
        track_id_c.reserve( ec_p.data->GetMcTracksN() );
        for( int i{0} ; i < ec_p.data->GetMcTracksN() ; ++ i){
            track_id_c.push_back( ec_p.data->GetMcTrackIdx(i) );
            action_m.logger_m << ec_p.data->GetMcTrackIdx(i) << " ";
        }
        action_m.logger_m << '\n';
        
        
        if( should_pass( track_id_c ) ){
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
    
    
    public:
    void output_current_hypothesis()
    {
        action_m.logger_m.add_sub_header("current_hypothesis");
        action_m.logger_m << "charge: " << action_m.particle_m.charge << '\n';
        action_m.logger_m << "mass: " << action_m.particle_m.mass << '\n';
        action_m.logger_m << "momentum: " << action_m.particle_m.momentum << '\n';
        
        auto * root_h = current_node_mh->get_ancestor();
        action_m.logger_m << "track_slope_x: " << root_h->get_value().vector(2, 0) << '\n';
        action_m.logger_m << "track_slope_y: " << root_h->get_value().vector(3, 0) << '\n';
    }
    

    template<class EnrichedCandidate>
    void check_validity( EnrichedCandidate const & ec_p,
                         double ec_chisquared_p,
                         double cutter_chisquared_p,
                         details::should_not_pass_tag )
    {
        bool went_through = ec_chisquared_p < cutter_chisquared_p;
        if( !went_through ){ return; }
        
        action_m.logger_m.template add_sub_header<details::fleeting_tag>("check_validity");
        action_m.logger_m.add_sub_header("should_not_pass");
        
        output_current_hypothesis();
        
        action_m.logger_m.add_sub_header("cluster");
        action_m.logger_m << "mc_id: ";
        std::vector<int> track_id_c;
        track_id_c.reserve( ec_p.data->GetMcTracksN() );
        for( int i{0} ; i < ec_p.data->GetMcTracksN() ; ++ i){
            track_id_c.push_back( ec_p.data->GetMcTrackIdx(i) );
            action_m.logger_m << ec_p.data->GetMcTrackIdx(i) << " ";
        }
        action_m.logger_m << '\n';
        
        
        if( !should_pass(track_id_c) ){
            action_m.logger_m.freeze();
        }
    }
    
public:
    template<class EnrichedCandidate>
    void check_validity( EnrichedCandidate const & ec_p,
                         double ec_chisquared_p,
                         double cutter_chisquared_p )
    {
        check_validity(ec_p, ec_chisquared_p, cutter_chisquared_p, details::should_pass_tag{} );
        check_validity(ec_p, ec_chisquared_p, cutter_chisquared_p, details::should_not_pass_tag{});
    }
    
public:
    template< int Charge, class Enabler = std::enable_if_t< (Charge > 0) > >
    void compute_results( details::charge_tag<Charge> ){
//        action_m.logger_m.freeze_everything();
        action_m.logger_m.add_root_header("RESULTS");
        action_m.logger_m.template add_header<1, details::immutable_tag>("one_charge_only");
        action_m.logger_m << "charge: " << Charge << '\n';
        auto & module = find_module( Charge );
        output_efficiency( module );
        output_purity( module );
    }
    
    void compute_results( details::all_mixed_tag ){
        //        action_m.logger_m.freeze_everything();
        action_m.logger_m.add_root_header("RESULTS");
        action_m.logger_m.template add_header<1, details::immutable_tag>("mixed");
        
        std::size_t reconstructed_number{0};
        std::size_t reconstructible_number{0};
        std::size_t local_reconstructed_number{0};
        std::size_t local_reconstructible_number{0};
        
        std::size_t correct_cluster_number{0};
        std::size_t total_cluster_number{0};
        std::size_t local_correct_cluster_number{0};
        std::size_t local_total_cluster_number{0};
        
        for(auto const & module : module_c){
            reconstructed_number += module.reconstructed_number;
            reconstructible_number += module.reconstructible_number;
            local_reconstructed_number += module.local_reconstructed_number;
            local_reconstructible_number += module.local_reconstructible_number;
            
            correct_cluster_number += module.correct_cluster_number;
            total_cluster_number += module.total_cluster_number;
            local_correct_cluster_number += module.local_correct_cluster_number;
            local_total_cluster_number += module.local_total_cluster_number;
        }
        
        action_m.logger_m.template add_header<1, details::immutable_tag>("efficiency");
        action_m.logger_m << "reconstructed_tracks: " << local_reconstructed_number << '\n';
        action_m.logger_m << "reconstructible_tracks: " << local_reconstructible_number << '\n';
        action_m.logger_m << "local_efficiency: " << local_reconstructed_number * 100./local_reconstructible_number << '\n';
        action_m.logger_m << "global_efficiency: " << reconstructed_number * 100./reconstructible_number << '\n';
        action_m.logger_m << "fake_rate: " << fake_number_m * 100. /reconstructed_number  << '\n';
        
        action_m.logger_m.template add_header<1, details::immutable_tag>("purity");
        action_m.logger_m << "correct_cluster: " << local_correct_cluster_number<< '\n';
        action_m.logger_m << "total_cluster: " << local_total_cluster_number << '\n';
        action_m.logger_m << "local_purity: " << local_correct_cluster_number * 100./local_total_cluster_number << '\n';
        action_m.logger_m << "global_purity: " << correct_cluster_number * 100./total_cluster_number << '\n';
    }
    
    void compute_results( details::all_separated_tag ){
        //        action_m.logger_m.freeze_everything();
        action_m.logger_m.add_root_header("RESULTS");
        action_m.logger_m.template add_header<1, details::immutable_tag>("separated");
        for(auto const & module : module_c){
            action_m.logger_m << "charge: " << module.charge << '\n';
            compute_efficiency( module );
            compute_purity( module );
        }
    }
    
    void register_histograms( details::all_separated_tag )
    {
        for(auto const & module : module_c){
            TH1D* efficiency_histogram_h = new TH1D{ Form("efficiency_charge%d", module.charge),
                                                     ";Momentum (Gev/c/n);Efficiency #epsilon", 50, 0, 1.3 };
            TH1D const * reconstructed_h = module.get_reconstructed_histogram() ;
            TH1D const * reconstructible_h = module.get_reconstructible_histogram() ;
    
            for(auto i{0} ; i < reconstructible_h->GetNbinsX() ; ++i)
            {
//                std::cout << "bin: " << i << '\n';
//                std::cout << "value: " << reconstructible_h->GetBinCenter(i) << '\n';
//                std::cout << "reconstructible: " << reconstructible_h->GetBinContent(i) << '\n';
//                std::cout << "reconstructed: " << reconstructed_h->GetBinContent(i) << '\n';
                if(reconstructible_h->GetBinContent(i) != 0){
                    auto reconstructed = reconstructed_h->GetBinContent(i);
                    auto reconstructible = reconstructible_h->GetBinContent(i);
                    auto efficiency = reconstructed * 1./reconstructible;
                    efficiency_histogram_h->SetBinContent( i, efficiency );
                    
                    auto error = sqrt( efficiency * ( 1 + efficiency) / reconstructible );
                    efficiency_histogram_h->SetBinError( i, error );
                }
                else {
                    efficiency_histogram_h->SetBinContent(i, 0 );
                    efficiency_histogram_h->SetBinError( i, 0);
                }
            }
    
            action_m.reconstructed_track_mhc->AddHistogram( efficiency_histogram_h );
//            action_m.reconstructed_track_mhc->AddHistogram( const_cast<TH1D*>(reconstructed_h) );
        }
    }
    
private:
    void compute_efficiency( computation_module const & module_p ){
        auto efficiency = module_p.reconstructed_number * 1./module_p.reconstructible_number;
        action_m.logger_m << "global_efficiency: " << efficiency * 100 << '\n';
        action_m.logger_m << "global_efficiency_error: " << sqrt(efficiency* (1+ efficiency))/sqrt(module_p.reconstructible_number) * 100<< '\n';
    
        
    }
    
    
    void compute_purity( computation_module const & module_p ){
        auto purity = module_p.correct_cluster_number * 1./module_p.total_cluster_number;
        action_m.logger_m << "global_purity: " << purity * 100 << '\n';
        action_m.logger_m << "global_purity_error: " << sqrt(purity* (1+purity))/sqrt(module_p.reconstructible_number)  * 100<< '\n';
        
//        if( 2 * local_correct_cluster_number_m < local_total_cluster_number_m ){
//            action_m.logger_m.freeze_everything();
//        }
    }
    
    
public:
    void reset_local_data(){
        for(auto& module : module_c){ module.reset_local(); }
        reconstructible_track_mc.clear();
        local_fake_number_m = 0;
    }
    

};




#endif
