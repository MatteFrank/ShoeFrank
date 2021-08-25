/*
 \author A. Sécher
 */

//
//File      : TATOEmatcher.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 10/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//


#ifndef _TATOEmatcher_HXX
#define _TATOEmatcher_HXX

#include "TAMCntuPart.hxx"
#include "TAGparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGcluster.hxx"

#include "aftereffect.hpp"
#include "TATOEutilities.hxx"

#include "TATWntuPoint.hxx"

template<class Action>
struct TATOEmatcher{
    
    using candidate = typename std::decay_t<decltype( std::declval<typename Action::detector_list_t>().last() )>::candidate;
    using detector_list_t = typename Action::detector_list_t;
    using node_type = typename Action::node_type;
    using data_type = typename Action::data_type;
    
    struct candidate_indices {
        std::vector< int > index_c;
        std::vector<int> const & get_indices() const { return index_c; }
        std::vector<int>& get_indices() { return index_c; }
        void insert( int index_p ) {
            if( std::none_of(
                     index_c.begin(), index_c.end(),
                     [&index_p]( int current_index_p ){ return index_p == current_index_p; }
                            ) ){ index_c.push_back( index_p ); }
        }
    };
    
private:
    TAMCntuPart* data_mhc;
    std::pair<double, double> target_limits_m;
    Action& action_m;
    node_type const * current_node_mh = nullptr;

    std::vector< candidate_indices > candidate_index_mc;
    std::vector< reconstruction_module<data_type> > reconstruction_module_mc;

public:
    TATOEmatcher( TAGparGeo const * global_parameters_ph,
                  Action& action_p ) :
    data_mhc{ static_cast<TAMCntuPart*>( gTAGroot->FindDataDsc( TAMCntuPart::GetDefDataName() )->Object() ) },
        target_limits_m{ retrieve_target_limits( global_parameters_ph ) },
        action_m{action_p}
    {
        reconstruction_module_mc.reserve(20);
    }
    
private:
    std::pair<double, double> retrieve_target_limits( TAGparGeo const * global_parameters_ph ) const
    {
        auto position_z = global_parameters_ph->GetTargetPar().Position.Z();
        auto size = global_parameters_ph->GetTargetPar().Size.Z();
        
        return { position_z - size /2 , position_z + size /2  };
    }

    
public:
    void generate_candidate_indices() {
//        std::cout << "====start_event====\n";
        
        candidate_index_mc.clear();
        //reconstruction_module_mc.clear();
        
        action_m.list_m.apply_for_each(
                                       [this](auto const & detector_p)
                                       {
                                           candidate_index_mc.push_back( candidate_indices{} );
                                           for( std::size_t i{0} ; i < detector_p.layer_count() ; ++i  ){
                                            
                                               auto candidate_c = detector_p.generate_candidates( i );
                                               for( auto const& candidate : candidate_c ) {
                                                   
                                                   for( int i{0}; i < candidate.data->GetMcTracksN() ; ++i  ){
                                                       auto id = candidate.data->GetMcTrackIdx(i);
                                                       candidate_index_mc.back().insert( id );
                                                   }
                                                   
                                               }
                                           }
                                       }
                                       );
        
//        std::cout << "generate_candidate:\n";
//        for( auto const& layer : candidate_index_mc ){
//            for( auto const& index : layer.get_indices() ){ std::cout << index << " "; }
//            std::cout << "\n";
//        }
    }
    
    double retrieve_momentum( candidate const& candidate_p ) const {
        for( auto const& module : reconstruction_module_mc ) {
            if( module.end_point_h == candidate_p.data && module.reconstructible_o.has_value() ){ return module.reconstructible_o.value().properties.momentum; }
        }
        return 0;
    }
    
    
    int retrieve_mass( candidate const& candidate_p ) const {
        for( auto const& module : reconstruction_module_mc ) {
            if( module.end_point_h == candidate_p.data && module.reconstructible_o.has_value() ){ return module.reconstructible_o.value().properties.mass; }
        }
        return 0;
    }
    
