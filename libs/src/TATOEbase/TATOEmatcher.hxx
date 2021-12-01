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


#include "flag_set.hpp"
#include "aftereffect.hpp"
#include "TATOEutilities.hxx"
#include "TATOEdetector.hxx"

#include "TAMCntuPart.hxx"
#include "TAGparGeo.hxx"
#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGcluster.hxx"

class TAVTcluster;
class TAITcluster;
#include "TAMSDcluster.hxx"
#include "TATWntuPoint.hxx"



template<class Derived>
struct empty_writer{
    void write() {}
};





template<class Action, template<class> class Writer>
struct TATOEmatcher : Writer<TATOEmatcher<Action, Writer>> {
    
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
    std::vector< reconstruction_module > reconstruction_module_mc;
    std::vector< data_type const * >     end_point_mc;
    
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
            mc_track_h->GetMass(),
            mc_track_h->GetInitP().X()/mc_track_h->GetInitP().Z(),
            mc_track_h->GetInitP().Y()/mc_track_h->GetInitP().Z()
        };
    }
    
    void register_reconstructible_track(data_type const* end_point_ph, reconstructible_track&& reconstructible_p)
    {
        reconstruction_module_mc.push_back(
                        reconstruction_module{
                            aftereffect::optional<reconstructible_track>{ std::move(reconstructible_p) },
                            aftereffect::optional<reconstructed_track>{ }
                                              }
                                           );
        end_point_mc.push_back( end_point_ph );
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
        
        std::vector< cluster > cluster_c;
        cluster_c.reserve( full_state_c.size() );
        for( auto const& full_state : full_state_c ){
            auto result = cluster{};
            result.position = cluster::vector{
                full_state.data->GetPositionG().X(),
                full_state.data->GetPositionG().Y(),
                full_state.data->GetPositionG().Z()
            };
            result.chi2 = chisquared{ full_state.prediction, full_state.correction, full_state.distance };
            if( dynamic_cast<TAVTcluster const*>( full_state.data ) ){ result.opcode = flag_set<details::vertex_tag>{}; }
            if( dynamic_cast<TAITcluster const*>( full_state.data ) ){ result.opcode = flag_set<details::it_tag>{}; }
            if( dynamic_cast<TAMSDcluster const*>( full_state.data ) ){
                auto msd_h = dynamic_cast<TAMSDcluster const*>( full_state.data );
                if( msd_h->GetPlaneView()  ){ result.opcode = flag_set<details::msd_tag, details::y_view_tag>{}; }
                else{ result.opcode = flag_set<details::msd_tag, details::x_view_tag>{}; }
            }
            if( dynamic_cast<TATWpoint const*>( full_state.data ) ){ result.opcode = flag_set<details::tof_tag>{}; }
            for(auto i{0}; i < full_state.data->GetMcTracksN(); ++i){ result.mc_index_c.push_back( full_state.data->GetMcTrackIdx(i) ); }
            cluster_c.push_back( std::move(result ));
        }
        
        //store both chisquare as well -> distribution of mean value then 
        auto particle = particle_properties{
                                       track_p.momentum,
                                       track_p.hypothesis.properties.charge,
                                       static_cast<int>(track_p.nucleon_number),
                                       track_p.mass };
        
        action_m.logger_m << "charge: " << particle.charge << '\n';
        action_m.logger_m << "mass: " << particle.mass << '\n';
        action_m.logger_m << "momentum: " << particle.momentum << '\n';
        
        auto reconstructed = reconstructed_track{ cluster_c, std::move(particle), track_p.total_chisquared, track_p.parameters, track_p.clone, track_p.hypothesis.properties.momentum };
        register_reconstructed_track( state_c.back().data , std::move(reconstructed) );
    }    
    
