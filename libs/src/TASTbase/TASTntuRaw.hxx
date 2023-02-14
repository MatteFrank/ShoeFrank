#ifndef _TASTntuRaw_HXX
#define _TASTntuRaw_HXX
/*!
  \file TASTntuRaw.hxx
  \brief   Declaration of TASTntuRaw.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;
#include "TObject.h"
#include "TClonesArray.h"
#include "TAGdata.hxx"
#include "TAGbaseWD.hxx"
#include "TFile.h"
//

/*!
 \class TASThit
 \brief This class stores the params of a single channel waveform
 */
class TASTrawHit : public TAGbaseWD {

public:
  TASTrawHit();
  TASTrawHit(TWaveformContainer *w, string algo, double frac, double del);
  virtual         ~TASTrawHit();
    
  ClassDef(TASTrawHit,2);
  //
  virtual Bool_t		CheckForPileUp(TWaveformContainer *w, Int_t event=-1);

  Bool_t GetPileUp()          const { return fPileUp;    }
  //void SetPileUp(bool b)  { fPileUp = b;    }   //  set happens in CheckForPileUp

private:
  bool fPileUp;
};

//##############################################################################

/*!
 \class TASTntuRaw
 \brief Raw hit container for ST
 */

class TASTntuRaw : public TAGdata {
public:

  TASTntuRaw();
  virtual         ~TASTntuRaw();

  Int_t             GetHitsN() {return fHitsN;}

  TASTrawHit*       GetHit(Int_t i_ind);
  const TASTrawHit* GetHit(Int_t i_ind) const;

  TASTrawHit*       GetSuperHit(){return fSuperHit;}
  
  void              NewHit(TWaveformContainer *W, string algo, double frac, double del);
  void              NewSuperHit(vector<TWaveformContainer*>, string algo, double frac, double del);
  void              SetupClones();

   
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;

  void              UpdateRunTime(int value){fRunTime+=value;}
   
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
    
  ClassDef(TASTntuRaw,3);
  
private:
   Int_t           fHitsN;          //
   TClonesArray*   fListOfHits;         // hits
   TASTrawHit*     fSuperHit;  //!
   Int_t           fRunTime;

   static TString fgkBranchName;    // Branch name in TTree
};

#endif
