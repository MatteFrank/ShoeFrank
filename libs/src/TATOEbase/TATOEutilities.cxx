///////////////////////////////////////////////////////////
//                                                        //
// Class Description of TATOEutilities                       //
//                                                        //
////////////////////////////////////////////////////////////

//
//File      : TATOEutilities.cpp
//Author    : Alexandre Sécher (alexandre.secher@iphc.cnrs.fr)
//Date      : 10/02/2020
//Framework : PhD thesis, CNRS/IPHC/DRHIM/Hadrontherapy, Strasbourg, France
//


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


std::vector<typename details::vertex_tag::candidate>
    detector_properties< details::vertex_tag >::generate_candidates(std::size_t index_p) const
{
    std::vector<candidate> candidate_c;
    std::size_t entries = cluster_mhc->GetClustersN( index_p );
    candidate_c.reserve( entries );
    for(std::size_t i{0}; i < entries ; ++i)
    {
        auto cluster_h = cluster_mhc->GetCluster(index_p, i);
        candidate_c.push_back( generate_candidate( cluster_h ) );
    }
    return candidate_c;
}


std::vector< TAVTvertex const *>
    detector_properties< details::vertex_tag >::retrieve_vertices( ) const

{
    std::vector< TAVTvertex const * > vertex_ch;
    vertex_ch.reserve( vertex_mhc->GetVertexN() );
    
    for( std::size_t i{0} ; i < vertex_mhc->GetVertexN() ; ++i ){
        vertex_ch.push_back( vertex_mhc->GetVertex(i) );
    }
    
    return vertex_ch;
}



std::vector< track_list< detector_properties< details::vertex_tag> >::iterable_track >
    detector_properties< details::vertex_tag >::form_tracks( std::vector< TAVTvertex const * > vertex_pch ) const
{
    using iterable_track = typename track_list< detector_properties >::iterable_track;
    
    std::vector< iterable_track > track_c;
    
    std::size_t size{0};
    for( auto const * vertex_h : vertex_pch ){
        size += vertex_h->GetTracksN();
    }
    track_c.reserve( size );

    
    auto fill = [this]( std::vector< iterable_track >& track_pc,
                    TAVTvertex const * vertex_h )
                {
                    for( auto i =0 ; i < vertex_h->GetTracksN() ; ++i ){
                        track_pc.emplace_back( *this, vertex_h, vertex_h->GetTrack(i) );
                    }
                };
    
    for( auto const * vertex_h : vertex_pch ){
        fill(track_c, vertex_h);
    }
    
    return track_c;
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


//______________________________________________________________________________
//                     MSD

std::vector<typename details::msd_tag::candidate> detector_properties< details::msd_tag >::generate_candidates(std::size_t index_p) const
{
    std::vector<candidate> candidate_c;
    candidate_c.reserve( 5 );
    
    std::size_t entries = cluster_mhc->GetClustersN(index_p);
    
    std::size_t view = view_mc[index_p];
    
    for(std::size_t i{0}; i < entries ; ++i) {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        auto cluster_h = cluster_mhc->GetCluster(index_p, i);
        auto position =  transformation_h->FromMSDLocalToGlobal(cluster_h->GetPositionG());
        auto error = cluster_h->GetPosError();
            
        
        candidate_c.emplace_back(
            measurement_vector{{ position( view ) }},
            measurement_covariance{{ pow(error( view ), 2) }},
            measurement_matrix{matrix_mc[view]},
            data_handle<data_type>{cluster_h}
                                );
    }
    
    return candidate_c;
}
