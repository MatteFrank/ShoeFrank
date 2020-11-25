#ifndef _TACAdatRaw_HXX
#define _TACAdatRaw_HXX
/*!
  \file
  \version $Id: TACAdatRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TACAdatRaw.
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

class TACAdatRaw : public TAGdata {
public:

  TACAdatRaw();
  virtual         ~TACAdatRaw();

  TACArawHit*       GetHit(Int_t i_ind);
  const TACArawHit* GetHit(Int_t i_ind) const;

  TACArawHit*       GetSuperHit(){return fSuperHit;}
  
  void              NewHit(TWaveformContainer *W);
  void              NewSuperHit(vector<TWaveformContainer*>);
  void              SetupClones();

   
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;

  void              UpdateRunTime(int value){fRunTime+=value;}
   
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
  ClassDef(TACAdatRaw,3);
  
private:
   Int_t           fHistN;          //
   TClonesArray*   fListOfHits;         // hits
   TACArawHit*     fSuperHit;  //sum
   Int_t           fRunTime;

   static TString fgkBranchName;    // Branch name in TTree
};

#endif
