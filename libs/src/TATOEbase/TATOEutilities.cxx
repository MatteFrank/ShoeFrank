///////////////////////////////////////////////////////////
//                                                        //
// Class Description of TATOEutilities                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TATOEutilities.hxx"

#include "TAVTntuCluster.hxx"
#include "TAVTntuVertex.hxx"


#include "TAITntuCluster.hxx"

//______________________________________________________________________________
//                          VTX

std::vector<typename details::vertex_tag::candidate> detector_properties< details::vertex_tag >::generate_candidates(std::size_t index_p) const
{
    std::vector<candidate> candidate_c;
    std::size_t entries = cluster_mhc->GetClustersN(index_p);
    candidate_c.reserve( entries );
    std::cout << "detector_properties<details::vertex_tag>::generate_candidate : " << entries << '\n';
    
    for(std::size_t i{0}; i < entries ; ++i)
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        
        
        auto cluster_h = cluster_mhc->GetCluster(index_p, i);
        auto position =  transformation_h->FromVTLocalToGlobal(cluster_h->GetPositionG());
        auto error = cluster_h->GetPosError();
        
        
        candidate_c.emplace_back( measurement_vector{{ position.X(), position.Y() }},
                                 measurement_covariance{{ error.X(),         0,
                                                                    0, error.Y()    }},
                                 measurement_matrix{matrix_m},
                                 data_handle<data_type>{cluster_h} );
        
        
    }
    
    return candidate_c;
}

const TAVTvertex * detector_properties< details::vertex_tag >::vertex_handle() const
{
    //if(vertex_mhc->GetVertexN()>1){std::cout << "More than one vertex: "<< vertex_mhc->GetVertexN() <<'\n';}
    
    return vertex_mhc->GetVertex(0);
}


//______________________________________________________________________________
//                     IT

std::vector<typename details::it_tag::candidate> detector_properties< details::it_tag >::generate_candidates(std::size_t index_p) const
{
    std::vector<candidate> candidate_c;
    std::size_t entries = cluster_mhc->GetClustersN(index_p);
    candidate_c.reserve( entries );
    std::cout << "detector_properties<details::it_tag>::generate_candidate : " << entries << '\n';
    
    for(std::size_t i{0}; i < entries ; ++i)
    {
        auto * transformation_h = static_cast<TAGgeoTrafo*>( gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data()));
        
        
        
        auto cluster_h = cluster_mhc->GetCluster(index_p, i);
        auto position =  transformation_h->FromITLocalToGlobal(cluster_h->GetPositionG());
        auto error = cluster_h->GetPosError();
        
        
        candidate_c.emplace_back( measurement_vector{{ position.X(), position.Y() }},
                                 measurement_covariance{{ error.X(),         0,
                                                                  0, error.Y()    }},
                                 measurement_matrix{matrix_m},
                                 data_handle<data_type>{cluster_h} );
        
        
    }
    
    return candidate_c;
}
