#ifndef _TAPWntuHit_HXX
#define _TAPWntuHit_HXX
/*!
  \file
  \version $Id: TAPWntuHit.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TAPWntuHit.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;

#include "TObject.h"
#include "TClonesArray.h"
#include "TAGdata.hxx"
#include "TAGbaseWC.hxx"
#include "TAGwaveCatcher.hxx"
//

/**
 * This class stores the params of a single channel waveform
 */
class TAPWhit : public TAGbaseWC
{
   
public:
  TAPWhit();
  TAPWhit(TAGwaveCatcher *w, Bool_t pw = true);
  virtual         ~TAPWhit();
  
  ClassDef(TAPWhit,1);
  //
};

//##############################################################################

class TAPWntuHit : public TAGdata {
public:

  TAPWntuHit();
  virtual         ~TAPWntuHit();

  TAPWhit*          GetHit();
  const TAPWhit*    GetHit() const;
  
  TAPWhit*          NewHit(TAGwaveCatcher *W, Bool_t pwFlag = false);
  TAPWhit*          NewHit();

  void              SetupClones();

  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
   TAPWhit*         fHit;         // hits

private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TAPWntuHit,1);
};

#endif
