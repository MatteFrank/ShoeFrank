#ifndef _TAMSDntuTrack_HXX
#define _TAMSDntuTrack_HXX

/*!
 \file TAMSDntuTrack.hxx
 \brief   Declaration of TAMSDntuTrack.
 */
/*------------------------------------------+---------------------------------*/


// ROOT classes
#include "TClonesArray.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TArrayF.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAMSDntuCluster.hxx"
#include "TAMSDtrack.hxx"


//##############################################################################

class TAMSDntuTrack : public TAGdata {
   
private:
   TClonesArray*    fListOfTracks;	///< List of  tracks
   TVector3         fBeamPosition;  ///< Position of beam in x-y direction at z-target given by BM
   Bool_t           fPileup;        ///< true if pileup events
   
public:
   TAMSDntuTrack();
   virtual         ~TAMSDntuTrack();
   
   TAMSDtrack*       GetTrack(Int_t i);
   const TAMSDtrack* GetTrack(Int_t i) const;
   Int_t             GetTracksN()      const;
   
   //! Get beam position
   TVector3         GetBeamPosition()  const        { return fBeamPosition; }
   //! Set beam position
   void             SetBeamPosition(TVector3& pos)  { fBeamPosition = pos;  }
   //! Is pileup
   Bool_t           IsPileUp()         const        { return   fPileup;     }
   //! Get pileup
   void             SetPileUp(Bool_t pileup = true) { fPileup = pileup;     }
   //! Get list of tracks
   TClonesArray*    GetListOfTracks()               { return fListOfTracks; }

   TAMSDtrack*       NewTrack();
   TAMSDtrack*       NewTrack(TAMSDtrack& track);
      
   virtual void      SetupClones();
   virtual void      Clear(Option_t* opt="");
   virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
   ClassDef(TAMSDntuTrack,2)
};

#endif

