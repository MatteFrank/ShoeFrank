#ifndef _TAVTntuTrack_HXX
#define _TAVTntuTrack_HXX

/*!
 \file TAVTntuTrack.hxx
 \version $Id: TAVTntuTrack
 \brief  class, simple container class for tracks with the associated clusters
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

// ROOT classes
#include "TClonesArray.h"
#include "TVector3.h"
#include "TVector2.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"

class TClonesArray;
class TAVTtrack;
class TAVTntuTrack : public TAGdata {
   
private:
   TClonesArray*    fListOfTracks;		/// tracks
   TVector3         fBeamPosition;     /// Position of beam in x-y direction at z-target given by BM
   Bool_t           fPileup;           /// true if pileup events
   
private:
   static TString fgkBranchName;      /// Branch name in TTree
   
public:
   TAVTntuTrack();
   virtual         ~TAVTntuTrack();
   
   //! Get track
   TAVTtrack*       GetTrack(Int_t i);
   //! Get track (const)
   TAVTtrack const * GetTrack(Int_t i) const;
   //! Get number of tracks
   Int_t            GetTracksN()      const; 
   
   //! Get beam position
   TVector3         GetBeamPosition() const         { return fBeamPosition; }
   //! Set beam position
   void             SetBeamPosition(TVector3& pos)  { fBeamPosition = pos;  }
   //! Get pileup
   Bool_t           IsPileUp()        const         { return   fPileup;     }
   //! Set pileup
   void             SetPileUp(Bool_t pileup = true) { fPileup = pileup;     }

   //! Get list of tracks
   TClonesArray*    GetListOfTracks() { return fListOfTracks; }

   //! Create new track
   TAVTtrack*       NewTrack();
   //! Create new track from existing one
   TAVTtrack*       NewTrack(TAVTtrack& track);
   
   //! Set up clones
   virtual void     SetupClones();
   //! Clear
   virtual void     Clear(Option_t* opt="");
   //! To stream
   virtual void     ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   //! Get branch
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }  
   
   ClassDef(TAVTntuTrack,2)
};

#endif

