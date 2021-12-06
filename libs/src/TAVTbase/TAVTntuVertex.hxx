#ifndef _TAVTntuVertex_HXX
#define _TAVTntuVertex_HXX

/*!
 \file TAVTntuVertex.hxx
 \version $Id: TAVTvertex
 \brief   class, simple container class for vertices with the associated tracks
 \author Ch. Finck, R. Rescigno
 */
/*------------------------------------------+---------------------------------*/

// ROOT classes
#include "TClonesArray.h"
#include "TVector3.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAVTntuTrack.hxx"

class TAVTtrack;
class TClonesArray;
class TAVTvertex : public TAGobject {
    
private:
    TClonesArray*   fListOfTracks;      ///< list of track associated to the vertex
    TVector3        fVertexPosition;    ///< vertex position
    TVector3        fVertexPosError;    ///< vertex position error
    Int_t           fIsValid;           ///< is zero if not valid 1 if it has almost two tracks
    Bool_t          fIsBmMatched;       ///< Matched flag with BM
    Double_t        fDistanceMin;       ///< distance value between the vertex point and the track. Sum of distance of all tracks

public:
    TAVTvertex();                                 
    ~TAVTvertex();
    TAVTvertex(const TAVTvertex& aVertex);
    
    //! Get list of tracks
    TClonesArray*  GetListOfTracks()        const { return   fListOfTracks;                      }
    //! Get number of tracks
    Int_t          GetTracksN()             const { return   fListOfTracks->GetEntries();        }
    //! Get Tracks
    TAVTtrack*     GetTrack(Int_t index)          { return (TAVTtrack*)fListOfTracks->At(index); }
   //! Get Tracks (const)
    TAVTtrack const* GetTrack(Int_t index)  const { return (TAVTtrack*)fListOfTracks->At(index); }
    //! Add track
    void           AddTrack(TAVTtrack* track);
   
    //! Get Vertex Position
    const TVector3& GetVertexPosition()     const { return fVertexPosition; }

    //! Get Vertex Position Error
    const TVector3&  GetVertexPosError()    const { return fVertexPosError; }
  
    //! Get Vertex Position
    const TVector3& GetPosition()           const { return fVertexPosition; }
  
    //! Get Vertex Position Error
    const TVector3&  GetPosError()          const { return fVertexPosError; }
   
    //! Set Vertex Position
    void SetVertexPosition(TVector3& xyz)         { fVertexPosition = xyz;}

    //! Set Vertex Position Error
    void SetVertexPosError(TVector3& xyz)          { fVertexPosError = xyz;}

    //! Set flag isValid
    void  SetVertexValidity(Int_t q)              { fIsValid = q;        }
    //! Get flag Validity
    Int_t GetValidity()                     const { return fIsValid;     }
    
    //! Set Distance Min
    void SetDistanceMin(Double_t q)               { fDistanceMin = q;    }
    //! Getdistance Min
    Double_t GetDistanceMin()               const { return fDistanceMin; }
    
    //! Set BM matched flag
    void SetBmMatched(Bool_t flag = true)         { fIsBmMatched = flag; }
    //! Get BM matched flag
    Bool_t IsBmMatched()                    const { return fIsBmMatched; }
   
    //! Reset
    void Reset();
    
    ClassDef(TAVTvertex,1)                      // Describes TAVTvertex
};


//##############################################################################

class TAVTntuVertex : public TAGdata {

private:
   TClonesArray*       fListOfVertex;	 ///< list of vertice, return an object TAVTvertex

private:
   static TString      fgkBranchName;   ///< Branch name in TTree
   
public:
    TAVTntuVertex();
    virtual           ~TAVTntuVertex();
   
    //! Get vertex
    TAVTvertex*        GetVertex(Int_t i);
    //! Get vertex (const)
    const TAVTvertex*  GetVertex(Int_t i) const;
    //! Get number of vertices
    Int_t              GetVertexN()  const;
   
    //! Create new vertex
    TAVTvertex*        NewVertex();
    //! Creat new vertex from existing one
    TAVTvertex*        NewVertex(TAVTvertex& vertex);
   
    //! Set up clones
    virtual void       SetupClones();
    //! Clear
    virtual void       Clear(Option_t* opt="");
        
public:
    //! Get branch
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

    ClassDef(TAVTntuVertex,1)
};

#endif

