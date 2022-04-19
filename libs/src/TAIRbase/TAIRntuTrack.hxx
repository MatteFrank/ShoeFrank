#ifndef _TAIRntuTrack_HXX
#define _TAIRntuTrack_HXX

/*!
 \file TAIRntuTrack.hxx
 \brief Simple container class for tracks with the associated clusters
 \author Ch. Finck
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
#include "TAIRtrack.hxx"


//##############################################################################

class TAIRntuTrack : public TAGdata {
   
private:
   TClonesArray*    fListOfTracks;		///< list of IR tracks
   
private:
   static TString   fgkBranchName;    ///< Branch name in TTree
   
public:
   TAIRntuTrack();
   virtual         ~TAIRntuTrack();
   
   // Get track
   TAIRtrack*       GetTrack(Int_t i);
   // Get track
   const TAIRtrack* GetTrack(Int_t i) const;
   // Get number of tracks
   Int_t            GetTracksN()      const; 
   
   //! Get list of tracks
   TClonesArray*    GetListOfTracks() { return fListOfTracks; }

   // New track
   TAIRtrack*       NewTrack();
   // New track
   TAIRtrack*       NewTrack(TAIRtrack& track);
      
   virtual void     SetupClones();
   virtual void     Clear(Option_t* opt="");
   
   virtual void     ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   //! Get branch name
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }  
   
   ClassDef(TAIRntuTrack,1)
};

#endif