private:
    void register_reconstructed_track(data_type const* end_point_ph, reconstructed_track&& reconstructed_p)
    {
        auto end_point_i = std::find_if( end_point_mc.begin(), end_point_mc.end(),
                                   [&end_point_ph](data_type const* en_ph){ return en_ph == end_point_ph;  } );
        
        if( end_point_i != end_point_mc.end() ){
            auto module_index = std::distance( end_point_mc.begin(), end_point_i );
            reconstruction_module_mc[module_index].reconstructed_o = aftereffect::optional<reconstructed_track>{ std::move(reconstructed_p) };
            
            action_m.logger_m << "corresponding_mc_id: ";
            auto index_c = reconstruction_module_mc[module_index].reconstructible_o.value().get_indices();
            for( auto index : index_c ){ action_m.logger_m << index << " "; }
            action_m.logger_m << "\n";
        }
        else{
            reconstruction_module_mc.emplace_back(
                                        aftereffect::optional<reconstructible_track>{  },
                                        aftereffect::optional<reconstructed_track>{ std::move(reconstructed_p) }
                                                  );
            end_point_mc.push_back( end_point_ph);
        }
    }


public:
    //will need a reset method
    void clear(){
        reconstruction_module_mc.clear();
        end_point_mc.clear();
    }
    
    std::vector<reconstruction_module> const& retrieve_results() const {
        return reconstruction_module_mc;
    }
    
    void end_event() {
        Writer<TATOEmatcher<Action, Writer>>::write();
        reconstruction_module_mc.clear();
        end_point_mc.clear();
    }
    
    
    reconstructible_track get_reconstructible_track( data_type const* end_point_ph ) const {
        for(auto i{0}; i < end_point_mc.size() ; ++i){
            if( end_point_mc[i] == end_point_ph ){ return reconstruction_module_mc[i].reconstructible_o.value(); }
        }
        return reconstructible_track{};
    }
    
};


template<class Action>
struct empty_matcher{
    using candidate = typename std::decay_t<decltype( std::declval<typename Action::detector_list_t>().last() )>::candidate;
    using track = typename Action::track;
    using node_type = typename Action::node_type;
    using data_type = typename Action::data_type;
    
    std::vector<reconstruction_module> reconstruction_module_mc;
    
    void submit_reconstructed_track(track const& ){}
    void submit_reconstructible_track(candidate const& ){}
    void generate_candidate_indices(){}
    std::vector<reconstruction_module> const& retrieve_results() const{ return reconstruction_module_mc; }
    void clear(){}
    void end_event() {}
    reconstructible_track get_reconstructible_track( data_type const* end_point_ph ) const { return {}; }
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
        virtual std::vector<reconstruction_module> const& retrieve_results() const = 0;
        virtual void clear() =0;
        virtual void end_event()  =0;
        virtual reconstructible_track get_reconstructible_track( data_type const* end_point_ph ) const =0;
    };

    template<class T>
    struct holder : eraser {
        constexpr holder() = default;
        constexpr holder(T t) : t_m{ std::move(t)} {}
        void submit_reconstructed_track(track const& track_p) override{ t_m.submit_reconstructed_track( track_p); }
        void submit_reconstructible_track(candidate const& candidate_p) override{ t_m.submit_reconstructible_track( candidate_p); }
        void generate_candidate_indices() override{ t_m.generate_candidate_indices();}
        std::vector<reconstruction_module> const& retrieve_results() const override{ return t_m.retrieve_results(); }
        void clear() override { t_m.clear(); }
        void end_event() override { t_m.end_event(); }
        reconstructible_track get_reconstructible_track( data_type const* end_point_ph ) const override { return t_m.get_reconstructible_track( end_point_ph); }
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
    std::vector<reconstruction_module> const& retrieve_results() const { return erased_m->retrieve_results(); }
    void clear(){ erased_m->clear(); }
    void end_event() { erased_m->end_event(); }
    reconstructible_track get_reconstructible_track( data_type const* end_point_ph ) const { return erased_m->get_reconstructible_track(end_point_ph); }
    
    private:
    std::unique_ptr<eraser> erased_m;
}; 


#endif
