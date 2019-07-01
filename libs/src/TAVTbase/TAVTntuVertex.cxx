
#include "TMath.h"
#include "TAGgeoTrafo.hxx" 
#include "TAVTparGeo.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuVertex.hxx"


//################################################################


ClassImp(TAVTvertex) // Description of a vertex

//______________________________________________________________________________
//  
TAVTvertex::TAVTvertex()
:  TAGobject(),
   fListOfTracks(new TClonesArray("TAVTtrack")),
   fIsValid(-1),
   fIsBmMatched(false),
   fDistanceMin(-1)
{
    fListOfTracks->SetOwner(true);
    fVertexPosition.SetXYZ(0,0,0);
}

//______________________________________________________________________________
//  
TAVTvertex::TAVTvertex(const TAVTvertex& aVertex)
:  TAGobject(aVertex),
   fVertexPosition(aVertex.fVertexPosition),
   fIsValid(aVertex.fIsValid),
   fIsBmMatched(aVertex.fIsBmMatched),
   fDistanceMin(aVertex.fDistanceMin)
{
    fListOfTracks = (TClonesArray*)aVertex.fListOfTracks->Clone();
}

//______________________________________________________________________________
//  
TAVTvertex::~TAVTvertex()
{
    delete fListOfTracks;
}

//______________________________________________________________________________
//
void TAVTvertex::AddTrack(TAVTtrack* track)
{
    TClonesArray &trackArray = *fListOfTracks;
    new(trackArray[trackArray.GetEntriesFast()]) TAVTtrack(*track);
}

//____________________________________________________________________________
//
void TAVTvertex::Reset()
{
    fListOfTracks->Delete();
    fVertexPosition.SetXYZ(0,0,0);
}

//##############################################################################

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
//!return the number of vertex
Int_t TAVTntuVertex::GetVertexN() const
{
    return fListOfVertex->GetEntries();
}

//------------------------------------------+-----------------------------------
//! return vertex for a given sensor
TAVTvertex* TAVTntuVertex::GetVertex(Int_t i)
{
    if(i>=0 || i< GetVertexN())
        return (TAVTvertex*)fListOfVertex->At(i);
    else
        return 0x0;
   
}

//------------------------------------------+-----------------------------------
//! return vertex for a given sensor
const TAVTvertex* TAVTntuVertex::GetVertex(Int_t i) const
{
    if(i>=0 || i< GetVertexN())
        return (TAVTvertex*)fListOfVertex->At(i);
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
//
TAVTvertex* TAVTntuVertex::NewVertex()
{
    TClonesArray &vtxArray = *fListOfVertex;
    TAVTvertex* vertex = new(vtxArray[vtxArray.GetEntriesFast()]) TAVTvertex();
    return vertex;
}

//______________________________________________________________________________
//  
TAVTvertex* TAVTntuVertex::NewVertex(TAVTvertex& vtx)
{
    TClonesArray &vtxArray = *fListOfVertex;
    TAVTvertex* vertex = new(vtxArray[vtxArray.GetEntriesFast()]) TAVTvertex(vtx);
    return vertex;
}

//----------------------------------------------------------------------------
void TAVTntuVertex::SetupClones()
{
    if (!fListOfVertex) {
        fListOfVertex = new TClonesArray("TAVTvertex");
        fListOfVertex->SetOwner(true);
    }
}
