#ifndef _TAPLntuHit_HXX
#define _TAPLntuHit_HXX
/*!
  \file
  \version $Id: TAPLntuHit.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TAPLntuHit.
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
class TAPLhit : public TAGbaseWC
{
   
public:
  TAPLhit();
  TAPLhit(TAGwaveCatcher *w);
  virtual         ~TAPLhit();
  
  ClassDef(TAPLhit,1);
  //
};

//##############################################################################

class TAPLntuHit : public TAGdata {
public:

  TAPLntuHit();
  virtual         ~TAPLntuHit();

  TAPLhit*          GetHit();
  const TAPLhit*    GetHit() const;
  
  TAPLhit*          NewHit(TAGwaveCatcher *W);
  TAPLhit*          NewHit();

  void              SetupClones();

  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
   TAPLhit*         fHit;         // hits

private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TAPLntuHit,1);
};

#endif
