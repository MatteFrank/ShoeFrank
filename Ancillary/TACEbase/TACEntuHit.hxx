#ifndef _TACEntuHit_HXX
#define _TACEntuHit_HXX
/*!
  \file
  \version $Id: TACEntuHit.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TACEntuHit.
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
class TACEhit : public TAGbaseWC
{
   
public:
  TACEhit();
  TACEhit(TAGwaveCatcher *w);
  virtual         ~TACEhit();
  
  ClassDef(TACEhit,1);
  //
};

//##############################################################################

class TACEntuHit : public TAGdata {
public:

  TACEntuHit();
  virtual         ~TACEntuHit();


  TACEhit*          GetHit();
  const TACEhit*    GetHit() const;
  
  TACEhit*          NewHit(TAGwaveCatcher *W);
  TACEhit*          NewHit();
   
  void              SetupClones();

  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
   TACEhit*        fHit;         // hits

private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TACEntuHit,1);
};

#endif
