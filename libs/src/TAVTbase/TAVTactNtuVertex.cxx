/*!
 \file TAVTactNtuVertex.cxx
 \brief   NTuplizer for VTX vertices
 */

#include "TClonesArray.h"
#include "TH2F.h"
#include "TMath.h"

#include "TAGparGeo.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"

#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTactNtuVertex.hxx"


/*!
 \class TAVTactNtuVertex
 \brief NTuplizer for VTX vertices
 */

//! Class Imp
ClassImp(TAVTactNtuVertex);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuTrack track container descriptor
//! \param[out] pNtuVertex vertex container descriptor
//! \param[in] pConfig configuration parameter descriptor
//! \param[in] pGeoMap geometry parameter descriptor
//! \param[in] pGeoMapG target geometry parameter descriptor
//! \param[in] pBmTrack input BM track container descriptor
TAVTactNtuVertex::TAVTactNtuVertex(const char* name, 
                                       TAGdataDsc* pNtuTrack, TAGdataDsc* pNtuVertex,
                                       TAGparaDsc* pConfig, TAGparaDsc* pGeoMap, TAGparaDsc* pGeoMapG, TAGdataDsc* pBmTrack)
: TAVTactBaseNtuVertex(name, pNtuTrack, pNtuVertex, pConfig, pGeoMap, pGeoMapG, pBmTrack)
{
    fEps  = 5e-8;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactNtuVertex::~TAVTactNtuVertex()
{    
}

//______________________________________________________________________
//! Compute vertex
Bool_t TAVTactNtuVertex::ComputeVertex()
{
    
   Double_t izMin = fMinZ;
   Double_t izMax = fMaxZ;
   
   
   Double_t lowerZdistance = 0;
   Double_t higherZdistance = 0;
   Double_t minZdistance = 0 ; 
   
   Double_t x_coordinate = 0;
   Double_t y_coordinate = 0;
   Double_t z_coordinate = 0;
   
   do {
	  lowerZdistance  = ComputeDistance(izMin);
	  higherZdistance = ComputeDistance(izMax);
      if(FootDebugLevel(1)) {
		 cout<<"izMin "<<izMin<<" distance @ low Z value "<<lowerZdistance<<" izMax "<<izMax<<" distance @ high Z value "<<higherZdistance<<endl;
		 cout<<"distance "<<lowerZdistance-higherZdistance<<endl;
	  }
	  
	  if (lowerZdistance < higherZdistance){ //siamo nella prima metá
		 izMax = (izMin + izMax)/2.;
		 minZdistance = lowerZdistance;
		 
	  } else if(lowerZdistance > higherZdistance){ 
		 izMin = (izMin + izMax)/2.;//simao nella seconda metá
		 minZdistance = higherZdistance;
	  }
	  
	  
   } while(TMath::Abs(lowerZdistance-higherZdistance) >fEps );
   
   
   z_coordinate = (izMin+izMax)/2.;
   x_coordinate = ComputeFinalCoordinate(0, z_coordinate);
   y_coordinate = ComputeFinalCoordinate(1, z_coordinate);
   
   if(FootDebugLevel(1)) {
	  cout<<"Distance: "<<minZdistance<<endl;
	  cout<<"Vertex coordinate: "<<x_coordinate<<" "<<y_coordinate<<" "<<z_coordinate<<" micron "<<endl;
   }
   
   TVector3 vtxCoord;
   vtxCoord.SetXYZ(x_coordinate, y_coordinate, z_coordinate);
   SetVertex(vtxCoord, minZdistance);
   
   return true;
}

//______________________________________________________________________
//! Comute mean distance between the tracks for a given z position
//!
//! \param[in] zVal Z position in target
Double_t TAVTactNtuVertex::ComputeDistance(Double_t zVal)
{
    TAVTntuTrack* ntuTrack = (TAVTntuTrack*)fpNtuTrack->Object();
    Int_t nTracks          = ntuTrack->GetTracksN();
    
    Double_t distance = 0;
    Double_t meanX    = 0;
    Double_t meanY    = 0;
    
    for(Int_t q = 0; q<nTracks; ++q ) {
        TAVTtrack* track0 = ntuTrack->GetTrack(q);
        meanX += track0->GetPoint(zVal).X();
        meanY += track0->GetPoint(zVal).Y();
    }
   
    meanX /= float(nTracks);
    meanY /= float(nTracks);
   
    for(Int_t qq = 0; qq < nTracks; ++qq) {
        TAVTtrack* track0 = ntuTrack->GetTrack(qq);
        distance += TMath::Sqrt( (track0->GetPoint(zVal).X() - meanX)*(track0->GetPoint(zVal).X() - meanX)
								+ (track0->GetPoint(zVal).Y() - meanY)*(track0->GetPoint(zVal).Y() - meanY) );
    }
    
    return distance;
}

//______________________________________________________________________
//! Comute final distance between the tracks for a given z position
//!
//! \param[in] zVal Z position in target
//! \param[in] a toggle between X and Y position (0, 1)
Double_t TAVTactNtuVertex::ComputeFinalCoordinate(Int_t a, Double_t zVal)
{
    Double_t returnValue   = 0;
    TAVTntuTrack* ntuTrack = (TAVTntuTrack*)fpNtuTrack->Object();
    Int_t nTracks          = ntuTrack->GetTracksN();
   
    for(Int_t q = 0; q<nTracks; ++q ){
        TAVTtrack* track0 = ntuTrack->GetTrack(q);
        if(a ==0)
            returnValue += track0->GetPoint(zVal).X();
        if(a == 1)
            returnValue += track0->GetPoint(zVal).Y();
    }
    returnValue /= float(nTracks);
   
    return returnValue;
}

//______________________________________________________________________
//! Set vertex position and minimal distance between tracks
//!
//! \param[in] xyz  position of vertex
//! \param[in] mind minimal distance between tracks
void TAVTactNtuVertex::SetVertex(TVector3& xyz, Double_t mind)
{
    TAVTntuVertex* ntuVertex = (TAVTntuVertex*)fpNtuVertex->Object();
    TAVTvertex* vtx = new TAVTvertex();
    vtx->SetVertexValidity(1);
    vtx->SetVertexPosition(xyz);
    vtx->SetDistanceMin(mind);
    
   if (ValidHistogram()) {
	  fpHisPosZ->Fill(xyz.Z());
	  fpHisPosXY->Fill(xyz.X(), xyz.Y());
   }
   
    TAVTntuTrack* ntuTrack = (TAVTntuTrack*)fpNtuTrack->Object();
    Int_t nTracks = ntuTrack->GetTracksN();
    for(Int_t q = 0; q < nTracks; ++q ){
        TAVTtrack* track0 = ntuTrack->GetTrack(q);
        vtx->AddTrack(track0);
    }
   
    ntuVertex->NewVertex(*vtx);

    delete vtx;
}