    int retrieve_charge( candidate const& candidate_p ) const {
        for( auto const& module : reconstruction_module_mc ) {
            if( module.end_point_h == candidate_p.data && module.reconstructible_o.has_value() ){ return module.reconstructible_o.value().properties.charge; }
        }
        return 0;
    }
    
    
    
    
    void submit_reconstructible_track(candidate const& candidate_p)
    {
        auto output_reconstructible = [this]( auto const& reconstructible_p )
                                    {
                                        action_m.logger_m << "charge: " << reconstructible_p.properties.charge << '\n';
                                        action_m.logger_m << "mass: " << reconstructible_p.properties.mass << '\n';
                                        action_m.logger_m << "momentum: " << reconstructible_p.properties.momentum << '\n';
                                        action_m.logger_m << "track_slope_x: " << reconstructible_p.properties.track_slope_x << '\n';
                                        action_m.logger_m << "track_slope_y: " << reconstructible_p.properties.track_slope_y << '\n';
                                        action_m.logger_m << "mc_track_index: " ;
                                        for( auto index : reconstructible_p.get_indices() ){
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
        
        
        
        auto check_reconstructibility = [this]( std::vector<int> const& index_pc )
        {
            bool is_reconstructible = true;
            
            auto index_found = [&index_pc](candidate_indices const& candidate_index_pc)
            {
                for( auto candidate_index : candidate_index_pc.get_indices() ){
                    if( std::any_of( index_pc.begin(),
                                     index_pc.end(),
                                    [&candidate_index](int index_p){ return candidate_index == index_p; } ) )
                    {
                        return true;
                    }
                }
                return false;
            };
            
            for( auto const& candidate_index_c : candidate_index_mc ){
                is_reconstructible = index_found( candidate_index_c );
                if( !is_reconstructible ){ break; }
            }
                
            return is_reconstructible;
        };
        
        
        
        auto origin_aftereffect = aftereffect::make_aftereffect(
                            [this]( int index_p )
                            {
                                auto const * track_h = data_mhc->GetTrack(index_p);
            
                                return track_h->GetCharge() > 0 ?
                                    ( track_h->GetInitPos().Z() >= target_limits_m.first ) &&
                                    ( track_h->GetInitPos().Z() <= target_limits_m.second )    :
                                    false;
                                return false;
                            },
                            [](int index_p){ return std::vector<int>{index_p}; }
                                                            );
        
        
        auto scattered_aftereffect = aftereffect::make_aftereffect(
                                [this]( int index_p )
                                {
                                    auto const * track_h = data_mhc->GetTrack(index_p);
                                    auto const * mother_track_h = track_h->GetCharge() > 0 ?
                                                        data_mhc->GetTrack( track_h->GetMotherID() ) :
                                                        nullptr;
                                    if( mother_track_h &&
                                        ( mother_track_h->GetCharge() == track_h->GetCharge() ) &&
                                        ( mother_track_h->GetMass() == track_h->GetMass() ) &&
                                        ( mother_track_h->GetInitPos().Z() >= target_limits_m.first ) &&
                                        ( mother_track_h->GetInitPos().Z() <= target_limits_m.second ) ){
                                        return true;
                                    }
                                    return false;
                                },
                                [this](int index_p)
                                {
                                    std::vector<int> index_c{index_p};
                                    auto const * track_h = data_mhc->GetTrack(index_p);
                                    index_c.push_back( track_h->GetMotherID() );
                                    return index_c;
                                }
                                                               );
        
       
        auto check_charge_reconstruction = [this]( candidate const& candidate_p, std::vector<int> const& index_pc )
        {
            auto real_charge = data_mhc->GetTrack(index_pc[0])->GetCharge();
            //std::cout << "real_charge: " << real_charge << '\n';
            auto reconstructed_charge = static_cast<TATWpoint const *>( candidate_p.data )->GetChargeZ();
            //std::cout << "reconstructed_charge: " << reconstructed_charge << '\n';
            return real_charge == reconstructed_charge;
        };
        
        action_m.logger_m.template add_sub_header< details::immutable_tag >("submit_reconstructible_track");
        
        auto track_index_c = find_indices( candidate_p );

        //need to clarify whats is going on here
        if( !track_index_c.empty() ){
            auto result_o = aftereffect::first_of( track_index_c.begin(),
                                                   track_index_c.end(),
                                                   origin_aftereffect,
                                                   scattered_aftereffect );
        
            
            if( result_o.has_value() &&
                check_reconstructibility( result_o.value() ) ) { //&&
                //check_charge_reconstruction(candidate_p, result_o.value()) ){
                
                action_m.logger_m.add_sub_header("track_registered");
                auto particle = form_particle(result_o.value());
                auto reconstructible = reconstructible_track{result_o.value(), std::move(particle)};
                output_reconstructible( reconstructible );
                register_reconstructible_track( candidate_p.data, std::move(reconstructible) );
            }
        }
    }
    
private:
    particle_properties form_particle( std::vector<int> const& index_pc ) const
    {
        auto mc_track_h = index_pc.size() > 1 ? data_mhc->GetTrack(index_pc[1]) : data_mhc->GetTrack(index_pc[0]) ;
        //if size > 1, then the particle has been scattered, therefore need to retrieve mother parameters
        return particle_properties{
            mc_track_h->GetInitP().Mag() * 1000,
            mc_track_h->GetCharge(),
            static_cast<int>( mc_track_h->GetMass() * 1.1 ), //bad hack to get number of nucleons
            mc_track_h->GetInitP().X()/mc_track_h->GetInitP().Z(),
            mc_track_h->GetInitP().Y()/mc_track_h->GetInitP().Z()
        };
    }
    
    void register_reconstructible_track(data_type const* end_point_ph, reconstructible_track&& reconstructible_p)
    {
        reconstruction_module_mc.push_back(
                                           reconstruction_module<data_type>{
                                               end_point_ph,
                                               aftereffect::optional<reconstructible_track>{ std::move(reconstructible_p) },
                                               aftereffect::optional<reconstructed_track<data_type>>{ }
                                           }
                                           );
    }

    
    
public:
    template<class T>
    void submit_reconstructed_track( T const& track_p )
    {
        action_m.logger_m.template add_sub_header< details::immutable_tag >("reconstructed_track");
        
        //output_current_hypothesis();
        
        auto const& state_c = track_p.get_clusters();
        using full_state = typename std::decay_t<decltype(state_c)>::value_type ;
        std::vector<full_state> full_state_c{ state_c.begin()+ 1 , state_c.end() }; //remove vertex
        
        std::vector< data_type const* > cluster_c;
        cluster_c.reserve( full_state_c.size() );
        chisquared chi2;
        for( auto const& full_state : full_state_c ){
            cluster_c.emplace_back( full_state.data );
            chi2.chisquared_predicted += full_state.chisquared_predicted;
            chi2.chisquared_corrected += full_state.chisquared_corrected;
            chi2.distance  += full_state.distance;
        }
        chi2.chisquared_predicted /= full_state_c.size();
        chi2.chisquared_corrected /= full_state_c.size();
        chi2.distance /= full_state_c.size();
        
        //store both chisquare as well -> distribution of mean value then 
        auto particle = particle_properties{
                                       track_p.momentum,
                                       track_p.hypothesis.properties.charge,
                                       track_p.hypothesis.properties.mass };
        
        action_m.logger_m << "charge: " << particle.charge << '\n';
        action_m.logger_m << "mass: " << particle.mass << '\n';
        action_m.logger_m << "momentum: " << particle.momentum << '\n';
        
        auto reconstructed = reconstructed_track<data_type>{ cluster_c, std::move(particle),  track_p.parameters, track_p.clone, chi2 };
        register_reconstructed_track( cluster_c.back(), std::move(reconstructed) );
    }
    
private:
    void register_reconstructed_track(data_type const* end_point_ph, reconstructed_track<data_type>&& reconstructed_p)
    {
        auto module_i = std::find_if( reconstruction_module_mc.begin(),
                                     reconstruction_module_mc.end(),
                                     [&end_point_ph](reconstruction_module<data_type> const& module_p){ return module_p.end_point_h == end_point_ph;  } );
        if( module_i != reconstruction_module_mc.end() ){
            module_i->reconstructed_o = aftereffect::optional<reconstructed_track<data_type>>{ std::move(reconstructed_p) };
            
            action_m.logger_m << "corresponding_mc_id: ";
            auto index_c = module_i->reconstructible_o.value().get_indices();
            for( auto index : index_c ){ action_m.logger_m << index << " "; }
            action_m.logger_m << "\n";
        }
        else{
            reconstruction_module_mc.emplace_back(
                                        end_point_ph,
                                        aftereffect::optional<reconstructible_track>{  },
                                        aftereffect::optional<reconstructed_track<data_type>>{ std::move(reconstructed_p) }
                                                  );
        }
    }


public:
    //will need a reset method
    void clear_reconstruction_modules(){ reconstruction_module_mc.clear(); }
    
    std::vector<reconstruction_module<data_type>> const& retrieve_results() const {
        return reconstruction_module_mc;
    }
    
//    void compute_results( details::all_mixed_tag ) const{
//
//        //        action_m.logger_m.freeze_everything();
//        action_m.logger_m.add_root_header("RESULTS");
//        action_m.logger_m.template add_header<1, details::immutable_tag>("mixed");
//
//        std::size_t reconstructed_number{0};
//        std::size_t reconstructible_number{0};
//
//        std::size_t correct_cluster_number{0};
//        std::size_t recovered_cluster_number{0};
//
////        std::size_t total_cluster_number{0};
////        std::size_t local_total_cluster_number{0};
//        std::size_t fake_number{0};
//
//        std::size_t clone_number{0};
//
//        for(auto const & module : computation_module_mc){
//            reconstructed_number += module.reconstructed_number;
//            reconstructible_number += module.reconstructible_number;
//
//            correct_cluster_number += module.correct_cluster_number;
//            recovered_cluster_number += module.recovered_cluster_number;
////            total_cluster_number += module.total_cluster_number;
//
//            fake_number += module.fake_number;
//            clone_number += module.clone_number;
//        }
//        //look at mass efficiency reconstruction ? because of the plage im momentum of protons !
//        action_m.logger_m.template add_header<1, details::immutable_tag>("efficiency");
//        auto efficiency = reconstructed_number * 1./reconstructible_number;
//        action_m.logger_m << "global_efficiency: " << efficiency * 100 << '\n';
//        auto efficiency_error = sqrt(efficiency* (1+ efficiency)/reconstructible_number);
//        action_m.logger_m << "global_efficiency_error: " << efficiency_error * 100<< '\n';
//
//        action_m.logger_m.template add_header<1, details::immutable_tag>("purity");
//        auto purity = correct_cluster_number * 1./recovered_cluster_number;
//        action_m.logger_m << "global_purity: " << purity * 100 << '\n';
//        auto purity_error = sqrt(purity* (1+purity)/recovered_cluster_number);
//        action_m.logger_m << "global_purity_error: " << purity_error * 100<< '\n';
//
////        action_m.logger_m.template add_header<1, details::immutable_tag>("coverage");
////        double coverage = recovered_cluster_number * 1./total_cluster_number;
////        action_m.logger_m << "global_coverage: " << coverage * 100 << '\n';
////        action_m.logger_m << "global_coverage_error: " << sqrt(coverage* (1+coverage)/total_cluster_number) * 100<< '\n';
//
//        action_m.logger_m.template add_header<1, details::immutable_tag>("fake_yield");
//        auto fake_yield = fake_number * 1./reconstructed_number;
//        action_m.logger_m << "fake_yield: " << fake_yield * 100 << '\n';
//        auto fake_yield_error = sqrt( fake_yield * (1+ fake_yield)/reconstructed_number);
//        action_m.logger_m << "fake_yield_error: " << fake_yield_error * 100<< '\n';
//
//        action_m.logger_m.template add_header<1, details::immutable_tag>("multiplicity");
//        auto multiplicity = clone_number * 1./reconstructed_number;
//        action_m.logger_m << "global_clone_ratio: " << multiplicity << '\n';
//        auto multiplicity_error = sqrt(multiplicity * (1+multiplicity)/reconstructed_number);
//        action_m.logger_m << "global_clone_ratio_error: " << multiplicity_error << '\n';
//
//    }
    
//    void compute_results( details::all_separated_tag ){
//        //        action_m.logger_m.freeze_everything();
//        action_m.logger_m.add_root_header("RESULTS");
//        action_m.logger_m.template add_header<1, details::immutable_tag>("separated");
//        for(auto const & module : computation_module_mc){
//            action_m.logger_m << "charge: " << module.charge << '\n';
//            compute_efficiency( module );
//            compute_purity( module );
//        }
//    }
//
//
//private:
//    void compute_efficiency( computation_module const & module_p ){
//        auto efficiency = module_p.reconstructed_number * 1./module_p.reconstructible_number;
//        action_m.logger_m << "global_efficiency: " << efficiency * 100 << '\n';
//        auto efficiency_error = sqrt( efficiency* (1+ efficiency)/module_p.reconstructible_number);
//        action_m.logger_m << "global_efficiency_error: " << efficiency_error * 100<< '\n';
//    }
//
//
//    void compute_purity( computation_module const & module_p ){
//        auto purity = module_p.correct_cluster_number * 1./module_p.recovered_cluster_number;
//        action_m.logger_m << "global_purity: " << purity * 100 << '\n';
//        auto purity_error = sqrt( purity* (1+purity)/module_p.recovered_cluster_number);
//        action_m.logger_m << "global_purity_error: " << purity_error * 100<< '\n';
//    }
//

};


template<class Action>
struct empty_matcher{
    using candidate = typename std::decay_t<decltype( std::declval<typename Action::detector_list_t>().last() )>::candidate;
    using track = typename Action::track;
    using node_type = typename Action::node_type;
    using data_type = typename Action::data_type;
    
    std::vector<reconstruction_module<data_type>> reconstruction_module_mc;
    
    void submit_reconstructed_track(track const& ){}
    void submit_reconstructible_track(candidate const& ){}
    void generate_candidate_indices(){}
    std::vector<reconstruction_module<data_type>> const& retrieve_results() const{ return reconstruction_module_mc; }
    void clear_reconstruction_modules(){}
};

template<class Action>
struct matcher{
    using candidate = typename std::decay_t<decltype( std::declval<typename Action::detector_list_t>().last() )>::candidate;
    using track = typename Action::track;
    using node_type = typename Action::node_type;
    using data_type = typename Action::data_type;

    struct eraser{
        virtual ~eraser() = default;
        virtual void submit_reconstructed_track(track const& track_p) = 0;
        virtual void submit_reconstructible_track(candidate const& candidate_p) = 0;
        virtual void generate_candidate_indices() = 0;
        virtual std::vector<reconstruction_module<data_type>> const& retrieve_results() const = 0;
        virtual void clear_reconstruction_modules() =0;
    };

    template<class T>
    struct holder : eraser {
        constexpr holder() = default;
        constexpr holder(T t) : t_m{ std::move(t)} {}
        void submit_reconstructed_track(track const& track_p) override{ t_m.submit_reconstructed_track( track_p); }
        void submit_reconstructible_track(candidate const& candidate_p) override{ t_m.submit_reconstructible_track( candidate_p); }
        void generate_candidate_indices() override{ t_m.generate_candidate_indices();}
        std::vector<reconstruction_module<data_type>> const& retrieve_results() const override{ return t_m.retrieve_results(); }
        void clear_reconstruction_modules() override { t_m.clear_reconstruction_modules(); }
        T t_m;
    };

    constexpr matcher() = default;
    template<class T>
    constexpr matcher(T t) : erased_m{ new holder<T>{ std::move(t) } } {}
    template<class T>
    constexpr matcher& operator=(T t){ 
        erased_m.reset( new holder<T>{ std::move(t) } ); 
        return *this;
    }


    void submit_reconstructed_track(track const& track_p) { erased_m->submit_reconstructed_track( track_p); }
    void submit_reconstructible_track(candidate const& candidate_p) { erased_m->submit_reconstructible_track( candidate_p); }
    void generate_candidate_indices() { erased_m->generate_candidate_indices();}
    std::vector<reconstruction_module<data_type>> const& retrieve_results() const { return erased_m->retrieve_results(); }
    void clear_reconstruction_modules(){ erased_m->clear_reconstruction_modules(); }

    private:
    std::unique_ptr<eraser> erased_m;
}; 


#endif
