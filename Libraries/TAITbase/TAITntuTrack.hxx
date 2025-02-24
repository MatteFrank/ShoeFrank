#ifndef _TAITntuTrack_HXX
#define _TAITntuTrack_HXX

/*!
 \file TAITntuTrack.hxx
 \brief   Declaration of TAITntuTrack.
 */
/*------------------------------------------+---------------------------------*/

// ROOT classes
#include "TClonesArray.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TArrayF.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAITntuCluster.hxx"
#include "TAITtrack.hxx"


//##############################################################################

class TAITntuTrack : public TAGdata {
   
private:
   TClonesArray*    fListOfTracks;	///< tracks
   TVector3         fBeamPosition;  ///< Position of beam in x-y direction at z-target given by BM
   Bool_t           fPileup;        ///< true if pileup events
   
public:
   TAITntuTrack();
   virtual         ~TAITntuTrack();
   
   // Get track
   TAITtrack*       GetTrack(Int_t i);
   // Get track (const)
   const TAITtrack* GetTrack(Int_t i) const;
   // Get number of tracks
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
   TClonesArray*    GetListOfTracks()               { return fListOfTracks; }

   // Create new track
   TAITtrack*       NewTrack();
   // Create new track from existing one
   TAITtrack*       NewTrack(TAITtrack& track);
   
   // Set up clones
   virtual void     SetupClones();
   // Clear
   virtual void     Clear(Option_t* opt="");
   // To stream
   virtual void     ToStream(ostream& os=cout, Option_t* option="") const;
   
   ClassDef(TAITntuTrack,2)
};

#endif

