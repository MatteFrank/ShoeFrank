#ifndef _TANEntuRaw_HXX
#define _TANEntuRaw_HXX
/*!
  \file TANEntuRaw.hxx
  \brief   Declaration of TANEntuRaw.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;
#include "TObject.h"
#include "TClonesArray.h"
#include "TAGdata.hxx"
#include "TAGbaseWD.hxx"


/*!
 \class TANErawHit
 \brief class for raw hit.
 */

class TANErawHit : public TAGbaseWD {

public:
   TANErawHit();
   TANErawHit(TWaveformContainer *w);
   virtual         ~TANErawHit();

   ClassDef(TANErawHit, 3);

};

//##############################################################################
/*!
 \class TANEntuRaw
 \brief Container for raw hit.
 */

class TANEntuRaw : public TAGdata {
public:

  TANEntuRaw();
  virtual         ~TANEntuRaw();

  Int_t             GetHitsN() const;
  TANErawHit*       GetHit(Int_t i_ind);
  const TANErawHit* GetHit(Int_t i_ind) const;
  
  void              NewHit(TWaveformContainer *W);
  void              SetupClones();
  
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;

public:
   //! Get branch name
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
private:
   TClonesArray*   fListOfHits;     ///< list of  hits

private:
   static TString fgkBranchName;    ///< Branch name in TTree
  
  ClassDef(TANEntuRaw,1);

};

#endif
