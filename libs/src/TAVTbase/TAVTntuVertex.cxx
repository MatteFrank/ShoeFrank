/*!
 \file TAVTntuVertex.cxx
 \brief   Class for VTX vertices
 */

#include "TMath.h"
#include "TAGgeoTrafo.hxx" 
#include "TAVTparGeo.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"


//################################################################

/*!
 \class TAVTvertex
 \brief Class for VTX vertices
 */

ClassImp(TAVTvertex) // Description of a vertex

//______________________________________________________________________________
//! Constructor
TAVTvertex::TAVTvertex()
:  TAGobject(),
   fListOfTracks(new TClonesArray("TAVTtrack")),
   fIsValid(-1),
   fIsBmMatched(false),
   fDistanceMin(-1)
{
    fListOfTracks->SetOwner(true);
    fVertexPosition.SetXYZ(0,0,0);
    fVertexPosError.SetXYZ(0,0,0);
}

//______________________________________________________________________________
//! Copy constructor
//!
//! \param[in] aVertex vertex to copy
TAVTvertex::TAVTvertex(const TAVTvertex& aVertex)
:  TAGobject(aVertex),
   fVertexPosition(aVertex.fVertexPosition),
   fVertexPosError(aVertex.fVertexPosError),
   fIsValid(aVertex.fIsValid),
   fIsBmMatched(aVertex.fIsBmMatched),
   fDistanceMin(aVertex.fDistanceMin)
{
    fListOfTracks = (TClonesArray*)aVertex.fListOfTracks->Clone();
}

//______________________________________________________________________________
//! Destructor
TAVTvertex::~TAVTvertex()
{
    delete fListOfTracks;
}

//______________________________________________________________________________
//! Add track to list
//!
//! \param[in] track track to add
void TAVTvertex::AddTrack(TAVTtrack* track)
{
    TClonesArray &trackArray = *fListOfTracks;
    new(trackArray[trackArray.GetEntriesFast()]) TAVTtrack(*track);
}

//____________________________________________________________________________
//! Clear list
void TAVTvertex::Reset()
{
    fListOfTracks->Delete();
    fVertexPosition.SetXYZ(0,0,0);
}

//##############################################################################

/*!
 \class TAVTntuVertex
 \brief Class for VTX vertices containers
 */


ClassImp(TAVTntuVertex);

TString TAVTntuVertex::fgkBranchName   = "vtvtx.";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTntuVertex::TAVTntuVertex() 
: TAGdata(),
fListOfVertex(new TClonesArray("TAVTvertex"))
{
    
    fListOfVertex->SetOwner(true);
}
//------------------------------------------+-----------------------------------
//! Destructor.
TAVTntuVertex::~TAVTntuVertex() 
{
    delete fListOfVertex;
}

//------------------------------------------+-----------------------------------
//! return the number of vertex
Int_t TAVTntuVertex::GetVertexN() const
{
    return fListOfVertex->GetEntries();
}

//------------------------------------------+-----------------------------------
//!  return vertex for a given sensor
//!
//! \param[in] sensorId sensor index
TAVTvertex* TAVTntuVertex::GetVertex(Int_t sensorId)
{
    if(sensorId >= 0 || sensorId < GetVertexN())
        return (TAVTvertex*)fListOfVertex->At(sensorId);
    else
        return 0x0;
}

//------------------------------------------+-----------------------------------
//!  return vertex for a given sensor (const)
//!
//! \param[in] sensorId sensor index
const TAVTvertex* TAVTntuVertex::GetVertex(Int_t sensorId) const
{
    if(sensorId >= 0 || sensorId < GetVertexN())
        return (TAVTvertex*)fListOfVertex->At(sensorId);
    else
        return 0x0;
}

//------------------------------------------+-----------------------------------
//! clear
void TAVTntuVertex::Clear(Option_t*)
{
    fListOfVertex->Delete();
}

//______________________________________________________________________________
//! Create a new vertex
TAVTvertex* TAVTntuVertex::NewVertex()
{
    TClonesArray &vtxArray = *fListOfVertex;
    TAVTvertex* vertex = new(vtxArray[vtxArray.GetEntriesFast()]) TAVTvertex();
    return vertex;
}

//______________________________________________________________________________
//! Create a new vertex with copy constructor
//!
//! \param[in] vtx vertex to copy
TAVTvertex* TAVTntuVertex::NewVertex(TAVTvertex& vtx)
{
    TClonesArray &vtxArray = *fListOfVertex;
    TAVTvertex* vertex = new(vtxArray[vtxArray.GetEntriesFast()]) TAVTvertex(vtx);
    return vertex;
}

//----------------------------------------------------------------------------
//! Set up clones
void TAVTntuVertex::SetupClones()
{
    if (!fListOfVertex) {
        fListOfVertex = new TClonesArray("TAVTvertex");
        fListOfVertex->SetOwner(true);
    }
}
