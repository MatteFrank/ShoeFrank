///////////////////////////////////////////////////////////
//                                                        //
// Class Description of TATOEutilities                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TATOEutilities.hxx"


#include "TAVTntuVertex.hxx"


#include "TAITntuCluster.hxx"

//______________________________________________________________________________
//                          VTX

typename details::vertex_tag::candidate detector_properties< details::vertex_tag >::generate_candidate( TAVTbaseCluster const * cluster_ph) const
{
    auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction( TAGgeoTrafo::GetDefaultActName().Data()) );
    auto position  = transformation_h->FromVTLocalToGlobal( cluster_ph->GetPositionG() );
    auto error     = cluster_ph->GetPosError();
    
    
    return candidate{
        measurement_vector{{ position.X(), position.Y() }},
        measurement_covariance{{ pow(error.X(), 2),                0,
                                                 0, pow(error.Y(), 2)    }},
        measurement_matrix{matrix_m},
        data_handle<data_type>{ cluster_ph }
    };
}



const TAVTvertex * detector_properties< details::vertex_tag >::retrieve_vertex( ) const
{
    for( std::size_t i{0} ; i < vertex_mhc->GetVertexN() ; ++i ){
        auto vertex_h = vertex_mhc->GetVertex(i);
        
        if( vertex_h->IsBmMatched() ){ return vertex_h; }
    }
    
    return nullptr;
}


detector_properties< details::vertex_tag >::track_list::pseudo_layer
detector_properties< details::vertex_tag >::track_list::iterable_track::form_layer( std::size_t index_p ) const
{
    //cluster are in inverse order
    std::size_t real_index = track_mh->GetClustersN() -1 -index_p;
    
    auto* cluster_h = track_mh->GetCluster( real_index );
    auto c = detector_m.generate_candidate( cluster_h );
    
    return pseudo_layer{
                std::move(c),
                detector_m.layer_depth( cluster_h->GetPlaneNumber() ),
                detector_m.cut_value()
                        };
}

    
std::vector< detector_properties< details::vertex_tag >::track_list::iterable_track >
detector_properties< details::vertex_tag >::track_list::form_tracks( TAVTvertex const * vertex_ph ) const
{
    using track = detector_properties< details::vertex_tag >::track_list::iterable_track;
    std::vector< track > track_c;
    track_c.reserve( vertex_ph->GetTracksN() );
    for( auto i = 0 ; i < vertex_ph->GetTracksN() ; ++i ){
        track_c.emplace_back( detector_m, vertex_ph->GetTrack(i) );
    }
    return track_c;
}


detector_properties< details::vertex_tag >::track_list detector_properties< details::vertex_tag >::get_track_list( TAVTvertex const * vertex_ph ) const
{
    return { *this, vertex_ph };
}



//______________________________________________________________________________
//                     IT

std::vector<typename details::it_tag::candidate> detector_properties< details::it_tag >::generate_candidates(std::size_t index_p) const
{
    std::vector<candidate> candidate_c;
    candidate_c.reserve( 8*5 );
    
    auto sensorID_c = plane_mc[index_p];
    for( auto id : sensorID_c ) {
       
        
//        std::cout << "detector_properties<details::it_tag>::generate_candidate : sensor " << id << '\n';
        std::size_t entries = cluster_mhc->GetClustersN(id);
       
        
//        std::cout << "detector_properties<details::it_tag>::generate_candidate : entries " << entries << '\n';
        
        for(std::size_t i{0}; i < entries ; ++i)
        {
            auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
            
            
            
            auto cluster_h = cluster_mhc->GetCluster(id, i);
            auto position =  transformation_h->FromITLocalToGlobal(cluster_h->GetPositionG());
            auto error = cluster_h->GetPosError();
            
            
            candidate_c.emplace_back( measurement_vector{{ position.X(), position.Y() }},
                                     measurement_covariance{{ pow(error.X(), 2),         0,
                0, pow(error.Y(), 2 )    }},
                                     measurement_matrix{matrix_m},
                                     data_handle<data_type>{cluster_h} );
            
            
        }
    }
    
    return candidate_c;
}
