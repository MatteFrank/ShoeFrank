#ifndef _TAIRntuTrack_HXX
#define _TAIRntuTrack_HXX

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
   TClonesArray*    fListOfTracks;		// tracks
   
private:
   static TString fgkBranchName;    // Branch name in TTree
   
public:
   TAIRntuTrack();
   virtual         ~TAIRntuTrack();
   
   TAIRtrack*       GetTrack(Int_t i);
   const TAIRtrack* GetTrack(Int_t i) const;
   Int_t            GetTracksN()      const; 
   
   TClonesArray*    GetListOfTracks() { return fListOfTracks; }

   TAIRtrack*       NewTrack();
   TAIRtrack*       NewTrack(TAIRtrack& track);
      
   virtual void     SetupClones();
   virtual void     Clear(Option_t* opt="");
   
   
   virtual void     ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }  
   
   ClassDef(TAIRntuTrack,2)
};

#endif

