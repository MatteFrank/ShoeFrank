#ifndef _TABMntuTrack_HXX
#define _TABMntuTrack_HXX
/*!
  \file
  \version $Id: TABMntuTrack.hxx,v 1.9 2003/07/07 18:42:17 mueller Exp $
  \brief   Declaration of TABMntuTrack.
*/

/*------------------------------------------+---------------------------------*/

#include "TAGroot.hxx"
#include "TAGdata.hxx"
#include "TABMtrack.hxx"

#include "TObject.h"
#include "TClonesArray.h"

//##############################################################################

class TABMntuTrack : public TAGdata {
  public:
                    TABMntuTrack();
    virtual         ~TABMntuTrack();

    Int_t            GetTracksN()  const{return fListOfTracks->GetEntries();};
    TABMtrack*       GetTrack(Int_t i) {return (GetTracksN()) ? (TABMtrack*) ((*fListOfTracks)[i]) : 0x0;};
    const TABMtrack* GetTrack(Int_t i) const{return (GetTracksN()) ? (const TABMtrack*) ((*fListOfTracks)[i]) : 0x0;};
    TABMtrack*       GetPrunedTrack() {return (fPrunedTrack->GetEntries()) ? (TABMtrack*) ((*fPrunedTrack)[0]) : 0x0;};
    Int_t            GetPrunedStatus() {return fPrunedStatus;};
    Int_t            GetTrackStatus(){return fStatus;};

    virtual void    SetupClones();
    virtual void    Clear(Option_t* opt="");
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    TClonesArray*   GetListOfTracks() { return fListOfTracks; }
    TABMtrack*      NewTrack(TABMtrack trk);
    TABMtrack*      NewPrunedTrack(TABMtrack trk, Int_t view);
    void            SetTrackStatus(Int_t instatus){fStatus=instatus;return;};

    static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

  private:
    Int_t           fStatus;                //-1000=notset, 0=ok, 1=firedUplane<plane_mincut, 2=firedVplane<plane_mincut, 3=hit rejected > rejmax_cut, 4=fit is not converged, 5=chi2red>chi2redcut, >6=error

    TClonesArray*   fPrunedTrack;          //list of pruned tracks
    Int_t           fPrunedStatus;          //-1=no pruned track, 0=pruned track on the yz view (view=0), 1= pruned track on the xz view (view=1)
    TClonesArray*   fListOfTracks;			    // list of saved tracks

   static TString fgkBranchName;            // Branch name in TTree

   ClassDef(TABMntuTrack,2)

};

#endif
