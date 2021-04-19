#ifndef _TACAntuRaw_HXX
#define _TACAntuRaw_HXX
/*!
  \file
  \version $Id: TACAntuRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TACAntuRaw.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;
#include "TObject.h"
#include "TClonesArray.h"
#include "TAGdata.hxx"
#include "TAGbaseWD.hxx"
//

/**
 * This class stores the params of a single channel waveform
 */
class TACArawHit : public TAGbaseWD {

public:
  TACArawHit();
  TACArawHit(TWaveformContainer *w);
  virtual         ~TACArawHit();
  

  virtual double ComputeTime( TWaveformContainer *w, double frac, double del, double tleft, double tright);
  virtual double ComputeCharge( TWaveformContainer *w);
  virtual double ComputeAmplitude( TWaveformContainer *w);
  virtual double ComputePedestal( TWaveformContainer *w);
  virtual double ComputeBaseline( TWaveformContainer *w);
  

  ClassDef(TACArawHit,2);
  //
};

//##############################################################################

class TACAntuRaw : public TAGdata {
public:

  TACAntuRaw();
  virtual         ~TACAntuRaw();

  Int_t             GetHitsN() const;
  TACArawHit*       GetHit(Int_t i_ind);
  const TACArawHit* GetHit(Int_t i_ind) const;
  
  void              NewHit(TWaveformContainer *W);
  void              SetupClones();
  
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;



  
public:
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
private:
   Int_t           fHistN;          //
   TClonesArray*   fListOfHits;         // hits

private:
   static TString fgkBranchName;    // Branch name in TTree
  
  ClassDef(TACAntuRaw,1);

};

#endif
